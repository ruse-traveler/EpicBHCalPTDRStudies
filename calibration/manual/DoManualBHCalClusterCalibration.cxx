/// ===========================================================================
/*! \file   DoManualBHCalClusterCalibration.cxx
 *  \author Derek Anderson
 *  \date   11.20.2024
 *
 *  A driver ROOT macro to run manual
 *  BIC + BHCa calibration routines.
 */
/// ===========================================================================

#define DoManualBHCalClusterCalibration_cxx

// analysis utilities
#include "./src/BHCalManualCalibrator.hxx"

// load calibrator library
R__LOAD_LIBRARY(./src/BHCalManualCalibrator_cxx.so)



// ============================================================================
//! Do manual cluster calibration
// ============================================================================
void DoManualBHCalClusterCalibration() {

  // announce start
  std::cout << "\n  Beginning manual BHCal calibration routines..." << std::endl;

  // create calibrator
  BHCalManualCalibrator calibrator = BHCalManualCalibrator();

  // announce end & exit
  std::cout << "  Finished manual calibration routines!\n" << std::endl;
  return;

}

// end ========================================================================
