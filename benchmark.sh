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
file_list=$(seq 0 5) # list of the indeces of the files used for the benchmark

algorithms=() # list of algorithms to be benchmarked

log_file="output_results.txt"

# main script
function print_res() {
  t=$1
  alg=$2
  input=$3
  echo ""
  echo "$t threads working...";
  echo "input: #$input",
  echo "algorithm: ${algs[$alg]}"

  write_on_file ""
  write_on_file "$t threads working...";
  write_on_file "input: #$input",
  write_on_file "algorithm: ${algs[$alg]}"
  
}

function write_on_file(){
	echo "$1" >> $log_file
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


echo "" > output_results.txt
#multithreading loops csr
for i in ${file_list[@]}; do
  print_res "1" "0" "$i"
  res="$(./runlim.sh "./GraphColoring -i $i -r $internal_representation" $ntrials | tail -n 2)" #sequential run
  echo "$res"
  write_on_file "$res"
  for ((t = 2; t <= nthread; t=t*2)); do #multithreading
    for a in ${algorithms[@]}; do
      print_res "$t" "$a" "$i"
      res="$(./runlim.sh "./GraphColoring -i $i -a $a -t $t -r $internal_representation" $ntrials | tail -n 2)"
      echo "$res"
	  write_on_file "$res"
    done
  done
done

exit 0