#!/bin/bash
INPUT_arpercent=$1
INPUT_runnum=$2

echo "root -b -q 'scint_perev_par.C(${INPUT_arpercent}, ${INPUT_runnum})'"
root -b -q 'scint_perev_par.C(${INPUT_arpercent},${INPUT_runnum})'