/// ===========================================================================
/*! \file   BHCalManualCalibrator.hxx
 *  \author Derek Anderson
 *  \date   11.20.2024
 *
 *  A class to consolidate plotting routines for
 *  the BHCal chapter of the ePIC pTDR.
 */
/// ===========================================================================

#define BHCalManualCalibrator_cxx

// class definition
#include "BHCalManualCalibrator.hxx"



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! default ctor
// ----------------------------------------------------------------------------
BHCalManualCalibrator::BHCalManualCalibrator() {

  //... nothing to do ...//

}  // end ctor()



// ----------------------------------------------------------------------------
//! default dtor
// ----------------------------------------------------------------------------
BHCalManualCalibrator::~BHCalManualCalibrator() {

  //... nothing to do ...//

}  // end dtor()



// ----------------------------------------------------------------------------
//! ctor accepting a configuration
// ----------------------------------------------------------------------------
BHCalManualCalibrator::BHCalManualCalibrator(const BHCalManualCalibratorConfig& cfg) {

  m_cfg = cfg;

}  // end ctor()



// public methods =============================================================

// ----------------------------------------------------------------------------
//! Initialize 
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::Init() {

  BuildIndices();
  BuildGraphs();
  BuildHists();
  return;

}  // end 'Init()'



// ----------------------------------------------------------------------------
//! Find calibration by minimizing chi2
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::DoChiSquareMinimization() {

  /* TODO fill in here */
  return;

}  // end 'DoChiSquareMinimization()'



// ----------------------------------------------------------------------------
//! Find calibration by minimizing mu/sigma of fits
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::DoMuSigmaMinimization() {

  /* TODO
   *  - add 1/mu calc
   *  - load values into graphs
   *      {rel, res}
   *      {epar, rel, res}
   *      {epar, reso}
   *      {epar, ecalib}
   *  - identify minima in res
   */

  for (std::size_t iRel = 0; iRel < m_cfg.relValues.size(); ++iRel) {
    for (std::size_t iPar = 0; iPar < m_cfg.ePar.size(); ++iPar) {

      // grab relevant histogram index
      const HistIndex   iMap  = std::make_tuple( Method::MuSigma, iPar, iRel, 0 );
      const std::size_t iHist = m_histIdxs[ iMap ];

      // run calcuulation
      const double par = m_cfg.ePar[iPar];
      const double rel = m_cfg.relValues[iRel];
      const double res = GetSigmaOverMu(rel, iHist, iPar);

      // 1/mu = eScaleSum / ePar should go here (or in SigamOverMu calc)

    }
  }

  // generate mu/sigma graphs
  m_graphs.emplace_back( m_graphDefs["MuSigma"]["ResVsRel"].MakeTGraph() );
  m_graph2Ds.emplace_back( m_graphDefs["MuSigma"]["ResVsRelVsPar"].MakeTGraph2D() );
  return;

}  // end 'DoMuSigmaMinimization()'



// ----------------------------------------------------------------------------
//! End and save output to file
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::End(TFile* outfile) {

  outfile -> cd();
  for (auto hist : m_hists) {
    hist -> Write();
  }
  for (auto func : m_funcs) {
    func -> Write();
  }
  for (auto graph : m_graphs) {
    graph -> Write();
  }
  for (auto graph2D : m_graph2Ds) {
    graph2D -> Write();
  }
  return;

}  // end 'End(TFile*)'



// private methods ============================================================

// ----------------------------------------------------------------------------
//! Calculate sigma over mu for given scale factor + particle bin
// ----------------------------------------------------------------------------
/* FIXME this might need to be avoid function */
double BHCalManualCalibrator::GetSigmaOverMu(
  const double rel,
  const std::size_t iHist,
  const std::size_t iParBin
) {

  // grab particle bin
  const auto eParBin = m_cfg.eParBins.at(iParBin);

  // open dataframe
  ROOT::RDataFrame frame(m_cfg.intuple, m_cfg.infile);

  // make sure file isn't empty
  auto entries = frame.Count();
  if (entries == 0) {
    std::cerr << "Error: No entries found!" << std::endl;
    assert(entries.GetValue() > 0);
  }

  // lambda to calculate raw energy sum
  auto doRawEneSum = [](const double eEM, const double eHad) {
    return eEM + eHad;
  };

  // lambda to calculate weightd energy sum
  auto doScaledEneSum = [&rel](const double eEM, const double eHad) {
    return eEM + (rel * eHad);
  };

  // lambda to check if in particle energy bin
  auto isInParEneBin = [&eParBin](const double ePar) {
    return ((ePar >= eParBin.first) && (ePar < eParBin.second));
  };

  // run analysis
  auto analysis = frame.Define( "eRawSum", doRawEneSum, {m_cfg.eEMLeaf, m_cfg.eHadLeaf} )
                       .Define( "eScaledSum", doScaledEneSum, {m_cfg.eEMLeaf, m_cfg.eHadLeaf} )
                       .Filter( isInParEneBin, {m_cfg.eParLeaf} );

  // get histograms
  auto hRawSum   = analysis.Histo1D( m_histDefs["RawSum"][iHist].MakeTH1Model(), "eRawSum" );
  auto hScaleSum = analysis.Histo1D( m_histDefs["ScaleSum"][iHist].MakeTH1Model(), "eScaledSum" );

  // get histogram min/max
  const uint32_t nbins = hScaleSum -> GetNbinsX();
  const double   min   = hScaleSum -> GetXaxis() -> GetBinLowEdge(1);
  const double   max   = hScaleSum -> GetYaxis() -> GetBinLowEdge(nbins);

  // make fit name
  std::string fName = m_histDefs["ScaleSum"][iHist].GetName();
  fName.replace(0, 1, "f");

  // create gaussian for fit
  TF1* fit = new TF1(fName.data(), "gaus(0)", min, max);
  fit -> SetParameter( 0, hScaleSum -> Integral() );
  fit -> SetParameter( 1, hScaleSum -> GetMean() );
  fit -> SetParameter( 2, hScaleSum -> GetRMS() );

  // do fitting
  hScaleSum -> Fit(fName.data());

  // calculate mu / sigma
  const double mu  = fit -> GetParameter(1);
  const double sig = fit -> GetParameter(2);
  const double res = sig / mu;

  // add points to graphs
  m_graphDefs["MuSigma"]["ResVsRel"].AddPoint(
    {rel, res}
  );
  m_graphDefs["MuSigma"]["ResVsRelVsPar"].AddPoint(
    {m_cfg.ePar.at(iParBin), rel, res}
  );

  /* TODO this might be where the 1/mu calc should go */

  // store outputs and return
  m_hists.emplace_back( hRawSum.GetPtr() );
  m_hists.emplace_back( hScaleSum.GetPtr() );
  m_funcs.emplace_back( fit );
  return res;

}  // end 'GetSigmaOverMu(double, std::size_t, std::size_t)'



// ----------------------------------------------------------------------------
//! Build histogram definitions
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::BuildHists() {

  /* TODO hist definition generation goes here
   *   - 1d histograms to define
   *     - esumhcal (method, epar)
   *     - esumecal (method, epar)
   *     - esumraw (method, epar)
   *     - esumscale (method, epar, rel, norm)
   *     - esumcorr (method, epar, rel, norm)
   *   - 2d histograms to define (??)
   *     - epar vs. esumhcal (method)
   *     - epar vs. esumecal (method)
   *     - esumhcal vs. esumecal (method, epar)
   */
  return;

}  // end 'BuildHists()'



// ----------------------------------------------------------------------------
//! Build graph definitions
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::BuildGraphs() {

  /* TODO graph hist definition goes here
   *   - mu/sigma graphs to define: 
   *      {rel, res}
   *      {epar, rel, res}
   *      {epar, reso}
   *      {epar, ecalib}
   *  - chi2 graphs to define:
   *      ...
   */

  // make mu/sigma graphs
  m_graphDefs["MuSigma"].insert(
    {"ResVsRel", GH::Definition("gMuSigma_ResVsRel")}
  );
  m_graphDefs["MuSigma"].insert(
    {"ResVsRelVsPar", GH::Definition("gMuSigma_ResVsRelVsPar")}
  );
  return;

}  // end 'BuildGraphs()'



// ----------------------------------------------------------------------------
//! Build map of histogram indices
// ----------------------------------------------------------------------------
void BHCalManualCalibrator::BuildIndices() {

  std::size_t iHist = 0;
  for (std::size_t iPar = 0; iPar < m_cfg.ePar.size(); ++iPar) {
    for (std::size_t iRel = 0; iRel < m_cfg.relValues.size(); ++iRel) {

      // add mu/sigma indices
      m_histIdxs[ std::make_tuple( Method::MuSigma, iPar, iRel, 0) ] = iHist;
      ++iHist;

      // add chi2 indices
      for (std::size_t iNorm = 0; iNorm < m_cfg.normValues.size(); ++iNorm) {
        m_histIdxs[ std::make_tuple( Method::Chi2, iPar, iRel, iNorm) ] = iHist;
        ++iHist;
      }  // end normalization loop
    }  // end relative scale loop
  }  // end particle bin loop
  return;

}  // end 'BuildIndices()'

// end ========================================================================
