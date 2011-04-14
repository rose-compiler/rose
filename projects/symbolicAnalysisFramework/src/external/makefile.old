Component = external
oFiles = obj/spearWrap.o obj/spearWrap.threaded.o
HFiles = spearWrap.h

all: obj ${oFiles}

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../state/makefile.external


full: obj selfTest ${oFiles}
	selfTest test_example.c |less #> out

obj/spearWrap.o : spearWrap.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../state -fPIC -c -o $@ spearWrap.C 

obj/spearWrap.threaded.o : spearWrap.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${state_H}
	g++ ${CFLAGS} -DTHREADED -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../state -fPIC -c -o $@ spearWrap.C 


selfTest : cfgUtils obj/selfTest.o variables cfgUtils
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        ${cfgUtils_OBJ} ${variables_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest
