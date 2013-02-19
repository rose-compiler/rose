include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use --with-libxml2 on configure command-line for ROSE to activate this


SUBDIRS =  gui

if ROSE_USE_SQLITE_DATABASE
# if HAVE_SSL
if ROSE_USE_SSL_SUPPORT

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

bin_PROGRAMS = createVectorsBinary createGML createDOT createDOT2 createGML2 createGML3 createGML4 diffBinary doGrouping findClones findExactDisjointSets \
               findLargestClones lshCloneDetection lshParameterFinding computeStatistics exactCloneDetection printOutClones printOutClones_distinctFiles printPairs

createVectorsBinary_SOURCES = createSignatureVectors.C createVectorsBinary.C vectorCompression.C

findClones_SOURCES = findClones.C callLSH.C computerangesFunc.h computerangesFunc.C vectorCompression.h vectorCompression.C lsh.C lsh.h lshParameterFinding.h
findClones_CPPFLAGS = -DABS_TOP_BUILDDIR="\"$(ABS_TOP_BUILDDIR)\""

exactCloneDetection_SOURCES = findExactClones.C lsh.C  lsh.h vectorCompression.h vectorCompression.C


doGrouping_SOURCES =  doGrouping.C computerangesFunc.C

diffBinary_SOURCES = diffBinary.C 

lshCloneDetection_SOURCES =  lsh.C lshCloneDetection.C lsh.h vectorCompression.h vectorCompression.C

lshParameterFinding_SOURCES =  lshParameterFinding.C lsh.C lsh.h computerangesFunc.C vectorCompression.h vectorCompression.C

computeStatistics_SOURCES = computeStatistics.C 

createGML_SOURCES = createGML.C 

createGML2_SOURCES = createGML2.C

createGML3_SOURCES = createGML3.C

createGML4_SOURCES = createGML4.C

createDOT_SOURCES = createDOT.C 

createDOT2_SOURCES = createDOT2.C

findLargestClones_SOURCES = findLargestClones.C 

findExactDisjointSets_SOURCES = findExactDisjointSets.C 

printPairs_SOURCES = printPairs.C 

printOutClones_SOURCES = printOutClones.C 

printOutClones_distinctFiles_SOURCES = printOutClones_distinctFiles.C 

endif
endif

clean-local:
	rm -f 

