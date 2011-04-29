include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external

all: obj obj/arrIndexLabeler.o

obj/arrIndexLabeler.o : arrIndexLabeler.C arrIndexLabeler.h ${common_H} 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} ${common_INC} -fPIC -c -o $@ arrIndexLabeler.C

clean:
	rm -f obj/* 