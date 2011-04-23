Component = lattice
oFiles = obj/lattice.o obj/intArithLogical.o obj/ConstrGraph.o 
HFiles = latticeFull.h lattice.h intArithLogical.h ConstrGraph.h 

all: obj ${oFiles}

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../analysis/makefile.external 
include ../state/makefile.external 
include ../external/makefile.external 
include ../simpleAnalyses/makefile.external 


full: obj selfTest ${oFiles}
	selfTest test_example.c # |less #> out

obj/lattice.o : lattice.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${state_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../state -fPIC -c -o $@ lattice.C 

obj/ConstrGraph.o : ConstrGraph.C ConstrGraph.h ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../state -I../simpleAnalyses -fPIC -c -o $@ ConstrGraph.C 

obj/intArithLogical.o : intArithLogical.C intArithLogical.h ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${state_H} ${external_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../state -I../external -I../simpleAnalyses -fPIC -c -o $@ intArithLogical.C 

# test -------------
obj/selfTest.o : selfTest.C ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H} ${analysis_H} ${state_H} ${analysis_H} ${external_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I..//variables -I../cfgUtils -I../arrIndexLabeler -I../analysis -I../state -I../analysis -I../external -I../simpleAnalyses -fPIC -c -o $@ selfTest.C

selfTest : cfgUtils obj/selfTest.o variables cfgUtils arrIndexLabeler rwAccessLabeler state analysis external simpleAnalyses ${oFiles}
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC -g  -o selfTest \
	        obj/selfTest.o ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${state_OBJ} ${analysis_OBJ} ${external_OBJ} ${simpleAnalyses_OBJ} ${oFiles}\
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest
