/// ===========================================================================
/*! \file   ResolutionLinearityComparison.hxx
 *  \author Derek Anderson
 *  \date   11.19.2024
 *
 *  A small namespace to define inputs to the
 *  resolution and linearity comparison plotting
 *  routines.
 */
/// ===========================================================================

#ifndef ResolutionLinearityComparison_hxx
#define ResolutionLinearityComparison_hxx

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// analysis utilities
#include <utility/PlotHelper.hxx>
#include <utility/HistHelper.hxx>

// abbreviate common namespaces
namespace PH = PlotHelper;
namespace HH = HistHelper;



// ============================================================================
//! Resolution/Linearity Comparison Options
// ============================================================================
/*! This namespace collects all the parameters associated with
 *  the `BHCalPlotter::DoResolutionLinearityComparison` routines.
 */
namespace ResolutionLinearityComparison {

  // --------------------------------------------------------------------------
  //! Input files
  // --------------------------------------------------------------------------
  /*! For convenience, all inputs files you'll need can
   *  be collected here.
   */ 
  const std::vector<std::string> input_files = {
    "../input/forNewPlottingMacro_nonNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d7m10y2024.root"
  };



  // ==========================================================================
  //! Create input list for resolution comparison
  // ==========================================================================
  /*! This method collects all information needed to plot
   *  various resolutions. Additional inputs can be added or
   *  removed. Needed information:
   *
   *    .file   = file object to be drawn is located in,
   *    .object = name of object to be drawn
   *    .rename = what to rename object to when saving to output
   *    .legend = what object's entry in a TLegend will say
   *    .style  = color, marker, line, and fill style
   *              (grouped into a PlotHelper::Style::Plot struct)
   */
  std::vector<PlotterInput> ResoInputs() {

    std::vector<PlotterInput> inputs = {
      {
       .file   = input_files[0],
       .object = "grUncalibResHist_eSumBHCal",
       .rename = "grReso_Uncal",
       .legend = "Sum of BHCal clusters",
       .style  = PH::Style::Plot(923, 20, 0)
      },
      {
       .file   = input_files[0],
       .object = "grCalibResHist_ePar_LD",
       .rename = "grReso_CalLD",
       .legend = "Calibrated energy [LD]",
       .style  = PH::Style::Plot(899, 25, 0)
      }
    };
    return inputs;

  }  // end 'ResoInputs()'



  // ==========================================================================
  //! Create input list for linearity comparison
  // ==========================================================================
  /*! This method collects all information needed to plot
   *  various linearities. Additional inputs can be added or
   *  removed. Needed information:
   *
   *    .file   = file object to be drawn is located in,
   *    .object = name of object to be drawn
   *    .rename = what to rename object to when saving to output
   *    .legend = what object's entry in a TLegend will say
   *    .style  = color, marker, line, and fill style
   *              (grouped into a PlotHelper::Style::Plot struct)
   */
  std::vector<PlotterInput> LineInputs() {

    std::vector<PlotterInput> inputs = {
      {
       .file   = input_files[0],
       .object = "grUncalibLinHist_eSumBHCal",
       .rename = "grLine_Uncal",
       .legend = "Sum of BHCal clusters",
       .style  = PH::Style::Plot(923, 20, 0)
      },
      {
       .file   = input_files[0],
       .object = "grCalibLinHist_ePar_LD",
       .rename = "grLine_CalLD",
       .legend = "Calibrated energy [LD]",
       .style  = PH::Style::Plot(899, 25, 0)
      }
    };
    return inputs;

  }  // end 'LineInputs()'



  // ==========================================================================
  //! Define resolution plot range
  // ==========================================================================
  /*! Plot range arguments:
   *    first  = x range to plot
   *    second = y range to plot
   */ 
  PH::PlotRange ResoPlotRange() {

    PH::PlotRange range = PH::PlotRange(
      {0., 15.},
      {0., 1.2}
    );
    return range;

  }  // end 'ResoPlotRange()'



  // ==========================================================================
  //! Define linearity plot range
  // ==========================================================================
  /*! Plot range arguments:
   *    first  = x range to plot
   *    second = y range to plot
   */ 
  PH::PlotRange LinePlotRange() {

    PH::PlotRange range = PH::PlotRange(
      {0., 15.},
      {0., 15.}
    );
    return range;

  }  // end 'LinePlotRange()'



  // ==========================================================================
  //! Define resolution canvas
  // ==========================================================================
  PH::Canvas ResoCanvas() {

    // define canvas (use default pad options)
    PH::Canvas canvas = PH::Canvas("cResolution", "", {950, 950}, PH::PadOpts());
    canvas.SetMargins( {0.02, 0.02, 0.15, 0.15} );
    return canvas;

  }  // end 'ResoCanvas()'



  // ==========================================================================
  //! Define linearity canvas
  // ==========================================================================
  PH::Canvas LineCanvas() {

    // define canvas (use default pad options)
    PH::Canvas canvas = PH::Canvas("cLinearity", "", {950, 950}, PH::PadOpts());
    canvas.SetMargins( {0.02, 0.02, 0.15, 0.15} );
    return canvas;

  }  // end 'LineCanvas()'



  // ==========================================================================
  //! Define resolution frame histogram
  // ==========================================================================
  HH::Definition ResoFrame() {

    // binnings
    HH::Binning xbins = HH::Binning(51,  -1., 50.);
    HH::Binning ybins = HH::Binning(102, -1., 50.);

    // axis labels
    std::string xtitle = "E_{par} [GeV]";
    std::string ytitle = "#sigma_{E}/#LTE_{reco}#GT";

    // define frame
    HH::Definition frame = HH::Definition(
      "hResoFrame",
      "",
      {xtitle, ytitle},
      {xbins, ybins}
    );
    return frame;

  }  // end 'ResoFrame()'



  // ==========================================================================
  //! Define linearity frame histogram
  // ==========================================================================
  HH::Definition LineFrame() {

    // binnings
    HH::Binning xbins = HH::Binning(51,  -1., 50.);
    HH::Binning ybins = HH::Binning(102, -1., 50.);

    // axis labels
    std::string xtitle = "E_{par} [GeV]";
    std::string ytitle = "#LTE_{reco}#GT [GeV]";

    // define frame
    HH::Definition frame = HH::Definition(
      "hLineFrame",
      "",
      {xtitle, ytitle},
      {xbins, ybins}
    );
    return frame;

  }  // end 'ResoFrame()'

}  // end ResolutionLinearityComparison namespace

#endif

// end ========================================================================
