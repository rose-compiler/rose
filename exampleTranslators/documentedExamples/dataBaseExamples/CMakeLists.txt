include ../../../config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# bin_PROGRAMS = customtable_example \
# databasegraph_example \
# rosedb_example \
# simplecallgraph_example

# objectFile_DEPENDENCIES = \
#    $(top_builddir)/src/librose.a \
#    $(top_builddir)/EDG/src/libedg.a

# customtable_example_SOURCES = customtable_example.C
# customtable_example_DEPENDENCIES = $(objectFile_DEPENDENCIES)

# databasegraph_example_SOURCES = databasegraph_example.C
# databasegraph_example_DEPENDENCIES = $(objectFile_DEPENDENCIES)

# rosedb_example_SOURCES = rosedb_example.C
# rosedb_example_DEPENDENCIES = $(objectFile_DEPENDENCIES)

# simplecallgraph_example_SOURCES = simplecallgraph_example.C
# simplecallgraph_example_DEPENDENCIES = $(objectFile_DEPENDENCIES)

## linker needs to know about STL
## By adding the -ptr. on the link line we should be able to avoid 
## specifying the Templates.DB/*.o file explicitly on the link line
## LDFLAGS = @STL_INCLUDES@

LDADD = $(ROSE_LIBS)

# If the purify linker is enabled, AUX_LINKER will be set by configure.
# These are now set in the ROSE SUPPORT PURIFY configure macro
# CCLD  = $(AUX_LINKER) $(CC)
# CXXLD = $(AUX_LINKER) $(CXX)

EXTRA_DIST = CMakeLists.txt customtable_example.C rosedb_example.C databasegraph_example.C simplecallgraph_example.C 

CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

