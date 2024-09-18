# Reconstruction

This directory consolidates code associated with running EICrecon or interfacing with
its output.  The three different sub-directories hold different types of code:

  - `macros`: ROOT/PODIO-based macros for reading in output from EICrecon;
  - `plugins`: EICrecon plugins to be run in parallel with EICrecon; and
  - `scripts`: bash/ruby/etc. scripts for running macros, EICrecon, and otherwise.

Detailed usage of specific pieces of code are detailed below.



## macros/FillBHCalClusterCalibrationTuple.cxx

A ROOT+PODIO macro to process EICrecon output (from the simulation campaign or otherwise).
This fills a TNtuple which holds information on the clusters from the BHCal and BIC used to
train an ML model for calibrating the combined BHCal+BIC response when reconstructing
particle energies.

### Input
---------

Ingests output from EICrecon, either `*.edm4eic.tree.root` or `*.podio.root`. However, it's
curently designed to work only **on single particle events.**

### Usage
---------

Can be run just like a typical ROOT macro:

```
root -b -q FillBHCalClusterCalibrationTuple.cxx
```

All options are consolidated into the `Options` struct at the top of the macro:

```
struct Options {
  std::string in_file;      // input file
  std::string out_file;     // output file
  std::string gen_par;      // generated particles
  std::string hcal_clust;   // hcal cluster collection
  std::string ecal_clust;   // ecal (scfi + imaging) cluster collection
  std::string scfi_clust;   // ecal (scfi) cluster collection
  std::string scfi_hits;    // ecal (scfi) hit collection
  std::string image_clust;  // ecal (imaging) cluster/layer collection
  std::string image_hits;   // ecal (imaging) hit collection
  bool        do_progress;  // print progress through frame loop
}
```

Options can be adjusted on the command-line via:

```
root -b -q "FillBHCalClusterCalibrationTuple.cxx({\
  .in_file = \"test_in.root\",\
  .out_file = \"test_out.root\",\
  .gen_par = \"GeneratedParticles\",\
  .hcal_clust = \"HcalBarrelClusters\",\
  .ecal_clust = \"EcalBarrelClusters\",\
  .scfi_clust = \"EcalBarrelScFiClusters\",\
  .scfi_hits = \"EcalBarrelScFiRecHits\",\
  .image_clust = \"EcalBarrelImagingLayers\",\
  .image_hits = \"EcalBarrelImagingRecHits\",\
  .do_progress = false\
})"
```



## plugins/FillBHCalClusterCalibrationTupleProcessor.{cc,h}

This EICrecon plugin fills the same function as `FillBHCalClusterCalibrationTuple.cxx`.
It's included here primarily for reference.

### Input
---------

Since this is an EICrecon plugin, it runs on `*.edm4hep.root` files.  However, it's  currently
designed to **only work on single particle events.**

###  Usage
----------

After compiling `EICrecon`, autogenerate relevant cmake files with:

```
eicmkplugin.py FillBHCalCalibrationTuple
```

Next copy `plugins/FillBHCalClusterCalibrationTupleProcessor.{cc,h}` from this repo to
the `FillBHCalCalibrationTuple` directory in your installation of EICrecon.  Make sure
your `EICrecon_MY` is set:

```
export EICrecon_MY=~/EICrecon_MY`
```

(Or wherever you want the `EICrecon_MY` directory to sit), and then compile with:

```
cmake -S JCalibrateHcal -B FillBHCalCalibrationTuple/build
cmake --build FillBHCalCalibrationTuple/build --target install
```

Finally, plugin can be run with EICrecon like so:

```
eicrecon -Pplugins=FillBHCalCalibrationTuple <input edm4hep file>
```



## plugins/GetRawEnergiesProcessor.{cc,h}

This EICrecon plugin fills a set of histograms of information from a set of "simulated hits"
(sum of all G4hits for a given sensitive volume), "raw hits" (digitized simulated hits), and
"reconstructed hits" (calorimeter cells reconstructed from the raw hits).

### Input
---------

Being an EICrecon plugin, it runs on `*.edm4hep.root`, regardless of what type of event.

## Usage
--------

Identical to `FillBHCalClusterCalibrationTupleProcessor`. Only change is that you should do

```
eicmkplugin.py GetRawEnergies
```

when generating the cmake files.
