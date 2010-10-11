Component = simpleAnalyses
execFiles = bin/factPlacer bin/latticePlacer bin/scalarFWDataflow bin/pathSensitive
all: obj bin ${execFiles}

include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 
include ../simpleAnalyses/makefile.external 
include ../analysis/makefile.external 
include ../lattice/makefile.external 
include ../state/makefile.external 
include ../external/makefile.external 

full: obj bin ${execFiles}
	factPlacer    test_example.c > factPlacer_out
	latticePlacer test_examples/latticePlacer_test.c > latticePlacer_out
	scalarFWDataflow test_examples/scalarFWDataflow_test.c > scalarFWDataflow_out
	pathSensitive test_examples/pathSensitive_test.c > pathSensitive_out
	
fullscr: obj bin ${execFiles}
	factPlacer    test_example.c | tee factPlacer_out
	latticePlacer test_examples/latticePlacer_test.c | tee latticePlacer_out
	scalarFWDataflow test_examples/scalarFWDataflow_test.c | tee scalarFWDataflow_out
	pathSensitive test_examples/pathSensitive_test.c | tee pathSensitive_out

obj/factPlacer.o : factPlacer.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${simpleAnalyses_H} ${state_H} ${lattice_H} ${external_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} \
	    -I${ANALYSIS_ROOT}/include ${common_INC} ${cfgUtils_INC} ${variables_INC} -I../analysis -I../simpleAnalyses -I../state -I../external \
	    -I../lattice -I${ANALYSIS_ROOT}/src/arrIndexLabeler -I${ANALYSIS_ROOT}/src/rwAccessLabeler \
	    -fPIC -c -o $@ factPlacer.C

bin/factPlacer : cfgUtils obj/factPlacer.o common cfgUtils variables analysis simpleAnalyses state external
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o bin/factPlacer \
	        obj/factPlacer.o ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${simpleAnalyses_OBJ} \
	        ${state_OBJ} ${lattice_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${external_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt
	        
obj/latticePlacer.o : latticePlacer.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${simpleAnalyses_H} ${state_H} ${lattice_H} ${external_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} \
	    -I${ANALYSIS_ROOT}/include ${common_INC} ${cfgUtils_INC} ${variables_INC} -I../analysis -I../simpleAnalyses -I../state -I../external \
	    -I../lattice -I${ANALYSIS_ROOT}/src/arrIndexLabeler -I${ANALYSIS_ROOT}/src/rwAccessLabeler \
	    -fPIC -c -o $@ latticePlacer.C

bin/latticePlacer : cfgUtils obj/latticePlacer.o common cfgUtils variables analysis simpleAnalyses state lattice external
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o bin/latticePlacer \
	        obj/latticePlacer.o ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${simpleAnalyses_OBJ} \
	        ${state_OBJ} ${lattice_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${external_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

obj/scalarFWDataflow.o : scalarFWDataflow.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${simpleAnalyses_H} ${state_H} ${lattice_H} ${external_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} \
	    -I${ANALYSIS_ROOT}/include ${common_INC} ${cfgUtils_INC} ${variables_INC} -I../analysis -I../simpleAnalyses -I../state -I../external \
	    -I../lattice -I../arrIndexLabeler -I../rwAccessLabeler \
	    -fPIC -c -o $@ scalarFWDataflow.C

bin/scalarFWDataflow : cfgUtils obj/scalarFWDataflow.o common cfgUtils variables analysis simpleAnalyses state lattice external
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++ -fPIC -o bin/scalarFWDataflow \
	        obj/scalarFWDataflow.o ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${simpleAnalyses_OBJ} \
	        ${state_OBJ} ${lattice_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${external_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt
	        
obj/pathSensitive.o : pathSensitive.C ${common_H} ${cfgUtils_H} ${variables_H} ${analysis_H} ${simpleAnalyses_H} ${state_H} ${lattice_H} ${external_H} ${arrIndexLabeler_H} ${rwAccessLabeler_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} \
	    -I${ANALYSIS_ROOT}/include ${common_INC} ${cfgUtils_INC} ${variables_INC} -I../analysis -I../simpleAnalyses -I../state -I../external -I../lattice \
	    -I../lattice -I../arrIndexLabeler -I../rwAccessLabeler \
	    -fPIC -c -o $@ pathSensitive.C

bin/pathSensitive : cfgUtils obj/pathSensitive.o common cfgUtils variables analysis simpleAnalyses state lattice external
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o bin/pathSensitive \
	        obj/pathSensitive.o ${common_OBJ} ${cfgUtils_OBJ} ${variables_OBJ} ${analysis_OBJ} ${simpleAnalyses_OBJ} \
	        ${state_OBJ} ${lattice_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} ${external_OBJ} \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/*  bin/* debug_bin/*
