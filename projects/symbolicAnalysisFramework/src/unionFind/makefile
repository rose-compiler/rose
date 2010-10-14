Component = unionFind
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 

all: obj obj/unionFind.o 

full: obj obj/unionFind.o selfTest
	selfTest test_example.c

obj/unionFind.o : unionFind.C unionFind.h ${common_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${variables_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} \
	    -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler \
	    -fPIC -c -o $@ unionFind.C

obj/selfTest.o : selfTest.C unionFind.h ${common_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${variables_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} \
	-I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler \
	-fPIC -c -o $@ selfTest.C

selfTest : obj/unionFind.o obj/selfTest.o variables cfgUtils rwAccessLabeler arrIndexLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o obj/unionFind.o \
	        ${cfgUtils_OBJ} ${arrIndexLabeler_OBJ} ${rwAccessLabeler_OBJ} ${variables_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest