#!/bin/bash
#./runlim.sh './GraphColoring 0 0 0 2' 10

if [[ $# -eq 0 ]]
then
  echo "Invalid options (try '-h')"
fi

if [[ $1 == '-h' || $1 == '--help' ]]
then
  echo "Usage: benchmark [-h|--help] PROGRAM_STRING [n]"
  echo "  OPTIONS:"
  echo "    -h, --help      print this help message"
  echo "    [n]             set the number of trials to specified number (default is 1)"
  echo ""
  echo "  PROGRAM_STRING is the actual program with arguments. This parameter has to be enquoted"
  exit 0
fi

# Check for PROGRAM_STRING
regexp1='.*GraphColoring.*'

if ! [[ $1 =~ $regexp1 ]]; then
  echo "PROGRAM_STRING not valid!"
  exit 1
fi

# Check for the number of trials
regexp='^[0-9]+$'

if ! [[ $2 =~ $regexp ]]; then
  echo "number of trials not valid!"
  exit 1
fi

echo "*** GRAPHCOLORING BENCHMARK RUNNER ***"
i=0
tot=0
space_tot=0;
echo "Number of trials: $2"
while [[ $i -lt $2 ]]
do
  res=$(./runlim $1 2>&1 | tail -n 4) #prende risultato "real"
  #echo $res  #DEBUG
  t=$(echo $res | cut -d' ' -f3) #real time
  tot=$(echo "$tot+$t" | bc -l)
  space=$(echo "$res" | tail -n 2 | head -n 1 | cut -d' ' -f2 | cut -d':' -f2 | sed 's/^[^0-9]*\([0-9]*'.'[0-9]\).*$/\1/')
  space_tot=$(echo "$space_tot+$space" | bc -l)
  i=$(echo "$i+1" | bc -l)
  echo "Time of trial #$i: $t s"
done

r=$(echo "$tot/$2" | bc -l | cut -c 1-5)
r2=$(echo "space_tot/$2" | bc -l | cut -c 1-5)
echo "Avg time: $r s"
echo "space: $space MB"