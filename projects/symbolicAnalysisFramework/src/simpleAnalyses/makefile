Component = simpleAnalyses
oFiles = obj/divAnalysis.o obj/sgnAnalysis.o obj/nodeConstAnalysis.o obj/saveDotAnalysis.o obj/printAnalysisStates.o obj/affineInequality.o obj/placeUIDs.o
HFiles = divAnalysis.h sgnAnalysis.h saveDotAnalysis.h printAnalysisStates.h affineInequality.h placeUIDs.h
all: obj ${oFiles}

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external
include ../arrIndexLabeler/makefile.external
include ../rwAccessLabeler/makefile.external
include ../variables/makefile.external
include ../analysis/makefile.external
include ../lattice/makefile.external
include ../state/makefile.external

full: obj selfTest ${oFiles}
	selfTest test_example.c |less #> out

obj/divAnalysis.o : divAnalysis.C divAnalysis.h ${common_H} ${variables_H}  ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ divAnalysis.C 

obj/sgnAnalysis.o : sgnAnalysis.C sgnAnalysis.h ${common_H} ${variables_H}  ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ sgnAnalysis.C 

obj/nodeConstAnalysis.o : nodeConstAnalysis.C nodeConstAnalysis.h ${common_H} ${variables_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  -I../common ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ nodeConstAnalysis.C 

obj/saveDotAnalysis.o : saveDotAnalysis.C saveDotAnalysis.h ${common_H} ${variables_H}  ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ saveDotAnalysis.C 

obj/printAnalysisStates.o : printAnalysisStates.C printAnalysisStates.h ${common_H} ${variables_H}  ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ printAnalysisStates.C 

obj/affineInequality.o : affineInequality.C affineInequality.h ${common_H} ${variables_H}  ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ affineInequality.C 

obj/placeUIDs.o : placeUIDs.C placeUIDs.h ${variables_H} ${common_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${analysis_H} ${lattice_H} ${state_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../rwAccessLabeler -I../arrIndexLabeler -I../analysis -I../lattice -I../state -fPIC -c -o $@ placeUIDs.C 

obj/selfTest.o : selfTest.C ${common_H}  ${variables_H} ${common_H} ${cfgUtils_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC}  ${variables_INC} ${cfgUtils_INC} -I../arrIndexLabeler -fPIC -c -o $@ selfTest.C

selfTest : cfgUtils obj/selfTest.o common variables cfgUtils arrIndexLabeler rwAccessLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} \
	        ${analysis_OBJ} ${lattice_OBJ} ${state_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest bin/* debug_bin/*
