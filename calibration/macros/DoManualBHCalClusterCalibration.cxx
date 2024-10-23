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
#include <TMultiDimFit.h>
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

  // lambda to calculate chisquare
  auto chisquare = [](const double reco, const double par) {
    const double diff = reco - par;
    const double chi2 = (reco * reco) / (par * par);
    return chi2;
  };

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning manual calibration macro..." << std::endl;

  // --------------------------------------------------------------------------
  // Open input/outputs
  // --------------------------------------------------------------------------

  // open files
  TFile* inTrain = new TFile(opt.in_file.data(),  "read");
  TFile* inApply = new TFile(opt.in_file.data(),  "read");
  TFile* output  = new TFile(opt.out_file.data(), "recreate");
  if (!inTrain || !inApply || !output) {
    std::cerr << "PANIC: couldn't open a file!\n"
              << "       inTrain = " << inTrain << "\n"
              << "       inApply = " << inApply << "\n"
              << "       output  = " << output
              << std::endl;
    assert(output && inTrain && inApply);
  }

  // print input/output files
  std::cout << "    Opened input/output files:\n"
            << "      input file  = " << opt.in_file << "\n"
            << "      output file = " << opt.out_file
            << std::endl;

  // grab input tuple
  TNtuple* ntInTrain = (TNtuple*) inTrain -> Get(opt.in_tuple.data());
  TNtuple* ntInApply = (TNtuple*) inApply -> Get(opt.in_tuple.data());
  if (!ntInTrain || !ntInApply) {
    std::cerr << "PANIC: couldn't grab input tuple!\n"
              << "       name  = " << opt.in_tuple << "\n"
              << "       tuple = " << ntInTrain
              << std::endl;
    assert(ntInTrain && ntInApply);
  }

  // set up helpers and print input tuple
  NTupleHelper train_helper( ntInTrain );
  NTupleHelper apply_helper( ntInApply );
  train_helper.SetBranches( ntInTrain );
  apply_helper.SetBranches( ntInApply );
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
    {"hChi2RawSum",    "", {"#chi^{2} = (E_{par} - #SigmaE)^{2} / E_{par}^{2}", "a.u."}, {bins.Get("chi2")}},
    {"hEneCalibSum",   "", {"#SigmaE_{c} = A(#SigmaE_{e} + B#SigmaE_{h}) [GeV]", "a.u."}, {bins.Get("energy")}},
    {"hChi2CalibSum",  "", {"#chi^{2} = (E_{par} - #SigmaE_{c})^{2} / E_{par}^{2}", "a.u."}, {bins.Get("chi2")}}
  };

  // 2d histograms
  std::vector<HistHelper::Definition> vecHistDef2D = {
    {
      "hEneRawSumVsPar",
      "",
      {"E_{par} [GeV]", "#SigmaE = #SigmaE_{h} + #SigmaE_{e} [GeV]", "a.u."},
      {bins.Get("energy"), bins.Get("energy")}
    },
    {
      "hChi2RawSumVsPar",
      "",
      {"E_{par} [GeV]", "#chi^{2} = (E_{par} - #SigmaE)^{2} / E_{par}^{2}"},
      {bins.Get("energy"), bins.Get("chi2")}
    },
    {
      "hEneCalibVsPar",
      "",
      {"E_{par} [GeV]", "#SigmaE_{c} = A(#SigmaE_{e} + B#SigmaE_{h}) [GeV]", "a.u."},
      {bins.Get("energy"), bins.Get("energy")}
    },
    {
      "hChi2CalibVsPar",
      "",
      {"E_{par} [GeV]", "#chi^{2} = (E_{par} - #SigmaE_{c})^{2} / E_{par}^{2}"},
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
  // Prepare multidimensional fitter
  // --------------------------------------------------------------------------

  // powers
  //  - FIXME probably should avoid c arrays
  int powers[2] = {1, 1};

  TMultiDimFit* mdFit = new TMultiDimFit(2, TMultiDimFit::kLegendre, "v");
  mdFit -> SetPowers( powers, 1 );
  mdFit -> SetMaxPowers( powers );
  mdFit -> SetMaxTerms( 1 );
  mdFit -> SetPowerLimit( 1 );
  mdFit -> SetMinAngle(0.);
  mdFit -> SetMinRelativeError( 0.01 );
  mdFit -> Print("p");

  // --------------------------------------------------------------------------
  // Process training ntuple
  // --------------------------------------------------------------------------

  // get number of entries for training
  const uint64_t nTrain = ntInTrain -> GetEntries();
  cout << "    Processing training tuple: " << nTrain << " events" << endl;

  // loop over entries in input tuple
  uint64_t nBytes = 0;
  for (uint64_t iTrain = 0; iTrain < nTrain; iTrain++) {

    // announce progress
    if (opt.do_progress) {
      std::cout << "      Processing entry " << iTrain + 1 << "/" << nTrain << "...";
      if (iTrain + 1 < nTrain) {
        std::cout << "\r" << std::flush;
      } else {
        std::cout << std::endl;
      }
    }

    // grab entry
    const uint64_t bytes = ntInTrain -> GetEntry(iTrain);
    if (bytes < 0.) {
      std::cerr << "WARNING error in entry #" << iTrain << "! Aborting loop!" << std::endl;
      break;
    } else {
      nBytes += bytes;
    }

    // grab raw variables
    const double ePar     = train_helper.GetVariable("ePar");
    const double eSumHCal = train_helper.GetVariable("eSumBHCal");
    const double eSumECal = train_helper.GetVariable("eSumBEMC");
    const double eSumRaw  = eSumHCal + eSumECal;

    // fill raw energy histograms
    mapHist1D["hEneRawSumHCal"]  -> Fill(eSumHCal);
    mapHist1D["hEneRawSumECal"]  -> Fill(eSumECal);
    mapHist1D["hEneRawSumBoth"]  -> Fill(eSumRaw);
    mapHist2D["hEneRawSumVsPar"] -> Fill(ePar, eSumRaw);

    // calculate chi2 on uncalibrated energies
    double chi2 = chisquare(eSumRaw, ePar);

    // fill raw chi2 histograms
    mapHist1D["hChi2RawSum"]      -> Fill(chi2);
    mapHist2D["hChi2RawSumVsPar"] -> Fill(ePar, chi2);

    // add row to fitter
    //   - FIXME probably should avoid c arrays...
    double mdx[2] = {eSumECal, eSumHCal};
    mdFit -> AddRow( mdx, ePar );

  }  // end entry loop
  std::cout << "    Training loop finished." << std::endl;


  // now run fitter calculations
  mdFit -> Print("s");
  mdFit -> MakeHistograms();
  mdFit -> FindParameterization();
  mdFit -> Print("rc");

  // --------------------------------------------------------------------------
  // Process application tuple
  //  -------------------------------------------------------------------------

  // get number of entries for application
  const uint64_t nApply = ntInApply -> GetEntries();
  cout << "    Processing application tuple: " << nApply << " events" << endl;

  // loop over entries in input tuple
  nBytes = 0;
  for (uint64_t iApply = 0; iApply < nApply; iApply++) {

    // announce progress
    if (opt.do_progress) {
      std::cout << "      Processing entry " << iApply + 1 << "/" << nApply << "...";
      if (iApply + 1 < nApply) {
        std::cout << "\r" << std::flush;
      } else {
        std::cout << std::endl;
      }
    }

    // grab entry
    const uint64_t bytes = ntInApply -> GetEntry(iApply);
    if (bytes < 0.) {
      std::cerr << "WARNING error in entry #" << iApply << "! Aborting loop!" << std::endl;
      break;
    } else {
      nBytes += bytes;
    }

    // grab raw variables
    const double ePar     = apply_helper.GetVariable("ePar");
    const double eSumHCal = apply_helper.GetVariable("eSumBHCal");
    const double eSumECal = apply_helper.GetVariable("eSumBEMC");

    // add test row to fitter
    double mdx2[2] = {eSumECal, eSumHCal};
    mdFit -> AddTestRow( mdx2, ePar );

    // and evaluate current parameterization
    const double eCalib = mdFit -> Eval( mdx2 );
    const double chi2   = chisquare(eCalib, ePar);

    // fill calibrated histograms
    mapHist1D["hEneCalibSum"]    -> Fill(eCalib);
    mapHist1D["hChi2CalibSum"]   -> Fill(chi2);
    mapHist2D["hEneCalibVsPar"]  -> Fill(ePar, eCalib);
    mapHist2D["hChi2CalibVsPar"] -> Fill(ePar, chi2);

  }  // end entry loop
  std::cout << "    Application loop finished." << std::endl;

  // now try fitting
  mdFit -> Fit("M");
  mdFit -> Print("fc v");

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

  // TEST
  for (auto hist : *(mdFit -> GetHistograms())) {
    hist -> Write();
  }
  mdFit -> MakeCode("TMultiDimFit.cxx");

  // close files
  output  -> cd();
  output  -> Close();
  inTrain -> cd();
  inTrain -> Close();
  inApply -> cd();
  inApply -> Close();

  // announce end & exit
  std::cout << "  Finished manual calibration macro!\n" << std::endl;
  return;

}

// end ========================================================================
