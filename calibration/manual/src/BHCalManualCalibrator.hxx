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
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
#include <iostream>
// root libraries
#include <TF1.h>
#include <TH1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TNtuple.h>
#include <TGraph2D.h>
// dataframe related classes
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>
// analysis utilities
#include "../../utils/HistHelper.hxx"
#include "../../utils/GraphHelper.hxx"
#include "../../utils/NTupleHelper.hxx"

// abbreviate common namespaces
namespace HH = HistHelper;
namespace GH = GraphHelper;

// handy aliases
typedef std::tuple<int, int64_t, int64_t, int64_t> HistIndex;
typedef std::pair<double, double> Bin;
typedef std::vector<double> ParVec;
typedef std::vector<double> EneVec;
typedef std::vector<Bin> BinVec;
typedef std::map<HistIndex, std::size_t> IndexMap;
typedef std::map<std::string, std::vector<HH::Definition>> HistDefs;
typedef std::map<std::string, std::vector<GH::Definition>> GraphDefs;



// ============================================================================
//! User options for manual calibration
// ============================================================================
struct BHCalManualCalibratorConfig {

  // input parameters
  std::string infile  = "test.root";
  std::string intuple = "ntForCalib";

  // tuple parameters
  std::string eParLeaf = "ePar";
  std::string eEMLeaf  = "eSumBEMC";
  std::string eHadLeaf = "eSumBHCal";

  // cut parameters
  EneVec ePar     = {1.};
  BinVec eParBins = { {0., 100.} };

  // calculation parameters
  ParVec relValues  = {1.0};
  ParVec normValues = {1.0};

};  // end BHCalManualCalibratorConfig



// ============================================================================
//! BHCal manual calibration 
// ============================================================================
/*! This class implements two approaches for
 *  manually deriving BHCal + BIC calibration
 *  factors.
 */
class BHCalManualCalibrator {

  public:

    // enumeration of methods
    enum Method {Chi2, MuSigma};

    // default ctor/dtor
    BHCalManualCalibrator();
    ~BHCalManualCalibrator();

    // ctor accepting a configuration
    BHCalManualCalibrator(const BHCalManualCalibratorConfig& cfg);

    // public methods
    void Init();
    void DoChiSquareMinimization();
    void DoMuSigmaMinimization();
    void End(TFile* outfile);

  private:

    // private methods
    double GetSigmaOverMu(const double rel, const std::size_t iHist, const std::size_t iParBin);
    void   BuildHists();  // TODO
    void   BuildGraphs();  // TODO
    void   BuildIndices();

    // map of hist index to vector index
    IndexMap m_histIdxs;

    // hist/graph definitions
    HistDefs  m_histDefs;
    GraphDefs m_graphDefs;

    // outputs
    std::vector<TH1*>      m_hists;
    std::vector<TF1*>      m_funcs;
    std::vector<TGraph*>   m_graphs;
    std::vector<TGraph2D*> m_graph2Ds;

    // configuration
    BHCalManualCalibratorConfig m_cfg;

};  // end BHCalManualCalibrator

#endif

// end ========================================================================
