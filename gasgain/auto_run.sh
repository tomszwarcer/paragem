#!/bin/bash


#initial jobs
python3 gainArgGen.py
printf "all_runs.txt generated\n" >> auto_run.log
condor_submit batch_gem.sub
echo "Jobs added.\n" >> auto_run.log
sleep 14400s
condor_rm upb99365
echo "Jobs removed.\n" >> auto_run.log
python3 missingArgGen.py
printf "all_runs.txt re-generated\n" >> auto_run.log

while [ -s all_runs.txt ]
do 
condor_submit batch_gem.sub
echo "Jobs added.\n" >> auto_run.log
sleep 3600s
condor_rm upb99365
echo "Jobs removed.\n" >> auto_run.log
python3 missingArgGen.py
printf "all_runs.txt re-generated\n" >> auto_run.log
done
printf "done\n" >> auto_run.log