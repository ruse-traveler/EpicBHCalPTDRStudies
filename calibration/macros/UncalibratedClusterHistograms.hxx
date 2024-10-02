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
      {"hELeadHCal", "", {"E_{clust}^{lead} [GeV]", "a.u."}, {bins.Get("energy")}}
    },
    {
      "eLeadBEMC",
      {"hELeadECal", "", {"E_{clust}^{lead} [GeV]", "a.u."}, {bins.Get("energy")}}
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
        "hLeadSumFracBECal",
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
   *    first  = <LIST of variables>
   *    second = histogram definition
   */
  /* TODO
   *   - fLeadHCal vs. hLeadHCal
   *   - fLeadECal vs. hLeadECal
   *   - eLeadECal vs. eLeadHCal
   *   - eSumECal vs. eSumHCal
   */

  // --------------------------------------------------------------------------
  //! List of variables to use for resolution calculation
  // --------------------------------------------------------------------------
  std::set<std::string> setOfVarForReso = {
    "eLeadHCalClust",
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
  const std::pair<std::string, std::string> grResName = {"grResHist", "grResFit"};
  const std::pair<std::string, std::string> grLinName = {"grLinHist", "grLinFit"};



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

    std::vector< std::vector< std::pair< TH1D*, bool > > > vecVar1D( par_bins.size() );
    std::vector< std::vector< std::pair< TH1D*, TTreeFormula* > > > vecForm1D( par_bins.size() );
    for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

      // create 1d variable hists
      for (auto def : vecVarDef1D) {

        // check if being used for reso calcualation
        const bool useForReso = (setOfVarForReso.count(def.first) > 0);

        // make histogram
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));
        vecVar1D[iBin].push_back( {def.second.MakeTH1(), useForReso} );

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

      /* TODO make 2d histograms here */

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

          /* TODO
           *   - Fill 2d histograms
           */

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
