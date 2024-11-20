/// ===========================================================================
/*! \file   BHCalManualCalibrator.hxx
 *  \author Derek Anderson
 *  \date   11.20.2024
 *
 *  A class to consolidate plotting routines for
 *  the BHCal chapter of the ePIC pTDR.
 */
/// ===========================================================================

#ifndef BHCalManualCalibrator_hxx
#define BHCalManualCalibrator_hxx

// c++ utilities
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
#include <iostream>
// root libraries
#include <TF1.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TGraphErrors.h>
// analysis utilities
#include "../../utils/HistHelper.hxx"
#include "../../utils/NTupleHelper.hxx"

// abbreviate common namespaces
namespace HH = HistHelper;



// ============================================================================
//! BHCal manual calibration 
// ============================================================================
/*! This class implements two approaches for
 *  manually deriving BHCal + BIC calibration
 *  factors.
 */
class BHCalManualCalibrator {

  public:

    // default ctor/dtor
    BHCalManualCalibrator();
    ~BHCalManualCalibrator();

    /* TODO fill in */

  private:

    /* TODO fill in */

};  // end BHCalManualCalibrator

#endif

// end ========================================================================
