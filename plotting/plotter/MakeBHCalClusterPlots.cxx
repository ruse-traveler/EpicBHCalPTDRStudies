/// ===========================================================================
/*! \file   MakeBHCalClusterPlots.cxx
 *  \author Derek Anderson
 *  \date   10.11.2024
 *
 *  A driver ROOT macro to generate plots
 *  of uncalibrated & calibrated BHCal
 *  clusters.
 */
/// ===========================================================================

#define MakeBHCalClusterPlots_cxx

// c++ utilities
#include <vector>
// root libraries
#include <TFile.h>
#include <TSystem.h>
// analysis utilities
#include "./PlotHelper.hxx"
#include "./src/BHCalPlotter.hxx"

// load libraries
R__LOAD_LIBRARY(./src/BHCalPlotter_cxx.so)

// abbreviate common namespaces
namespace PH = PlotHelper;


// ============================================================================
//! Struct to consolidate user options
// ============================================================================
/* FIXME this is probably overkill for this particular macro */
struct Options {
  std::string out_file; // output file
}  DefaultOptions = {
  "test.root"
};



// ============================================================================
//! Make uncalibrated and calibrated cluster plots
// ============================================================================
void MakeBHCalClusterPlots(const Options& opt = DefaultOptions) {

  // announce start
  std::cout << "\n  Beginning BHCal plotting routines..." << std::endl;

  // options ------------------------------------------------------------------

  // open output file
  TFile* ofile = BHCalPlotter::OpenFile(opt.out_file, "recreate");
  std::cout << "    Opened output file" << std::endl;

  // create plotter
  BHCalPlotter* plotter = new BHCalPlotter();
  std::cout << "    Made plotter." << std::endl;

  // compare resolutions ------------------------------------------------------
  /* TODO might be good to move this to a header */

  const std::vector<std::string> in_files = {
    "../input/forNewPlottingMacro_nonNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d7m10y2024.root"
  };

  const std::vector<PlotterInput> reso_inputs = {
    {in_files[0], "grUncalibResHist_eSumBHCal", "Sum of BHCal clusters", PH::Style::Plot(923, 20, 0)},
    {in_files[0], "grCalibResHist_ePar_LD", "Calibrated energy [LD]", PH::Style::Plot(899, 25, 0)}
  };

  plotter -> DoResolutionComparison(reso_inputs, ofile);

  // close files & exit -------------------------------------------------------

  // close output file
  ofile -> cd();
  ofile -> Close();

  // announce end & exit
  std::cout << "  Finished plotting routines!\n" << std::endl;
  return;

}

// end ========================================================================
