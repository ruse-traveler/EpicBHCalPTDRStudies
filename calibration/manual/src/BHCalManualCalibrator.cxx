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

   /* TODO definition generation goes here */
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
   *      (rel, res)
   *      (epar, rel, res)
   *      (epar, reso)
   *      (epar, ecalib)
   *  - identify minima in res
   */

  for (std::size_t iRel = 0; iRel < m_cfg.relValues.size(); ++iRel) {
    for (std::size_t iPar = 0; iPar < m_cfg.ePar.size(); ++iPar) {

      const double par = m_cfg.ePar[iPar];
      const double rel = m_cfg.relValues[iRel];
      const double res = GetSigmaOverMu(rel, iHist, iPar);

      // 1/mu = eScaleSum / ePar should go here (or in SigamOverMu calc)

    }
  }
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
double BHCalManualCalibrator::GetSigmaOverMu(
  const double rel,
  const std::size_t iHist,
  const std::size_t iParBin
) {

  // grab particle bin
  const auto eParbin = m_cfg.eParBins.at(iParBin);

  // open dataframe
  ROOT::RDataFrame frame(m_cfg.intuple, m_cfg.infile);

  // make sure file isn't empty
  auto entries = frame.Count();
  if (entries == 0) {
    std::cerr << "Error: No entries found!" << std::endl;
    assert(entries > 0);
  }

  // lambda to calculate weight energy sum
  auto doScaledEneSum = [&rel](const double eEM, const double eHad) {
    return eEM + (rel * eHad);
  }

  // lambda to check if in particle energy bin
  auto isInParEneBin = [&eParBin](const double ePar) {
    return ((ePar >= eParBin.first) && (ePar < eParBin.second));
  }

  auto hSum = frame.Define( "eScaledSum", doScaledEneSum, {m_cfg.eEMLeaf, m_cfg.eHadLeaf} )
                   .Filter( eParCut, {m_cfg.eParLeaf} )
                   .Histo1D( m_histDefs[iHist].MakeTH1Model(), "eScaledSum" );

  // get histogram min/max
  const uint32_t nbins = hSum -> GetNbinsx();
  const double   min   = hSum -> GetXaxis() -> GetBinLowEdge(1);
  const double   max   = hSum -> GetYaxis() -> GetBinLowEdge(nbins);

  // make fit name
  std::string fName = m_histDefs[iHist].GetName();
  fName.replace(0, 1, "f");

  // create gaussian for fit
  TF1* fit = new TF1(fName.data(), "gaus(0)", min, max);
  fit -> SetParameter( 0, hSum -> Integral() );
  fit -> SetParameter( 1, hSum -> GetMean() );
  fit -> SetParameter( 2, hSum -> GetRMS() );

  // do fitting
  hSum -> Fit(fName.data());

  // calculate mu / sigma
  const double mu  = fit -> GetParameter(1);
  const double sig = fit -> GetParameter(2);
  const double res = sig / mu;

  // store outputs and return
  m_hists.push_back( hSum );
  m_funcs.push_back( fit );
  return res;

}  // end 'GetSigmaOverMu(double, std::size_t, std::size_t)'



// ----------------------------------------------------------------------------
//! Get histogram index
// ----------------------------------------------------------------------------
std::size_t GetHistIndex() {

  /* TODO fill in */
  return;

}  // end 'GetHistIndex()'



// ----------------------------------------------------------------------------
//! Get string expression to check if particle energy is in a bin
// ---------------------------------------------------------------------------- 
/*! FIXME might not be needed */
std::string BHCalManualCalibrator::GetParCutString(const std::size_t iParBin) {

  // convert doubles to string
  const std::string sPar = m_cfg.eParLeaf;
  const std::string sMin = std::to_string( m_cfg.eParBins.at(iParBin).first );
  const std::string sMax = std::to_string( m_cfg.eParBins.at(iParBin).second );

  // construct string
  std::string cut = "((" + sPar + " >= " + sMin ") && (" + sPar + " < " + sMax + "));";
  return cut;

}  // end 'GetParCutString(std::size_t)'


// end ========================================================================
