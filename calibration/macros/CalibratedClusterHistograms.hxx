/// ===========================================================================
/*! \file   CalibratedClusterHistograms.hxx
 *  \author Derek Anderson
 *  \date   09.21.2024
 *
 *  A small namespace to define parameters of and
 *  routines for filling calibrated BHCal+BIC
 *  clusters.
 */
/// ===========================================================================

#ifndef CalibratedClusterHistograms_hxx
#define CalibratedClusterHistograms_hxx

// c++ utilities
#include <map>
#include <set>
#include <regex>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <functional>
// root libraries
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TGraphErrors.h>
// dataframe related classes
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>
// analysis utilities
#include "HistHelper.hxx"
#include "GraphHelper.hxx"
#include "NTupleHelper.hxx"



// ============================================================================
//! Calibrated Cluster Histograms
// ============================================================================
namespace CalibratedClusterHistograms {

  // create an instance of the bin database
  HistHelper::Bins bins;

  // --------------------------------------------------------------------------
  //! 1D Quantities to be histogrammed & their histogram definition
  // --------------------------------------------------------------------------
  /*! List of pairs that define which variables to be placed in 1d histograms
   *  and the corresponding histograms. 
   *    first  = variable to be histogrammed
   *    second = histogram definition
   */ 
  std::vector<std::pair<std::string, HistHelper::Definition>> vecHistDef1D = {
    {"ePar",        {"hEnePar", "", {"E_{par} [GeV]", "a.u."}, {bins.Get("energy")}}     },
    {"ePar_LD",     {"hEneLD", "", {"E_{calib} [GeV]", "a.u."}, {bins.Get("energy")}}    },
    {"ePar_KNN",    {"hEneKNN", "", {"E_{calib} [GeV]", "a.u."}, {bins.Get("energy")}}   },
    {"ePar_MLP",    {"hEneMLP", "", {"E_{calib} [GeV]", "a.u."}, {bins.Get("energy")}}   },
    {"ePar_FDA_GA", {"hEneFDAGA", "", {"E_{calib} [GeV]", "a.u."}, {bins.Get("energy")}} }
  }; 

  // --------------------------------------------------------------------------
  //! List of variables to use for resolution calculation
  // --------------------------------------------------------------------------
  std::set<std::string> setOfVarForReso = {
    "ePar_LD",
    "ePar_KNN",
    "ePar_MLP",
    "ePar_FDA_GA"
  };

  // --------------------------------------------------------------------------
  //! Helper lambda to normalize histograms
  // -------------------------------------------------------------------------
  auto norm1D = [](const double norm, TH1D* hist1D) {
    const double integral = hist1D -> Integral();
    if (integral > 0.) hist1D -> Scale(norm / integral);
    return;
  };

  // --------------------------------------------------------------------------
  //! Options for histograms
  // --------------------------------------------------------------------------
  const bool   doNorm   = true;
  const double histNorm = 1.;

  // --------------------------------------------------------------------------
  //! Options for fits
  // --------------------------------------------------------------------------
  const float       nSigFit = 1.5;
  const std::string fitFunc = "gaus(0)";
  const std::string fitOpt  = "rQ";

  // --------------------------------------------------------------------------
  //! Option for graphs
  // --------------------------------------------------------------------------
  const std::pair<std::string, std::string> grResName = {"grResHist", "grResFit"};
  const std::pair<std::string, std::string> grLinName = {"grLinHist", "grLinFit"};



  // --------------------------------------------------------------------------
  //! Fill calibrated cluster histograms
  // --------------------------------------------------------------------------
  void Fill(
    TFile* out_file,
    const std::string& in_file,
    const std::string& in_tuple,
    const std::vector<std::tuple<std::string, float, float, float>> par_bins,
    const bool do_progress = false
  ) {

    // turn on histogram errors & announce start
    TH1::SetDefaultSumw2(true);
    TH2::SetDefaultSumw2(true);
    std::cout << "\n------------------------------\n"
              << "  Starting calibrated cluster filling..."
              << std::endl;

    // ------------------------------------------------------------------------
    // Open inputs
    // ------------------------------------------------------------------------

    // make sure output file isn't null
    if (!out_file) {
      std::cerr << "PANIC: pointer to output file is NULL!" << std::endl;
      assert(out_file);
    }

    // open input file
    TFile* input  = new TFile(in_file.data(),  "read");
    if (!input) {
      std::cerr << "PANIC: couldn't open input file!\n"
                << "       input  = " << input
                << std::endl;
      assert(input);
    }

    // grab input tuple
    TNtuple* ntInput = (TNtuple*) input -> Get(in_tuple.data());
    if (!input) {
      std::cerr << "PANIC: couldn't grab input tuple!\n"
                << "       name  = " << in_tuple
                << std::endl;
      assert(ntInput);
    }

    // print input
    std::cout << "    Opened dataframe:\n"
              << "      input file  = " << in_file << "\n"
              << "      input tuple = " << in_tuple
              << std::endl;

    // create helper to process input tuple
    NTupleHelper helper( ntInput );
    helper.SetBranches( ntInput );

    // ------------------------------------------------------------------------
    // Generate histograms
    // -----------------------------------------------------------------------

    std::vector<std::vector<std::pair<TH1D*, bool>>> vecHist1D( par_bins.size() );
    for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

      // create 1d hist
      for (auto def : vecHistDef1D) {

        // check if being used for reso calcualation
        const bool useForReso = (setOfVarForReso.count(def.first) > 0);

        // make histogram
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));
        vecHist1D[iBin].push_back(  {def.second.MakeTH1(), useForReso} );

      }
    }  // end particle bin loop

    // ------------------------------------------------------------------------
    // Process input tuple
    // ------------------------------------------------------------------------

    // lambda to check if a particle energy is in bin
    auto isInParBin = [](const float energy, const std::pair<float, float>& bin) {
      return ((energy >= bin.first) && (energy < bin.second));
    };

    // get number of entries
    const uint64_t nEntries = ntInput -> GetEntries();
    cout << "    Processing: " << nEntries << " events" << endl;

    // loop over entries in input tuple
    uint64_t nBytes = 0;
    for (uint64_t iEntry = 0; iEntry < nEntries; iEntry++) {

      // announce progress
      if (do_progress) {
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

      // fill histograms for each bin of particle energy
      for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

        // check if particle energy is in bin
        std::pair<float, float> bin = {get<2>(par_bins[iBin]), get<3>(par_bins[iBin])};
        if ( isInParBin(helper.GetVariable("ePar"), bin) ) {

          // fill 1d histograms
          for (std::size_t iVar = 0; iVar < vecHistDef1D.size(); ++iVar) {
            vecHist1D[iBin][iVar].first -> Fill( 
              helper.GetVariable(vecHistDef1D[iVar].first) 
            );
          }  // end variable loop
        }
      }  // end bin loop
    }  // end entry loop
    std::cout << "    Finished processing tuple." << std::endl;

    // ------------------------------------------------------------------------
    // Normalize histograms if need be
    // ------------------------------------------------------------------------

    if (doNorm) {
      for (auto& row1D : vecHist1D) {
        for (auto& hist1D : row1D) {
          norm1D(histNorm, hist1D.first);
        }
      }
      std::cout << "    Normalized histograms." << std::endl;
    }

    // ------------------------------------------------------------------------
    // Fit energies and generate graphs
    // ------------------------------------------------------------------------

    // for resolution graphs
    std::vector<GraphHelper::Definition> vecResHist;
    std::vector<GraphHelper::Definition> vecResFit;
    std::vector<GraphHelper::Definition> vecLinHist;
    std::vector<GraphHelper::Definition> vecLinFit;

    // loop over histograms
    std::vector<std::vector<TF1*>> vecFit1D( vecHist1D.front().size() );
    for (std::size_t iVar = 0; iVar < vecHist1D.front().size(); ++iVar) {

      // if not being used for reso calculation, skip
      const bool useForReso = vecHist1D[0][iVar].second;
      if (!useForReso) continue; 

      // define graphs
      vecResHist.push_back(
        GraphHelper::Definition(grResName.first + vecHistDef1D[iVar].first)
      );
      vecResFit.push_back(
        GraphHelper::Definition(grResName.second + vecHistDef1D[iVar].first)
      );
      vecLinHist.push_back(
        GraphHelper::Definition(grLinName.first + vecHistDef1D[iVar].first)
      );
      vecLinFit.push_back(
        GraphHelper::Definition(grLinName.second + vecHistDef1D[iVar].first)
      );
  
      // loop over particle bins
      for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

        // grab hist integral, mean, width
        const double intHist    = vecHist1D[iBin][iVar].first -> Integral();
        const double muValHist  = vecHist1D[iBin][iVar].first -> GetMean();
        const double muErrHist  = vecHist1D[iBin][iVar].first -> GetMeanError();
        const double rmsValHist = vecHist1D[iBin][iVar].first -> GetRMS();
        const double rmsErrHist = vecHist1D[iBin][iVar].first -> GetRMSError();
        const double resValHist = rmsValHist / muValHist;
        const double resErrHist = std::hypot((muErrHist/muValHist), (rmsErrHist/rmsValHist));

        // add points to hist graphs
        vecResHist.back().AddPoint( {get<1>(par_bins[iBin]), resValHist, 0., resErrHist} );
        vecLinHist.back().AddPoint( {get<1>(par_bins[iBin]), muValHist,  0., muErrHist}  );

        // create fit name
        std::string fitName( vecHist1D[iBin][iVar].first -> GetName() );
        fitName[0] = 'f';

        // calculate fit ranges
        const double fitMin = muValHist - (nSigFit * rmsValHist);
        const double fitMax = muValHist + (nSigFit * rmsValHist);

        // create fit function
        TF1* fit = new TF1(fitName.data(), fitFunc.data(), fitMin, fitMax);
        fit -> SetParameter(0, intHist);
        fit -> SetParameter(1, muValHist);
        fit -> SetParameter(2, rmsValHist);

        // fit histogram
        vecHist1D[iBin][iVar].first -> Fit(fitName.data(), fitOpt.data());
        vecFit1D[iVar].push_back(fit);

      }  // end bin loop
    }  // end hist loop

    /* TODO fill in
     *   3. grab mu, sigma and errors from fit and load into vectors
     *   4. turn vectors into TGraphs
     */

    // ------------------------------------------------------------------------
    // Save and exit
    // ------------------------------------------------------------------------

    // save histograms
    out_file -> cd();
    for (auto& row1D : vecHist1D) {
      for (auto& hist1D : row1D) {
        hist1D.first -> Write();
      }
    }
    /* TODO save TF1s here */
    /* TODO save graphs here */

    // announce end
    std::cout << "  Finished filling calibrated cluster histograms!\n"
              << "------------------------------\n"
              << std::endl;

    // exit
    return;

  }  // end 'Fill(TFile*, std::string&, std::string&, std::vector<std::tuple<*>>, bool)'

}  // end CalibratedClusterHistograms

#endif

// end ========================================================================
