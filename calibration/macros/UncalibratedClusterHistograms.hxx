/// ===========================================================================
/*! \file   UncalibratedClusterHistograms.hxx
 *  \author Derek Anderson
 *  \date   10.01.2024
 *
 *  A small namespace to define parameters of and
 *  routines for filling uncalibrated BHCal+BIC
 *  histograms.
 */
/// ===========================================================================

#ifndef UncalibratedClusterHistograms_hxx
#define UncalibratedClusterHistograms_hxx

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
#include <TTreeFormula.h>
// analysis utilities
#include "../../utility/HistHelper.hxx"
#include "../../utility/GraphHelper.hxx"
#include "../../utility/NTupleHelper.hxx"



// ============================================================================
//! Uncalibrated Cluster Histograms
// ============================================================================
namespace UncalibratedClusterHistograms {

  // create an instance of the bin database
  HistHelper::Bins bins;

  // --------------------------------------------------------------------------
  //! 1D variables to be histogrammed & definition
  // --------------------------------------------------------------------------
  /*! List of pairs that define which variables to be placed in 1d histograms
   *  and the corresponding histograms. 
   *    first  = variable to be histogrammed
   *    second = histogram definition
   */ 
  std::vector<std::pair<std::string, HistHelper::Definition>> vecVarDef1D = {
    {
      "eLeadBHCal", 
      {"hELeadBHCal", "", {"E_{clust}^{lead} [GeV]", "a.u."}, {bins.Get("energy")}}
    },
    {
      "eLeadBEMC",
      {"hELeadBEMC", "", {"E_{clust}^{lead} [GeV]", "a.u."}, {bins.Get("energy")}}
    },
    {
      "eSumBHCal",
      {"hESumBHCal", "", {"#SigmaE_{clust} [GeV]", "a.u."}, {bins.Get("energy")}}
    },
    {
      "eSumBEMC",
      {"hESumBEMC", "", {"#SigmaE_{clust} [GeV]", "a.u."}, {bins.Get("energy")}}
    }
  };

  // --------------------------------------------------------------------------
  //! 1D formulas to be histogrammed & definition
  // --------------------------------------------------------------------------
  /*! List of pairs that define which formulas to be placed in 1d histograms
   *  and the corresponding histograms. 
   *    first  = formula to be histogrammed
   *    second = histogram definition
   */ 
  std::vector<std::pair<std::string, HistHelper::Definition>> vecFormDef1D = {
    {
      "eLeadBHCal/eSumBHCal",
      {
        "hLeadSumFracBHCal",
        "", 
        {"{E_{clust}^{lead}/#SigmaE_{clust}", "a.u."}, 
        {bins.Get("fraction")}
      } 
    },
    {
      "eLeadBEMC/eSumBEMC",
      {
        "hLeadSumFracBEMC",
        "", 
        {"{E_{clust}^{lead}/#SigmaE_{clust}", "a.u."}, 
        {bins.Get("fraction")}
      } 
    }
  };

  // --------------------------------------------------------------------------
  //! 2D variables to be histogrammed & definition
  // --------------------------------------------------------------------------
  /*! List of pairs that define which variables to be placed in 2d histograms
   *  and the corresponding histograms. 
   *    first  = pair of variables to be histogrammed
   *    second = histogram definition
   */
  std::vector<std::pair<std::pair<std::string, std::string>, HistHelper::Definition>> vecVarDef2D = {
    {
      {"hLeadBHCal", "fLeadBHCal"},
      {
        "hLeadPhiVsEtaBHCal",
        "",
        {"#eta_{clust}^{lead}", "#varphi_{clust}^{lead}", "counts"},
        {bins.Get("eta"), bins.Get("phi")}
      }
    },
    {
      {"hLeadBEMC", "fLeadBEMC"},
      {
        "hLeadPhiVsEtaBEMC",
        "",
        {"#eta_{clust}^{lead}", "#varphi_{clust}^{lead}", "counts"},
        {bins.Get("eta"), bins.Get("phi")}
      }
    },
    {
      {"eLeadBHCal", "eLeadBEMC"},
      {
        "hLeadEneBEMCvsBHCal",
        "",
        {"E_{clust, BHCal}^{lead}", "E_{clust, BEMC}^{lead}", "counts"},
        {bins.Get("energy"), bins.Get("energy")}
      }
    },
    {
      {"eSumBHCal", "eSumBEMC"},
      {
        "hSumEneBEMCvsBHCal",
        "",
        {"#SigmaE_{clust, BHCal}", "#SigmaE_{clust, BEMC}", "counts"},
        {bins.Get("energy"), bins.Get("energy")}
      }
    }
  };

  // --------------------------------------------------------------------------
  //! List of variables to use for resolution calculation
  // --------------------------------------------------------------------------
  std::set<std::string> setOfVarForReso = {
    "eLeadBHCal",
    "eSumBHCal"
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
  const float       nSigFit = 2.0;
  const std::string fitFunc = "gaus(0)";
  const std::string fitOpt  = "rQ";

  // --------------------------------------------------------------------------
  //! Option for graphs
  // --------------------------------------------------------------------------
  const std::string grResName = "grUncalibResHist";
  const std::string grLinName = "grUncalibLinHist";



  // --------------------------------------------------------------------------
  //! Fill Uncalibrated cluster histograms
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
              << "  Starting uncalibrated cluster filling..."
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
    std::cout << "    Opened inputs:\n"
              << "      input file  = " << in_file << "\n"
              << "      input tuple = " << in_tuple
              << std::endl;

    // create helper to process input tuple
    NTupleHelper helper( ntInput );
    helper.SetBranches( ntInput );

    // ------------------------------------------------------------------------
    // Generate histograms
    // -----------------------------------------------------------------------

    std::vector<std::vector<TH2D*>>                           vecVar2D( par_bins.size() );
    std::vector<std::vector<std::pair<TH1D*, bool>>>          vecVar1D( par_bins.size() );
    std::vector<std::vector<std::pair<TH1D*, TTreeFormula*>>> vecForm1D( par_bins.size() );
    for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

      // create 1d variable hists
      for (auto def : vecVarDef1D) {
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));
        vecVar1D[iBin].push_back(
          {def.second.MakeTH1(), (setOfVarForReso.count(def.first) > 0)}
        );
      }  // end 1d hist loop

      // create 1d formula hists
      for (auto def : vecFormDef1D) {

        // create hist name
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));

        // create formula name
        std::string formName = def.second.GetName();
        formName[0] = 'f';

        // create histogram
        vecForm1D[iBin].push_back(
          {def.second.MakeTH1(), new TTreeFormula(formName.data(), def.first.data(), ntInput)}
        );
      }  // end 1d formula loop

      // create 2d variable hists
      for (auto def : vecVarDef2D) {
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));
        vecVar2D[iBin].push_back( def.second.MakeTH2() );
      }  // end 2d hist loop

    }  // end particle bin loop
    std::cout << "    Generated histograms." << std::endl;

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


          // fill 1d variable histograms
          for (std::size_t iVar = 0; iVar < vecVarDef1D.size(); ++iVar) {
            vecVar1D[iBin][iVar].first -> Fill( 
              helper.GetVariable(vecVarDef1D[iVar].first) 
            );
          }  // end variable loop

          // fill 1d formula histograms
          for (std::size_t iForm = 0; iForm < vecFormDef1D.size(); ++iForm) {
            vecForm1D[iBin][iForm].first -> Fill(
              vecForm1D[iBin][iForm].second -> EvalInstance()
            );
          }  // end formula loop

          // fill 2d variable histograms
          for (std::size_t iVar = 0; iVar < vecVarDef2D.size(); ++iVar) {
            vecVar2D[iBin][iVar] -> Fill(
              helper.GetVariable(vecVarDef2D[iVar].first.first),
              helper.GetVariable(vecVarDef2D[iVar].first.second)
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

      // normalize 1d variables hists
      for (auto& row1D : vecVar1D) {
        for (auto& hist1D : row1D) {
          norm1D(histNorm, hist1D.first);
        }
      }

      // normalize 1d formula hists
      for (auto& row1D : vecForm1D) {
        for (auto& hist1D : row1D) {
          norm1D(histNorm, hist1D.first);
        }
      }
      std::cout << "    Normalized histograms." << std::endl;
    }

    // ------------------------------------------------------------------------
    // Generate graphs
    // ------------------------------------------------------------------------

    // for resolution graphs
    std::vector<GraphHelper::Definition> vecResHist;
    std::vector<GraphHelper::Definition> vecLinHist;

    // loop over histograms
    for (std::size_t iVar = 0; iVar < vecVar1D.front().size(); ++iVar) {

      // if not being used for reso calculation, skip
      const bool useForReso = vecVar1D[0][iVar].second;
      if (!useForReso) continue;

      // define graphs
      vecResHist.push_back(
        GraphHelper::Definition(grResName + "_" + vecVarDef1D[iVar].first)
      );
      vecLinHist.push_back(
        GraphHelper::Definition(grLinName + "_" + vecVarDef1D[iVar].first)
      );

      // loop over particle bins
      for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

        // grab hist integral, mean, width
        const double intHist    = vecVar1D[iBin][iVar].first -> Integral();
        const double muValHist  = vecVar1D[iBin][iVar].first -> GetMean();
        const double muErrHist  = vecVar1D[iBin][iVar].first -> GetMeanError();
        const double rmsValHist = vecVar1D[iBin][iVar].first -> GetRMS();
        const double rmsErrHist = vecVar1D[iBin][iVar].first -> GetRMSError();
        const double resValHist = rmsValHist / muValHist;
        const double resErrHist = std::hypot((muErrHist/muValHist), (rmsErrHist/rmsValHist));

        // add points to hist graphs
        vecResHist.back().AddPoint( {get<1>(par_bins[iBin]), resValHist, 0., resErrHist} );
        vecLinHist.back().AddPoint( {get<1>(par_bins[iBin]), muValHist,  0., muErrHist}  );

      }  // end bin loop
    }  // end hist loop

    // ------------------------------------------------------------------------
    // Save and exit
    // ------------------------------------------------------------------------

    // save histograms, fits, and graphs
    out_file -> cd();
    for (auto& row1D : vecVar1D) {
      for (auto& hist1D : row1D) {
        hist1D.first -> Write();
      }
    }
    for (auto& row1D : vecForm1D) {
      for (auto& hist1D : row1D) {
        hist1D.first -> Write();
      }
    }
    for (auto& row2D : vecVar2D) {
      for (auto& hist2D : row2D) {
        hist2D -> Write();
      }
    }
    for (std::size_t iGraph = 0; iGraph < vecResHist.size(); ++iGraph) {
      vecResHist[iGraph].MakeTGraphErrors() -> Write();
      vecLinHist[iGraph].MakeTGraphErrors() -> Write();
    }

    // announce end
    std::cout << "  Finished filling uncalibrated cluster histograms!\n"
              << "------------------------------\n"
              << std::endl;


    // exit
    return;

  }  // end 'Fill(TFile*, std::string&, std::string&, std::vector<std::tuple<*>>, bool)'

}  // end UncalibratedClusterHistograms

#endif

// end ========================================================================
