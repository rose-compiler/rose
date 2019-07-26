#
# AMReX Fortran to C++ Translator
#

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#------------------------------------------------------------------------------
# Subdirectories
#------------------------------------------------------------------------------
SUBDIRS=

#------------------------------------------------------------------------------
# Global Build Flags
#------------------------------------------------------------------------------
AM_CPPFLAGS := $(ROSE_INCLUDES)
AM_LDFLAGS  := $(ROSE_RPATHS)
LDADD       := $(ROSE_LIBS)

#------------------------------------------------------------------------------
# ROSE Dependency
#------------------------------------------------------------------------------
# Ensure librose has been built since our binaries will link with it
librose:
	$(MAKE) librose.la -C $(top_builddir)/src


#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------
all-local: librose

check-local: $(TEST_TARGETS)

$(TEST_TARGETS):
	@$(RTH_RUN) CMD="./$(@:.test=)" $(top_srcdir)/scripts/test_exit_status $@

lib_LTLIBRARIES = librajatracer.la

librajatracer_la_SOURCES = \
  src/rajatracer.C

#~ librajatracer_la_LDFLAGS = -version-info 0:0:1

#~ classMemberVariablesInLambdas.test: classMemberVariablesInLambdas
#~ 	./classMemberVariablesInLambdas -std=c++11 --edg:no_warnings -c $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx11_tests/classMemberVariablesInLambdas_test_01.C

#------------------------------------------------------------------------------
#~ TEST_TARGETS = ${BIN_EXECUTABLES:=.test}


#~ check-local:
#~ 	@echo "********************************************************************************"
#~ 	@echo "*** ROSE/tools/fortranTranslator (Amrex): make check rule completed normally ***"
#~ 	@echo "********************************************************************************"

#~ else

#~ check-local:
#~ 	@echo "*********************************************************************************"
#~ 	@echo "*** ROSE/tools/fortranTranslator (Amrex): configure failed                    ***"
#~ 	@echo "*********************************************************************************"

#~ endif
