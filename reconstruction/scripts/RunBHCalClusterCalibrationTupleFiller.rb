#!/usr/bin/env ruby
# =============================================================================
# @file   RunBHCalClusterCalibrationTupleFiller.rb
# @author Derek Anderson
# @date   09.10.2024
#
# An easy script to run the macro `FillBHCalClusterCalibrationTuple.cxx`.
# =============================================================================

require 'optparse'
require 'ostruct'

# input/output files (can be set from command line)
input  = "input/pi+_10GeV_45to135deg.0002.eicrecon.tree.edm4eic.root"
output = "script_test.tuple.root"

# TODO allow for input/output to be specified

# branches to process
particles   = "GeneratedParticles"
hcalclust   = "HcalBarrelClusters"
ecalclust   = "EcalBarrelClusters"
scficlust   = "EcalBarrelScFiClusters"
scfihit     = "EcalBarrelScFiRecHits"
imageclust  = "EcalBarrelImagingLayers"
imagehit    = "EcalBarrelImagingRecHits"

# turn on/off event counter
progress = true

# make command to run
command = <<STR
  root -b -q \'FillBHCalClusterCalibrationTuple.cxx({\
    .in_file     = "#{input}",\
    .out_file    = "#{output}",\
    .gen_par     = "#{particles}",\
    .hcal_clust  = "#{hcalclust}",\
    .ecal_clust  = "#{ecalclust}",\
    .scfi_clust  = "#{scficlust}",\
    .scfi_hits   = "#{scfihit}",\
    .image_clust = "#{imageclust}",\
    .image_hits  = "#{imagehit}",\
    .do_progress = #{progress}\
  })\'
STR

# run macro
exec(command);

# end =========================================================================
