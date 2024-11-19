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

// c++ utilities
#include <map>
#include <array>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <iostream>
#include <algorithm>
// root libraries
#include <TF1.h>
#include <TF2.h>
#include <TF3.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TPad.h>
#include <TLine.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TObject.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TGraph2DErrors.h>
#include <TGraphAsymmErrors.h>



// ============================================================================
//! Plot Helper
// ============================================================================
/*! A small namespace to help plot things
 *  in ROOT.
 */
namespace PlotHelper {

  // ==========================================================================
  //! Convenient types
  // ==========================================================================
  typedef std::array<float, 4>               Vertices;
  typedef std::array<float, 4>               Margins;
  typedef std::vector<TObject*>              Objects;
  typedef std::vector<std::string>           TextList;
  typedef std::vector<std::string>           LabelList;
  typedef std::pair<float, float>            Range;
  typedef std::pair<uint32_t, uint32_t>      Dimensions;
  typedef std::map<std::string, std::size_t> LabelToIndexMap;



  // ==========================================================================
  //! enum for different axes
  // ==========================================================================
  enum Axis {X, Y, Z};

  // ==========================================================================
  //! enum for different margins
  // ==========================================================================
  enum Margin {Top, Right, Bottom, Left};



  // ==========================================================================
  //! Calculate a height based on line spacing
  // ==========================================================================
  /*! Helper function to calculate how high a text box/legend
   *  should be based on line spacing and no. of lines.
   */
  float GetHeight(
    const std::size_t nlines,
    const float spacing,
    std::optional<float> off = std::nullopt
  ) {

    float height = nlines * spacing;
    if (off.has_value()) {
      height += off.value();
    }
    return height;

  }  // end 'GetHeight(std::size_t float, std::optional<float>)'



  // ==========================================================================
  //! Plot range
  // ==========================================================================
  /*! This struct groups together the start/stop
   *  points for all 3 axes for convenience.
   */
  struct PlotRange {

    // members
    Range x;
    Range y;
    Range z;

    // ------------------------------------------------------------------------
    //! default ctor/dtor
    // ------------------------------------------------------------------------
    PlotRange()  {};
    ~PlotRange() {};

    // ------------------------------------------------------------------------
    //! ctor accepting only x range
    // ------------------------------------------------------------------------
    PlotRange(const Range& xrange) {
      x = xrange;
    }

    // ------------------------------------------------------------------------
    //! ctor accepting x, y, and possibly z range
    // ------------------------------------------------------------------------
    PlotRange(
      const Range& xrange,
      const Range& yrange,
      std::optional<Range> zrange = std::nullopt
    ) {
      x = xrange;
      y = yrange;
      if (zrange.has_value()) {
        z = zrange.value();
      }
    }

  };  // end PlotRange



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
        uint32_t color  = 1;
        uint32_t marker = 1;
        uint32_t fill   = 0;
        uint32_t line   = 1;
        uint32_t width  = 1;

        // ----------------------------------------------------------------------
        //! default ctor/dtor
        // ----------------------------------------------------------------------
        Plot()  {};
        ~Plot() {};

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
        uint32_t color   = 1;
        uint32_t font    = 42;
        uint32_t align   = 12;
        float    spacing = 0.05;

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
          const uint32_t aln_arg = 12,
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
      struct Label {

        // members
        uint32_t color  = 1;
        uint32_t font   = 42;
        float    size   = 0.04;
        float    offset = 0.005;

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
      struct Title {

        // members
        uint32_t color  = 1;
        uint32_t center = 0;
        uint32_t font   = 42;
        float    size   = 0.04;
        float    offset = 1.0;

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

      // general members
      Plot m_plot;
      Text m_text;

      // axis dependent members
      std::array<Label, 3> m_labels;
      std::array<Title, 3> m_titles;

    public:

      // ----------------------------------------------------------------------
      //! General getters
      // ----------------------------------------------------------------------
      Plot GetPlotStyle() const {return m_plot;}
      Text GetTextStyle() const {return m_text;}

      // ----------------------------------------------------------------------
      //! General setters
      // ----------------------------------------------------------------------
      void SetPlotStyle(const Plot& plot) {m_plot = plot;}
      void SetTextStyle(const Text& text) {m_text = text;}

      // ----------------------------------------------------------------------
      //! Axis dependent getters
      // ----------------------------------------------------------------------
      Label GetLabelStyle(const Axis& axis) const {return m_labels[axis];}
      Title GetTitleStyle(const Axis& axis) const {return m_titles[axis];}

      // ----------------------------------------------------------------------
      //! Get all label styles
      // ----------------------------------------------------------------------
      std::vector<Label> GetLabelStyles() const {

        std::vector<Label> labels;
        for (const auto& label : m_labels) {
          labels.push_back( label );
        }
        return labels;

      }  // end 'GetLabelStyles()'

      // ----------------------------------------------------------------------
      //! Get all title styles
      // ----------------------------------------------------------------------
      std::vector<Title> GetTitleStyles() const {

        std::vector<Title> titles;
        for (const auto& title : m_titles) {
          titles.push_back( title );
        }
        return titles;

      }  // end 'GetTitleStyles()'

      // ----------------------------------------------------------------------
      //! Set a specific label style
      // ----------------------------------------------------------------------
      void SetLabelStyle(const Label& label, const Axis& axis) {

        m_labels[axis] = label;
        return;

      }  // end 'SetLabelStyle(Label&, Axis&)'

      // ----------------------------------------------------------------------
      //! Set a specific title style
      // ----------------------------------------------------------------------
      void SetTitleStyle(const Title& title, const Axis& axis) {

        m_titles[axis] = title;
        return;

      }  // end 'SetTitleStyle(Title&, Axis&)'

      // ----------------------------------------------------------------------
      //! Set all axis labels to same style
      // ----------------------------------------------------------------------
      void SetLabelStyles(const Label& label) {

        std::fill(m_labels.begin(), m_labels.end(), label);
        return;

      }  // end 'SetLabelStyles(Label&)'

      // ----------------------------------------------------------------------
      //! Set all axis titles to same style
      // ----------------------------------------------------------------------
      void SetTitleStyles(const Title& title) {

        std::fill(m_titles.begin(), m_titles.end(), title);
        return;

      }  // end 'SetTitleStyle(Title&)'

      // ----------------------------------------------------------------------
      //! Set all label styles
      // ----------------------------------------------------------------------
      void SetLabelStyles(const std::vector<Label>& labels) {

        if (labels.size() >= 1) m_labels[Axis::X] = labels.at(0);
        if (labels.size() >= 2) m_labels[Axis::Y] = labels.at(1);
        if (labels.size() >= 3) m_labels[Axis::Z] = labels.at(2);
        return;

      }  // end 'SetLabelStyles(std::vector<Label>&)'

      // ----------------------------------------------------------------------
      //! Set all title styles
      // ----------------------------------------------------------------------
      void SetTitleStyles(const std::vector<Title>& titles) {

        if (titles.size() >= 1) m_titles[Axis::X] = titles.at(0);
        if (titles.size() >= 2) m_titles[Axis::Y] = titles.at(1);
        if (titles.size() >= 3) m_titles[Axis::Z] = titles.at(2);
        return;

      }  // end 'SetTitleStyles(std::vector<Title>&)'

      // ----------------------------------------------------------------------
      //! Apply styles to a function
      // ----------------------------------------------------------------------
      /*! Note that this method is valid for a TF1, TF2, or TF3.
       */ 
      void Apply(TF1* func) const {

        func -> SetFillColor( m_plot.color );
        func -> SetFillStyle( m_plot.fill );
        func -> SetLineColor( m_plot.color );
        func -> SetLineStyle( m_plot.line );
        func -> SetLineWidth( m_plot.width );
        func -> SetMarkerColor( m_plot.color );
        func -> SetMarkerStyle( m_plot.marker );
        func -> GetXaxis() -> CenterTitle( m_titles[Axis::X].center );
        func -> GetXaxis() -> SetTitleFont( m_titles[Axis::X].font );
        func -> GetXaxis() -> SetTitleSize( m_titles[Axis::X].size );
        func -> GetXaxis() -> SetTitleOffset( m_titles[Axis::X].offset );
        func -> GetXaxis() -> SetLabelFont( m_labels[Axis::X].font );
        func -> GetXaxis() -> SetLabelSize( m_labels[Axis::X].size );
        func -> GetXaxis() -> SetLabelOffset( m_labels[Axis::X].offset );
        func -> GetYaxis() -> CenterTitle( m_titles[Axis::Y].center );
        func -> GetYaxis() -> SetTitleFont( m_titles[Axis::Y].font );
        func -> GetYaxis() -> SetTitleSize( m_titles[Axis::Y].size );
        func -> GetYaxis() -> SetTitleOffset( m_titles[Axis::Y].offset );
        func -> GetYaxis() -> SetLabelFont( m_labels[Axis::Y].font );
        func -> GetYaxis() -> SetLabelSize( m_labels[Axis::Y].size );
        func -> GetYaxis() -> SetLabelOffset( m_labels[Axis::Y].offset );
        func -> GetZaxis() -> CenterTitle( m_titles[Axis::Z].center );
        func -> GetZaxis() -> SetTitleFont( m_titles[Axis::Z].font );
        func -> GetZaxis() -> SetTitleSize( m_titles[Axis::Z].size );
        func -> GetZaxis() -> SetTitleOffset( m_titles[Axis::Z].offset );
        func -> GetZaxis() -> SetLabelFont( m_labels[Axis::Z].font );
        func -> GetZaxis() -> SetLabelSize( m_labels[Axis::Z].size );
        func -> GetZaxis() -> SetLabelOffset( m_labels[Axis::Z].offset );
        return;

      }  // end 'Apply(TFN*)'

      // ----------------------------------------------------------------------
      //! Apply styles to a histogram
      // ----------------------------------------------------------------------
      /*! Note that this method is valid for a TH1, TH2, or TH3.
       */ 
      void Apply(TH1* hist) const {

        hist -> SetFillColor( m_plot.color );
        hist -> SetFillStyle( m_plot.fill );
        hist -> SetLineColor( m_plot.color );
        hist -> SetLineStyle( m_plot.line );
        hist -> SetLineWidth( m_plot.width );
        hist -> SetMarkerColor( m_plot.color );
        hist -> SetMarkerStyle( m_plot.marker );
        hist -> SetTitleFont( m_text.font );
        hist -> GetXaxis() -> CenterTitle( m_titles[Axis::X].center );
        hist -> GetXaxis() -> SetTitleFont( m_titles[Axis::X].font );
        hist -> GetXaxis() -> SetTitleSize( m_titles[Axis::X].size );
        hist -> GetXaxis() -> SetTitleOffset( m_titles[Axis::X].offset );
        hist -> GetXaxis() -> SetLabelFont( m_labels[Axis::X].font );
        hist -> GetXaxis() -> SetLabelSize( m_labels[Axis::X].size );
        hist -> GetXaxis() -> SetLabelOffset( m_labels[Axis::X].offset );
        hist -> GetYaxis() -> CenterTitle( m_titles[Axis::Y].center );
        hist -> GetYaxis() -> SetTitleFont( m_titles[Axis::Y].font );
        hist -> GetYaxis() -> SetTitleSize( m_titles[Axis::Y].size );
        hist -> GetYaxis() -> SetTitleOffset( m_titles[Axis::Y].offset );
        hist -> GetYaxis() -> SetLabelFont( m_labels[Axis::Y].font );
        hist -> GetYaxis() -> SetLabelSize( m_labels[Axis::Y].size );
        hist -> GetYaxis() -> SetLabelOffset( m_labels[Axis::Y].offset );
        hist -> GetZaxis() -> CenterTitle( m_titles[Axis::Z].center );
        hist -> GetZaxis() -> SetTitleFont( m_titles[Axis::Z].font );
        hist -> GetZaxis() -> SetTitleSize( m_titles[Axis::Z].size );
        hist -> GetZaxis() -> SetTitleOffset( m_titles[Axis::Z].offset );
        hist -> GetZaxis() -> SetLabelFont( m_labels[Axis::Z].font );
        hist -> GetZaxis() -> SetLabelSize( m_labels[Axis::Z].size );
        hist -> GetZaxis() -> SetLabelOffset( m_labels[Axis::Z].offset );
        return;

      }  // end 'Apply(TH1*)'

      // ----------------------------------------------------------------------
      //! Apply styles to a 1D graph
      // ----------------------------------------------------------------------
      /*! Note that this method is valid for TGraph, TGraphErrors, and
       *  TGraphAsymmErrors.
       */ 
      void Apply(TGraph* graph) const {

        graph -> SetFillColor( m_plot.color );
        graph -> SetFillStyle( m_plot.fill );
        graph -> SetLineColor( m_plot.color );
        graph -> SetLineStyle( m_plot.line );
        graph -> SetLineWidth( m_plot.width );
        graph -> SetMarkerColor( m_plot.color );
        graph -> SetMarkerStyle( m_plot.marker );
        graph -> GetXaxis() -> CenterTitle( m_titles[Axis::X].center );
        graph -> GetXaxis() -> SetTitleFont( m_titles[Axis::X].font );
        graph -> GetXaxis() -> SetTitleSize( m_titles[Axis::X].size );
        graph -> GetXaxis() -> SetTitleOffset( m_titles[Axis::X].offset );
        graph -> GetXaxis() -> SetLabelFont( m_labels[Axis::X].font );
        graph -> GetXaxis() -> SetLabelSize( m_labels[Axis::X].size );
        graph -> GetXaxis() -> SetLabelOffset( m_labels[Axis::X].offset );
        graph -> GetYaxis() -> CenterTitle( m_titles[Axis::Y].center );
        graph -> GetYaxis() -> SetTitleFont( m_titles[Axis::Y].font );
        graph -> GetYaxis() -> SetTitleSize( m_titles[Axis::Y].size );
        graph -> GetYaxis() -> SetTitleOffset( m_titles[Axis::Y].offset );
        graph -> GetYaxis() -> SetLabelFont( m_labels[Axis::Y].font );
        graph -> GetYaxis() -> SetLabelSize( m_labels[Axis::Y].size );
        graph -> GetYaxis() -> SetLabelOffset( m_labels[Axis::Y].offset );
        return;

      }  // end 'Apply(TGraph*)'

      // ----------------------------------------------------------------------
      //! Apply styles to a 2D graph
      // ----------------------------------------------------------------------
      /*! Note that this method is valid for TGraph2D and TGraph2DErrors.
       */ 
      void Apply(TGraph2D* graph) const {

        graph -> SetFillColor( m_plot.color );
        graph -> SetFillStyle( m_plot.fill );
        graph -> SetLineColor( m_plot.color );
        graph -> SetLineStyle( m_plot.line );
        graph -> SetLineWidth( m_plot.width );
        graph -> SetMarkerColor( m_plot.color );
        graph -> SetMarkerStyle( m_plot.marker );
        graph -> GetXaxis() -> CenterTitle( m_titles[Axis::X].center );
        graph -> GetXaxis() -> SetTitleFont( m_titles[Axis::X].font );
        graph -> GetXaxis() -> SetTitleSize( m_titles[Axis::X].size );
        graph -> GetXaxis() -> SetTitleOffset( m_titles[Axis::X].offset );
        graph -> GetXaxis() -> SetLabelFont( m_labels[Axis::X].font );
        graph -> GetXaxis() -> SetLabelSize( m_labels[Axis::X].size );
        graph -> GetXaxis() -> SetLabelOffset( m_labels[Axis::X].offset );
        graph -> GetYaxis() -> CenterTitle( m_titles[Axis::Y].center );
        graph -> GetYaxis() -> SetTitleFont( m_titles[Axis::Y].font );
        graph -> GetYaxis() -> SetTitleSize( m_titles[Axis::Y].size );
        graph -> GetYaxis() -> SetTitleOffset( m_titles[Axis::Y].offset );
        graph -> GetYaxis() -> SetLabelFont( m_labels[Axis::Y].font );
        graph -> GetYaxis() -> SetLabelSize( m_labels[Axis::Y].size );
        graph -> GetYaxis() -> SetLabelOffset( m_labels[Axis::Y].offset );
        graph -> GetZaxis() -> CenterTitle( m_titles[Axis::Z].center );
        graph -> GetZaxis() -> SetTitleFont( m_titles[Axis::Z].font );
        graph -> GetZaxis() -> SetTitleSize( m_titles[Axis::Z].size );
        graph -> GetZaxis() -> SetTitleOffset( m_titles[Axis::Z].offset );
        graph -> GetZaxis() -> SetLabelFont( m_labels[Axis::Z].font );
        graph -> GetZaxis() -> SetLabelSize( m_labels[Axis::Z].size );
        graph -> GetZaxis() -> SetLabelOffset( m_labels[Axis::Z].offset );
        return;

      }  // end 'Apply(TGraph2D*)'

      // ----------------------------------------------------------------------
      //! Apply styles to text box
      // ----------------------------------------------------------------------
      /*! n.b. this assumes the fill and border of the
       *  TPave will be the same color.
       */
      void Apply(TPaveText* text) const {

        text -> SetFillColor( m_plot.color );
	text -> SetFillStyle( m_plot.fill );
	text -> SetLineColor( m_plot.color );
	text -> SetLineStyle( m_plot.line );
        text -> SetTextColor( m_text.color );
        text -> SetTextFont( m_text.font );
        text -> SetTextAlign( m_text.align );
        return;

      }  // end 'Apply(TPaveText*)'

      // ----------------------------------------------------------------------
      //! Apply styles to a legend
      // ----------------------------------------------------------------------
      /*! n.b. this assumes the fill and border of the
       *  TLegend will be the same color.
       */
      void Apply(TLegend* leg) const {

        leg -> SetFillColor( m_plot.color );
	leg -> SetFillStyle( m_plot.fill );
	leg -> SetLineColor( m_plot.color );
	leg -> SetLineStyle( m_plot.line );
        leg -> SetTextColor( m_text.color );
        leg -> SetTextFont( m_text.font );
        leg -> SetTextAlign( m_text.align );
        return;

      }  //  end 'Apply(TLegend*)'
      
      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Style()  {};
      ~Style() {};

      // ----------------------------------------------------------------------
      //! ctor accepting only plot style
      // ----------------------------------------------------------------------
      Style(const Plot& plot) {

        SetPlotStyle(plot);

      }  // end ctor(Plot&)

      // ----------------------------------------------------------------------
      //! ctor accepting styles other than plot (single values)
      // ----------------------------------------------------------------------
      Style(const Text& text, const Label& label, const Title& title) {

        SetTextStyle(text);
        SetLabelStyles(label);
        SetTitleStyles(title);

      }  // end ctor(Text&, Label&, Title&)'

      // ----------------------------------------------------------------------
      //! ctor accepting styles other than plot (axis-specific values)
      // ----------------------------------------------------------------------
      Style(
        const Text& text,
        const std::vector<Label>& labels,
        const std::vector<Title>& titles
      ) {

        SetTextStyle(text);
        SetLabelStyles(labels);
        SetTitleStyles(titles);

      }  // end ctor(Text&, std::vector<Label>&, std::vector<Title>&)'

      // ----------------------------------------------------------------------
      //! ctor accepting all styles (single values)
      // ----------------------------------------------------------------------
      Style(
        const Plot& plot,
        const Text& text,
        const Label& label,
        const Title& title
      ) {

        SetPlotStyle(plot);
        SetTextStyle(text);
        SetLabelStyles(label);
        SetTitleStyles(title);

      }  // ctor(Plot&, Text&, Label&, Title&)'

      // ----------------------------------------------------------------------
      //! ctor accepting all styles (axis-specific values)
      // ----------------------------------------------------------------------
      Style(
        const Plot& plot,
        const Text& text,
        const std::vector<Label>& labels,
        const std::vector<Title>& titles
      ) {

        SetPlotStyle(plot);
        SetTextStyle(text);
        SetLabelStyles(labels);
        SetTitleStyles(titles);

      }  // ctor(Plot&, Text&, std::vector<Label>&, std::vector<Title>&)'

  };  // end Style



  // ==========================================================================
  //! Text box definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define a TPaveText
   */
  class TextBox {

    private:

      // data members
      TextList    m_text;
      Vertices    m_vtxs;
      std::string m_opt = "NDC NB";

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      TextList    GetText()     const {return m_text;}
      Vertices    GetVertices() const {return m_vtxs;}
      std::string GetOption()   const {return m_opt;}

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetText(const TextList& text)     {m_text = text;}
      void SetVertices(const Vertices& vtxs) {m_vtxs = vtxs;}
      void SetOption(const std::string& opt) {m_opt  = opt;}

      // ----------------------------------------------------------------------
      //! Add a line of text
      // ----------------------------------------------------------------------
      void AddText(const std::string& line) {

        m_text.push_back( line );
	return;

      }  // end 'AddText(std::string&)'

      // ----------------------------------------------------------------------
      //! Create a TPaveText*
      // ----------------------------------------------------------------------
      TPaveText* MakeTPaveText() {

        TPaveText* pt = new TPaveText( m_vtxs[0], m_vtxs[1], m_vtxs[2], m_vtxs[3], m_opt.data() );
        for (const std::string& text : m_text) {
          pt -> AddText( text.data() );
        }
        return pt;

      }  // end 'MakeTPaveText()'
 
      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      TextBox()  {};
      ~TextBox() {};

      // ----------------------------------------------------------------------
      //! ctor accepting arguments
      // ----------------------------------------------------------------------
      TextBox(
        const TextList& text, 
	const Vertices& vtxs,
        const std::string& opt = "NDC NB"
      ) {

        m_text = text;
        m_vtxs = vtxs;
        m_opt  = opt;

      }  // end ctor(TextList&, Vertices&, std::string&)'

  };  // end TextBox




  // ==========================================================================
  //! Legend definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define a TLegend.
   */ 
  class Legend {

    public:

      // ======================================================================
      //! Legend entry
      // ======================================================================
      /*! A small struct to consolidate the data for an individual
       *  entry in a TLegend.
       */
      struct Entry {

        // members
        TObject*    object;
        std::string label;
        std::string option = "PF";

        // --------------------------------------------------------------------	
        // default ctor/dtor
	// --------------------------------------------------------------------
        Entry()  {};
        ~Entry() {};

	// --------------------------------------------------------------------
        // ctor accepting arguments
	// --------------------------------------------------------------------
        Entry(
          TObject* obj,
          const std::string& lbl,
	  const std::string& opt = "PF"
        ) {
          object = obj;
          label  = lbl;
          option = opt;
        }  // end ctor (TOject*, std::string x 2)

      };  // end Entry

    private:

      // data members
      Vertices           m_vtxs;
      std::string        m_header = "";
      std::vector<Entry> m_entries;

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      Vertices           GetVertices() const {return m_vtxs;}
      std::string        GetHeader()   const {return m_header;}
      std::vector<Entry> GetEntries()  const {return m_entries;}

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetVertices(const Vertices& vtxs)             {m_vtxs    = vtxs;}
      void SetHeader(const std::string& hdr)             {m_header  = hdr;}
      void SetEntries(const std::vector<Entry>& entries) {m_entries = entries;}

      // ----------------------------------------------------------------------
      //! Add an entry
      // ----------------------------------------------------------------------
      void AddEntry(const Entry& entry) {

        m_entries.emplace_back( entry );
        return;

      }  // end 'AddEntry(Entry&)'

      // ----------------------------------------------------------------------
      //! Create a TLegend
      // ----------------------------------------------------------------------
      TLegend* MakeLegend() {

        TLegend* leg = new TLegend( m_vtxs[0], m_vtxs[1], m_vtxs[2], m_vtxs[3], m_header.data() );
	for (const Entry& entry : m_entries) {
          leg -> AddEntry( entry.object, entry.label.data(), entry.option.data() );
        }
        return leg;

      }  // end 'MakeLegend()

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Legend()  {};
      ~Legend() {};

      // ----------------------------------------------------------------------
      //! ctor accepting arguments 
      // ----------------------------------------------------------------------
      Legend(
        const Vertices& vtxs,
        const std::vector<Entry>& entries,
        const std::string& hdr = ""
      ) {

        m_vtxs    = vtxs;
	m_header  = hdr;
        m_entries = entries;

      };  // end ctor(Vertices&, std::vector<Entry>&, std::string&)'

  };  // end Legend



  // ==========================================================================
  //! General options for pads/canvases
  // ==========================================================================
  /*! A small struct to consolidate options
   *  common to both TPads & TCanvases
   */  
  struct PadOpts {

    // members
    uint32_t logx  = 0;
    uint32_t logy  = 0;
    uint32_t tickx = 1;
    uint32_t ticky = 1;
    uint32_t gridx = 0;
    uint32_t gridy = 0;
    uint32_t bmode = 0;
    uint32_t bsize = 2;
    uint32_t frame = 0;

    // ------------------------------------------------------------------------
    //! Apply options to a TPad or TCanvas
    // ------------------------------------------------------------------------
    template <typename T> void Apply(T* pad) const {

      pad -> SetLogx(logx);
      pad -> SetLogy(logy);
      pad -> SetTicks(tickx, ticky);
      pad -> SetGrid(gridx, gridy);
      pad -> SetBorderMode(bmode);
      pad -> SetBorderSize(bsize);
      pad -> SetFrameBorderMode(frame);
      return;

    }  // end 'Apply(T*)'

    // ------------------------------------------------------------------------
    //! default ctor/dtor
    // ------------------------------------------------------------------------
    PadOpts()  {};
    ~PadOpts() {};

    // ------------------------------------------------------------------------
    //! ctor accepting only log values
    // ------------------------------------------------------------------------
    PadOpts(const std::pair<uint32_t, uint32_t> log) {

      logx = log.first;
      logy = log.second;

    }  // end ctor(std::pair<uint32_t, uint32_t>)'

    // ------------------------------------------------------------------------
    //! ctor accepting all arguments
    // ------------------------------------------------------------------------
    PadOpts(
      const std::pair<uint32_t, uint32_t> log,
      const std::pair<uint32_t, uint32_t> tick,
      const std::pair<uint32_t, uint32_t> grid,
      const uint32_t mode = 0,
      const uint32_t size = 2,
      const uint32_t framearg = 0
    ) {

      logx  = log.first;
      logy  = log.second;
      tickx = tick.first;
      ticky = tick.second;
      gridx = grid.first;
      gridy = grid.second;
      bmode = mode;
      bsize = size;
      frame = framearg;

    }  // end ctor(std::pair<uint32_t, uint32_t> x 3, uint32_t x 3)'

  };  // end Options



  // ==========================================================================
  //! Pad definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define and work with a TPad 
   */  
  class Pad {

    private:

      // members
      PadOpts     m_opts;
      Vertices    m_vtxs;
      Margins     m_mgns;
      std::string m_name;
      std::string m_title = "";

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      PadOpts     GetOptions()  const {return m_opts;}
      Vertices    GetVertices() const {return m_vtxs;}
      Margins     GetMargins()  const {return m_mgns;}
      std::string GetName()     const {return m_name;}
      std::string GetTitle()    const {return m_title;}

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetOptions(const PadOpts& opts)   {m_opts  = opts;}
      void SetVertices(const Vertices& vtxs) {m_vtxs  = vtxs;}
      void SetMargins(const Margins& mgns)   {m_mgns  = mgns;}
      void SetName(const std::string& name)  {m_name  = name;}
      void SetTitle(const std::string& ttl)  {m_title = ttl;}

      // ----------------------------------------------------------------------
      //! Create a TPad 
      // ----------------------------------------------------------------------
      TPad* MakeTPad() const {

        // create pad
        TPad* pad = new TPad(
          m_name.data(),
          m_title.data(),
          m_vtxs[0],
          m_vtxs[1],
          m_vtxs[2],
          m_vtxs[3]
        );

        // set margins
        pad -> SetTopMargin( m_mgns[Margin::Top] );
        pad -> SetRightMargin( m_mgns[Margin::Right] );
        pad -> SetBottomMargin( m_mgns[Margin::Bottom] );
        pad -> SetLeftMargin( m_mgns[Margin::Left] );

        // apply options and return pointer
	m_opts.Apply(pad);
        return pad;

      }  // end 'MakeTPad()'

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Pad()  {};
      ~Pad() {};

      // ----------------------------------------------------------------------
      //! ctor accepting all members
      // ----------------------------------------------------------------------
      Pad(
        const std::string& name,
        const std::string& title,
        const Vertices& vtxs,
        const Margins& mgns,
        const PadOpts& opts
      ) {

        m_name  = name;
        m_title = title;
        m_vtxs  = vtxs;
        m_mgns  = mgns;
        m_opts  = opts;

      }  // end ctor(std::string& x 2, Vertices&, Margins&, PadOpts&)'

  };  // end Pad



  // ==========================================================================
  //! Canvas definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define and work with a TCanvas.
   */ 
  class Canvas {

    private:

      // members
      PadOpts          m_opts;
      Margins          m_mgns;
      Dimensions       m_dims;
      std::string      m_name;
      std::string      m_title = "";
      std::vector<Pad> m_pads;

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      PadOpts          GetOptions()    const {return m_opts;}
      Margins          GetMargins()    const {return m_mgns;}
      Dimensions       GetDimensions() const {return m_dims;}
      std::string      GetName()       const {return m_name;}
      std::string      GetTitle()      const {return m_title;}
      std::vector<Pad> GetPads()       const {return m_pads;}

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetOptions(const PadOpts& opts)       {m_opts  = opts;}
      void SetMargins(const Margins& mgns)       {m_mgns  = mgns;}
      void SetDimensions(const Dimensions& dims) {m_dims  = dims;}
      void SetName(const std::string& name)      {m_name  = name;}
      void SetTitle(const std::string& ttl)      {m_title = ttl;}
      void SetPads(const std::vector<Pad>& pads) {m_pads  = pads;}

      // ----------------------------------------------------------------------
      //! Add an associated pad
      // ----------------------------------------------------------------------
      void AddPad(const Pad& pad) {

        m_pads.push_back( pad );
        return;

      }  // end 'AddPad(Pad&)'

      // ----------------------------------------------------------------------
      //! Get a apecific associated pad
      // ----------------------------------------------------------------------
      Pad GetPad(const std::size_t index) {

        return m_pads.at(index);

      }  // end 'GetPad(std::size_t)'

      // ----------------------------------------------------------------------
      //! Create a TCanvas
      // ----------------------------------------------------------------------
      TCanvas* MakeTCanvas() const {

        // create canvas
        TCanvas* canvas = new TCanvas(
          m_name.data(),
          m_title.data(),
          m_dims.first,
          m_dims.second
        );

        // set margins if needed
        if (m_pads.empty()) {
          canvas -> SetTopMargin( m_mgns[Margin::Top] );
          canvas -> SetRightMargin( m_mgns[Margin::Right] );
          canvas -> SetBottomMargin( m_mgns[Margin::Bottom] );
          canvas -> SetLeftMargin( m_mgns[Margin::Left] );
        }

        // apply options and return pointer
	m_opts.Apply(canvas);
        return canvas;

      }  // end 'MakeTCanvas()'

      // ----------------------------------------------------------------------
      //! Create associated TPads
      // ----------------------------------------------------------------------
      std::vector<TPad*> MakeTPads() {

        std::vector<TPad*> pads;
        for (const auto& pad : m_pads) {
          pads.push_back( pad.MakeTPad() ); 
        }
        return pads;

      }  // end 'MakeTPads()'

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Canvas()  {};
      ~Canvas() {};

      // ----------------------------------------------------------------------
      //! ctor accepting all members
      // ----------------------------------------------------------------------
      Canvas(
        const std::string& name,
        const std::string& title,
        const Dimensions& dims,
        const PadOpts& opts,
        std::optional<Margins> mgns = std::nullopt,
        std::optional<std::vector<Pad>> pads = std::nullopt
      ) {

        // set necessary arguments
        m_name  = name;
        m_title = title;
        m_dims  = dims;
        m_opts  = opts;

        // set optional arguments
        if (mgns.has_value()) m_mgns = mgns.value();
        if (pads.has_value()) m_pads = pads.value();

      }  // end ctor(std::string& x 2, Dimensions&, PadOpts&, std::optional x 2)'

  };  // end Canvas



  // ==========================================================================
  //! Plot (pad/canvas) manager
  // ==========================================================================
  /*! A small class to hold/organize the pointers
   *  to a TCanvas and all of its associated pads.
   */ 
  class PlotManager {

    private:

      // canvas definition
      Canvas m_define;

      // root members
      TCanvas*           m_canvas;
      std::vector<TPad*> m_pads;

      // pad-label-to-index map
      LabelList       m_labels;
      LabelToIndexMap m_labtoindex;

      // ----------------------------------------------------------------------
      //! Make a pad label
      // ----------------------------------------------------------------------
      std::string MakePadLabel(const std::size_t index) const {

        // by default use the index as a label
        std::string label = std::to_string(index);

        // but if corresponding label exists, use that
        if (index < m_labels.size()) {
          label = m_labels[index];
        }
        return label;

      }  // end 'MakePadLabel(std::size_t)'

      // ----------------------------------------------------------------------
      //! Make label-to-index map
      // ----------------------------------------------------------------------
      void MakeLabelToIndexMap() {

        // emit warning if size of label list and pad vector
        // are different
        if (m_labels.size() != m_define.GetPads().size()) {
          std::cerr << "WARNING: provided label list is NOT the same length as the list of pads to make!" << std::endl;
        }

        // create label-to-index map
        for (std::size_t ipad = 0; ipad < m_define.GetPads().size(); ++ipad) {
          const std::string label = MakePadLabel(ipad);
          m_labtoindex[label] = ipad;
        }
        return;

      }  // end 'MakeLabelToIndexMap()'

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      Canvas             GetDefinition() const {return m_define;}
      TCanvas*           GetTCanvas()    const {return m_canvas;}
      LabelList          GetPadLabels()  const {return m_labels;}
      std::vector<TPad*> GetTPads()      const {return m_pads;}

      // ----------------------------------------------------------------------
      //! Setters 
      // ----------------------------------------------------------------------
      void SetDefinition(const Canvas& define)   {m_define = define;}
      void SetPadLabels(const LabelList& labels) {m_labels = labels;}

      // ----------------------------------------------------------------------
      //! Make canvas and pads 
      // ----------------------------------------------------------------------
      void MakePlot() {

        // create canvas/pads
        m_canvas = m_define.MakeTCanvas();
        m_pads   = m_define.MakeTPads();

        // generate lable-index map & exit
        MakeLabelToIndexMap();
        return;

      }  // end 'MakePlot()' '

      // ----------------------------------------------------------------------
      //! Draw canvas and pads
      // ----------------------------------------------------------------------
      void Draw() {

        m_canvas -> Draw();
        for (TPad* pad : m_pads) {
          pad -> Draw();
        }
        return;

      }  // end 'Draw()'

      // ----------------------------------------------------------------------
      //! Write canvas
      // ----------------------------------------------------------------------
      void Write() {

        m_canvas -> Write();
        return;

      }  // end 'Write()'

      // ----------------------------------------------------------------------
      //! Close canvas
      // ----------------------------------------------------------------------
      void Close() {

        m_canvas -> Close();
        return;

      }  // end 'Close()'

      // ----------------------------------------------------------------------
      //! Get a specific pad via its label
      // ----------------------------------------------------------------------
      TPad* GetTPad(const std::string& label) const {

        return m_pads.at( m_labtoindex.at(label) );

      }  // end 'GetTPad(std::string&)'

      // ----------------------------------------------------------------------
      //! Get a specific pad via its index
      // ----------------------------------------------------------------------
      TPad* GetTPad(const std::size_t index) const {

        return m_pads.at(index);

      }  // end 'GetTPad(std::size_t)'

      // ----------------------------------------------------------------------
      //! default ctor/dtor 
      // ----------------------------------------------------------------------
      PlotManager()  {};
      ~PlotManager() {};

      // ----------------------------------------------------------------------
      //! ctor accepting a canvas definition and possibly a list of pad labels
      // ----------------------------------------------------------------------
      PlotManager(const Canvas& define, std::optional<LabelList> padlabels = std::nullopt) {

        m_define = define;
        if (padlabels.has_value()) {
          m_labels = padlabels.value();
        }

      }   // end ctor(Canvas&)' 

  };  // end PlotManager

}  // end PlotHelper namespace

#endif

// end ========================================================================
