include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libquerySources = \
     nodeQuery.C    nodeQueryInheritedAttribute.C    \
     booleanQuery.C booleanQueryInheritedAttribute.C \
     nameQuery.C    nameQueryInheritedAttribute.C    \
     numberQuery.C  numberQueryInheritedAttribute.C  \
     astQuery.C     astQueryInheritedAttribute.C

# queryVariant.C

# nodeQuery.o : nodeQuery.C queryVariant.C
nodeQuery.lo : nodeQuery.C queryVariant.C

# lib_LTLIBRARIES = libquery.a
# libquery_a_SOURCES  = $(libquerySources)

noinst_LTLIBRARIES = libquery.la
libquery_la_SOURCES = $(libquerySources)

libquery_la_DEPENDENCIES = nodeQuery.o

include_HEADERS = \
     nodeQuery.h nodeQueryInheritedAttribute.h \
     booleanQuery.h booleanQueryInheritedAttribute.h \
     nameQuery.h nameQueryInheritedAttribute.h \
     numberQuery.h numberQueryInheritedAttribute.h \
     astQuery.h astQueryInheritedAttribute.h \
     roseQueryLib.h

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

# Note: queryVariant.C is included in nodeQuery.C
EXTRA_DIST = CMakeLists.txt queryVariant.C QueryLib.docs 

# A way to add additional error checking at compile time
# ROSE_WARNINGS = -Wall -W -Wswitch-enum -Wswitch-default -Wswitch
# GXX_COMPILER_REPORT = -fmem-report -ftime-report
# GXX_COMPILER_REPORT =
# AM_CXXFLAGS = $(ROSE_WARNINGS) $(GXX_COMPILER_REPORT)








