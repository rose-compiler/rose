include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE
if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = compilerFlagsGui

compilerFlagsGui_SOURCES = \
	compilerFlagsMainGui.C \
        compilerFlagsGui.C

compilerFlagsGui_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


compass_parameters:
	cd .. && $(MAKE) compass_parameters
	cp -f ../compass_parameters .

check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass/gui: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 

EXTRA_DIST = 

endif

endif
