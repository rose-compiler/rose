#
# Matlab to C++ Translator
#

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_WITH_GNU_OCTAVE_PARSER

SUBDIRS=\
  src \
  tests

EXTRA_DIST=\
	README

check-local:
	@echo "**************************************************************************************"
	@echo "*** ROSE/projects/MatlabTranslator: make check rule complete (terminated normally) ***"
	@echo "**************************************************************************************"

else

check-local:
	@echo "**************************************************************************************"
	@echo "*** ROSE/projects/MatlabTranslator: requires Octave frontend support!              ***"
	@echo "**************************************************************************************"

endif
