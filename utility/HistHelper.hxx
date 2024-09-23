/// ===========================================================================
/*! \file   HistHelper.hxx
 *  \author Derek Anderson
 *  \date   09.21.2024
 *
 *  A lightweight namespace to help work with histograms.
 */
/// ===========================================================================

#ifndef HistHelper_hxx
#define HistHelper_hxx

// c++ utilities
#include <map>
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
#include <algorithm>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>



// ============================================================================
//! Hist Helper
// ============================================================================
/*! A small namespace to help work with
 *  ROOT histograms (and graphs).
 */
namespace HistHelper {

  // --------------------------------------------------------------------------
  //! Histogram axes
  // --------------------------------------------------------------------------
  /* FIXME might not be needed... */
  enum Axis {X, Y, Z};



  // ==========================================================================
  //! Binning definitions
  // ==========================================================================
  /* TODO put here */



  // ==========================================================================
  //! Histogram definition
  // ==========================================================================
  /*! A small class to consolidate necessary
   *  data to define a TH1, TH2, or TH3.
   */ 
  class Definition {

    private:

      // title members
      std::string m_title_hist;
      std::string m_title_hist_x;
      std::string m_title_hist_y;
      std::string m_title_hist_z;

      // bin members
      /* TODO put here */

    public:

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetHistTitle(const std::string& title) {hist_title = title;}

      // ----------------------------------------------------------------------
      //! Set axis titles via list
      // ----------------------------------------------------------------------
      void SetAxisTitles(const std::vector<std::string>& titles) {

        if (titles.size() >= 1) m_title_hist_x = titles[0];
        if (titles.size() >= 2) m_title_hist_y = titles[1];
        if (titles.size() >= 3) m_title_hist_z = titles[2];
        return;

      }  // end 'SetAxisTitles(std::vector<std::string>&)'

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Definition()   {};
       ~Definition() {};

      // ----------------------------------------------------------------------
      //! ctor accepting a various vectors
      // ----------------------------------------------------------------------
      void Definition(
        const std::string& hist_title,
        const std::vector<std::string>& axis_titles
      ) {

        SetHistTitle(hist_title);
        SetAxisTitles(axis_titles);

      }  // end 'ctor(std::string&, std::vector<std::string>&

  };  // end Definition

}  // end HistHelper namespace

#endif

// end ========================================================================
