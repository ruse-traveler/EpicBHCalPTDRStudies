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
#include <string>
#include <vector>
#include <utility>
#include <iostream>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
// dataframe related classes
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>
// analysis utilities
#include "../../utility/HistHelper.hxx"



// ============================================================================
//! Calibrated Cluster Histograms
// ============================================================================
namespace CalibratedClusterHistograms {

  // --------------------------------------------------------------------------
  //! 1D Quantities to be histogrammed & binning
  // --------------------------------------------------------------------------
  std::vector<std::pair<std::string, HistHelper::Definition>> vecHist1D = {
    { "ePar", {"hEnePar", {"E_{par} [GeV]", "counts"}, {/* TODO binning goes here */} } },
    { "ePar_LD", {"hEneLD", {"E_{calib} [GeV]", "counts"}, {/* TODO binning goes here */} }
  }; 


  // --------------------------------------------------------------------------
  //! Fill calibrated cluster histograms
  // --------------------------------------------------------------------------
  void Fill(
    const std::string& in_file,
    const std::string& in_tuple,
    const std::vector<std::pair<float, float>> par_bins,
    TFile* out_file
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

    // open dataframe
    ROOT::RDataFrame frame(in_tuple.data(), in_file.data());

    // make sure file isn't empty
    auto frames = frame.Count();
    if (frames == 0) {
      std::cerr << "PANIC: no frames found!" << std::endl;
      assert(frames > 0);
    }

    // print input
    std::cout << "    Opened dataframe:\n"
              << "      input file  = " << in_file << "\n"
              << "      input tuple = " << in_tuple
              << std::endl;

    /* TODO generate histograms for each energy bin here */

    // ------------------------------------------------------------------------
    // Process input tuple
    // ------------------------------------------------------------------------

    // lambda to check if particle energy is in bin
    std::size_t iBin;
    auto isInEneBin = [&iBin, bins](const float energy) {
      return ((energy >= bins.at(iBin).first) && (energy < bins.at(iBin).second));
    }

    /* TODO Dataframe processing goes here
     * Will look like:
     *   for (iBin = 0; iBin < bins.size(); ++iBin) {
     *     for (variable : variables) {
     *       vecRResultPtr<TH1D> = frame.Filter(isInEneBin, {"ePar"})
     *                                  .Hist1D(TH1DModel);
     *     }
     *   }
     */

    // ------------------------------------------------------------------------
    // Save and exit
    // ------------------------------------------------------------------------

    // save histograms
    out_file -> cd();
    /* TODO saving goes here */

    // announce end
    std::cout << "  Finished filling calibrated cluster histograms!\n"
              << "------------------------------\n"
              << std::endl;

    // exit
    return;

  }  // end 'Fill(std::string&, std::string&, std::string&)'

}  // end CalibratedClusterHistograms

#endif

// end ========================================================================
