#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'RunNPSim.rb'
# Derek Anderson
# 10.30.2023
#
# Run a certain number events in npsim based on
# specified steering and compact files.
# -----------------------------------------------------------------------------

# output file
out_file = "test.npsim_output.edm4hep.root"

# simulation parameters
numevts = 100
steerer = "../steering/SingleParticle/pion/central.e15pim.py"
compact = "$DETECTOR_PATH/epic.xml"

# run ddsim
exec("npsim --steeringFile #{steerer} --compactFile #{compact} -G -N #{numevts} --outputFile #{out_file}")

# end -------------------------------------------------------------------------
