// ----------------------------------------------------------------------------
// 'GetRawEnergiesProcessor.cc'
// Derek Anderson
// 07.04.2023
//
// A simple JANA2 plugin to pull out some BHCal energies
// as a function of eta.
// ----------------------------------------------------------------------------

// c includes
#include <cmath>
// root includes
#include <TString.h>
#include <TVector3.h>
// jana includes
#include <services/rootfile/RootFile_service.h>
// user include
#include "GetRawEnergiesProcessor.h"

// the following just makes this a JANA plugin
extern "C" {
  void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    app -> Add(new GetRawEnergiesProcessor);
  }
}



void GetRawEnergiesProcessor::InitWithGlobalRootLock(){

  // make output directory in plugin file
  auto rootfile_svc = GetApplication() -> GetService<RootFile_service>();
  auto rootfile     = rootfile_svc     -> GetHistFile();
  rootfile -> mkdir("GetRawEnergies") -> cd();

  // histogram names
  const TString sAdcHitRaw("hAdcHitRaw");
  const TString sEneHitSim[NEtaRanges] = {"hEneHitSim", "hEneHitSim_etaM1M05", "hEneHitSim_etaM0505", "hEneHitSim_eta051"};
  const TString sPhiHitSim[NEtaRanges] = {"hPhiHitSim", "hPhiHitSim_etaM1M05", "hPhiHitSim_etaM0505", "hPhiHitSim_eta051"};
  const TString sEtaHitSim[NEtaRanges] = {"hEtaHitSim", "hEtaHitSim_etaM1M05", "hEtaHitSim_etaM0505", "hEtaHitSim_eta051"};
  const TString sEneHitRec[NEtaRanges] = {"hEneHitRec", "hEneHitRec_etaM1M05", "hEneHitRec_etaM0505", "hEneHitRec_eta051"};
  const TString sPhiHitRec[NEtaRanges] = {"hPhiHitRec", "hPhiHitRec_etaM1M05", "hPhiHitRec_etaM0505", "hPhiHitRec_eta051"};
  const TString sEtaHitRec[NEtaRanges] = {"hEtaHitRec", "hEtaHitRec_etaM1M05", "hEtaHitRec_etaM0505", "hEtaHitRec_eta051"};

  // histogram binnin
  const size_t nEneHit = 500;
  const size_t nPhiHit = 720;
  const size_t nEtaHit = 30;
  const size_t nAdcHit = 10000;
  const float  rEneHit[NRange] = {0.0,  5.0};
  const float  rPhiHit[NRange] = {-6.3, 6.3};
  const float  rEtaHit[NRange] = {-1.5, 1.5};
  const float  rAdcHit[NRange] = {-0.5, 9999.5};

  // initialize sim hit histograms
  hEneHitSim[0] = new TH1D(sEneHitSim[0], "", nEneHit, rEneHit[0], rEneHit[1]);
  hPhiHitSim[0] = new TH1D(sPhiHitSim[0], "", nPhiHit, rPhiHit[0], rPhiHit[1]);
  hEtaHitSim[0] = new TH1D(sEtaHitSim[0], "", nEtaHit, rEtaHit[0], rEtaHit[1]);
  hEneHitSim[0] -> Sumw2();
  hPhiHitSim[0] -> Sumw2();
  hEtaHitSim[0] -> Sumw2();
  for (size_t iEtaRange = 1; iEtaRange < NEtaRanges; iEtaRange++) {
    hEneHitSim[iEtaRange] = new TH1D(sEneHitSim[iEtaRange], "", nEneHit, rEneHit[0], rEneHit[1]);
    hPhiHitSim[iEtaRange] = new TH1D(sPhiHitSim[iEtaRange], "", nPhiHit, rPhiHit[0], rPhiHit[1]);
    hEtaHitSim[iEtaRange] = new TH1D(sEtaHitSim[iEtaRange], "", nEtaHit, rEtaHit[0], rEtaHit[1]);
    hEneHitSim[iEtaRange] -> Sumw2();
    hPhiHitSim[iEtaRange] -> Sumw2();
    hEtaHitSim[iEtaRange] -> Sumw2();
  }

  // initialize reco hit histograms
  hEneHitRec[0] = new TH1D(sEneHitRec[0], "", nEneHit, rEneHit[0], rEneHit[1]);
  hPhiHitRec[0] = new TH1D(sPhiHitRec[0], "", nPhiHit, rPhiHit[0], rPhiHit[1]);
  hEtaHitRec[0] = new TH1D(sEtaHitRec[0], "", nEtaHit, rEtaHit[0], rEtaHit[1]);
  hEneHitRec[0] -> Sumw2();
  hPhiHitRec[0] -> Sumw2();
  hEtaHitRec[0] -> Sumw2();
  for (size_t iEtaRange = 1; iEtaRange < NEtaRanges; iEtaRange++) {
    hEneHitRec[iEtaRange] = new TH1D(sEneHitRec[iEtaRange], "", nEneHit, rEneHit[0], rEneHit[1]);
    hPhiHitRec[iEtaRange] = new TH1D(sPhiHitRec[iEtaRange], "", nPhiHit, rPhiHit[0], rPhiHit[1]);
    hEtaHitRec[iEtaRange] = new TH1D(sEtaHitRec[iEtaRange], "", nEtaHit, rEtaHit[0], rEtaHit[1]);
    hEneHitRec[iEtaRange] -> Sumw2();
    hPhiHitRec[iEtaRange] -> Sumw2();
    hEtaHitRec[iEtaRange] -> Sumw2();
  }

  // initialize raw hit histogram
  hAdcHitRaw = new TH1D(sAdcHitRaw, "", nAdcHit, rAdcHit[0], rAdcHit[1]);
  hAdcHitRaw -> Sumw2();

}  // end 'InitWithGlobalRootLock()'



void GetRawEnergiesProcessor::ProcessSequential(const std::shared_ptr<const JEvent>& event) {

  // eta ranges
  const float etaMin[NEtaRanges] = {-10., -1.0, -0.5, 0.5};
  const float etaMax[NEtaRanges] = {10.,  -0.5, 0.5,  1.0};

  // fill sim hit histograms
  for (auto sim : simHits()) {

    // calculate get hit energy and position
    const double eSim  = sim -> getEnergy();
    const double rSimX = sim -> getPosition().x;
    const double rSimY = sim -> getPosition().y;
    const double rSimZ = sim -> getPosition().z;

    // calculate eta and phi
    const TVector3 xySim = TVector3(rSimX, rSimY, rSimZ);
    const double   fSim  = xySim.Phi();
    const double   hSim  = xySim.Eta();

    // fill whole eta range
    hEneHitSim[0] -> Fill(eSim);
    hPhiHitSim[0] -> Fill(fSim);
    hEtaHitSim[0] -> Fill(hSim);

    // individual eta ranges
    for (size_t iEtaRange = 1; iEtaRange < NEtaRanges; iEtaRange++) {

      // check if in eta range
      const bool isInEtaRange = ((hSim >= etaMin[iEtaRange]) && (hSim < etaMax[iEtaRange]));

      // fill hists
      if (isInEtaRange) {
        hEneHitSim[iEtaRange] -> Fill(eSim);
        hPhiHitSim[iEtaRange] -> Fill(fSim);
        hEtaHitSim[iEtaRange] -> Fill(hSim);
      }
    }  // end eta range loop
  }  // end sim hit loop

  // fill reco hit histograms
  for (auto rec : recHits()) {

    // get hit energy and position
    const double eRec  = rec -> getEnergy();
    const double rRecX = rec -> getPosition().x;
    const double rRecY = rec -> getPosition().y;
    const double rRecZ = rec -> getPosition().z;

    // calculate eta and phi
    const TVector3 xyRec = TVector3(rRecX, rRecY, rRecZ);
    const double   fRec  = xyRec.Phi();
    const double   hRec  = xyRec.Eta();

    // fill whole eta range
    hEneHitRec[0] -> Fill(eRec);
    hPhiHitRec[0] -> Fill(fRec);
    hEtaHitRec[0] -> Fill(hRec);

    // individual eta ranges
    for (size_t iEtaRange = 1; iEtaRange < NEtaRanges; iEtaRange++) {

      // check if in eta range
      const bool isInEtaRange = ((hRec >= etaMin[iEtaRange]) && (hRec < etaMax[iEtaRange]));

      // fill hists
      if (isInEtaRange) {
        hEneHitRec[iEtaRange] -> Fill(eRec);
        hPhiHitRec[iEtaRange] -> Fill(fRec);
        hEtaHitRec[iEtaRange] -> Fill(hRec);
      }
    }  // end eta range loop
  }  // end reco hit loop

  // fill raw hit histogram
  for (auto raw : rawHits()) hAdcHitRaw -> Fill(raw -> getAmplitude());

}  // end 'ProcessSequential(shared_ptr<JEvent>&)'



void GetRawEnergiesProcessor::FinishWithGlobalRootLock() {

  /* nothing to do here */
  return;

}  // end 'FinishWithGlobalRootLock()'

// end ------------------------------------------------------------------------
