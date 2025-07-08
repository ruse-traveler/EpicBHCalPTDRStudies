/// ===========================================================================
/*! \file   TMVAClusterParameters.hxx
 *  \author Derek Anderson
 *  \date   09.17.2024
 *
 *  A small namespace to define the parameters
 *  TMVA uses to train/apply various ML models
 *  for the calibration of energy for clusters
 *  in the BHCal and BIC.
 */
/// ===========================================================================

#ifndef TMVAClusterParameters_hxx
#define TMVAClusterParameters_hxx

// c++ utilities
#include <map>
#include <string>
#include <vector>
#include <utility>
// tmva components
#include <TMVA/Types.h>
// analysis utilities
#include <utility/TMVAHelper.hxx>



// ============================================================================
//! Cluster TMVA Parameters
// ============================================================================
namespace TMVAClusterParameters {

  // --------------------------------------------------------------------------
  //! Input variables & their usage
  // --------------------------------------------------------------------------
  const std::vector<std::pair<TMVAHelper::Use, std::string>> vecUseAndVar = {
    {TMVAHelper::Use::Target, "ePar"},
    {TMVAHelper::Use::Watch, "fracParVsLeadBHCal"},
    {TMVAHelper::Use::Watch, "fracParVsLeadBEMC"},
    {TMVAHelper::Use::Watch, "fracParVsSumBHCal"},
    {TMVAHelper::Use::Watch, "fracParVsSumBEMC"},
    {TMVAHelper::Use::Watch, "fracLeadBHCalVsBEMC"},
    {TMVAHelper::Use::Watch, "fracSumBHCalVsBEMC"},
    {TMVAHelper::Use::Train, "eLeadBHCal"},
    {TMVAHelper::Use::Train, "eLeadBEMC"},
    {TMVAHelper::Use::Watch, "eSumBHCal"},
    {TMVAHelper::Use::Watch, "eSumBEMC"},
    {TMVAHelper::Use::Watch, "diffLeadBHCal"},
    {TMVAHelper::Use::Watch, "diffLeadBEMC"},
    {TMVAHelper::Use::Watch, "diffSumBHCal"},
    {TMVAHelper::Use::Watch, "diffSumBEMC"},
    {TMVAHelper::Use::Watch, "nHitsLeadBHCal"},
    {TMVAHelper::Use::Watch, "nHitsLeadBEMC"},
    {TMVAHelper::Use::Watch, "nClustBHCal"},
    {TMVAHelper::Use::Watch, "nClustBEMC"},
    {TMVAHelper::Use::Watch, "hLeadBHCal"},
    {TMVAHelper::Use::Watch, "hLeadBEMC"},
    {TMVAHelper::Use::Watch, "fLeadBHCal"},
    {TMVAHelper::Use::Watch, "fLeadBEMC"},
    {TMVAHelper::Use::Watch, "eLeadImage"},
    {TMVAHelper::Use::Watch, "eSumImage"},
    {TMVAHelper::Use::Watch, "eLeadScFi"},
    {TMVAHelper::Use::Watch, "eSumScFi"},
    {TMVAHelper::Use::Watch, "nClustImage"},
    {TMVAHelper::Use::Watch, "nClustScFi"},
    {TMVAHelper::Use::Watch, "hLeadImage"},
    {TMVAHelper::Use::Watch, "hLeadScFi"},
    {TMVAHelper::Use::Watch, "fLeadImage"},
    {TMVAHelper::Use::Watch, "fLeadScFi"},
    {TMVAHelper::Use::Train, "eSumScFiLayer1"},
    {TMVAHelper::Use::Train, "eSumScFiLayer2"},
    {TMVAHelper::Use::Train, "eSumScFiLayer3"},
    {TMVAHelper::Use::Train, "eSumScFiLayer4"},
    {TMVAHelper::Use::Train, "eSumScFiLayer5"},
    {TMVAHelper::Use::Train, "eSumScFiLayer6"},
    {TMVAHelper::Use::Train, "eSumScFiLayer7"},
    {TMVAHelper::Use::Train, "eSumScFiLayer8"},
    {TMVAHelper::Use::Train, "eSumScFiLayer9"},
    {TMVAHelper::Use::Train, "eSumScFiLayer10"},
    {TMVAHelper::Use::Train, "eSumScFiLayer11"},
    {TMVAHelper::Use::Train, "eSumScFiLayer12"},
    {TMVAHelper::Use::Train, "eSumImageLayer1"},
    {TMVAHelper::Use::Watch, "eSumImageLayer2"},
    {TMVAHelper::Use::Train, "eSumImageLayer3"},
    {TMVAHelper::Use::Train, "eSumImageLayer4"},
    {TMVAHelper::Use::Watch, "eSumImageLayer5"},
    {TMVAHelper::Use::Train, "eSumImageLayer6"}
  };

  // --------------------------------------------------------------------------
  //! List of methods to use
  // --------------------------------------------------------------------------
  const std::vector<std::string> vecMethodsToUse = {
    "LD",
    "KNN",
    "MLP",
    "BDTG",
    "FDA_GA"
  };

  // --------------------------------------------------------------------------
  //! Cuts to apply when training
  // -------------------------------------------------------------------------
  const TCut trainCut(
    "((eSumBHCal>=0)||"
    "(eSumBEMC>=0))&&"
    "(abs(hLeadBHCal)<1.1)&&"
    "(abs(hLeadBEMC)<1.1)"
  );

  // --------------------------------------------------------------------------
  //! Cuts to apply while reading input tuple
  // --------------------------------------------------------------------------
  const TCut readCut(
    "(eLeadBEMC>0.5)&&"
    "(eLeadBEMC<100)"
  );

  // --------------------------------------------------------------------------
  //! General TMVA options
  // --------------------------------------------------------------------------
  std::vector<std::string> vecFactoryOpts = {
    "!V",
    "Color",
    "AnalysisType=Regression"
  };

  // --------------------------------------------------------------------------
  //! Training options
  // --------------------------------------------------------------------------
  std::vector<std::string> vecTrainOpts = {
    "nTrain_Regression=100",
    "nTest_Regression=0",
    "SplitMode=Random:NormMode=NumEvents",
    "!V"
  };

  // --------------------------------------------------------------------------
  //! Reading options
  // --------------------------------------------------------------------------
  std::vector<std::string> vecReadOpts = {
    "!Color"
  };

  // --------------------------------------------------------------------------
  //! Misc other options
  // -------------------------------------------------------------------------
  const bool  addSpectators(false);
  const float treeWeight(1.0);



  // --------------------------------------------------------------------------
  //! Set method-specific options
  // --------------------------------------------------------------------------
  /*! This method is used to define what options to use
   *  with each method. Which methods are actually used
   *  are defined by vecMethodsToUse above.
   */
  std::map<std::string, std::vector<std::string>> SetMethodOptions() {

    std::map<std::string, std::vector<std::string>> mapMethodToOpt;
    {

      // BDTG algorithm
      mapMethodToOpt["BDTG"] = {
        "!H",
        "!V",
        "NTrees=2000",
        "BoostType=Grad",
        "Shrinkage=0.1",
        "UseBaggedBoost",
        "BaggedSampleFraction=0.5",
        "nCuts=20",
        "MaxDepth=3",
        "MaxDepth=4"
      };

      // FDA (GA) algorithm
      mapMethodToOpt["FDA_GA"] = {
        "!H",
        "!V",
        "Formula=(0)+(1)*x0+(2)*x1",
       "ParRanges=(-100,100);(-100,100);(-100,100)",
        "FitMethod=GA",
        "PopSize=100",
        "Cycles=3",
        "Steps=30",
        "Trim=True",
        "SaveBestGen=1",
        "VarTransform=Norm"
      };

      // kNN algorithm
      mapMethodToOpt["KNN"] = {
        "nkNN=20",
        "ScaleFrac=0.8",
        "SigmaFact=1.0",
        "Kernel=Gaus",
        "UseKernel=F",
        "UseWeight=T",
	"UseLDA=T",
        "!Trim"
      };

      // LD algorithm
      mapMethodToOpt["LD"] = {
        "!H",
        "!V",
        "VarTransform=None"
      };

      // MLP algorithm
      mapMethodToOpt["MLP"] = {
        "!H",
        "!V",
        "VarTransform=Norm",
        "NeuronType=ReLU",
        "NCycles=200000",
        "HiddenLayers=N-16",
        "TestRate=10",
        "TrainingMethod=BP",
        "Sampling=1",
        "SamplingEpoch=1",
        "LearningRate=0.005",
        "DecayRate=5e-6",
        "ConvergenceImprove=1e-11",
        "ConvergenceTests=25000",
        "!UseRegulator"
      };

      // PDE Foam algorithm
      //   - FIXME these options haven't
      //     worked for the test cases
      //     yet...
      mapMethodToOpt["PDEFoam"] = {
        "!H:",
        "!V:",
        "MultiTargetRegression=F",
        "TargetSelection=Mpv",
        "TailCut=0.001",
        "VolFrac=0.0666",
        "nActiveCells=500",
        "nSampl=2000",
        "nBin=5",
        "Compress=T",
        "Kernel=None",
        "Nmin=10",
        "VarTransform=None"
      };

    }  // end option setting
    return mapMethodToOpt;

  }  // end 'SetMethodOptions()'



  // --------------------------------------------------------------------------
  //! Collect methods to use and their options into a list of pairs
  // --------------------------------------------------------------------------
  std::vector<std::pair<std::string, std::string>> GetMethodsAndOptions() {

    // set method-specific options
    std::map<std::string, std::vector<std::string>> mapMethodToOpt = SetMethodOptions();

    // then collect into list of pairs
    std::vector<std::pair<std::string, std::string>> vecMethodAndOpt;
    for (const std::string& method : vecMethodsToUse) {
      vecMethodAndOpt.push_back(
        {method, TMVAHelper::CompressList( mapMethodToOpt[method] )}
      );
    }
    return vecMethodAndOpt;    

  }  // end 'GetMethodsAndOptions()'



  // --------------------------------------------------------------------------
  //! Collect options into parameter struct
  // --------------------------------------------------------------------------
  TMVAHelper::Parameters GetParameters(const bool do_progress = true) {

    // add options relevant for verbosity
    if (do_progress) {
      vecFactoryOpts.push_back("!Silent");
      vecFactoryOpts.push_back("DrawProgressBar");
      vecReadOpts.push_back("!Silent");
    } else {
      vecFactoryOpts.push_back("Silent");
      vecFactoryOpts.push_back("!DrawProgressBar");
      vecReadOpts.push_back("Silent");
    }

    // collect options
    TMVAHelper::Parameters param {
      .variables      = vecUseAndVar,
      .methods        = GetMethodsAndOptions(),
      .opts_factory   = vecFactoryOpts,
      .opts_training  = vecTrainOpts,
      .opts_reading   = vecReadOpts,
      .add_spectators = addSpectators,
      .training_cuts  = trainCut,
      .reading_cuts   = readCut,
      .tree_weight    = treeWeight
    };
    return param;

  }  // end 'GetParameters(bool)'

}  // end TMVAClusterParameters namespace

#endif

// end ========================================================================
