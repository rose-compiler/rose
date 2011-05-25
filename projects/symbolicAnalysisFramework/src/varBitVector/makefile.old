Component = varBitVector
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 

all: obj obj/varBitVector.o 

full: obj obj/varBitVector.o selfTest
	selfTest test_example.c

obj/varBitVector.o : varBitVector.C varBitVector.h ${common_H} ${cfgUtils_H} ${variables_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  \
	    -I../variables -I../cfgUtils  \
	    -fPIC -c -o $@ varBitVector.C

obj/selfTest.o : selfTest.C varBitVector.h ${common_H} ${cfgUtils_H} ${variables_H} 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  \
	-I../variables -I../cfgUtils  \
	-fPIC -c -o $@ selfTest.C

selfTest : obj/varBitVector.o obj/selfTest.o variables cfgUtils rwAccessLabeler
	/bin/sh ${ROSE_BUILD_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o obj/varBitVector.o ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ}  \
	        ${ROSE_BUILD_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest