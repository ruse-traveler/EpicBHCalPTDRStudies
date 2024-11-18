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
#include <array>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <algorithm>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TPad.h>
#include <TLine.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TObject.h>
#include <TPaveText.h>
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
  //! Convenient types
  // ==========================================================================
  typedef std::array<float, 4>     Vertices;
  typedef std::vector<TObject*>    Objects;
  typedef std::vector<std::string> TextList;



  // ==========================================================================
  //! enum for different axes
  // ==========================================================================
  enum Axis {X, Y, Z};



  // ==========================================================================
  //! Calculate a height based on line spacing
  // ==========================================================================
  /*! Helper function to calculate how high a text box/legend
   *  should be based on line spacing and no. of lines.
   */
  float GetHeight(const std::size_t nlines, const float spacing, std::optional<float> off) {

    float height = nlines * spacing;
    if (off.has_value()) {
      height += off.value();
    }
    return height;

  }  // end 'GetHeight(std::size_t float, std::optional<float>)'



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
      struct Label {

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
      struct Title {

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
      //! Apply styles to a histogram
      // ----------------------------------------------------------------------
      template <typename THN> void ApplyStyle(THN* hist) const {

        hist -> SetFillColor( m_plot.color );
        hist -> SetFillStyle( m_plot.fill );
        hist -> SetLineColor( m_plot.color );
        hist -> SetLineStyle( m_plot.line );
        hist -> SetLineWdith( m_plot.width );
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

      }  // end 'ApplyStyle(THN*)'

      // ----------------------------------------------------------------------
      //! Apply styles to text box
      // ----------------------------------------------------------------------
      /*! n.b. this assumes the fill and border of the
       *  TPave will be the same color.
       */
      void ApplyStyle(TPaveText* text) const {

        text -> SetFillColor( m_plot.color );
	text -> SetFillStyle( m_plot.fill );
	text -> SetLineColor( m_plot.color );
	text -> SetLineStyle( m_plot.line );
        return;

      }  // end 'ApplyStyle(TPaveText*)'

      // ----------------------------------------------------------------------
      //! Apply styles to a legend
      // ----------------------------------------------------------------------
      /*! n.b. this assumes the fill and border of the
       *  TLegend will be the same color.
       */
      void ApplyStyle(TLegend* leg) const {

        leg -> SetFillColor( m_plot.color );
	leg -> SetFillStyle( m_plot.fill );
	leg -> SetLineColor( m_plot.color );
	leg -> SetLineStyle( m_plot.line );
        return;

      }  //  end 'ApplyStyle(TLegend*)'
      
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

      };

  };  // end Legend



  // ==========================================================================
  //! Pad definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define and work with a TPad 
   */  
  class Pad {

    private:

      /* TODO fill in */

    public:

      /* TODO fill in */

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Pad()  {};
      ~Pad() {};


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
      std::vector<Pad> m_pads;

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
