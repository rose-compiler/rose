include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external

all: obj obj/rwAccessLabeler.o

obj/rwAccessLabeler.o : rwAccessLabeler.C rwAccessLabeler.h ${common_H} 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} ${common_INC} -fPIC -c -o $@ rwAccessLabeler.C

clean:
	rm -f obj/* 