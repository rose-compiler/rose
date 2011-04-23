Component = common
oFiles = obj/genUID.o 
oThreadedFiles = obj/genUID.threaded.o 
HFiles = genUID.h

all: obj lib lib/libCommon.a lib/libCommon.threaded.a

include ${ANALYSIS_ROOT}/makefile.common


full: obj selfTest ${oFiles}
	selfTest test_example.c |less #> out

obj/genUID.o : genUID.C ${HFiles} common.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include -fPIC -c -o $@ genUID.C 
	
obj/genUID.threaded.o : genUID.C ${HFiles} common.h 
	g++ ${CFLAGS} -DHAVE_CONFIG_H  ${ROSE_INCLUDE} -I../../include -I/g/g15/bronevet/apps/tbb/include -DTHREADED -fPIC -c -o $@ genUID.C 
	
lib/libCommon.a: ${oFiles}
	ar -r lib/libCommon.a ${oFiles}

lib/libCommon.threaded.a: ${oThreadedFiles} ${HOME}/apps/tbb/lib/${machine}/libtbb.so
	ar -r lib/libCommon.threaded.a ${oThreadedFiles} ${HOME}/apps/tbb/lib/${machine}/libtbb.so
	
selfTest : cfgUtils obj/selfTest.o ${oFiles}
	/bin/sh ${ROSE_MAKE_BASE}/libtool --mode=link --tag=CXX g++  -fPIC  -o selfTest \
	        obj/selfTest.o ${oFiles}\
	        ${ROSE_MAKE_BASE}/src/librose.la -lfl -lrt
clean:
	rm -f obj/* selfTest
