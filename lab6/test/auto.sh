#! /bin/bash

cd ../build
make
cd ../test
opt -load ../build/DivZeroPass.so -enable-new-pm=0 -DivZero -disable-output test11.opt.ll
