Component = ompAnal
include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external
include ../cfgUtils/makefile.external 
include ../arrIndexLabeler/makefile.external 
include ../rwAccessLabeler/makefile.external 
include ../variables/makefile.external 

all: lib obj lib/libompAnal.a

full: lib obj lib/libompAnal.a selfTest 
	selfTest test_example.c

obj/ompUtils.o : ompUtils.C ompUtils.h ${common_H} ${cfgUtils_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -fPIC -c -o $@ ompUtils.C

obj/ompNormalForm.o : ompNormalForm.C ${common_H} ${cfgUtils_H} ${variables_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  -DBOOST_REGEX_MATCH_EXTRA ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../variables -I../cfgUtils -fPIC -c -o $@ ompNormalForm.C
	
obj/selfTest.o : selfTest.C ompUtils.h ${common_H} ${cfgUtils_H} ${variables_H}
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -I../variables -fPIC -c -o $@ selfTest.C
	
obj/errorExample.o : errorExample.C 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I${ANALYSIS_ROOT}/include ${common_INC} -I../cfgUtils -fPIC -c -o $@ errorExample.C
	
obj/errorExample.commaOp.o : errorExample.commaOp.C 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -fPIC -c -o $@ errorExample.commaOp.C

lib/libompAnal.a: obj/ompUtils.o obj/ompNormalForm.o
	ar -r lib/libompAnal.a obj/ompUtils.o obj/ompNormalForm.o

selfTest : lib/libompAnal.a obj/selfTest.o cfgUtils variables
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o lib/libompAnal.a ${cfgUtils_OBJ} ${variables_OBJ} \
	        ${unionFind_OBJ} ${varBitVector_OBJ} ${rwAccessLabeler_OBJ} ${arrIndexLabeler_OBJ} \
	        ../../obj/common.o \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex

#errorExample : obj/errorExample.o
#	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o errorExample \
#	        obj/errorExample.o ${cfgUtils_OBJ} ../variables/obj/variables.o \
#	        ../unionFind/obj/unionFind.o ../varBitVector/obj/varBitVector.o \
#	        ../rwAccessLabeler/obj/rwAccessLabeler.o ../arrIndexLabeler/obj/arrIndexLabeler.o \../../obj/common.o \
#	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex
	        
errorExample : obj/errorExample.o
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o errorExample \
	        obj/errorExample.o \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex
	        
errorExample.commaOp : obj/errorExample.commaOp.o
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o errorExample.commaOp \
	        obj/errorExample.commaOp.o \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt -lboost_regex

clean:
	rm -f obj/* selfTest