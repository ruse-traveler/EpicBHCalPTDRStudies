/// ===========================================================================
/*! \file    ClusterMergedHitsProcessor.h
 *  \authors Derek Anderson
 *  \date    05.28.2025
 *
 *  A small EICrecon plugin to cluster
 *  merged BHCal hits.
 */
/// ===========================================================================

#include <JANA/JEventProcessorSequentialRoot.h>
#include <TH2D.h>
#include <TFile.h>



// ============================================================================
//! Cluster Merged BHCal Hits
// ============================================================================
/*! A small EICrecon plugin to cluster
 *  merged BHCal hits.
 */
class ClusterMergedHitsProcessor : public JEventProcessorSequentialRoot {

  private:

    /* TODO put any things here */

  public:

    // ctor
    ClusterMergedHitsProcessor() { SetTypeName(NAME_OF_THIS); }

    // inherited plugin methods
    void InitWithGlobalRootLock() override;
    void ProcessSequential(const std::shared_ptr<const JEvent>& event) override;
    void FinishWithGlobalRootLock() override;

};  // end ClusterMergedHitsProcessor

/// end =======================================================================
