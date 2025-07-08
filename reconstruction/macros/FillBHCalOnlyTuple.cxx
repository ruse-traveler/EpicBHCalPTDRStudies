/// ===========================================================================
/*! \file   FillBHCalOnlyTuple.cxx
 *  \author Derek Anderson
 *  \date   10.31.2024
 *
 *  A ROOT macro to read EICrecon output (either `*.podio.root` or
 *  `*.tree.edm4eic.root` and fill an NTuple for making performance
 *  plots for BHCal only sim.s
 */
/// ===========================================================================

#define FillBHCalOnlyTuple_cxx

// root libraries
#include <TFile.h>
#include <TNtuple.h>
#include <TSystem.h>
// podio libraries
#include <podio/Frame.h>
#include <podio/ROOTReader.h>
#include <podio/CollectionBase.h>
// edm4eic types
#include <edm4eic/ClusterCollection.h>
#include <edm4eic/CalorimeterHitCollection.h>
#include <edm4eic/ReconstructedParticleCollection.h>
// edm4hep types
#include <edm4hep/Vector3f.h>
#include <edm4hep/utils/vector_utils.h>
// analysis utilities
#include <utility/NTupleHelper.hxx>



// ============================================================================
//! Struct to consolidate user options
// ============================================================================
struct Options {
  std::string in_file;      // input file
  std::string out_file;     // output file
  std::string gen_par;      // generated particles
  std::string hcal_clust;   // hcal cluster collection
  bool        do_progress;  // print progress through frame loop
  bool        do_clust_cut; // require sum of cluster energy to be nonzero
} DefaultOptions = {
  "./reco/forBHCalOnlyCheck.evt5Ke1pim_central.d31m10y2024.podio.root",
  "forBHCalOnlyCheck.evt5ke1pim_central.d31m10y2024.tuple.root",
  "GeneratedParticles",
  "HcalBarrelClusters",
  true,
  false
};



// ============================================================================
//! Fill BHCal-only NTuple
// ============================================================================
void FillBHCalOnlyTuple(const Options& opt = DefaultOptions) {

  // --------------------------------------------------------------------------
  // calculation parameters
  // --------------------------------------------------------------------------

  // output variables
  NTupleHelper helper({
    "ePar",
    "fracParVsLeadBHCal",
    "fracParVsSumBHCal",
    "eLeadBHCal",
    "eSumBHCal",
    "diffLeadBHCal",
    "diffSumBHCal",
    "nHitsLeadBHCal",
    "nClustBHCal",
    "hLeadBHCal",
    "fLeadBHCal"
  });

  // announce start of macro
  std::cout << "\n  Beginning BHCal only tuple-filling macro!" << std::endl;

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

  // create output ntuple
  TNtuple* ntOutput = new TNtuple("ntBHCalOnly", "NTuple for BHCal only plots", helper.CompressVariables().c_str());

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
    auto& genParticles  = frame.get<edm4eic::ReconstructedParticleCollection>( opt.gen_par );
    auto& hcalClusters  = frame.get<edm4eic::ClusterCollection>( opt.hcal_clust );

    // reset output values
    helper.ResetValues();

    // ------------------------------------------------------------------------
    // particle loop
    // ------------------------------------------------------------------------
    std::optional<edm4eic::ReconstructedParticle> optPrimary = std::nullopt;
    for (edm4eic::ReconstructedParticle particle : genParticles) {
      if (particle.getType() == 1) {
        optPrimary = particle;
        break;
      }
    }  // end particle loop

    // skip event if no primary found
    if (!optPrimary.has_value()) {
      continue;
    }
    edm4eic::ReconstructedParticle primary = optPrimary.value();

    // set particle output variables
    helper.SetVariable( "ePar", primary.getEnergy() );

    // ------------------------------------------------------------------------
    // hcal cluster loop
    // ------------------------------------------------------------------------
    edm4eic::Cluster hLeadClust;

    // find leading cluster, sum energies
    float eSumHCal  = 0.;
    float eLeadHCal = 0.;
    for (edm4eic::Cluster hClust : hcalClusters) {

      if (hClust.getEnergy() > eLeadHCal) {
        hLeadClust = hClust;
        eLeadHCal  = hClust.getEnergy();
      }
      eSumHCal += hClust.getEnergy();

    }  // end hcal cluster loop

    // if needed, apply cluster cut
    if ( opt.do_clust_cut ) {
      if (eSumHCal <= 0.) continue;
    }

    // fill lead hcal cluster variables
    helper.SetVariable( "eLeadBHCal", hLeadClust.getEnergy() );
    helper.SetVariable( "nHitsLeadBHCal", (float) hLeadClust.getHits().size() );
    helper.SetVariable( "hLeadBHCal", edm4hep::utils::eta(hLeadClust.getPosition()) );
    helper.SetVariable( "fLeadBHCal", edm4hep::utils::angleAzimuthal(hLeadClust.getPosition()) );

    // fill event-level output variables
    helper.SetVariable( "eSumBHCal", eSumHCal);
    helper.SetVariable( "nClustBHCal", (float) hcalClusters.size());
    helper.SetVariable( "fracParVsSumBHCal", eSumHCal / primary.getEnergy());
    helper.SetVariable( "fracParVsLeadBHCal", hLeadClust.getEnergy() / primary.getEnergy());
    helper.SetVariable( "diffSumBHCal", (eSumHCal - primary.getEnergy()) / primary.getEnergy());
    helper.SetVariable( "diffLeadBHCal", (hLeadClust.getEnergy() - primary.getEnergy()) / primary.getEnergy());

    // ------------------------------------------------------------------------
    // fill ntuple
    //  -----------------------------------------------------------------------
    ntOutput -> Fill( helper.GetValues().data() );

  }  // end frame loop
  std::cout << "    Finished frame loop" << std::endl;

  // save output & close files
  output   -> cd();
  ntOutput -> Write();
  output   -> Close();

  // announce end & exit
  std::cout << "  End of macro!\n" << std::endl;
  return;

}

// end ========================================================================
