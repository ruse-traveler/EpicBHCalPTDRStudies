/// ===========================================================================
/*! \file   BHCalPlotter.cxx
 *  \author Derek Anderson
 *  \date   10.31.2024
 *
 *  A class to consolidate plotting routines for
 *  the BHCal chapter of the ePIC pTDR.
 */
/// ===========================================================================

#define BHCalPlotter_cxx

// class definition
#include "BHCalPlotter.hxx"



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! default ctor
// ----------------------------------------------------------------------------
BHCalPlotter::BHCalPlotter() {

  //... nothing to do ...//

}  // end ctor()



// ----------------------------------------------------------------------------
//! default dtor
// ----------------------------------------------------------------------------
BHCalPlotter::~BHCalPlotter() {

  //... nothing to do ...//

}  // end dtor()



// ----------------------------------------------------------------------------
//! ctor accepting arguments
// ----------------------------------------------------------------------------
BHCalPlotter::BHCalPlotter(
  const PH::Style& plot,
  const PH::Style& text,
  std::optional<PH::TextBox> box = std::nullopt
) {

  m_basePlotStyle = plot;
  m_baseTextStyle = text;
  if (box.has_value()) {
    m_textBox = box.value();
  }

}  // end ctor(PH::Style&, PH::Style&, std::optional<PH::TextBox>)'



// public methods =============================================================

// ----------------------------------------------------------------------------
//! Compare resolutions or linearities
// ----------------------------------------------------------------------------
/*! Compares a variety of resolutions or linearities from different
 *  sources.
 *
 *  \param[in]  inputs    list of objects to plot and their details
 *  \param[in]  plotrange (x, y) ranges to plot
 *  \param[in]  candef    definition of the canvas to draw on
 *  \param[in]  framedef  definition of the frame histogram used in plotting  
 *  \param[out] ofile     file to write to
 *
 *  TODO should add an optional parameter to provide a function or TLine
 *  to be plotted as well
 */
void BHCalPlotter::DoResolutionLinearityComparison(
  const std::vector<PlotterInput>& inputs,
  const PH::PlotRange& plotrange,
  const PH::Canvas& candef,
  const HH::Definition& framedef,
  TFile* ofile
) {

  // announce start
  std::cout << "\n -------------------------------- \n"
            << "  Beginning resolution/linearity comparison!\n"
            << "    Opening inputs:"
            << std::endl;

  // open inputs
  std::vector<TFile*>  ifiles;
  std::vector<TGraph*> igraphs;
  for (const PlotterInput& input : inputs) {

    ifiles.push_back(
      OpenFile(input.file, "read")
    );
    igraphs.push_back(
      (TGraph*) GrabObject( input.object, ifiles.back() )
    );
    igraphs.back() -> SetName( input.rename.data() );
    std::cout << "      File  = " << input.file << "\n"
              << "      Graph = " << input.object
              << std::endl;

  }  // end input loop

  // legend dimensions
  const float legheight = PH::GetHeight(
    igraphs.size(),
    m_baseTextStyle.GetTextStyle().spacing
  );
  const PH::Vertices vtxleg = {0.3, 0.1, 0.5, (float) 0.1 + legheight};

  // define legend
  PH::Legend legdef;
  for (std::size_t igr = 0; igr < igraphs.size(); ++igr) {
    legdef.AddEntry( PH::Legend::Entry(igraphs[igr], inputs[igr].legend, "p") );
  }
  legdef.SetVertices( vtxleg );

  // create root objects
  TH2*       frame  = framedef.MakeTH2();
  TLegend*   legend = legdef.MakeLegend();
  TPaveText* text   = m_textBox.MakeTPaveText();
  std::cout << "    Created frame histogram, legend, and text box." << std::endl;

  // set graph styles
  std::vector<PH::Style> styles = GenerateStyles( inputs );
  for (std::size_t igr = 0; igr < igraphs.size(); ++igr) {
    styles[igr].SetPlotStyle( inputs[igr].style );
    styles[igr].Apply( igraphs[igr] );
    igraphs[igr] -> GetXaxis() -> SetRangeUser( plotrange.x.first, plotrange.x.second );
    igraphs[igr] -> GetYaxis() -> SetRangeUser( plotrange.y.first, plotrange.y.second );
  }

  // set frame sytle
  m_basePlotStyle.Apply( frame );
  frame -> GetXaxis() -> SetRangeUser( plotrange.x.first, plotrange.x.second );
  frame -> GetYaxis() -> SetRangeUser( plotrange.y.first, plotrange.y.second );

  // set legend/text styles
  m_baseTextStyle.Apply( legend );
  m_baseTextStyle.Apply( text );
  std::cout << "    Set styles." << std::endl;

  // draw plot
  PH::PlotManager manager = PH::PlotManager( candef );
  manager.MakePlot();
  manager.Draw();
  manager.GetTCanvas() -> cd();
  frame -> Draw();
  for (auto graph : igraphs) {
    graph -> Draw("LP");
  }
  legend -> Draw();
  text   -> Draw();
  std:: cout << "    Made plot." << std::endl;

  // save output
  ofile -> cd();
  frame -> Write();
  for (auto graph : igraphs) {
    graph -> Write();
  }
  manager.Write();
  manager.Close();
  std::cout << "    Saved output." << std::endl;

  // close input files
  for (TFile* ifile : ifiles) {
    ifile -> cd();
    ifile -> Close();
  }
  std::cout << "    Closed input files." << std::endl;

  // announce end
  std::cout << "  Finished resolution/linearity comparison!\n"
            << " -------------------------------- \n"
            << endl;

  // exit routine
  return;

}  // end 'DoResolutionLinearityComparison(std::vector<PlotterInput>&, PH::PlotRange&, PH::Canvas&, HH::Definition&, TFile*)'



// static methods =============================================================

// ----------------------------------------------------------------------------
//! Open file and check if good
// ----------------------------------------------------------------------------
TFile* BHCalPlotter::OpenFile(const std::string& name, const std::string& option) {

  // try to open file, throw error if not able
  TFile* file = new TFile( name.data(), option.data() );
  if (!file) {
    std::cerr << "PANIC: couldn't open file!\n"
              << "       file = " << name << "\n"
              << std::endl;
    assert(file);
  }

  // then check file by cd'ing into it
  const bool isGoodCD = file -> cd();
  if (!isGoodCD) {
    std::cerr << "PANIC: couldn't cd into file!\n"
              << "       file = " << name << "\n"
              << std::endl;
    assert(isGoodCD);
  }
  return file;

}  // end 'OpenFile(std::string&, std::string&)'



// ----------------------------------------------------------------------------
//! Grab an object from a file
// ----------------------------------------------------------------------------
TObject* BHCalPlotter::GrabObject(const std::string& object, TFile* file) {

   // try to grab object from file, throw error if not able
   TObject* grabbed = (TObject*) file -> Get( object.data() );
   if (!grabbed) {
     std::cerr << "PANIC: couldn't grab object!\n"
               << "       file   = " << file   << "\n"
               << "       object = " << object << "\n"
               << std::endl;
     assert(grabbed);
   }
   return grabbed;

}  // end 'GrabObject(std::string&, TFile*)'



// private methods ============================================================

// ----------------------------------------------------------------------------
//! Generate list of styles to be applied
// ---------------------------------------------------------------------------- 
Styles BHCalPlotter::GenerateStyles(const std::vector<PlotterInput>& inputs) {

  Styles styles( inputs.size(), m_basePlotStyle );
  for (std::size_t isty = 0; isty < inputs.size(); ++isty) {
    styles[isty].SetPlotStyle( inputs[isty].style );
  }
  return styles;

}  // end 'GenerateStyles(std::size_t)'

// end ========================================================================
