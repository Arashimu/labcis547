#!/bin/bash

cd /home/srh/cis547/lab7/build 
make  
cd ../test 
opt -load ../build/DivZeroPass.so -enable-new-pm=0 -DivZero test14.ll

