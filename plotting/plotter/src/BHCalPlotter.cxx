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
 *  \param[in]  inputs list of objects to plot
 *  \param[out] ofile  file to write to
 */
void BHCalPlotter::DoResolutionComparison(
  const std::vector<PlotterInput>& inputs,
  TFile* ofile
) {

  // announce start
  std::cout << "\n -------------------------------- \n"
            << "   Beginning resolution comparison!\n"
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
  }
  std::cout << "     Opened inputs." << std::endl;

  // close input files
  for (TFile* ifile : ifiles) {
    ifile -> cd();
    ifile -> Close();
  }

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

// end ========================================================================
