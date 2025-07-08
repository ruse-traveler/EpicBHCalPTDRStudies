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
#include <utility/PlotHelper.hxx>

// plotter definition
#include "./src/BHCalPlotter.hxx"

// plotting options
#include "./BaseOptions.hxx"
#include "./EnergySpectra.hxx"
#include "./ResolutionLinearityComparison.hxx"

// load plotter library
R__LOAD_LIBRARY(./src/BHCalPlotter_cxx.so)

// abbreviate common namespaces
namespace PH  = PlotHelper;
namespace BO  = BaseOptions;
namespace ES  = EnergySpectra;
namespace RLC = ResolutionLinearityComparison;



// ============================================================================
//! Make uncalibrated and calibrated cluster plots
// ============================================================================
void MakeBHCalClusterPlots( const std::string out_file = "test.plots.root" ) {

  // announce start
  std::cout << "\n  Beginning BHCal plotting routines..." << std::endl;

  // open output & create plotter ---------------------------------------------

  // open output file
  TFile* ofile = BHCalPlotter::OpenFile(out_file, "recreate");
  std::cout << "    Opened output file" << std::endl;

  // create plotter
  BHCalPlotter plotter = BHCalPlotter(
    BO::BasePlotStyle(),
    BO::BaseTextStyle(),
    BO::Text()
  );
  std::cout << "    Made plotter." << std::endl;

  // make energy spectra ------------------------------------------------------

  plotter.DoEnergySpectra(
    ES::Inputs(),
    ES::PlotRange(),
    ES::Canvas(),
    ofile
  );

  // compare resolutions and linearity ----------------------------------------

  plotter.DoResolutionLinearityComparison(
    RLC::ResoInputs(),
    RLC::ResoPlotRange(),
    RLC::ResoCanvas(),
    RLC::ResoFrame(),
    ofile
  );
  plotter.DoResolutionLinearityComparison(
    RLC::LineInputs(),
    RLC::LinePlotRange(),
    RLC::LineCanvas(),
    RLC::LineFrame(),
    ofile
  );

  // close files & exit -------------------------------------------------------

  // close output file
  ofile -> cd();
  ofile -> Close();

  // announce end & exit
  std::cout << "  Finished plotting routines!\n" << std::endl;
  return;

}

// end ========================================================================
