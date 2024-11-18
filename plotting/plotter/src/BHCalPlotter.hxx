/// ===========================================================================
/*! \file   BHCalPlotter.hxx
 *  \author Derek Anderson
 *  \date   10.31.2024
 *
 *  A class to consolidate plotting routines for
 *  the BHCal chapter of the ePIC pTDR.
 */
/// ===========================================================================

#ifndef BHCalPlotter_hxx
#define BHCalPlotter_hxx

// c++ utilities
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
#include <iostream>
// root libraries
#include <TFile.h>
#include <TObject.h>
// analysis utilities
#include "../PlotHelper.hxx"

// abbreviate common namespaces
namespace PH = PlotHelper;



// ============================================================================
//! BHCal plotter input
// ============================================================================
/*! A small struct to consolidate input for
 *  plotting routines
 */
struct PlotterInput {

  std::string     file;    // file where object is
  std::string     object;  // object to be plotted
  std::string     legend;  // legend entry
  PH::Style::Plot style;   // marker, line, and fill style

};  // end PlotterInput



// ============================================================================
//! BHCal pTDR plotting routines
// ============================================================================
/*! This class conslidates plotting routines
 *  for the BHCal chapter of the ePIC pTDR.
 */
class BHCalPlotter {

  public:

    // default ctor/dtor
    BHCalPlotter();
    ~BHCalPlotter();

    // ctor accepting arguments
    BHCalPlotter(const PH::Style& plot, const PH::Style& text, std::optional<PH::TextBox> box);

    // getters
    PH::Style   GetBasePlotStyle() const {return m_basePlotStyle;}
    PH::Style   GetBaseTextStyle() const {return m_baseTextStyle;}
    PH::TextBox GetTextBox()       const {return m_textBox;}

    // setters
    void SetBasePlotStyle(const PH::Style& style) {m_basePlotStyle = style;}
    void SetBaseTextStyle(const PH::Style& style) {m_baseTextStyle = style;}
    void SetTextBox(const PH::TextBox& text)      {m_textBox       = text;}

    // plotting routines
    void DoResolutionComparison(const std::vector<PlotterInput>& inputs, TFile* ofile);

    // static methods
    static TFile*   OpenFile(const std::string& name, const std::string &option);
    static TObject* GrabObject(const std::string& object, TFile* file);

  private:

    // members
    PH::Style   m_basePlotStyle;
    PH::Style   m_baseTextStyle;
    PH::TextBox m_textBox;

};  // end BHCalPlotter

#endif

// end ========================================================================
