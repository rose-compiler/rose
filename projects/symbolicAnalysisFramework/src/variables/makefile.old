Component = variables
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external

all: obj obj/variables.o obj/varSets.o 

full: obj obj/variables.o obj/varSets.o selfTest 
	selfTest test_example.c

obj/variables.o : variables.C variables.h ${common_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${cfgUtils_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  -I../cfgUtils -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ variables.C

obj/varSets.o : varSets.C varSets.h ${common_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${cfgUtils_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  -I../cfgUtils -I../arrIndexLabeler -I../rwAccessLabeler -I../cfgUtils -fPIC -c -o $@ varSets.C

obj/selfTest.o : selfTest.C variables.h varSets.h ${common_H} ${common_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${cfgUtils_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  -I../cfgUtils -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ selfTest.C
	
selfTest : obj/selfTest.o obj/variables.o obj/varSets.o cfgUtils rwAccessLabeler arrIndexLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++ -g -fPIC  -o selfTest \
	        obj/selfTest.o obj/variables.o obj/varSets.o ${cfgUtils_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest
