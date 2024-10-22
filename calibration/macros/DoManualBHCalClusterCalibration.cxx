/// ===========================================================================
/*! \file   DoManualBHCalClusterCalibration.cxx
 *  \author Derek Anderson
 *  \date   10.22.2024
 *
 *  A ROOT macro to manually calibrate rough
 *  calibration factors for the energy of
 *  clusters in the BHCal and BIC. Ingests the
 *  TNtuple produced by
    'FillBHCalClusterCalibrationTuple.cxx'
 */
/// ===========================================================================

#define DoManualBHCalClusterCalibration_cxx

// c++ utilities
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
// analysis utilities
#include "../../utility/HistHelper.hxx"
#include "../../utility/GraphHelper.hxx"
#include "../../utility/NTupleHelper.hxx"



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;      // input file
  std::string in_tuple;     // input ntuple
  std::string out_file;     // output file
  bool        do_progress;  // print progress through entry loop
}  DefaultOptions = {
  "./input/forNewTrainingMacro_noNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d14m9y2024.root",
  "ntForCalib",
  "test.root",
  true
};



// ============================================================================
//! Manually calculate calibration factors for BHCal (and BIC) clusters
// ============================================================================
void DoManualBHCalClusterCalibration(const Options& opt = DefaultOptions) {

  // parameters
  //   - TODO Maybe should go into separate header?
  auto minimizee = [](double *x, double *par) {
    const double eReco = par[0] * (x[0] + (par[1] * x[1]));
    const double eDiff = x[2] - eReco;
    const double chi2  = (eDiff * eDiff) / (x[2] * x[2]);
    return chi2;
  };

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning manual calibration macro..." << std::endl;

  // --------------------------------------------------------------------------
  // Open input/outputs
  // --------------------------------------------------------------------------

  // open files
  TFile* input  = new TFile(opt.in_file.data(),  "read");
  TFile* output = new TFile(opt.out_file.data(), "recreate");
  if (!input || !output) {
    std::cerr << "PANIC: couldn't open a file!\n"
              << "       input  = " << input << "\n"
              << "       output = " << output
              << std::endl;
    assert(output && input);
  }

  // print input/output files
  std::cout << "    Opened input/output files:\n"
            << "      input file  = " << opt.in_file << "\n"
            << "      output file = " << opt.out_file
            << std::endl;

  // grab input tuple
  TNtuple* ntInput = (TNtuple*) input -> Get(opt.in_tuple.data());
  if (!ntInput) {
    std::cerr << "PANIC: couldn't grab input tuple!\n"
              << "       name  = " << opt.in_tuple << "\n"
              << "       tuple = " << ntInput
              << std::endl;
    assert(ntInput);
  }

  // set up helper and print input tuple
  NTupleHelper in_helper( ntInput );
  in_helper.SetBranches( ntInput );
  std::cout << "    Grabbed input tuples:\n"
            << "      tuple = " << opt.in_tuple
            << std::endl;

  // --------------------------------------------------------------------------
  // Set up histograms
  // --------------------------------------------------------------------------
  /* TODO might want to move to a separate header */

  // create an instance of the bin database
  HistHelper::Bins bins;

  // 1D histograms
  std::vector<HistHelper::Definition> vecHistDef1D = {
    {"hEneRawSumHCal", "", {"#SigmaE_{h} [GeV]", "a.u."}, {bins.Get("energy")}},
    {"hEneRawSumECal", "", {"#SigmaE_{e} [GeV]", "a.u."}, {bins.Get("energy")}},
    {"hEneRawSumBoth", "", {"#SigmaE = #SigmaE_{h} + #SigmaE_{e} [GeV]", "a.u."}, {bins.Get("energy")}},
    {"hChi2RawSum",    "", {"#chi^{2} = (E_{par} - #SigmaE)^{2} / E_{par}^{2}", "a.u."}, {bins.Get("chi2")}}
  };

  // 2d histograms
  std::vector<HistHelper::Definition> vecHistDef2D = {
    {
      "hEneRawSumVsPar",
      "",
      {"E_{par} [GeV]", "#SigamE = #SigmaE_{H} + #SigmaE_{E} [GeV]", "a.u."},
      {bins.Get("energy"), bins.Get("energy")}
    },
    {
      "hChi2RawSumVsPar",
      "",
      {"E_{par} [GeV]", "#chi^{2} = (E_{par} - #SigmaE)^{2} / E_{par}^{2}"},
      {bins.Get("energy"), bins.Get("chi2")}
    }
  };

  // turn on histogram errors
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // create 1D histograms
  std::map<std::string, TH1D*> mapHist1D;
  for (const auto histDef1D : vecHistDef1D) {
    mapHist1D[ histDef1D.GetName() ] = histDef1D.MakeTH1();
  }

  // create 2D histograms
  std::map<std::string, TH2D*> mapHist2D;
  for (const auto histDef2D : vecHistDef2D) {
    mapHist2D[ histDef2D.GetName() ] = histDef2D.MakeTH2();
  }

  // --------------------------------------------------------------------------
  // Run calculations
  // --------------------------------------------------------------------------

  // get number of entries for application
  const uint64_t nEntries = ntInput -> GetEntries();
  cout << "    Processing: " << nEntries << " events" << endl;

  // loop over entries in input tuple
  uint64_t nBytes = 0;
  for (uint64_t iEntry = 0; iEntry < nEntries; iEntry++) {

    // announce progress
    if (opt.do_progress) {
      std::cout << "      Processing entry " << iEntry + 1 << "/" << nEntries << "...";
      if (iEntry + 1 < nEntries) {
        std::cout << "\r" << std::flush;
      } else {
        std::cout << std::endl;
      }
    }

    // grab entry
    const uint64_t bytes = ntInput -> GetEntry(iEntry);
    if (bytes < 0.) {
      std::cerr << "WARNING error in entry #" << iEntry << "! Aborting loop!" << std::endl;
      break;
    } else {
      nBytes += bytes;
    }

    // grab raw variables
    const double ePar     = in_helper.GetVariable("ePar");
    const double eSumHCal = in_helper.GetVariable("eSumBHCal");
    const double eSumECal = in_helper.GetVariable("eSumBEMC");
    const double eSumRaw  = eSumHCal + eSumECal;

    // fill raw energy histograms
    mapHist1D["hEneRawSumHCal"]  -> Fill(eSumHCal);
    mapHist1D["hEneRawSumECal"]  -> Fill(eSumECal);
    mapHist1D["hEneRawSumBoth"]  -> Fill(eSumRaw);
    mapHist2D["hEneRawSumVsPar"] -> Fill(ePar, eSumRaw);

    // calculate chi2 on uncalibrated energies
    //   - FIXME probably should avoid c arrays...
    double x[3]   = {eSumECal, eSumHCal, ePar};
    double par[3] = {1.0, 1.0, 1.0};  // dummy values for now
    double chi2   = minimizee(x, par);

    // fill raw chi2 histograms
    mapHist1D["hChi2RawSum"]      -> Fill(chi2);
    mapHist2D["hChi2RawSumVsPar"] -> Fill(ePar, chi2);

  }  // end entry loop
  std::cout << "    Entry loop finished." << std::endl;


  // --------------------------------------------------------------------------
  // Save output and exit
  // --------------------------------------------------------------------------

  // save histograms
  output -> cd();
  for (const auto& hist1D : mapHist1D) {
    hist1D.second -> Write();
  }
  for (const auto& hist2D : mapHist2D) {
    hist2D.second -> Write();
  }

  // close files
  output -> cd();
  output -> Close();
  input  -> cd();
  input  -> Close();

  // announce end & exit
  std::cout << "  Finished manual calibration macro!\n" << std::endl;
  return;

}

// end ========================================================================
