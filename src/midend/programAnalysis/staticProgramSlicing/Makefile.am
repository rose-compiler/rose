include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (9/10/2005): Commented out until Millind can get this code compiled (Qing changed the Graph interface)
libstaticProgramSlicing_so_SOURCES =  \
   ControlDependenceGraph.C  DataDependenceGraph.C  MergedDependenceGraph.C \
   SlicingInfo.C CreateSlice.C DependenceGraph.C ProgramDependenceGraph.C \
   SystemDependenceGraph.C

lib_LIBRARIES = libstaticProgramSlicing.so

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libstaticProgramSlicing.a: $(libstaticProgramSlicing_a_OBJECTS)
	rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libstaticProgramSlicing_a_OBJECTS)
	test -f $@ || exit 1

libstaticProgramSlicing.la: $(libstaticProgramSlicing_la_OBJECTS)
	rm -f $@
	$(CXX_SHARED_LIB_UPDATE) $@ $(libstaticProgramSlicing_la_OBJECTS)
	test -f $@ || exit 1

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = CreateSlice.h  DependenceGraph.h  SlicingInfo.h
