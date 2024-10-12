/// ===========================================================================
/*! \file   PlotHelper.hxx
 *  \author Derek Anderson
 *  \date   10.11.2024
 *
 *  A lightweight namespace to help plot things in ROOT.
 */
/// ===========================================================================

#ifndef PlotHelper_hxx
#define PlotHelper_hxx

// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>



// ============================================================================
//! Plot Helper
// ============================================================================
/*! A small namespace to help plot things
 *  in ROOT.
 */
namespace PlotHelper {

  // ==========================================================================
  //! Style definition
  // ==========================================================================
  /*! This class consolidates all the various
   *  options that define the style (e.g. 
   *  marker color & style) of thigns like
   *  histograms, graphs, etc.
   */
  class Style { 

    public:

      // =======================================================================
      //! Plot Style
      // =======================================================================
      /*! A small struct to consolidate options
       *  for color, marker, lines, and fill styles.
       */ 
      struct Plot {

        // members
        uint32_t color;
        uint32_t marker;
        uint32_t fill;
        uint32_t line;
        uint32_t width;

        // ----------------------------------------------------------------------
        //! default ctor/dtor
        // ----------------------------------------------------------------------
        PlotStyle()  {};
        ~PlotStyle() {};

        // ---------------------------------------------------------------------
        //! ctor accepting arguments
        // ---------------------------------------------------------------------
        Plot(
          const uint32_t col_arg,
          const uint32_t mar_arg,
          const uint32_t fil_arg,
          const uint32_t lin_arg = 1,
          const uint32_t lin_wid = 1
        ) {
          color  = col_arg;
          marker = mar_arg;
          fill   = fil_arg;
          line   = lin_arg;
          width  = lin_wid;
        }  // end ctor (uint32_t x 5)

      };  // end Plot

      // =======================================================================
      //! Text style
      // =======================================================================
      /*! A small struct to consolidate options
       *  for various text attributes.
       */ 
      struct Text {

        // members
        uint32_t color;
        uint32_t font;
        uint32_t align;
        float    spacing;

        // ---------------------------------------------------------------------
        //! default ctor/dtor
        // ---------------------------------------------------------------------
        Text()  {};
        ~Text() {};

        // ---------------------------------------------------------------------
        //! ctor accepting arguments
        // ---------------------------------------------------------------------
        Text(
          const uint32_t col_arg,
          const uint32_t fon_arg,
          const uint32_t aln_arg,
          const float    spa_arg = 0.05
        ) {
          color   = col_arg;
          font    = fon_arg;
          align   = aln_arg;
          spacing = spa_arg;
        }  // end ctor(uint32_t x 3, float)

      };  // end Text

      // =======================================================================
      //! Label style
      // =======================================================================
      /*! A small struct to consolidate options
       *  for histogram, graph axis labels.
       */ 
      Label {

        // members
        uint32_t color;
        uint32_t font;
        float    size;
        float    offset;

        // --------------------------------------------------------------------
        //! default ctor/dtor
        // --------------------------------------------------------------------
        Label()  {};
        ~Label() {};

        // --------------------------------------------------------------------
        //! ctor accepting arguments
        // --------------------------------------------------------------------
        Label(
          const uint32_t col_arg,
          const uint32_t fon_arg,
          const float    siz_arg = 0.04,
          const float    off_arg = 0.005
        ) {
          color  = col_arg;
          font   = fon_arg;
          size   = siz_arg;
          offset = off_arg;
        }  // end ctor(uint32_t x 2, float x 2)

      };  // end LabelStyle

      // ======================================================================
      //! Title style
      // ======================================================================
      /*! A small struct to consolidate options
       *  for options for histogram, graph, etc.
       *  titles. 
       */
      Title {

        // members
        uint32_t color;
        uint32_t center;
        uint32_t font;
        float    size;
        float    offset;

        // --------------------------------------------------------------------
        //! defualt ctor/dtor
        // --------------------------------------------------------------------
        Title()  {};
        ~Title() {};

        // --------------------------------------------------------------------
        //! ctor accepting arguments
        // --------------------------------------------------------------------
        Title(
          const uint32_t col_arg,
          const uint32_t cen_arg,
          const uint32_t fon_arg,
          const float    siz_arg = 0.04,
          const float    off_arg = 1.0
        ) {
          color  = col_arg;
          center = cen_arg;
          font   = fon_arg;
          size   = siz_arg;
          offset = off_arg;
        }  // end ctor(uint32_t x 3, float x 3)

      };  // end Title

    private:

      /* TODO fill in */

  };  // end Style



  // ==========================================================================
  //! Text box definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define a TPaveText
   */
  class TextBox {

    private:

      /* TODO fill in */

    public:

      /* TODO fill in */

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      TextBox()  {};
      ~TextBox() {};

  };  // end TextBox



  // ==========================================================================
  //! Legend definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define a TLegend.
   */ 
  class Legend {

    private:

      /* TODO fill in */

    public:

      /* TODO fill in */

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Legend()  {};
      ~Legend() {};

  };  // end Legend


  // ==========================================================================
  //! Canvas definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define and work with a TCanvas.
   */ 
  class Canvas {

    private:

      /* TODO fill in */

    public:

      /* TODO fill in */

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Canvas()  {};
      ~Canvas() {};

  };  // end Canvas

}  // end PlotHelper namespace

#endif

// end ========================================================================
