include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = tf1.C tf2.C tf3.C tfif1.C tfif2.C tfif3.C tfif4.C mf1.C mf3.C mf4.C mf5.C mf8.C mf9.C mf10.C mf11.C mf12.C mf13.C eif.C eif2.C eif3.C eif4.C efor2.C efor3.C input1.C test11.C test12.C test13.C test14.C createTest.C t1 t2 t3 t4 mfEx.C mfEx2.C mfEx3.C mfEx4.C mfEx5.C mfEx6.C mfEx7.C mfEx8.C mfEx9.C mfEx12.C mfEx13.C mfEx14.C mfEx15.C whiletest.C whiletest2.C whiletest3.C

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .


# This test program does not require the rest of ROSE so it can be handled locally

# DQ (7/2/2011): Fixed this to only handle binary work when binary support is available.
# bin_PROGRAMS  = astTraversalTest processnew3Down4SgIncGraph processnew3Down4 binaryPaths
bin_PROGRAMS  = proFunSIG interproceduralCFG e0 e1 ff1 ff2 ff3 f1 f2 f3 f4 createTest astTraversalTest processnew3Down4SgIncGraph2 processnew3Down4SgIncGraph3 strictGraphTest strictGraphTest2 strictGraphTest3 smtlibParser sourcePTP
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
bin_PROGRAMS += binaryPaths bPTP
endif
if ROSE_HAVE_YICES
bin_PROGRAMS += yicesParser
endif
#if ROSE_HAVE_YICES
#bin_PROGRAMS += yicesParser
#endif
# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)






#include_HEADERS = yicesLib.h

e0_SOURCES = efor2.C.maintest.C
e0_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#e0_LDFLAGS = -fopenmp -O3 

e1_SOURCES = efor3.C.maintest.C
e1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#e1_LDFLAGS = -fopenmp -O3 

f1_SOURCES = tfif1.C.maintest.C
f1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#f1_LDFLAGS = -fopenmp -O3 

f2_SOURCES = tfif2.C.maintest.C
f2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#f2_LDFLAGS = -fopenmp -O3 

f3_SOURCES = tfif3.C.maintest.C
f3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#f3_LDFLAGS = -fopenmp -O3 

f4_SOURCES = tfif4.C.maintest.C
f4_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#f4_LDFLAGS = -fopenmp -O3 

ff1_SOURCES = tf1.C.maintest.C
ff1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#ff1_LDFLAGS = -fopenmp -O3 

ff2_SOURCES = tf2.C.maintest.C
ff2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#ff2_LDFLAGS = -fopenmp -O3 

ff3_SOURCES = tf3.C.maintest.C
ff3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#ff3_LDFLAGS = -fopenmp -O3 

interproceduralCFG_SOURCES = interproceduralCFG.C
interproceduralCFG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#interproceduralCFG_LDFLAGS = -fopenmp -O3 

astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#astTraversalTest_LDFLAGS = -fopenmp -O3 


proFunSIG_SOURCES = proFunSIG.C
proFunSIG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#proFunSIG_LDFLAGS = -fopenmp -O3 
#t14tt_SOURCES = test14.C.maintest.C
#t14tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
 

#t13tt_SOURCES = test13.C.maintest.C
#t13tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
 

#t12tt_SOURCES = test12.C.maintest.C
#t12tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
 

#t11tt_SOURCES = test11.C.maintest.C
#t11tt_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
 

createTest_SOURCES = createTest.C
createTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#createTest_LDFLAGS = -fopenmp -O3 

bPTP_SOURCES = bPTP.C
bPTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#bPTP_LDFLAGS = -fopenmp -O3 

processnew3Down4SgIncGraph2_SOURCES      = processnew3Down4SgIncGraph2.C
processnew3Down4SgIncGraph2_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#processnew3Down4SgIncGraph2_LDFLAGS = -fopenmp -O3 

processnew3Down4SgIncGraph3_SOURCES      = processnew3Down4SgIncGraph3.C
processnew3Down4SgIncGraph3_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#processnew3Down4SgIncGraph3_LDFLAGS = -fopenmp -O3 
 #


binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#binaryPaths_LDFLAGS =  -fopenmp -O3 

strictGraphTest_SOURCES = strictGraphTest.C
strictGraphTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest_LDFLAGS =  -fopenmp -O3

strictGraphTest2_SOURCES = strictGraphTest2.C
strictGraphTest2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#strictGraphTest2_LDFLAGS =  -fopenmp -O3

smtlibParser_SOURCES = smtlibParser.C
smtlibParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#smtlibParser_LDFLAGS =  -fopenmp -O3

sourcePTP_SOURCES = sourcePTP.C
sourcePTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
#sourcePTP_LDFLAGS = -fopenmp -O3 


if ROSE_HAVE_YICES
yicesParser_SOURCES = yicesParser.C
yicesParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
endif


strictGraphTest3_SOURCES = strictGraphTest3.C
strictGraphTest3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)
strictGraphTest3_LDFLAGS = -fopenmp -O3 
# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

#strongCheckThree: t12tt
#	./t12tt $(srcdir)/test12.C
#strongCheckFour: t11tt
#	./t11tt $(srcdir)/test11.C
strongCheckFive: e0
	./e0 $(srcdir)/efor2.C
strongCheckSix: e1
	./e1 $(srcdir)/efor3.C
f1Check: f1
	./f1 $(srcdir)/tfif1.C
f2Check: f2
	./f2 $(srcdir)/tfif2.C
f3Check: f3
	./f3 $(srcdir)/tfif3.C
f4Check: f4
	./f4 $(srcdir)/tfif4.C
ff1Check: ff1
	./ff1 $(srcdir)/tf1.C
ff2Check: ff2
	./ff2 $(srcdir)/tf2.C
ff3Check: ff3
	./ff3 $(srcdir)/tf3.C


testTraversals: astTraversalTest
	./astTraversalTest -edg:w -c $(srcdir)/input1.C

testRunExamples2: processnew3Down4SgIncGraph2
	./processnew3Down4SgIncGraph2 $(srcdir)/test11.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test12.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test13.C
	./processnew3Down4SgIncGraph2 $(srcdir)/test14.C
	rm *.dot
testInter: interproceduralCFG
	./interproceduralCFG $(srcdir)/test11.C
	./interproceduralCFG $(srcdir)/test12.C
	./interproceduralCFG $(srcdir)/test13.C
	./interproceduralCFG $(srcdir)/test14.C
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

testProFunSIG: proFunSIG
	./proFunSIG $(srcdir)/mf1.C
	./proFunSIG $(srcdir)/mf3.C
	./proFunSIG $(srcdir)/mf4.C
	./proFunSIG $(srcdir)/mf5.C

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
if ROSE_HAVE_YICES
yicesParserTest: yicesParser
	./yicesParser $(srcdir)/mfEx.C
	./yicesParser $(srcdir)/mfEx2.C
	./yicesParser $(srcdir)/mfEx3.C
	./yicesParser $(srcdir)/mfEx4.C
	./yicesParser $(srcdir)/mfEx6.C
	./yicesParser $(srcdir)/mfEx7.C
	./yicesParser $(srcdir)/mfEx8.C
	./yicesParser $(srcdir)/eif.C
	./yicesParser $(srcdir)/eif2.C
	./yicesParser $(srcdir)/eif3.C
	./yicesParser $(srcdir)/efor1.C
	./yicesParser $(srcdir)/efor2.C
	./yicesParser $(srcdir)/efor3.C
	./yicesParser $(srcdir)/mf1.C
	./yicesParser $(srcdir)/mf2.C
	./yicesParser $(srcdir)/mf3.C
	./yicesParser $(srcdir)/mf8.C
	./yicesParser $(srcdir)/mf9.C
	./yicesParser $(srcdir)/mf10.C
	./yicesParser $(srcdir)/mf11.C
	./yicesParser $(srcdir)/mf12.C
	./yicesParser $(srcdir)/tf1.C
	./yicesParser $(srcdir)/tf2.C
	./yicesParser $(srcdir)/tf3.C
	./yicesParser $(srcdir)/mf13.C
	./yicesParser $(srcdir)/whiletest.C
	./yicesParser $(srcdir)/whiletest2.C
	./yicesParser $(srcdir)/whiletest3.C
	./yicesParser $(srcdir)/tfif1.C
	./yicesParser $(srcdir)/tfif2.C
	./yicesParser $(srcdir)/tfif3.C
	./yicesParser $(srcdir)/tfif4.C
endif
check-local:
	@$(MAKE) testTraversals
	@$(MAKE) testRunExamples2
	@$(MAKE) testRunExamples3
	@$(MAKE) testInter
	@$(MAKE) strictGraph
	@$(MAKE) strictGraph2
	@$(MAKE) strictGraph3
	@$(MAKE) smtParse
	@$(MAKE) testProFunSIG
	@$(MAKE) sourcePTPTest
	@$(MAKE) ff1Check
	@$(MAKE) ff2Check
	@$(MAKE) ff3Check
	@$(MAKE) f1Check
	@$(MAKE) f2Check
	@$(MAKE) f3Check
	@$(MAKE) f4Check
if ROSE_HAVE_YICES
	@$(MAKE) yicesParserTest
endif
#endif
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


