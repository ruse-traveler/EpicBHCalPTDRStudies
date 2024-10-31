/// ===========================================================================
/*! \file   FillBHCalOnlyHistograms.cxx
 *  \author Derek Anderson
 *  \date   10.31.2024
 *
 *  A ROOT macro fill histograms for BHCal-only simulations.
 *  Ingests the TNtuples produced by  'FillBHCalONlyTuple.cxx'.
 */
/// ===========================================================================

#define FillBHCalOnlyHistograms_cxx

// c++ utilities
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
// analysis utilities
#include "BHCalOnlyHistograms.hxx"



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;     // input file
  std::string in_tuple;    // input ntuple
  std::string out_file;    // output file
  bool        do_progress; // print progress through entry loop
}  DefaultOptions = {
  "./reco/forBHCalOnlyCheck.evt5Ke120pim_central.d31m10y2024.tuple.root",
  "ntBHCalOnly",
  "forBHCalOnlyCheck.evt5Ke120pim_central.d31m10y2024.hists.root",
  true
};



// ============================================================================
//! Fill uncalibrated and calibrated cluster histograms
// ============================================================================
void FillBHCalOnlyHistograms(const Options& opt = DefaultOptions) {

  // particle energy, binning & histogram tags
  //   <0> = tag
  //   <1> = particle energy
  //   <2> = bin low edge
  //   <3> = bin high edge
  const std::vector<std::tuple<std::string, float, float, float>> vecParBins = {
    std::make_tuple("Ene1",  1.,  0.5, 1.5),
    std::make_tuple("Ene2",  2.,  1.5, 4.),
    std::make_tuple("Ene5",  5.,  4.,  6.),
    std::make_tuple("Ene7",  7.,  6.,  8.),
    std::make_tuple("Ene10", 10., 8.,  12.),
    std::make_tuple("Ene15", 15., 12., 18.),
    std::make_tuple("Ene20", 20., 18., 100.)
  };

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning BHCal-only histogramming macro..." << std::endl;

  // open output file
  TFile* output = new TFile(opt.out_file.data(), "recreate");
  if (!output) {
    std::cerr << "PANIC: couldn't open output file!" << std::endl;
    assert(output);
  }
  std::cout << "    Opened output file: " << opt.out_file << std::endl;

  // fill uncalibrated histograms
  BHCalOnlyHistograms::Fill(output, opt.in_file, opt.in_tuple, vecParBins, opt.do_progress);
  std::cout << "    Filled BHCal-only histograms." << std::endl;

  // close output file
  output -> cd();
  output -> Close();

  // announce end & exit
  std::cout << "  Finished BHCal-only histogramming macro!\n" << std::endl;
  return;

}

// end ========================================================================
