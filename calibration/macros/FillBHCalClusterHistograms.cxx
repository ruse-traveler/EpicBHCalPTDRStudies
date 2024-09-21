/// ===========================================================================
/*! \file   FillBHCalClusterHistograms.cxx
 *  \author Derek Anderson
 *  \date   09.21.2024
 *
 *  A ROOT macro fill histograms of uncalibrated
 *  and calibrated BHCal + BIC clusters. Ingests
 *  The TNtuples produced by  'FillBHCalClusterCalibrationTuple.cxx'
 *  and 'TrainAndApplyBHCalClusterCalibration.cxx'
 *  (and related macros).
 */
/// ===========================================================================

#define FillBHCalClusterHistograms_cxx

// c++ utilities
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
// analysis utilities
#include "CalibratedClusterHistograms.hxx"



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_uncalib_file;   // input uncalibrated file
  std::string in_uncalib_tuple;  // input uncalibrated ntuple
  std::string in_calib_file;     // input calibrated file
  std::string in_calib_tuple;    // input calibrated ntuple
  std::string out_file;          // output file
  bool        do_progress;       // print progress through entry loop
}  DefaultOptions = {
  "./input/forNewTrainingMacro_noNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d14m9y2024.root",
  "ntForCalib",
  "./input/forNewHistogrammingMacro_noNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d21m9y2024.root",
  "ntTmvaOutput",
  "test.root",
  true
};



// ============================================================================
//! Fill uncalibrated and calibrated cluster histograms
// ============================================================================
void FillBHCalClusterHistograms(const Options& opt = DefaultOptions) {

  /* TODO set binning, color scheme, etc. here */

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning cluster histogramming  macro..." << std::endl;

  // open output file
  TFile* output = new TFile(opt.out_file.data(), "recreate");
  if (!output) {
    std::cerr << "PANIC: couldn't open output file!" << std::endl;
    assert(output);
  }
  std::cout << "    Opened output file: " << opt.out_file << std::endl;

  // fill uncalibrated histograms
  /* TODO call will go here */

  // fill calibrated histograms
  CalibratedClusterHistograms::Fill(opt.in_calib_file, opt.in_calib_tuple, output);
  std::cout << "    Filled calibrated histograms." << std::endl;

  // close output file
  output -> cd();
  output -> Close();

  // announce end & exit
  std::cout << "  Finished cluster histogramming macro!\n" << std::endl;
  return;

}

// end ========================================================================
