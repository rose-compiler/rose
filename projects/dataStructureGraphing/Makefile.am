include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

AM_CPPFLAGS = $(ROSE_INCLUDES)

# DQ (2/2/2004): Not clear why ROSE_LIBS is not enough.  Need to fix ROSE to use just two libraries!
# LDADD = $(ROSE_LIBS)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# DQ (12/5/2009): Skip testing of this code to establish the size of the problem!
if !ROSE_BUILD_OS_IS_CYGWIN
# if ROSE_BUILD_OS_IS_CYGWIN

bin_PROGRAMS = cgraphpp

cgraphpp_SOURCES = \
     ControlStructure.C \
     GenGeneration.C \
     GenRepresentation.C \
     helpFunctions.C \
     main.C

#    handle.C
#    helpMethods.C

# Need to get together with Andreas to get this working.
# He will be here in another week, so I will not worry about this now!
testLinkedListTestCode: cgraphpp
	@echo "Testing of cgraphpp commented out because it appears need more work to be able use the latest version of ROSE ..."
	./cgraphpp -rose:gall $(srcdir)/linkedListTestCode.C

# This rule is run after automake's internal check rule (which we don't want to use)
check-local: testLinkedListTestCode
	@echo "*************************************************************************************************"
	@echo "****** ROSE/projects/dataStructureGraphing: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************"

endif

include_HEADERS = \
     AttributeType.h \
     ControlStructure.h \
     GenGeneration.h \
     GenRepresentation.h \
     helpFunctions.h

EXTRA_DIST = linkedListTestCode.C

clean-local:
	rm -rf Templates.DB ii_files ti_files
	rm -f a.out *.o rose_*.C *.sage.C *.C.pdf *.C.edg.pdf *.pdf *.dot *.int.C rose_directives_list.txt core CodeGenerated_*
	rm -rf ti_files



















