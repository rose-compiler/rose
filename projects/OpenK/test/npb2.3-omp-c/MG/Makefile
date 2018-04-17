SHELL=/bin/sh
BENCHMARK=mg
BENCHMARKU=MG

include ../config/make.def

OBJS = mg.o ${COMMON}/c_print_results.o  \
       ${COMMON}/c_${RAND}.o ${COMMON}/c_timers.o ${COMMON}/c_wtime.o

include ../sys/make.common

${PROGRAM}: config ${OBJS}
	${CLINK} -o ${PROGRAM} ${OBJS} ${C_LIB} ${CLINKFLAGS}

mg.o:		mg.c npbparams.h
	${CCOMPILE} mg.c

clean:
	- rm -f *.o *~ 
	- rm -f npbparams.h core



