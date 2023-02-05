TARGETS=simple0 simple1 simple2 simple3 simple4 simple5 simple6 simple7 simple8 simple9


all: simple

simple: $(TARGETS)

export LD_LIBRARY_PATH=${PWD}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load /home/srh/cis547/lab2/build/StaticAnalysisPass.so  -StaticAnalysisPass -enable-new-pm=0 -S $@.ll -o $@.static.ll
	opt -load /home/srh/cis547/lab2/build/DynamicAnalysisPass.so  -DynamicAnalysisPass -enable-new-pm=0 -S $@.ll -o $@.dynamic.ll
	clang -o $@ -L${PWD}/../build  -lruntime $@.dynamic.ll 

# %: %.c
# 	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
# 	opt -load /home/srh/cis547/lab2/build/StaticAnalysisPass.so  -StaticAnalysisPass -enable-new-pm=0 -S $@.ll -o $@.static.ll
# 	opt -load /home/srh/cis547/lab2/build/DynamicAnalysisPass.so  -DynamicAnalysisPass -enable-new-pm=0 -S $@.ll -o $@.dynamic.ll
# 	clang -o $@  -lruntime $@.dynamic.ll
clean:
	rm -f *.ll *.*cov *.binops ${TARGETS}
