include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astInterface/

bin_PROGRAMS = cloneDetection getSubVariants
cloneDetection_SOURCES      = cloneDetection.C

getSubVariants_SOURCES      = getSubVariants.C


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = createCloneDetectionVectors.h createCloneDetectionVectors.C

test: cloneDetection $(srcdir)/cloneDetectionInputTest.C $(srcdir)/inVector.txt $(srcdir)/toFile.txt
	./cloneDetection $(srcdir)/cloneDetectionInputTest.C -rose:clone:inVector $(srcdir)/inVector.txt -rose:clone:toFile $(srcdir)/toFile.txt
 

