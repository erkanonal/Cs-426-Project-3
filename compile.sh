#!/bin/bash
clear
gcc -Wall -no-pie -pg -o lbp_seq lbp_seq.c util.h util.c -lm
gcc -Wall -no-pie -pg -o lbp_omp -fopenmp lbp_omp.c util.h util.c -lm
echo "Compiled"
