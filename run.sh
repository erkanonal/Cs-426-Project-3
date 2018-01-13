#!/bin/bash
clear

truncate -s 0 onal_erkan.output

#serial part
for ((k=1; k<=18; k+=1))
do	
 	./lbp_seq "$k" >> onal_erkan.output
done

#parallel part
for ((k=1, NUM_THREADS=1; k<=16; k++,NUM_THREADS+=1))
do	
 	export "OMP_NUM_THREADS=$k" && ./lbp_omp "$k" >> onal_erkan.output
done
