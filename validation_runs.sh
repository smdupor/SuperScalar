#!/bin/bash
./sim 16 8 1 val_trace_gcc1 > val.1
./sim 16 8 2 val_trace_gcc1 > val.2
./sim 60 15 3 val_trace_gcc1 > val.3
./sim 64 16 8 val_trace_gcc1 > val.4
./sim 64 16 4 val_trace_perl1 > val.5
./sim 128 16 5 val_trace_perl1 > val.6
./sim 256 64 5 val_trace_perl1 > val.7
./sim 512 64 7 val_trace_perl1 > val.8


for i in 1 2 3 4 5 6 7 8
do

  echo "trace$i"
  diff -iw val.$i z_Resources/validation/val$i.txt
  rm val.$i
done