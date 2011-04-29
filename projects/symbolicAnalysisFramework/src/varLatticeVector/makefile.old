Component = varLatticeVector
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 

all: obj obj/VarLatticeVector.o 

full: obj obj/VarLatticeVector.o selfTest
	selfTest test_example.c

obj/VarLatticeVector.o : VarLatticeVector.C VarLatticeVector.h ${cfgUtils_H} ${variables_H} ${common_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  \
	    -I../variables -I../cfgUtils  \
	    -fPIC -c -o $@ VarLatticeVector.C

obj/boolPlus.o : boolPlus.C boolPlus.h ${common_H} ${cfgUtils_H} ${variables_H} 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  \
	    -I../variables -I../cfgUtils  \
	    -fPIC -c -o $@ boolPlus.C


obj/selfTest.o : selfTest.C VarLatticeVector.h boolPlus.h ${cfgUtils_H} ${variables_H} ${common_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  \
	-I../variables -I../cfgUtils  \
	-fPIC -c -o $@ selfTest.C

selfTest : obj/selfTest.o obj/VarLatticeVector.o  obj/boolPlus.o variables cfgUtils rwAccessLabeler
	/bin/sh ${ROSE_BUILD_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o obj/VarLatticeVector.o ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ} 
	        ${ROSE_BUILD_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest