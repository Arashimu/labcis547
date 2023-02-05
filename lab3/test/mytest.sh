#!/bin/sh

mkdir "fuzz_output"

timeout 5s ../build/fuzzer  ./sanity1 fuzz_input fuzz_output

