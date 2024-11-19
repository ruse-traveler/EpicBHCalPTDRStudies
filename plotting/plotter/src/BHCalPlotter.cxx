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
//! Compare resolutions
// ----------------------------------------------------------------------------
/*! Compares a variety of resolutions from different
 *  sources.
 *
 *  \param[in]  inputs list of objects to plot and their details
 *  \param[out] ofile  file to write to
 */
void BHCalPlotter::DoResolutionComparison(
  const std::vector<PlotterInput>& inputs,
  TFile* ofile
) {

  // announce start
  std::cout << "\n -------------------------------- \n"
            << "  Beginning resolution comparison!\n"
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
      (TGraph*) GrabObject(input.object, ifiles.back())
    );
    std::cout << "      File  = " << input.file   << "\n"
              << "      Graph = " << input.object << "\n"
              << std::endl;

  }  // end input loop

  // set styles
  std::vector<PH::Style> styles = GenerateStyles( inputs );
  for (std::size_t igr = 0; igr < igraphs.size(); ++igr) {
    styles[igr].SetPlotStyle( inputs[igr].style );
    styles[igr].ApplyStyle( igraphs[igr] );
  }
  std::cout << "    Set graph styles." << std::endl;

  // legend dimensions
  const float leg_height = PH::GetHeight(
    igraphs.size(),
    m_baseTextStyle.GetTextStyle().spacing
  );
  const PH::Vertices vtx_leg = {0.1, 0.1, 0.3, (float) 0.1 + leg_height};

  // define legends
  PH::Legend leg_def;
  for (std::size_t igr = 0; igr < igraphs.size(); ++igr) {
    leg_def.AddEntry( PH::Legend::Entry(igraphs[igr], inputs[igr].legend, "p") );
  }
  leg_def.SetVertices( vtx_leg );

  /* TODO create frame histogram here */

  // create root objects
  TLegend*   legend = leg_def.MakeLegend();
  TPaveText* text   = m_textBox.MakeTPaveText();
  std::cout << "    Created legend and text box." << std::endl;

  // define canvas
  PH::Canvas can_def = PH::Canvas("cResolution", "", {950, 950}, PH::PadOpts());
  can_def.SetMargins( {0.02, 0.02, 0.15, 0.15} );

  // draw things
  PH::PlotManager manager = PH::PlotManager( can_def );
  manager.MakePlot();
  manager.Draw();
  manager.GetTCanvas() -> cd();
  for (auto graph : igraphs) {
    graph -> Draw("LP");
  }
  legend -> Draw();
  text   -> Draw();

  ofile -> cd();
  manager.Write();
  manager.Close();

  // close input files
  for (TFile* ifile : ifiles) {
    ifile -> cd();
    ifile -> Close();
  }
  std::cout << "    Closed input files." << std::endl;

  // announce end
  std::cout << "  Finished resolution comparison!\n"
            << " -------------------------------- \n"
            << endl;

  // exit routine
  return;

}  // end 'DoResolutionComparison()'



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



// ----------------------------------------------------------------------------
//! Make a 2D frame histogram
// ----------------------------------------------------------------------------
TH2* BHCalPlotter::MakeFrameHist() {

  // TODO FILL IN
  TH2* frame = nullptr; 
  return frame;

}  // end 'MakeFrameHist(...)'

// end ========================================================================
