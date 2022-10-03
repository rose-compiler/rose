include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=\
	src 

#####
# The source files used for testing were all Fortran *.F sources
# and had been removed previously by someone. It appeared that these
# files were translanted to C and then the tools in this project run
# on them. Since the Fortran_to_C translator is not maintained
# and has been removed from the ROSE distribution project files,
# the test files have been removed.  The tests directory had been
# removed from the SUBDIRS list previously [Rasmussen, 2022.10.1].
#
#	tests
#

EXTRA_DIST=\
	src/README

check-local:
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/RoseBlockLevelTracing: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

