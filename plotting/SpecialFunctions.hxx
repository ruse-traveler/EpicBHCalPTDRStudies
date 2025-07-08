/// ===========================================================================
/*! \file   SpecialFunctions.hxx
 *  \author Derek Anderson
 *  \date   11.23.2024
 *
 *  A small namespace to define special distributions,
 *  functions, etc. such as the sPHENIX test beam
 *  energy resolution.
 */
/// ===========================================================================

#ifndef SpecialFunctions_hxx
#define SpecialFunctions_hxx

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// root libraries
#include <TF1.h>
#include <TGraph.h>
// analysis utilities
#include <utility/PlotHelper.hxx>
#include <utility/HistHelper.hxx>

// abbreviate common namespaces
namespace GH = GraphHelper;



// ============================================================================
//! Special functions
// ============================================================================
/*! This namespace collects methods to generate special distrbutions,
 *  functions, etc.
 */
namespace SpecialFunctions {

  // --------------------------------------------------------------------------
  //! Make sPHENIX test beam resolution curve
  // --------------------------------------------------------------------------
  TGraph* MakeSPhenixTestBeamRes() {

    // make graph definition
    GH::Definition def("grSphxTestBeam");

    // collect points
    std::vector<GH::Point> points = {
      {4.14959877108356, 0.47719893154717},
      {6.14450880383323, 0.34697739951106},
      {8.1692122326946, 0.30316859721537},
      {12.15563223082159, 0.26110700323024}, 
      {16.20408511280676, 0.23476189744027}, 
      {24.14495469139409, 0.20405296417384},
      {32.17897143943406, 0.19063440434873}
    };
    def.SetPoints(points);

    // return TGraph
    return def.MakeTGraph();

  }  // end 'MakeSPhenixTestBeamRes()'



  // --------------------------------------------------------------------------
  //! Make yellow report BHCal requirement
  // --------------------------------------------------------------------------
  /*! YR recommended a modest resolution of 100%/sqrt(E) oplus 10%
   *  for the BHCal.
   */
  TF1* MakeYellowReportRequirement() {

    // make TF1 and return
    TF1* requirement = new TF1("fRequireYR", "TMath::Power([0] / TMath::Sqrt(x), 2.) + TMath::Power([1], 2.)", 0., 100.);
    requirement -> FixParameter(0, 1.0);
    requirement -> FixParameter(1, 0.1);
    return requirement;

  }  // end 'MakeYellowReportRequirement()'

}  // end SpecialFunctions namespace

#endif

// end ========================================================================
