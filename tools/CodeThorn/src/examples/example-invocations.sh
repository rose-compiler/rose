# polybench3.2 programs (generate fp-variable update sequence)
codethorn polybench3.2/jacobi-1d-imper_orig.c -Ipolybench3.2 --dump-sorted=result.check --dump-non-sorted=result.nscheck --specialize-fun-name=kernel_jacobi_1d_imper --specialize-fun-param=0 --specialize-fun-const=2 --specialize-fun-param=1 --specialize-fun-const=16

# rers 2014 program
codethorn rers-eval/Problem1401.c --input-values={"1,2,3,4,5"} --threads=12 --rersmode=yes
