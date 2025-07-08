/// ===========================================================================
/*! \file   BaseOptions.hxx
 *  \author Derek Anderson
 *  \date   11.19.2024
 *
 *  A small namespace to collect all of the
 *  general options for the `BHCalPlotter`
 *  class (i.e. plotting options constant
 *  across multiple plotting routines).
 */
/// ===========================================================================

#ifndef BaseOptions_hxx
#define BaseOptions_hxx

// c++ utilities
#include <string>
#include <vector>
// analysis utilities
#include <utility/PlotHelper.hxx>

// abbreviate common namespaces
namespace PH = PlotHelper;



// ============================================================================
//! Base/General Plotting Options
// ============================================================================
/*! This namespace collects all the generic parameters associated with
 *  the `BHCalPlotter` routines, i.e. anything common across multiple
 *  routines.
 */
namespace BaseOptions {

  // ==========================================================================
  //! Define base plotting style
  // ==========================================================================
  /*! This defines the various style components
   *  for plottable-things (e.g. histograms, graphs,
   *  etc.) to be used across multiple plotting
   *  routines.
   */
  PH::Style BasePlotStyle() {

    // define text, axis label styles
    PH::Style::Text  text  = PH::Style::Text(1, 42);
    PH::Style::Label label = PH::Style::Label(1, 42, 0.03);

    // define axis title styles
    //   [0] = x
    //   [1] = y
    //   [2] = z
    std::vector<PH::Style::Title> titles = {
      {1, 1, 42, 0.04, 1.0},
      {1, 1, 42, 0.04, 1.2},
      {1, 1, 42, 0.04, 1.2}
    };

    // collect style components
    PH::Style style;
    style.SetTextStyle( text );
    style.SetLabelStyles( label );
    style.SetTitleStyles( titles );
    return style;

  }  // end 'BasePlotStyle()'



  // ==========================================================================
  //! Define base text style
  // ==========================================================================
  /*! This defines the various style components for
   *  text boxes (TPaveText, TLegend, etc.) to be
   *  used across multiple plotting routines.
   */ 
  PH::Style BaseTextStyle() {

    // define needed style components
    //   - note that the marker is unused for text boses
    PH::Style::Plot plot = PH::Style::Plot(0, 1, 0, 0, 1);
    PH::Style::Text text = PH::Style::Text(1, 42, 12, 0.05);

    // collect style components
    PH::Style style;
    style.SetPlotStyle( plot );
    style.SetTextStyle( text );
    return style;

  }  // end 'BaseTextStyle()'



  // ==========================================================================
  //! Define text box
  // ==========================================================================
  /*! This method defines the text box (a TPaveText)
   *  which holds information (e.g. about which
   *  simulation was used for the plots).
   */ 
  PH::TextBox Text() {

    // text to display
    PH::TextList lines = {
      "#bf{ePIC} simulation [24.10.0]",
      "single #pi^{-}, E = 2 - 10 GeV",
      "#bf{full detector}"
    };

    // grab base text style for line spacing
    const PH::Style& style = BaseTextStyle();

    // set box dimenisons
    const float height = PH::GetHeight(
      lines.size(),
      style.GetTextStyle().spacing
    );
    const PH::Vertices vertices = {0.1, 0.1, 0.3, (float) 0.1 + height};

    // return text box definition
    return PH::TextBox(lines, vertices);

  }  // end 'Text()'

}  // end BaseOptions namespace

#endif

// end ========================================================================
