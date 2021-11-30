#!/bin/bash
#### CONSTANTS - NOT TOUCH
algs=(
  "sequential", #0
  "jones plassman", #1
  "largest degree v3 (jp structure)", #2
  "smallest degree" #3,
  "largest degree v2 (without STL)", #4
  "largest degree v1 (STL implementation, old)", #5
)
nfileinput=$(./GraphColoring -p ./Graph/benchmark/ -l | grep ^[0-9] | wc -l | bc -l)

###### Editable parameters

# file indices are the same as shown when running GraphColoring -p path/to/benchmark -l
first_file_input=0 # The first file index to be used for the benchmark
last_file_input=$nfileinput # last file index for the benchmark

first_algorithm=2 # first algorithm for the benchmark (as in the above list algs)
last_algorithm=2 # last algorithm for the benchmark

# main script
function print_res() {
  t=$1
  alg=$2
  input=$3
  echo ""
  echo "$t threads working...";
  echo "input: #$input",
  echo "algorithm: ${algs[$alg]}"
}

if [[ $# -eq 0 ]]; then
  echo "Invalid arguments (try '-h')"
  exit
fi
if [[ $# -gt 2 ]]; then
  echo "Too many arguments (try '-h' for help)"
  exit
fi

if [[ $1 == '-h' || $1 == '--help' ]]; then
  echo "Usage: benchmark [-h|--help]"
  echo "  OPTIONS:"
  echo "    -h, --help      print this help message"
  echo "    [n]             set the number of threads to use in the benchmark, depending on your own pc!"
  echo ""
  exit
fi

if [[ $# -eq 1 ]]
then
  ntrials=1
else
  ntrials=$2
fi

if [[ $1 -le 1 ]]; then
  echo "Can't run the program with less than 2 threads"
  exit -1
fi

nthread=$1
internal_representation=0
#echo $nfileinput

echo "*** GRAPHCOLORING FINAL BENCHMARK ***"
echo "- default number trials equals to 1"

#multithreading loops csr
for ((i = 0; i < nfileinput; i++)); do
  print_res "1" "0" "$i"
  res="$(./runlim.sh "./GraphColoring -i $i -r $internal_representation" $ntrials | tail -n 2)" #sequential run
  echo "$res"
  for ((t = 2; t <= nthread; t=t*2)); do #multithreading
    for ((a = first_algorithm; a <= last_algorithm; a++)); do
      print_res "$t" "$a" "$i"
      res="$(./runlim.sh "./GraphColoring -i $i -a $a -t $t -r $internal_representation" $ntrials | tail -n 2)"
      echo "$res"
    done
  done
done

exit 0