TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load /home/srh/cis547/lab4/lib/InstrumentPass.so -enable-new-pm=0 -Instrument -S $@.ll -o $@.instrumented.ll
	clang -o $@ -L../lib -lruntime -lm $@.instrumented.ll

fuzz-%: %
	@./test.sh $< 6s

clean:
	rm -rf *.ll *.cov ${TARGETS} core.* fuzz_output* out_*.txt
