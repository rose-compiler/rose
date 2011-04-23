Component = analysis

oFiles = obj/analysis.o obj/analysisCommon.o obj/partitionedAnalysis.o
HFiles = analysis.h dataflow.h analysisCommon.h partitionedAnalysis.h

all: obj ${oFiles}

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../lattice/makefile.external 
include ../simpleAnalyses/makefile.external 

full: obj selfTest ${oFiles}
	selfTest test_example.c |less #> out

obj/analysis.o : analysis.C ${HFiles} ${common_H} ${variables_H} ${cfgUtils_H} ${lattice_H} ${state_H} ${arrIndexLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../lattice -I../state -fPIC -c -o $@ analysis.C 

obj/analysisCommon.o : analysisCommon.C analysisCommon.h ${variables_H} ${common_H} ${cfgUtils_H} ${lattice_H} ${state_H} ${arrIndexLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../lattice -I../state -fPIC -c -o $@ analysisCommon.C 

obj/partitionedAnalysis.o : partitionedAnalysis.C partitionedAnalysis.h ${common_H} ${variables_H} ${cfgUtils_H} ${lattice_H} ${state_H} ${arrIndexLabeler_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ partitionedAnalysis.C 

#---------------test here ???------------ 
obj/selfTest.o : selfTest.C ${common_H} ${variables_H} ${cfgUtils_H} ${lattice_H} ${state_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include -I${ANALYSIS_ROOT}/src/variables -I${ANALYSIS_ROOT}/src/cfgUtils -I${ANALYSIS_ROOT}/src/arrIndexLabeler -I${ANALYSIS_ROOT}/src/rwAccessLabeler -fPIC -c -o $@ selfTest.C


selfTest : cfgUtils obj/selfTest.o variables rwAccessLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${lattice_OBJ} ${state_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest
