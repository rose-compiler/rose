Component = mpiAnal
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../analysis/makefile.external
include ../lattice/makefile.external
include ../state/makefile.external
include ../simpleAnalyses/makefile.external
include ../external/makefile.external

all: lib obj lib/libmpiAnal.a

full: lib obj lib/libmpiAnal.a selfTest commTopoAnalTest
#	selfTest test_example.c

obj/rankDepAnalysis.o : rankDepAnalysis.C rankDepAnalysis.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ rankDepAnalysis.C

obj/ranknpDepIfMeet.o : ranknpDepIfMeet.C ranknpDepIfMeet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ ranknpDepIfMeet.C

#obj/commTopoAnal.o : commTopoAnal.C commTopoAnal.h mesgExpr.h procSet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
#	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ commTopoAnal.C

obj/contProcMatchAnalysis.o : contProcMatchAnalysis.C contProcMatchAnalysis.h mesgExpr.h procSet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ contProcMatchAnalysis.C

obj/pCFG.o : pCFG.C pCFG.h mesgExpr.h procSet.h ranknpDepIfMeet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ pCFG.C

obj/mesgBuf.o : mesgBuf.C mesgBuf.h mesgExpr.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ mesgBuf.C

obj/mesgExpr.o : mesgExpr.C mesgExpr.h procSet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ mesgExpr.C

obj/procSet.o : procSet.C procSet.h ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ procSet.C

lib/libmpiAnal.a: obj/contProcMatchAnalysis.o obj/pCFG.o obj/rankDepAnalysis.o obj/ranknpDepIfMeet.o obj/mesgBuf.o obj/mesgExpr.o obj/procSet.o
	ar -r lib/libmpiAnal.a obj/pCFG.o obj/contProcMatchAnalysis.o obj/rankDepAnalysis.o obj/ranknpDepIfMeet.o obj/mesgBuf.o obj/mesgExpr.o obj/procSet.o

obj/selfTest.o : selfTest.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -fPIC -c -o $@ selfTest.C

obj/commTopoAnalTest.o : commTopoAnalTest.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${lattice_H} ${state_H} ${simpleAnalyses_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -g -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../analysis -I../lattice -I../state -I../simpleAnalyses -I../arrIndexLabeler -I../rwAccessLabeler -fPIC -c -o $@ commTopoAnalTest.C

selfTest : lib/libmpiAnal.a obj/selfTest.o common cfgUtils variables analysis lattice state simpleAnalyses external rwAccessLabeler arrIndexLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++ -g  -fPIC  -o selfTest \
	        obj/selfTest.o lib/libmpiAnal.a ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${lattice_OBJ} ${state_OBJ} ${simpleAnalyses_OBJ} ${external_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ}\
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex

commTopoAnalTest : lib/libmpiAnal.a obj/commTopoAnalTest.o common cfgUtils variables analysis lattice state simpleAnalyses external rwAccessLabeler arrIndexLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++ -g  -fPIC  -o commTopoAnalTest \
	        obj/commTopoAnalTest.o lib/libmpiAnal.a ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${lattice_OBJ} ${state_OBJ} ${simpleAnalyses_OBJ} ${external_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ}\
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex


clean:
	rm -f lib/*.a obj/*.o selfTest commTopoAnalTest
