include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

LDADD = $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.la

TESTCODES =

NEGTESTCODES = 

NEGATIVETESTCODES2 = \
	test_exceptions1.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:test 6

include_HEADERS = agppc.hpp agppe.hpp agppp.hpp agpps.hpp set.h collect.h crt.h 

EXTRA_DIST = cplus2

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -rf  core
	rm -rf ti_files



