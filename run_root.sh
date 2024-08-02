#!/bin/bash
INPUT_arpercent=$1
INPUT_runnum=$2

cd /opt/ppd/scratch/szwarcer/paragem
source /cvmfs/sft.cern.ch/lcg/views/LCG_104c/x86_64-el9-gcc13-opt/setup.sh
export PATH=/opt/ppd/darkmatter/elmer/install/bin/:$PATH
export GARFIELD_HOME=/opt/ppd/darkmatter/garfield-new
source /opt/ppd/darkmatter/garfield-new/build/setupGarfield.sh
root.exe -b -q "scint_perev_par.C($INPUT_arpercent,$INPUT_runnum)"
cd output