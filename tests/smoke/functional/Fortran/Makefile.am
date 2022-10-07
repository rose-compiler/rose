include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =
PHONIES =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

########################################################################################################################
## Fortran Parser Tests
#
# These tests should quickly check whether the flang parser is installed correctly
#

noinst_PROGRAMS += fmain.f90
fmain_SOURCES = fmain.f90

fortran_parser_tests =

## Test targets
#
fortran_targets = $(addsuffix .passed, $(fortran_parser_tests))
TEST_TARGETS += $(fortran_targets)

## Test rules
#
$(fortran_targets): %.passed: %
	@$(RTH_RUN)                                             \
                TITLE="Fortran parser test [$@]"                \
                CMD="$$(pwd)/$^"                                \
                $(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-fortran
check-fortran: $(FORTRAN_TEST_TARGETS)


###############################################################################################################################
# Boilerplate
###############################################################################################################################

endif

check-local: $(TEST_TARGETS)
