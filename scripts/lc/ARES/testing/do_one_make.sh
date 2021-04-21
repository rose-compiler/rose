#!/bin/sh

#rosetest: rosetestopt
#rosetest%:
#	$(AT) $(GMAKE) CXX=$(ROSE_TOOL) XTRA_CXXFLAGS=$(ROSE_FLAGS) roseenv$*
#
#roseenv: roseenvopt
#roseenv%:
#	$(AT) LD_LIBRARY_PATH+=:$(ROSE_SUPPORT_ROOT)/java/jdk/7.0_51/jre/lib/amd64/server && \
#	LD_LIBRARY_PATH+=:$(ROSE_SUPPORT_ROOT)/boost/1_54_0/gcc/4.6.1/lib && \
#	$(GMAKE) COLOR=1 CXX="$(CXX)" XTRA_CXXFLAGS=$(ROSE_FLAGS) $*

ROSE_ROOT=/collab/usr/global/tools/rose
ROSE_SUPPORT_ROOT=${ROSE_ROOT}/opt/${SYS_TYPE}
ROSE_INSTALLATION=${ROSE_ROOT}/${SYS_TYPE}/rose-0.9.9.33-7a88543
#ROSE_INSTALLATION=${ROSE_ROOT}/${SYS_TYPE}/rose-0.9.8.36-6e86b73
ROSE_TOOL=${ROSE_INSTALLATION}/bin/globalVariablesInLambdas
ROSE_FLAGS="--edg:no_warnings -rose:keep_going"

AT="" && \
LD_LIBRARY_PATH+=:${ROSE_SUPPORT_ROOT}/java/jdk/7.0_51/jre/lib/amd64/server && \
LD_LIBRARY_PATH+=:${ROSE_SUPPORT_ROOT}/boost/1_54_0/gcc/4.6.1/lib && \
make -k AT="" COLOR=1 CXX=${ROSE_TOOL} XTRA_CXXFLAGS=${ROSE_FLAGS} $@
