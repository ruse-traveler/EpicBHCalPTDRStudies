/// ===========================================================================
/*! \file   EnergySpectra.hxx
 *  \author Derek Anderson
 *  \date   11.22.2024
 *
 *  A small namespace to define inputs to the
 *  energy spectra plotting routine.
 */
/// ===========================================================================

#ifndef EnergySpectra_hxx
#define EnergySpectra_hxx

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
//! Energy Spectra Options
// ============================================================================
/*! This namespace collects all the parameters associated with
 *  the `BHCalPlotter::EnergySpectra` routines. 
 */
namespace EnergySpectra {

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
  //! Create input list for energy spectra
  // ==========================================================================
  /*! This method collects all information needed to plot
   *  various energy spectra. Additional inputs can be added
   *  or removed. Needed information:
   *
   *    .file   = file object to be drawn is located in,
   *    .object = name of object to be drawn
   *    .rename = what to rename object to when saving to output
   *    .legend = what object's entry in a TLegend will say
   *    .style  = color, marker, line, and fill style
   *              (grouped into a PlotHelper::Style::Plot struct)
   */
  std::vector<PlotterInput> Inputs() {

    std::vector<PlotterInput> inputs = {
      {
       .file   = input_files[0],
       .object = "hEneLD_Ene2",
       .rename = "hEneLD_Ene2",
       .legend = "E_{par} = 2 GeV/c",
       .style  = PH::Style::Plot(809, 21, 0)
      },
      {
       .file   = input_files[0],
       .object = "hEneLD_Ene5",
       .rename = "hEneLD_Ene5",
       .legend = "E_{par} = 5 GeV/c",
       .style  = PH::Style::Plot(899, 22, 0)
      },
      {
       .file   = input_files[0],
       .object = "hEneLD_Ene7",
       .rename = "hEneLD_Ene7",
       .legend = "E_{par} = 7 GeV/c",
       .style  = PH::Style::Plot(879, 223, 0)
      },
      {
       .file   = input_files[0],
       .object = "hEneLD_Ene10",
       .rename = "hEneLD_Ene10",
       .legend = "E_{par} = 10GeV/c",
       .style  = PH::Style::Plot(859, 20, 0)
      },
    };
    return inputs;

  }  // end 'Inputs()'



  // ==========================================================================
  //! Define plot range
  // ==========================================================================
  /*! Plot range arguments:
   *    first  = x range to plot
   *    second = y range to plot
   */ 
  PH::PlotRange PlotRange() {

    PH::PlotRange range = PH::PlotRange(
      {0., 30.},
      {0.00003, 0.7}
    );
    return range;

  }  // end 'PlotRange()'



  // ==========================================================================
  //! Define canvas
  // ==========================================================================
  PH::Canvas Canvas() {

    // grab default pad options, and
    // turn on log y
    PH::PadOpts opts = PH::PadOpts();
    opts.logy = 1;

    // define canvas (use default pad options)
    PH::Canvas canvas = PH::Canvas("cEnergies", "", {950, 950}, opts);
    canvas.SetMargins( {0.02, 0.02, 0.15, 0.15} );
    return canvas;

  }  // end 'Canvas()'



  // ==========================================================================
  //! Define legend header
  // ==========================================================================
  /*! Note that the header is optional parameter that
   *  can be provided as the last argument of
   *  `BHCalPlotter::DoEnergySpectra`.
   */
  std::optional<std::string> Header() {

    std::optional<std::string> header;
    header = "#bf{LD Model}";
    return header;

  }  // end 'Header()'

}  // end EnergySepctra namespace

#endif

// end ========================================================================
