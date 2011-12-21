include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


EXTRA_DIST = efor.C efor1.C input1.C test11.C test12.C test13.C test14.C createTest.C t1 t2 t3 t4

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -O3 -fopenmp

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally

# DQ (7/2/2011): Fixed this to only handle binary work when binary support is available.
# bin_PROGRAMS  = astTraversalTest processnew3Down4SgIncGraph processnew3Down4 binaryPaths
bin_PROGRAMS  = e0 e1 createTest astTraversalTest processnew3Down4SgIncGraph2 processnew3Down4SgIncGraph3 strictGraphTest strictGraphTest2 strictGraphTest3 smtlibParser sourcePTP
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
bin_PROGRAMS += binaryPaths bPTP
endif
if HAVE_YICES
bin_PROGRAMS += yicesParser
endif
# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)

e0_SOURCES = efor.C.maintest.C
e0_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
e0_LDFLAGS = -O3 -fopenmp

e1_SOURCES = efor1.C.maintest.C
e1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
e1_LDFLAGS = -O3 -fopenmp


astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

#t14tt_SOURCES = test14.C.maintest.C
#t14tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t14tt_LDFLAGS = -O3 -fopenmp

#t13tt_SOURCES = test13.C.maintest.C
#t13tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t13tt_LDFLAGS = -O3 -fopenmp

#t12tt_SOURCES = test12.C.maintest.C
#t12tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t12tt_LDFLAGS = -O3 -fopenmp

#t11tt_SOURCES = test11.C.maintest.C
#t11tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#t11tt_LDFLAGS = -O3 -fopenmp

createTest_SOURCES = createTest.C
createTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
createTest_LDFLAGS = -O3 -fopenmp

bPTP_SOURCES = bPTP.C
bPTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
bPTP_LDFLAGS = -O3 -fopenmp

processnew3Down4SgIncGraph2_SOURCES      = processnew3Down4SgIncGraph2.C
processnew3Down4SgIncGraph2_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#processnew3Down4SgIncGraph2_LDFLAGS = -O3 -fopenmp

processnew3Down4SgIncGraph3_SOURCES      = processnew3Down4SgIncGraph3.C
processnew3Down4SgIncGraph3_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#processnew3Down4SgIncGraph3_LDFLAGS = -O3 -fopenmp


binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#binaryPaths_LDFLAGS = -O3 -fopenmp

strictGraphTest_SOURCES = strictGraphTest.C
strictGraphTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest_LDFLAGS = -O3 -fopenmp

strictGraphTest2_SOURCES = strictGraphTest2.C
strictGraphTest2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest2_LDFLAGS = -O3 -fopenmp

smtlibParser_SOURCES = smtlibParser.C
smtlibParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#smtlibParser_LDFLAGS = -O3 -fopenmp

sourcePTP_SOURCES = sourcePTP.C
sourcePTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#sourcePTP_LDFLAGS = -O3 -fopenmp

if HAVE_YICES
yicesParser_SOURCES = yicesParser.C
yicesParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#yicesParser_LDFLAGS = -O3 -fopenmp -I/home/hoffman34/yices/yices-1.0.292/include -L/home/hoffman34/yices/yices-1.0.292/lib -lyices
endif

strictGraphTest3_SOURCES = strictGraphTest3.C
strictGraphTest3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest3_LDFLAGS = -O3 -fopenmp
# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

#strongCheckThree: t12tt
#	./t12tt $(srcdir)/test12.C
#strongCheckFour: t11tt
#	./t11tt $(srcdir)/test11.C
strongCheckFive: e0
	./e0 $(srcdir)/efor.C
strongCheckSix: e1
	./e1 $(srcdir)/efor1.C

testTraversals: astTraversalTest
	./astTraversalTest -edg:w -c $(srcdir)/input1.C

testRunExamples2: processnew3Down4SgIncGraph2
	./processnew3Down4SgIncGraph2 $(srcdir)/test11.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test12.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test13.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test14.C
	rm *.dot

testRunExamples3: processnew3Down4SgIncGraph3
	./processnew3Down4SgIncGraph3 $(srcdir)/eif.C
	./processnew3Down4SgIncGraph3 $(srcdir)/eif2.C
	./processnew3Down4SgIncGraph3 $(srcdir)/eif3.C
	./processnew3Down4SgIncGraph3 $(srcdir)/eif4.C
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
if HAVE_YICES
yicesParserTest: yicesParser
	./yicesParser $(srcdir)/test14.C 
endif
check-local:
	@$(MAKE) strongCheckFive
	@$(MAKE) strongCheckSix
	@$(MAKE) testTraversals
	@$(MAKE) testRunExamples2
	@$(MAKE) testRunExamples3
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


