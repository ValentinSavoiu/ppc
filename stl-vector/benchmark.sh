#!/bin/bash

threadNumber=$1
BASEDIR=$(dirname "$0")

for t in ${threadNumber[@]}; do
    export "OMP_NUM_THREADS=$t"
    g++ -fopenmp $BASEDIR/stl-vector.cpp -o $BASEDIR/ex && $BASEDIR/ex $2
done