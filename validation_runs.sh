#!/bin/bash
./sim_proc 16 8 1 val_trace_gcc1 > val.1

for i in 1
# 2 3 4 5 6 7
do

  echo "trace$i"
  diff -iw val.$i z_Resources/validation/val$i.txt
 # rm val.$i
done