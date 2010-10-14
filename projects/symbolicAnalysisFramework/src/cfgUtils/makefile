include ${ANALYSIS_ROOT}/makefile.common
include ../common/makefile.external

all: obj obj/cfgUtils.o obj/VirtualCFGIterator.o obj/DataflowCFG.o obj/CFGRewrite.o obj/CallGraphTraverse.o # obj/DataflowCFG.cfgToDot.o

full: obj obj/cfgUtils.o obj/VirtualCFGIterator.o obj/DataflowCFG.o obj/CFGRewrite.o obj/CallGraphTraverse.o selfTest # obj/DataflowCFG.cfgToDot.o
	selfTest test_example.c

obj/cfgUtils.o : cfgUtils.C cfgUtils.h VirtualCFGIterator.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -I../variables -fPIC -c -o $@ cfgUtils.C
	
obj/VirtualCFGIterator.o : VirtualCFGIterator.C cfgUtils.h VirtualCFGIterator.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -I../variables -fPIC -c -o $@ VirtualCFGIterator.C

obj/DataflowCFG.o : DataflowCFG.C DataflowCFG.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -fPIC -c -o $@ DataflowCFG.C

#obj/DataflowCFG.cfgToDot.o : DataflowCFG.cfgToDot.C DataflowCFG.h DataflowCFG.cfgToDot.h 
#	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -fPIC -c -o $@ DataflowCFG.cfgToDot.C

obj/CFGRewrite.o : CFGRewrite.C cfgUtils.h VirtualCFGIterator.h CFGRewrite.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -I../variables -I../rwAccessLabeler -fPIC -c -o $@ CFGRewrite.C

obj/CallGraphTraverse.o : CallGraphTraverse.C CallGraphTraverse.h  ../variables
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -I../variables -fPIC -c -o $@ CallGraphTraverse.C

#-------------test---------------
obj/selfTest.o : selfTest.C cfgUtils.h CallGraphTraverse.C CallGraphTraverse.h VirtualCFGIterator.h ../variables 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include ${common_INC} -I../variables -fPIC -c -o $@ selfTest.C

.PHONY: variables
variables:
	cd ../variables; ${MAKE}; cd ../cfgUtils
	
.PHONY: rwAccessLabeler
rwAccessLabeler:
	cd ../rwAccessLabeler; ${MAKE}; cd ../cfgUtils
	
.PHONY: arrIndexLabeler
arrIndexLabeler:
	cd ../arrIndexLabeler; ${MAKE}; cd ../cfgUtils
	
selfTest : obj/cfgUtils.o obj/selfTest.o obj/VirtualCFGIterator.o obj/CFGRewrite.o obj/DataflowCFG.o  \
           obj/DataflowCFG.cfgToDot.o  variables rwAccessLabeler
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o obj/cfgUtils.o ../variables/obj/variables.o ../rwAccessLabeler/obj/rwAccessLabeler.o  ../arrIndexLabeler/obj/arrIndexLabeler.o \
	        obj/DataflowCFG.cfgToDot.o obj/DataflowCFG.o obj/VirtualCFGIterator.o obj/CFGRewrite.o \
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt

clean:
	rm -f obj/* selfTest
