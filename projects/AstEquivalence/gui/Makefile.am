include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT
if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = compilerFlagsGui

compilerFlagsGui_SOURCES =	compilerFlagsMainGui.C compilerFlagsGui.C

# DQ (3/8/2009): These are included in $(ROSE_LIBS) as I understand it, plus -lrt will cause problems on OSX.
# compilerFlagsGui_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
compilerFlagsGui_LDADD = $(ROSE_LIBS)

# DQ (3/8/2009): These are included in $(ROSE_INCLUDES) as I understand it.
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES)

compass_parameters:
	cd .. && $(MAKE) compass_parameters
	cp -f ../compass_parameters .

check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass/gui: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 


endif
endif

EXTRA_DIST =  disks.xpm  folder.xpm compilerFlagsGui.h  icons.h

