Component = chkptRangeAnalysis
include ../../makefile.common

# simpleAnalyses: divAnalysisi.C, affineInequality.C, sgnAnalysis.C
# lattice: ConstrGraph.C, lattice.C
# analysis: analysis.C , analysisCommon.C
# state: nodeState.C, functionState.C 
oFiles = obj/chkptRangeAnalysis.o obj/ConstrGraph.o obj/divAnalysis.o obj/sgnAnalysis.o obj/affineInequality.o obj/analysis.o obj/nodeState.o obj/lattice.o obj/functionState.o obj/analysisCommon.o #obj/chkptRangeAnalysis.o 
all: ${oFiles}

full: selfTest ${oFiles}
	selfTest test_example.c |less #> out

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
#include ../ompAnal/makefile.external 

HFiles = analysis.h dataflow.h latticeFull.h lattice.h analysisCommon.h nodeState.h functionState.h

obj/chkptRangeAnalysis.o : chkptRangeAnalysis.C chkptRangeAnalysis.h ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ chkptRangeAnalysis.C 

obj/ConstrGraph.o : ConstrGraph.C ConstrGraph.h ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ ConstrGraph.C 


obj/divAnalysis.o : divAnalysis.C divAnalysis.h ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ divAnalysis.C 

obj/sgnAnalysis.o : sgnAnalysis.C sgnAnalysis.h ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ sgnAnalysis.C 


obj/affineInequality.o : affineInequality.C affineInequality.h ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ affineInequality.C 

obj/analysis.o : analysis.C ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ analysis.C 

obj/nodeState.o : nodeState.C ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ nodeState.C 

obj/lattice.o : lattice.C ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ lattice.C 

obj/functionState.o : functionState.C ${HFiles} ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ functionState.C 
	
obj/analysisCommon.o : analysisCommon.C analysisCommon.h ${common_H} ../variables/variables.h  ${cfgUtils_H}\
                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ analysisCommon.C 
	
#obj/chkptRangeAnalysis.o : chkptRangeAnalysis.C chkptRangeAnalysis.h nodeState.h functionState.h analysisCommon.h ${common_H} ../variables/variables.h  ${cfgUtils_H}\
#                          ${cfgUtils_H}  ../arrIndexLabeler/arrIndexLabeler.h
#	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../ompAnal -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ chkptRangeAnalysis.C 

obj/selfTest.o : selfTest.C chkptRangeAnalysis.h ${common_H} ../variables/variables.h ${cfgUtils_H} ../unionFind/unionFind.h ../rwAccessLabeler/rwAccessLabeler.h ../arrIndexLabeler/arrIndexLabeler.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -I../ompAnal -I../unionFind  -I../rwAccessLabeler -I../arrIndexLabeler -I../varBitVector -fPIC -c -o $@ selfTest.C

.PHONY: rwAccessLabeler
rwAccessLabeler:
	cd ../rwAccessLabeler; ${MAKE}; cd ../chkptRangeAnalysis

.PHONY: arrIndexLabeler
arrIndexLabeler:
	cd ../arrIndexLabeler; ${MAKE}; cd ../chkptRangeAnalysis
	
.PHONY: variables
variables:
	cd ../variables; ${MAKE}; cd ../chkptRangeAnalysis
	
.PHONY: unionFind
unionFind:
	cd ../unionFind; ${MAKE}; cd ../chkptRangeAnalysis
	
.PHONY: varBitVector
varBitVector:
	cd ../varBitVector; ${MAKE}; cd ../chkptRangeAnalysis


selfTest : rwAccessLabeler arrIndexLabeler variables cfgUtils unionFind varBitVector \
           ../variables/variables.h ../unionFind/unionFind.h ${cfgUtils_H} \
           ../rwAccessLabeler/rwAccessLabeler.h ../arrIndexLabeler/arrIndexLabeler.h \
           obj/selfTest.o \
           ${oFiles} ../variables/obj/variables.o ../variables/obj/varSets.o obj/selfTest.o ${cfgUtils_OBJ} 
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o \
	        ${oFiles} ../variables/obj/variables.o ../variables/obj/varSets.o  ../../obj/common.o \
	        ${cfgUtils_OBJ} \
	        ../unionFind/obj/unionFind.o ../varBitVector/obj/varBitVector.o \
	        ../rwAccessLabeler/obj/rwAccessLabeler.o ../arrIndexLabeler/obj/arrIndexLabeler.o \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest bin/* debug_bin/*
