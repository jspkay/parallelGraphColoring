#!/bin/bash

algs=(
  "sequential", #0
  "jones plassman", #1
  "largest degree v3 (jp structure)", #2
  "smallest degree" #3,
  "largest degree v2 (without STL)", #4
  "largest degree v1 (STL implementation, old)", #5
)

function print_res() {
  alg=$1
  input=$2
  toPrint=$3
  echo "input: #$input",
  echo "algorithm: ${algs[$alg]}"
  echo "$toPrint"
  echo ""
}

if [[ $# -eq 0 ]]; then
  echo "Invalid options (try '-h')"
fi

if [[ $1 == '-h' || $1 == '--help' ]]; then
  echo "Usage: benchmark [-h|--help]"
  echo "  OPTIONS:"
  echo "    -h, --help      print this help message"
  echo "    [n]             set the number of threads to use in the benchmark, depending on your own pc!"
  echo ""
  exit 0
fi

echo "*** GRAPHCOLORING FINAL BENCHMARK ***"
echo "- default number trials equals to 1"
echo ""
ntrials=1
nthread=$1
nfileinput=$(./GraphColoring -p ./Graph/benchmark/ -l | grep ^[0-9] | wc -l | bc -l)
#echo $nfileinput

#sequential loop csr
for ((i = 0; i < nfileinput  ; i++)); do
 print_res "0" "$i" "$(./runlim.sh "./GraphColoring -a 0 -i $i -r 2" $ntrials | tail -n 2)"
done

#multithreading loops csr
for ((a = 1; a <= 5; a++)); do
  for ((i = 0; i < nfileinput; i++)); do
    for ((t = 2; t <= nthread; t++)); do #multithreading
      echo "$t threads working..."
      print_res "$a" "$i" "$(./runlim.sh "./GraphColoring -i $i -a $a -t $t" $ntrials | tail -n 2)"
    done
  done
done

exit 0