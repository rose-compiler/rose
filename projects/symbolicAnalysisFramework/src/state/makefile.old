Component = state

all: obj lib lib/libState.a lib/libState.threaded.a

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../analysis/makefile.external 
include ../lattice/makefile.external 

oFiles         = obj/nodeState.o obj/functionState.o
oThreadedFiles = obj/nodeState.threaded.o obj/functionState.o
HFiles = nodeState.h functionState.h

full: obj lib selfTest lib/libState.a lib/libState.threaded.a
	selfTest test_example.c |less #> out

obj/nodeState.o : nodeState.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I/g/g15/bronevet/apps/tbb/include -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler  -I../analysis -I../lattice -fPIC -c -o $@ nodeState.C 

obj/nodeState.threaded.o : nodeState.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H -DTHREADED ${ROSE_INCLUDE} -I/g/g15/bronevet/apps/tbb/include -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler  -I../analysis -I../lattice -fPIC -c -o $@ nodeState.C 

obj/functionState.o : functionState.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I/g/g15/bronevet/apps/tbb/include -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler  -I../analysis -I../lattice -fPIC -c -o $@ functionState.C 
	
lib/libState.a: ${oFiles}
	ar -r lib/libState.a ${oFiles}

lib/libState.threaded.a: ${oThreadedFiles} ${HOME}/apps/tbb/lib/${machine}/libtbb.so
	ar -r lib/libState.threaded.a ${oThreadedFiles} ${HOME}/apps/tbb/lib/${machine}/libtbb.so

	
obj/selfTest.o : selfTest.C ${ANALYSIS_ROOT}/include/common.h ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${analysis_H} ${lattice_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include -I../variables -I../cfgUtils -I../arrIndexLabeler -I../analysis -I../lattice -fPIC -c -o $@ selfTest.C
	
selfTest : cfgUtils obj/selfTest.o variables rwAccessLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} \
	        ${analysis_OBJ} ${lattice_OBJ} ${state_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* lib/* selfTest
