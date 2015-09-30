include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = multiPassTest generateSDG generateCDG generateDDG \
                generateSFBDT generateSFCFG generateSFDF generateSFDT #generateSlicedSDG testSlicing slice 

# generateSFBDT.C  generateSFCFG.C  generateSFDF.C  generateSFDT.C  

# Done:
# generateCDG.C  generateDDG.C  generateSDG.C  generateSlicedSDG.C testSlicing.C multiPassTest.C  slice.C  


multiPassTest_SOURCES = multiPassTest.C
multiPassTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateSDG_SOURCES = generateSDG.C
generateSDG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#generateSlicedSDG_SOURCES = generateSlicedSDG.C
#generateSlicedSDG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateCDG_SOURCES = generateCDG.C
generateCDG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateDDG_SOURCES = generateDDG.C
generateDDG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#testSlicing_SOURCES = testSlicing.C
#testSlicing_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#slice_SOURCES = slice.C
#lice_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateSFBDT_SOURCES = generateSFBDT.C
generateSFBDT_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateSFCFG_SOURCES = generateSFCFG.C
generateSFCFG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateSFDF_SOURCES = generateSFDF.C
generateSFDF_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

generateSFDT_SOURCES = generateSFDT.C
generateSFDT_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TESTCODES = test1.C test2.C test3.C test4.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

#TEST_TRANSLATOR = ./slice $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): multiPassTest $(TESTCODES) 
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	@echo "Compiling test code using multiple programs: slice multiPassTest generateSDG generateCDG generateDDG and ..." #generateSlicedSDG testSlicing 
#	$(TEST_TRANSLATOR) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	./slice -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./multiPassTest -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateSDG -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#./generateSlicedSDG -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateCDG -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateDDG -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	./testSlicing -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateSFBDT -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateSFCFG -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateSFDF -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	./generateSFDT -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)


EXTRA_DIST = $(TESTCODES)

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/staticInterproceduralSlicingTests: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) testSlicing *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot

