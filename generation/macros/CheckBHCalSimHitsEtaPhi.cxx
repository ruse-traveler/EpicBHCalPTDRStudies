/// ===========================================================================
/*! \file   CheckBHCalSimHitsEtaPhi.cxx
 *  \author Derek Anderson
 *  \date   03.08.2025
 *
 *  A ROOT macro to check (eta, phi) distribution for
 *  npsim/ddsim output.
 */
/// ===========================================================================

#define CheckBHCalSimHitsEtaPhi_cxx

// c++ utilities
#include <utility>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TSystem.h>
// podio libraries
#include <podio/Frame.h>
#include <podio/ROOTReader.h>
#include <podio/CollectionBase.h>
// edm4eic types
#include <edm4hep/SimCalorimeterHitCollection.h>
// edm4hep types
#include <edm4hep/Vector3f.h>
#include <edm4hep/utils/vector_utils.h>



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;      // input file
  std::string out_file;     // output file
  std::string hcal_hits;    // bhcal sim hit collection
  bool        do_progress;  // print progress through frame loop
} DefaultOptions = {
  "./forBHCalHoleMystery.epicBHCalOnly2025010_central_e5evt5Kpim.d13m2y2025.edm4hep.root",
  "forBHCalHoleMystery.checkingSimHitEtaPhi.epicBHCalOnly20250510_central_e5evt5Kpim.d8m3y2025.root",
  "HcalBarrelHits",
  true
};



// ============================================================================
//! Fill BHCal sim hit (eta, phi) histograms
// ============================================================================
void CheckBHCalSimHitsEtaPhi(const Options& opt = DefaultOptions) {

  // announce start of macro
  std::cout << "\n  Beginning BHCal sim hit (eta, phi) macro!" << std::endl;

  // --------------------------------------------------------------------------
  // Open input/outputs
  // --------------------------------------------------------------------------

  // open file w/ frame reader
  podio::ROOTReader reader = podio::ROOTReader();
  reader.openFile( opt.in_file );

  // open output file
  TFile* output = new TFile(opt.out_file.data(), "recreate");
  if (!output) {
    std::cerr << "PANIC: couldn't open output file!" << std::endl;
    assert(output);
  }

  // print input/output
  std::cout << "    Opened input/output files:\n"
            << "      input file  = " << opt.in_file << "\n"
            << "      output file = " << opt.out_file
            << std::endl;

  // for binning
  std::tuple<size_t, float, float> etaBins  = {14,  -1.2,  1.2};
  std::tuple<size_t, float, float> phiBins  = {640, -6.35, 6.35};

  // turn on errors
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // create output histograms
  TH1D* hPosEta    = new TH1D("hPosEta", ";#eta", get<0>(etaBins), get<1>(etaBins), get<2>(etaBins));
  TH1D* hPosPhi    = new TH1D("hPosPhi", ";#phi", get<0>(phiBins), get<1>(phiBins), get<2>(phiBins));
  TH2D* hPosPhiEta = new TH2D("hPosPhiEta", ";#eta;#phi", get<0>(etaBins), get<1>(etaBins), get<2>(etaBins), get<0>(phiBins), get<1>(phiBins), get<2>(phiBins));

  // announce histogram making
  std::cout << "    Made output histograms" << std::endl;

  // --------------------------------------------------------------------------
  // Loop over input frames
  // --------------------------------------------------------------------------
  const uint64_t nFrames = reader.getEntries(podio::Category::Event);
  std::cout << "    Starting frame loop: " << reader.getEntries(podio::Category::Event) << " frames to process." << std::endl;

  // iterate through frames
  for (uint64_t iFrame = 0; iFrame < nFrames; ++iFrame) {

    // announce progress
    if (opt.do_progress) {
      std::cout << "      Processing frame " << iFrame + 1 << "/" << nFrames << "...";
      if (iFrame + 1 < nFrames) {
        std::cout << "\r" << std::flush;
      } else {
        std::cout << std::endl;
      }
    }

    // grab frame & collections
    auto  frame    = podio::Frame( reader.readNextEntry(podio::Category::Event) );
    auto& hcalHits = frame.get<edm4hep::SimCalorimeterHitCollection>( opt.hcal_hits );

    // hcal sim hit loop
    for (edm4hep::SimCalorimeterHit hHit : hcalHits) {

      // calculate eta/phi
      const double eta = edm4hep::utils::eta( hHit.getPosition() );
      const double phi = edm4hep::utils::angleAzimuthal( hHit.getPosition() );

      // fill histograms
      hPosEta    -> Fill(eta);
      hPosPhi    -> Fill(phi);
      hPosPhiEta -> Fill(eta, phi);

    }  // end hcal cluster loop

  }  // end frame loop
  std::cout << "    Finished frame loop" << std::endl;

  // save output & close files
  output     -> cd();
  hPosEta    -> Write();
  hPosPhi    -> Write();
  hPosPhiEta -> Write();
  output     -> Close();

  // announce end & exit
  std::cout << "  End of macro!\n" << std::endl;
  return;

}

// end ========================================================================
