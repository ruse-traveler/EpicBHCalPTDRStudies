/// ===========================================================================
/*! \file   ApplyBHCalClusterCalibration.cxx
 *  \author Derek Anderson
 *  \date   09.17.2024
 *
 *  A ROOT macro to apply a pre-trained TMVA model to
 *  calibrate the energy of clusters in the BHCal and
 *  BIC. Ingests the TNtuple produced by
 *  'FillBHCalClusterCalibrationTuple.cxx'
 */
/// ===========================================================================

#define ApplyBHCalClusterCalibration_cxx

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
#include <TTreeFormula.h>
// tmva components
#include <TMVA/Reader.h>
// analysis utilities
#include <utility/TMVAHelper.hxx>
#include <utility/NTupleHelper.hxx>
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
  bool        do_read_cut;  // apply cuts while reading ntuple
}  DefaultOptions = {
  "./input/forNewTrainingMacro_noNonzeroEvts_andDefinitePrimary.evt5Ke210pim_central.d14m9y2024.root",
  "ntForCalib",
  "testB.root",
  "tmva_test",
  "TMVARegression",
  true,
  false
};



// ============================================================================
//! Apply a TMVA model for BHCal cluster calibration
// ============================================================================
void ApplyBHCalClusterCalibration(const Options& opt = DefaultOptions) {

  // --------------------------------------------------------------------------
  // Grab calculation parameters
  // --------------------------------------------------------------------------
  TMVAHelper::Parameters param = TMVAClusterParameters::GetParameters(opt.do_progress);

  // lower verbosity & announce start
  gErrorIgnoreLevel = kError;
  std::cout << "\n  Beginning calibration evaluation macro..." << std::endl;

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
  if (!input) {
    std::cerr << "PANIC: couldn't grab input tuple!\n"
              << "       name  = " << opt.in_tuple << "\n"
              << "       input = " << input
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
  TMVAHelper::Reader read_helper( param.variables, param.methods );
  read_helper.SetOptions(param.opts_reading);

  // collect input leaves into a single vector
  std::vector<std::string> inputs;
  for (const auto& useAndVar : param.variables) {
    inputs.push_back(useAndVar.second);
  }

  // create input/output helpers
  NTupleHelper in_helper( inputs );
  NTupleHelper out_helper( read_helper.GetOutputs() );

  // set input/output tuple branches
  TNtuple* ntOutput = new TNtuple("ntTmvaOutput", "Output of TMVA regression", out_helper.CompressVariables().data());
  in_helper.SetBranches(ntInput);
  std::cout << "    Set input/output tuple branches." << std::endl;

  // --------------------------------------------------------------------------
  // Apply tmva models
  // --------------------------------------------------------------------------

  // instantiate formula object for applying ntuple cuts
  TTreeFormula* selector = new TTreeFormula("selector", param.reading_cuts, ntInput);

  // instantiate reader
  TMVA::Reader* reader = new TMVA::Reader(read_helper.CompressOptions().data());
  std::cout << "    Begin applying calibration models:" << std::endl;

  // add input variables to reader, book methods
  read_helper.ReadVariables(reader, in_helper);
  read_helper.BookMethodsToRead(reader, opt.out_tmva, opt.name_tmva);
  std::cout << "      Added variables and methods to read." << std::endl;

  // get number of entries for application
  const uint64_t nEntries = ntInput -> GetEntries();
  cout << "    Processing: " << nEntries << " events" << endl;

  // loop over entries in input tuple
  uint64_t nBytes = 0;
  for (uint64_t iEntry = 0; iEntry < nEntries; iEntry++) {

    // announce progress
    if (opt.do_progress) {
      std::cout << "      Processing entry " << iEntry + 1 << "/" << nEntries << "...";
      if (iEntry + 1 < nEntries) {
        std::cout << "\r" << std::flush;
      } else {
        std::cout << std::endl;
      }
    }

    // grab entry
    const uint64_t bytes = ntInput -> GetEntry(iEntry);
    if (bytes < 0.) {
      std::cerr << "WARNING error in entry #" << iEntry << "! Aborting loop!" << std::endl;
      break;
    } else {
      nBytes += bytes;
    }

    // make sure output variables are empty
    out_helper.ResetValues();
    read_helper.ResetValues();

    // evaluate targets
    read_helper.EvaluateMethods(reader, in_helper);

    // apply cuts if need be
    const bool isInCut = selector -> EvalInstance();
    if (opt.do_read_cut && !isInCut) continue;

    // set values in output tuple & fill
    for (const std::string& output : read_helper.GetOutputs()) {
      out_helper.SetVariable( output, read_helper.GetVariable(output) );
    }
    ntOutput -> Fill( out_helper.GetValues().data() );

  }  // end entry loop
  std::cout << "    Application loop finished." << std::endl;

  // --------------------------------------------------------------------------
  // Save output and exit
  // --------------------------------------------------------------------------

  // save & close files
  output   -> cd();
  ntOutput -> Write(); 
  output   -> Close();
  input    -> cd();
  input    -> Close();

  // delete tmva object
  delete reader;

  // announce end & exit
  std::cout << "  Finished BHCal calibration evaluation macro!\n" << std::endl;
  return;

}

// end ========================================================================
