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
  //! Whether or not 1D histogram is used to calculate resolution
  // --------------------------------------------------------------------------
  /*! List of bool's that indicate whether or not a 1D histogram
   *  should be used to calculate a resolution. Should be 1-to-1
   *  with vecHistDef1D.
   *
   *  FIXME there might be an easier way to do this...
   */
  std::vector<bool> vecDoReso = {
    false,
    true,
    true,
    true,
    true
  };



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

    std::vector<std::vector<TH1D*>> vecHist1D( par_bins.size() );
    for (std::size_t iBin = 0; iBin < par_bins.size(); ++iBin) {

      // create 1d hist
      for (auto def : vecHistDef1D) {
        def.second.AppendToName("_" + get<0>(par_bins[iBin]));
        vecHist1D[iBin].push_back(  def.second.MakeTH1() );
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
            vecHist1D[iBin][iVar] -> Fill( helper.GetVariable(vecHistDef1D[iVar].first) );
          }
        }
      }  // end bin loop
    }  // end entry loop
    std::cout << "    Finished processing tuple." << std::endl;

    // ------------------------------------------------------------------------
    // Fit energies and generate graphs
    // ------------------------------------------------------------------------

    /* TODO fill in
     *   1. loop over histograms
     *   2. if flagged as being used for reco, fit w/ gaussian based on
     *      provided info
     *   3. grab mu, sigma and errors from fit/hists and load into vectors
     *   4. turn vectors into TGraphs
     */

    // ------------------------------------------------------------------------
    // Save and exit
    // ------------------------------------------------------------------------

    // save histograms
    out_file -> cd();
    for (auto& row1D : vecHist1D) {
      for (auto& hist1D : row1D) {
        hist1D -> Write();
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
