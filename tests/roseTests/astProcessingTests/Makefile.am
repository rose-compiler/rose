include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


EXTRA_DIST = input1.C test11.C test12.C test13.C test14.C createTest.C t1 t2 t3 t4

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) #-fopenmp -O3

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally

# DQ (7/2/2011): Fixed this to only handle binary work when binary support is available.
# bin_PROGRAMS  = astTraversalTest processnew3Down4SgIncGraph processnew3Down4 binaryPaths
bin_PROGRAMS  = t14tt t13tt t12tt t11tt createTest astTraversalTest processnew3Down4SgIncGraph2 strictGraphTest strictGraphTest2 strictGraphTest3 smtlibParser sourcePTP
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
bin_PROGRAMS += binaryPaths bPTP
endif
if HAVE_YICES
bin_PROGRAMS += yicesParser
endif
# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)



astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

bPTP_SOURCES = bPTP.C
bPTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
bPTP_LDFLAGS = -lboost_serialization

processnew3Down4SgIncGraph2_SOURCES      = processnew3Down4SgIncGraph2.C
processnew3Down4SgIncGraph2_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
processnew3Down4SgIncGraph2_LDFLAGS = -lboost_serialization

binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
binaryPaths_LDFLAGS = -lboost_serialization

strictGraphTest_SOURCES = strictGraphTest.C
strictGraphTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
strictGraphTest_LDFLAGS = -lboost_serialization

strictGraphTest2_SOURCES = strictGraphTest2.C
strictGraphTest2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
strictGraphTest2_LDFLAGS = -lboost_serialization

smtlibParser_SOURCES = smtlibParser.C
smtlibParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
smtlibParser_LDFLAGS = -lboost_serialization

sourcePTP_SOURCES = sourcePTP.C
sourcePTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
sourcePTP_LDFLAGS = -lboost_serialization

if HAVE_YICES
yicesParser_SOURCES = yicesParser.C
yicesParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
yicesParser_LDFLAGS = -I$(YICES_PREFIX)/include -L$(YICES_PREFIX)/lib -lyices
endif
strictGraphTest3_SOURCES = strictGraphTest3.C
strictGraphTest3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
<<<<<<< HEAD
strictGraphTest3_LDFLAGS = -lboost_serialization


astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

t14tt_SOURCES = test14.C.maintest.C
t14tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t14tt_LDFLAGS = -fopenmp -O3

t13tt_SOURCES = test13.C.maintest.C
t13tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t13tt_LDFLAGS = -fopenmp -O3

t12tt_SOURCES = test12.C.maintest.C
t12tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t12tt_LDFLAGS = -fopenmp -O3

t11tt_SOURCES = test11.C.maintest.C
t11tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t11tt_LDFLAGS = -fopenmp -O3

createTest_SOURCES = createTest.C
createTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#createTest_LDFLAGS = -fopenmp -O3

bPTP_SOURCES = bPTP.C
bPTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#bPTP_LDFLAGS = -fopenmp -O3

processnew3Down4SgIncGraph2_SOURCES      = processnew3Down4SgIncGraph2.C
processnew3Down4SgIncGraph2_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#processnew3Down4SgIncGraph2_LDFLAGS = -fopenmp -O3

binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#binaryPaths_LDFLAGS = -fopenmp -O3

strictGraphTest_SOURCES = strictGraphTest.C
strictGraphTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest_LDFLAGS = -fopenmp -O3

strictGraphTest2_SOURCES = strictGraphTest2.C
strictGraphTest2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest2_LDFLAGS = -fopenmp -O3

smtlibParser_SOURCES = smtlibParser.C
smtlibParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#smtlibParser_LDFLAGS = -fopenmp -O3

sourcePTP_SOURCES = sourcePTP.C
sourcePTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#sourcePTP_LDFLAGS = -fopenmp -O3

yicesParser_SOURCES = yicesParser.C
yicesParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#yicesParser_LDFLAGS = -fopenmp -O3

strictGraphTest3_SOURCES = strictGraphTest3.C
strictGraphTest3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

strongCheckOne: t14tt
	./t14tt $(srcdir)/test14.C
strongCheckTwo: t13tt
	./t13tt $(srcdir)/test13.C
strongCheckThree: t12tt
	./t12tt $(srcdir)/test12.C
strongCheckFour: t11tt
	./t11tt $(srcdir)/test11.C




testTraversals: astTraversalTest
	./astTraversalTest -edg:w -c $(srcdir)/input1.C

testRunExamples2: processnew3Down4SgIncGraph2
	./processnew3Down4SgIncGraph2 $(srcdir)/test11.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test12.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test13.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test14.C
	rm *.dot

testRunExamplesBin: binaryPaths
	./binaryPaths $(srcdir)/t1
	./binaryPaths $(srcdir)/t2
	./binaryPaths $(srcdir)/t3
	./binaryPaths $(srcdir)/t4
	rm *.dot

testRunExamplesbPTP: bPTP
	./bPTP $(srcdir)/t1
	./bPTP $(srcdir)/t2
	./bPTP $(srcdir)/t3
	./bPTP $(srcdir)/t4

strictGraph: strictGraphTest
	./strictGraphTest

strictGraph2: strictGraphTest2
	./strictGraphTest2

strictGraph3: strictGraphTest3
	./strictGraphTest3

smtParse: smtlibParser
	./smtlibParser $(srcdir)/test11.C

sourcePTPTest: sourcePTP
	./sourcePTP $(srcdir)/test14.C
yicesParserTest: yicesParser
	./yicesParser $(srcdir)/test14.C 
check-local:
	@$(MAKE) strongCheckOne
	@$(MAKE) strongCheckTwo
	@$(MAKE) strongCheckThree
	@$(MAKE) strongCheckFour
	@$(MAKE) testTraversals
	@$(MAKE) testRunExamples2
	@$(MAKE) strictGraph
	@$(MAKE) smtParse
	@$(MAKE) sourcePTPTest
if HAVE_YICES
	@$(MAKE) yicesParserTest
endif
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
	@$(MAKE) testRunExamplesbPTP
	@$(MAKE) testRunExamplesBin
endif

	@echo "***********************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astProcessingTests: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_*.C

distclean-local:
	rm -rf Templates.DB 


