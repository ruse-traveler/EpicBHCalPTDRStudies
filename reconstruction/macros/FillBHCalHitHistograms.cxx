/// ===========================================================================
/*! \file   FillBHCalHitHistograms.cxx
 *  \author Derek Anderson
 *  \date   11.06.2024
 *
 *  A ROOT macro to read EICrecon output (either `*.podio.root` or
 *  `*.tree.edm4eic.root` and fill a couple histograms of hits in
 *  the BHCal.
 */
/// ===========================================================================

#define FillBHCalHitHistograms_cxx

// c++ utilities
#include <map>
#include <string>
#include <utility>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TSystem.h>
// podio libraries
#include <podio/Frame.h>
#include <podio/CollectionBase.h>
#include <podio/ROOTFrameReader.h>
// edm4eic types
#include <edm4eic/CalorimeterHitCollection.h>
#include <edm4eic/ReconstructedParticleCollection.h>
// edm4hep types
#include <edm4hep/Vector3f.h>
#include <edm4hep/utils/vector_utils.h>



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;      // input file
  std::string out_file;     // output file
  std::string gen_par;      // particle collection
  std::string hcal_hit;     // hcal cluster collection
  bool        do_progress;  // print progress through frame loop
} DefaultOptions = {
  "../output/forTileMerger.change0_test_mergeBHCalHitsInEta.d11m5y2024.podio.root",
  "forTileMerger.change0_test_mergeBHCalHitsInEta.d6m5y2024.hist.root",
  "GeneratedParticles",
  "HcalBarrelMergedHits",
  true
};



// ============================================================================
//! Fill BHCal-only NTuple
// ============================================================================
void FillBHCalHitHistograms(const Options& opt = DefaultOptions) {

  // announce start of macro
  std::cout << "\n  Beginning BHCa hit histogram-filling macro!" << std::endl;

  // --------------------------------------------------------------------------
  // Open input/outputs
  // --------------------------------------------------------------------------

  // open file w/ frame reader
  podio::ROOTFrameReader reader = podio::ROOTFrameReader();
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

  // --------------------------------------------------------------------------
  // Make histograms
  // --------------------------------------------------------------------------

  // binnings
  std::map<std::string, std::tuple<uint64_t, float, float>> bins = {
    {"eta", std::make_tuple(24,  -1.1,   1.1)},
    {"phi", std::make_tuple(320, -3.15, 3.15)},
    {"num", std::make_tuple(200, -0.5,  199.5)}
  };

  // turn on histogram errors
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // make 1d histograms
  TH1D* hHitEta = new TH1D("hHitEta", "", get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]));
  TH1D* hHitPhi = new TH1D("hHitPhi", "", get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"]));
  TH1D* hHitNum = new TH1D("hHitNum", "", get<0>(bins["num"]), get<1>(bins["num"]), get<2>(bins["num"]));

  // make 2d histograms
  TH2D* hHitPhiVsEta = new TH2D(
    "hHitPhiVsEta",
    "", 
    get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]),
    get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"])
  );

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

    // grab frame
    auto frame = podio::Frame( reader.readNextEntry(podio::Category::Event) );

    // grab needed collections
    auto& genParticles = frame.get<edm4eic::ReconstructedParticleCollection>( opt.gen_par );
    auto& hcalHits     = frame.get<edm4eic::CalorimeterHitCollection>( opt.hcal_hit );

    // ------------------------------------------------------------------------
    // hcal hit loop
    // ------------------------------------------------------------------------
    for (edm4eic::CalorimeterHit hit : hcalHits) {

      // get eta/phi
      const double hHit = edm4hep::utils::eta( hit.getPosition() );
      const double fHit = edm4hep::utils::angleAzimuthal( hit.getPosition() );

      // fill hit histograms
      hHitEta      -> Fill(hHit);
      hHitPhi      -> Fill(fHit);
      hHitPhiVsEta -> Fill(hHit, fHit);

    }  // end hcal cluster loop

    // fill event histograms
    hHitNum -> Fill( hcalHits.size() );

  }  // end frame loop
  std::cout << "    Finished frame loop" << std::endl;

  // save output & close files
  output       -> cd();
  hHitEta      -> Write();
  hHitPhi      -> Write();
  hHitNum      -> Write();
  hHitPhiVsEta -> Write();
  output       -> Close();

  // announce end & exit
  std::cout << "  End of macro!\n" << std::endl;
  return;

}

// end ========================================================================
