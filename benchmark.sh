#!/bin/bash

if [[ $# -eq 0 ]]
then
  echo "Invalid options (try '-h')"
fi

if [[ $1 == '-h' || $1 == '--help' ]]
then
  echo "Usage: benchmark [-h|--help] PROGRAM_STRING [n]"
  echo "  OPTIONS:"
  echo "    -h, --help      print this help message"
  echo "    n               set the number of trials to"
  echo "                    be executed (default is 1)"
  echo "\n\n  PROGRAM_STRING is the actual program comprised"
  echo "   of arguments. This parameter has to enquoted"
  exit 0
fi

# Check for the number of trials
re='^[0-9]+$'

if ! [[ $2 =~ $re ]]; then
  echo "number of trials not valid!"
  exit 1
fi

i=0
tot=0
echo "Args: $2"
while [[ $i -lt $2 ]]
do
  res=$(./runlim $1 2>&1 | tail -n 4 | head -n 1)
  t=$(echo $res | cut -d' ' -f3)
  tot=$(echo "$tot+$t" | bc -l)
  i=$(echo "$i+1" | bc -l)
done

r=$(echo "$tot/$2" | bc -l | cut -c 1-5)
echo "Avg time: $r"