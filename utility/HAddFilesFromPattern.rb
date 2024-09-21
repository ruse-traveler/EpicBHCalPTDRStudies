#!/usr/bin/env ruby
# -----------------------------------------------------------------------------
# 'HAddFilesFromPattern.rb'
# Derek Anderson
# 10.11.2023
#
# Runs hadd over set of files matching a certain pattern...
# -----------------------------------------------------------------------------

# modules to use
require 'fileutils'

# input parameters
in_path = "./singles/neutrons/npsim"
in_pref = "forLowThresholdCheck.withNPSim_run"
in_suff = "th45n5Kneu.d3m10y2023.plugin.root"

# output parameters
out_file = "forLowThresholdCheck.withDDSim.epic23080image.e220th45n250Kneu.d11m10y2023.plugin.root"

# create input matching pattern
in_pattern = in_path + "/" + in_pref + "*" + in_suff
in_pattern.gsub!("//", "/")
in_pattern.gsub!("..", ".")

# create input argument
arg_input = ""
num_input = Dir[in_pattern].size
Dir[in_pattern].each_with_index do |file, iFile|
  arg_input += file
  arg_input += " " if iFile + 1 != num_input
end

# run hadd
exec("hadd #{out_file} #{arg_input}")

# end -------------------------------------------------------------------------
