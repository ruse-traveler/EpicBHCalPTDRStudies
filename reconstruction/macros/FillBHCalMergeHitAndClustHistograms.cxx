/// ===========================================================================
/*! \file   FillBHCalMergeHitAndClustHistograms.cxx.cxx
 *  \author Derek Anderson
 *  \date   11.06.2024
 *
 *  A ROOT macro to read EICrecon output (either `*.podio.root` or
 *  `*.tree.edm4eic.root` and fill a couple histograms of hits in
 *  the BHCal.
 */
/// ===========================================================================

#define FillBHCalMergeHitAndClustHistograms_cxx

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
#include <podio/ROOTReader.h>
#include <podio/CollectionBase.h>
// edm4eic types
#include <edm4eic/CalorimeterHitCollection.h>
#include <edm4eic/ClusterCollection.h>
// edm4hep types
#include <edm4hep/Vector3f.h>
#include <edm4hep/utils/vector_utils.h>



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;         // input file
  std::string out_file;        // output file
  std::string hcal_rec_hit;    // hcal reco hit collection
  std::string hcal_merge_hit;  // hcal merged hit collection
  std::string hcal_cluster;    // hcal cluster collection
  bool        do_progress;     // print progress through frame loop
} DefaultOptions = {
  "../run/output/forHoleMystery.bhcalOnly_rerun_nMerge5_e10pim.d27m5y2025.podio.root",
  "forBHCalOnlyCheck_checkMerging.evt5Ke10pim_nMerge5_central.d28m5y2025.hists.root",
  "HcalBarrelRecHits",
  "HcalBarrelMergedHits",
  "HcalBarrelClusters",
  true
};



// ============================================================================
//! Fill BHCal-only NTuple
// ============================================================================
void FillBHCalMergeHitAndClustHistograms(const Options& opt = DefaultOptions) {

  // announce start of macro
  std::cout << "\n  Beginning BHCa hit histogram-filling macro!" << std::endl;

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

  // --------------------------------------------------------------------------
  // Make histograms
  // --------------------------------------------------------------------------

  // binnings
  std::map<std::string, std::tuple<uint64_t, float, float>> bins = {
    {"eta", std::make_tuple(24,  -1.1,   1.1)},
    {"phi", std::make_tuple(320, -3.15, 3.15)},
    {"num", std::make_tuple(200, -0.5,  199.5)},
    {"ene", std::make_tuple(200, -0.25, 99.75)}
  };

  // turn on histogram errors
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // make 1d histograms
  TH1D* hRHitEta  = new TH1D("hRecoHitEta",  "", get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]));
  TH1D* hRHitPhi  = new TH1D("hRecoHitPhi",  "", get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"]));
  TH1D* hRHitNum  = new TH1D("hRecoHitNum",  "", get<0>(bins["num"]), get<1>(bins["num"]), get<2>(bins["num"]));
  TH1D* hRHitEne  = new TH1D("hRecoHitEne",  "", get<0>(bins["ene"]), get<1>(bins["ene"]), get<2>(bins["ene"]));
  TH1D* hMHitEta  = new TH1D("hMergeHitEta", "", get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]));
  TH1D* hMHitPhi  = new TH1D("hMergeHitPhi", "", get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"]));
  TH1D* hMHitNum  = new TH1D("hMergeHitNum", "", get<0>(bins["num"]), get<1>(bins["num"]), get<2>(bins["num"]));
  TH1D* hMHitEne  = new TH1D("hMergeHitEne", "", get<0>(bins["ene"]), get<1>(bins["ene"]), get<2>(bins["ene"]));
  TH1D* hClustEta = new TH1D("hClustEta",    "", get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]));
  TH1D* hClustPhi = new TH1D("hClustPhi",    "", get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"]));
  TH1D* hClustNum = new TH1D("hClustNum",    "", get<0>(bins["num"]), get<1>(bins["num"]), get<2>(bins["num"]));
  TH1D* hClustEne = new TH1D("hClustEne",    "", get<0>(bins["ene"]), get<1>(bins["ene"]), get<2>(bins["ene"]));

  // make 2d histograms
  TH2D* hRHitPhiVsEta = new TH2D(
    "hRecoHitPhiVsEta",
    "", 
    get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]),
    get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"])
  );
  TH2D* hMHitPhiVsEta = new TH2D(
    "hMergeHitPhiVsEta",
    "", 
    get<0>(bins["eta"]), get<1>(bins["eta"]), get<2>(bins["eta"]),
    get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"])
  );
  TH2D* hClustPhiVsEta = new TH2D(
    "hClustHitPhiVsEta",
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
    auto& hcalRHits  = frame.get<edm4eic::CalorimeterHitCollection>( opt.hcal_rec_hit );
    auto& hcalMHits  = frame.get<edm4eic::CalorimeterHitCollection>( opt.hcal_merge_hit );
    auto& hcalClusts = frame.get<edm4eic::ClusterCollection>( opt.hcal_cluster );

    // ------------------------------------------------------------------------
    // hcal reco hit loop
    // ------------------------------------------------------------------------
    for (edm4eic::CalorimeterHit rhit : hcalRHits) {

      // get eta/phi
      const double hRHit = edm4hep::utils::eta( rhit.getPosition() );
      const double fRHit = edm4hep::utils::angleAzimuthal( rhit.getPosition() );

      // fill hit histograms
      hRHitEta      -> Fill(hRHit);
      hRHitPhi      -> Fill(fRHit);
      hRHitEne      -> Fill(rhit.getEnergy());
      hRHitPhiVsEta -> Fill(hRHit, fRHit);

    }  // end hcal reco hit loop

    // ------------------------------------------------------------------------
    // hcal merged hit loop
    // ------------------------------------------------------------------------
    for (edm4eic::CalorimeterHit mhit : hcalMHits) {

      // get eta/phi
      const double hMHit = edm4hep::utils::eta( mhit.getPosition() );
      const double fMHit = edm4hep::utils::angleAzimuthal( mhit.getPosition() );

      // fill hit histograms
      hMHitEta      -> Fill(hMHit);
      hMHitPhi      -> Fill(fMHit);
      hMHitEne      -> Fill(mhit.getEnergy());
      hMHitPhiVsEta -> Fill(hMHit, fMHit);

    }  // end hcal merged hit loop

    // ------------------------------------------------------------------------
    // hcal cluster loop
    // ------------------------------------------------------------------------
    for (edm4eic::Cluster clust : hcalClusts) {

      // get eta/phi
      const double hClust = edm4hep::utils::eta( clust.getPosition() );
      const double fClust = edm4hep::utils::angleAzimuthal( clust.getPosition() );

      // fill cluster histograms
      hClustEta      -> Fill(hClust);
      hClustPhi      -> Fill(fClust);
      hClustEne      -> Fill(clust.getEnergy());
      hClustPhiVsEta -> Fill(hClust, fClust);

    }  // end hcal cluster loop

    // fill event histograms
    hRHitNum  -> Fill( hcalRHits.size() );
    hMHitNum  -> Fill( hcalMHits.size() );
    hClustNum -> Fill( hcalClusts.size() );

  }  // end frame loop
  std::cout << "    Finished frame loop" << std::endl;

  // save output & close files
  output         -> cd();
  hRHitEta       -> Write();
  hRHitPhi       -> Write();
  hRHitNum       -> Write();
  hRHitEne       -> Write();
  hMHitEta       -> Write();
  hMHitPhi       -> Write();
  hMHitNum       -> Write();
  hMHitEne       -> Write();
  hMHitPhiVsEta  -> Write();
  hRHitPhiVsEta  -> Write();
  hClustEta      -> Write();
  hClustPhi      -> Write();
  hClustNum      -> Write();
  hClustEne      -> Write();
  hClustPhiVsEta -> Write();
  output         -> Close();

  // announce end & exit
  std::cout << "  End of macro!\n" << std::endl;
  return;

}

// end ========================================================================
