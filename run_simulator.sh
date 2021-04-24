#!/bin/sh
# Run Simulator on input traces 

input_file=${1:-'traces/'}   #Pass absolute path
cd Simulator/src/

for entry in $input_file/*.out
do
  f=$(echo "${entry##*/}");
  tracename=$(echo $f| cut  -d'.' -f 1);
  echo $tracename	
  filename="${tracename}_stats.out"
  echo "Running $tracename on simulator"
  time ./cache_simulator.py -pdc ../config/config_simple_multilevel -t $entry | tee stats.txt
  mv cache_simulator.log $filename  
done
cp *.out $input_file/
cd -
