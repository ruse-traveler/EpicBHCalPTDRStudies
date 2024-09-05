# ePIC BHCal pTDR Studies

This repository tracks code used in studies for the BHCal chapter of
the ePIC pTDR. It is structured like so:

  - `generation`: this directory holds code used to run the
    DD4hep simulation of [ePIC](https://github.com/eic/epic)
    including steering files, shell scripts, etc.
  - `reconstruction`: this directory holds code used to interface
    with the output of [EICrecon](https://github.com/eic/EICrecon)
    including PODIO-based ROOT macros, EICrecon plugins, etc.
  - `calibration`: this directory holds code used to calibrate
    the BHCal response in various contexts, e.g. using TMVA
    to calibrate the energy of BHCal+BIC clusters.
  - `plotting`: this directory holds various ROOT macros
    used to generate plots.
  - `utility`: this directory holds a variety of useful tools
    like the `eic-build` script.

**NOTE:** the code here will eventually be integrated into a
repository under the [eic GitHub organization](https://github.com/eic).
