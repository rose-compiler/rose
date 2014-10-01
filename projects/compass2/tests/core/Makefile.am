include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Globals
# ------------------------------------------------------------------------------

COMPASS2=$(top_builddir)/projects/compass2/bin/compass2
CHECKER=$(COMPASS2)
COMPASS_PARAMETERS_XSD=$(top_srcdir)/projects/compass2/share/xml/compass_parameters.xsd

# ------------------------------------------------------------------------------
#  Test rules
# ------------------------------------------------------------------------------

check-local: parsers
	./test_parser $(srcdir)/blank.cpp
	./test_parameters_parser $(srcdir)/blank.cpp

parsers:
	$(MAKE) tests/core/test_parameters_parser -C $(top_builddir)/projects/compass2
	$(MAKE) tests/core/test_parser -C $(top_builddir)/projects/compass2

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

clean-local:
	rm -f \
		rose_*.C \
		*.o \
		*.out \
		*.ti

