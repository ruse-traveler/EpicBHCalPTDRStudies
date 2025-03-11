#!/usr/bin/env ruby
# =============================================================================
# @file   RunEICRecon.rb
# @author Derek Anderson
# @date   05.11.2023
#
# An easy script to interface with EICRecon
# =============================================================================

# i/o parameters
in_ddsim   = "./test.npsim_output.edm4hep.root"
out_podio  = "test.eicrecon_output.edm4eic.root"
out_plugin = "test.eicrecon_output.plugin.root"

# output collections from EICrecon
out_collect = [
].compact.reject(&:empty?).join(',')

# plugins to run in EICrecon
plugins = [
].compact.reject(&:empty?).join(',')

# options
options = [
  "-Peicrecon:LogLevel=debug"
].compact.reject(&:empty?).join(' ')

# run EICrecon
exec("eicrecon -Pplugins=#{plugins} -Ppodio:output_collections=#{out_collect} #{options} -Ppodio:output_file=#{out_podio} #{in_ddsim}")

# end =========================================================================
