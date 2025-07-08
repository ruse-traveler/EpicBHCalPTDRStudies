/// ===========================================================================
/*! \file   TrainBHCalClusterCalibration.cxx
 *  \author Derek Anderson
 *  \date   09.17.2024
 *
 *  A ROOT macro to train a TMVA model to calibrate
 *  the energy of clusters in the BHCal and BIC.
 *  Ingests the TNtuple produced by
 *  'FillBHCalClusterCalibrationTuple.cxx'
 */
/// ===========================================================================

#define TrainBHCalClusterCalibration_cxx

// c++ utilities
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
// root libraries
#include <TCut.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TSystem.h>
// tmva components
#include <TMVA/Tools.h>
#include <TMVA/Factory.h>
#include <TMVA/DataLoader.h>
// analysis utilities
#include <utility/TMVAHelper.hxx>

// calculation parameters
#include "TMVAClusterParameters.hxx"



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;      // input file
  std::string in_tuple;     // input ntuple
  std::string out_file;     // output file
  std::string out_tmva;     // output tmva directory
  std::string name_tmva;    // name of TMVA process
  bool        do_progress;  // print progress through entry loop
}  DefaultOptions = {
  "./input/forNewTrainingMacro_noNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d14m9y2024.root",
  "ntForCalib",
  "testA.root",
  "tmva_test",
  "TMVARegression",
  true
};



// ============================================================================
//! Train a TMVA model for BHCal cluster calibration
// ============================================================================
void TrainBHCalClusterCalibration(const Options& opt = DefaultOptions) {

  // --------------------------------------------------------------------------
  // Grab calculation parameters
  // --------------------------------------------------------------------------
  TMVAHelper::Parameters param = TMVAClusterParameters::GetParameters(opt.do_progress);

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning calibration training macro..." << std::endl;

  // --------------------------------------------------------------------------
  // Open input/outputs
  // --------------------------------------------------------------------------

  // open files
  TFile* input  = new TFile(opt.in_file.data(),  "read");
  TFile* output = new TFile(opt.out_file.data(), "recreate");
  if (!input || !output) {
    std::cerr << "PANIC: couldn't open a file!\n"
              << "       input  = " << input << "\n"
              << "       output = " << output
              << std::endl;
    assert(output && input);
  }

  // print input/output files
  std::cout << "    Opened input/output files:\n"
            << "      input file  = " << opt.in_file << "\n"
            << "      output file = " << opt.out_file
            << std::endl;

  // grab input tuple
  TNtuple* ntInput = (TNtuple*) input -> Get(opt.in_tuple.data());
  if (!ntInput) {
    std::cerr << "PANIC: couldn't grab input tuple!\n"
              << "       name  = " << opt.in_tuple << "\n"
              << "       tuple = " << ntInput
              << std::endl;
    assert(ntInput);
  }

  // print input tuples
  std::cout << "    Grabbed input tuples:\n"
            << "      tuple = " << opt.in_tuple
            << std::endl;

  // --------------------------------------------------------------------------
  // Set up helpers
  // --------------------------------------------------------------------------

  // create tmva helper
  TMVAHelper::Trainer train_helper( param.variables, param.methods );
  train_helper.SetFactoryOptions(param.opts_factory);
  train_helper.SetTrainOptions(param.opts_training);
  std::cout << "    Created TMVA helper." << std::endl;


  // --------------------------------------------------------------------------
  // Train tmva models
  // --------------------------------------------------------------------------

  // instantiate tmva library
  TMVA::Tools::Instance();
  std::cout << "    Begin training calibration models:" << std::endl;

  // create tmva factory & load data
  TMVA::Factory*    factory = new TMVA::Factory(opt.name_tmva.data(), output, train_helper.CompressFactoryOptions().data());
  TMVA::DataLoader* loader  = new TMVA::DataLoader(opt.out_tmva.data());
  std::cout << "      Created factory and data loader..." << std::endl;

  // now load variables
  train_helper.LoadVariables(loader, param.add_spectators);
  std::cout << "      Loaded variables..." << std::endl;

  // add tree & prepare for training
  loader -> AddRegressionTree(ntInput, param.tree_weight);
  loader -> PrepareTrainingAndTestTree(param.training_cuts, train_helper.CompressTrainingOptions().data());
  std::cout << "      Added tree, prepared training..." << std::endl;

  // book methods
  train_helper.BookMethodsToTrain(factory, loader);
  std::cout << "      Booked methods for training..." << std::endl;

  // train, test, & evaluate
  factory -> TrainAllMethods();
  factory -> TestAllMethods();
  factory -> EvaluateAllMethods();
  std::cout << "      Trained models.\n"
            << "    Finished training calibration models!"
            << endl;

  // --------------------------------------------------------------------------
  // Close I/O and exit
  // --------------------------------------------------------------------------

  // close files
  output -> cd();
  output -> Close();
  input  -> cd();
  input  -> Close();

  // delete tmva objects
  delete factory;
  delete loader;

  // announce end & exit
  std::cout << "  Finished BHCal calibration training macro!\n" << std::endl;
  return;

}

// end ========================================================================
