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
  //! Fill calibrated cluster histograms
  // --------------------------------------------------------------------------
  void Fill(
    const std::string& in_file,
    const std::string& in_tuple,
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

    // ------------------------------------------------------------------------
    // Save and exit
    // -----------------------------------------------------------------------

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
