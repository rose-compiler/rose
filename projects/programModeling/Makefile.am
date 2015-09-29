include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Automake conditional to control use of ROSE with Maple Symbolic Algebra Package
if ROSE_USE_MAPLE

AM_CPPFLAGS = $(ROSE_INCLUDES) $(MAPLE_INCLUDES)

bin_PROGRAMS = mapleBugReport1 mapleAPIExample2 mapleRoseTest loopTest

# mapleAPIExample3

mapleBugReport1_SOURCES = mapleBugReport1.c
mapleAPIExample2_SOURCES = mapleAPIExample2.c
# mapleAPIExample3_SOURCES = mapleAPIExample3.C
mapleRoseTest_SOURCES = mapleRoseTest.C

loopTest_SOURCES = loopTest.C mapleSupport.C programModel.C memoryAccessModel.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(MAPLE_LIBS)

testMapleBug: mapleBugReport1
	./mapleBugReport1

testCtoMaple: mapleAPIExample2
	./mapleAPIExample2

testRosetoMaple: mapleRoseTest $(srcdir)/mapleRoseInput.C
	./mapleRoseTest $(srcdir)/mapleRoseInput.C

testLoopTest: loopTest $(srcdir)/loopTestInput.C
	./loopTest $(srcdir)/loopTestInput.C
endif

clean-local:
	rm -f a.out rose_*.C

