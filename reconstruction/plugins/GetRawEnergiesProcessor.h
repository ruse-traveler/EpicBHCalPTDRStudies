// ----------------------------------------------------------------------------
// 'GetRawEnergiesProcessor.h'
// Derek Anderson
// 07.04.2023
//
// A simple JANA2 plugin to pull out some BHCal energies
// as a function of eta.
// ----------------------------------------------------------------------------

// root includes
#include <TH1D.h>
#include <TFile.h>
// jana includes
#include <JANA/JEventProcessorSequentialRoot.h>
// edm definitions
#include <edm4eic/CalorimeterHit.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/RawCalorimeterHit.h>

// global constants
static const size_t NEtaRanges(4);
static const size_t NRange(2);



class GetRawEnergiesProcessor : public JEventProcessorSequentialRoot {

  private:

    // data objects we need from jana
    PrefetchT<edm4hep::SimCalorimeterHit> simHits = {this, "HcalBarrelHits"};
    PrefetchT<edm4hep::RawCalorimeterHit> rawHits = {this, "HcalBarrelRawHits"};
    PrefetchT<edm4eic::CalorimeterHit>    recHits = {this, "HcalBarrelRecHits"};

    // sim hit histograms
    TH1D* hEneHitSim[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};
    TH1D* hPhiHitSim[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};
    TH1D *hEtaHitSim[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};

    // reco hit histograms
    TH1D* hEneHitRec[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};
    TH1D* hPhiHitRec[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};
    TH1D *hEtaHitRec[NEtaRanges] = {nullptr, nullptr, nullptr, nullptr};

    // raw hit histograms
    TH1D* hAdcHitRaw = nullptr;

  public:

    // ctor
    GetRawEnergiesProcessor() { SetTypeName(NAME_OF_THIS); }

    // required jana methods
    void InitWithGlobalRootLock() override;
    void ProcessSequential(const std::shared_ptr<const JEvent>& event) override;
    void FinishWithGlobalRootLock() override;

};  // end 'GetRawEnergies' definition

// end ------------------------------------------------------------------------
