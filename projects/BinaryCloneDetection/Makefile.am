include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use --with-libxml2 on configure command-line for ROSE to activate this


SUBDIRS =  gui

if ROSE_USE_SQLITE_DATABASE
# if HAVE_SSL
if ROSE_USE_SSL_SUPPORT

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 
ABS_BUILDDIR = @abs_builddir@

bin_PROGRAMS = createVectorsBinary createGML createDOT createDOT2 createGML2 createGML3 createGML4 diffBinary doGrouping findClones findExactDisjointSets \
               findLargestClones lshCloneDetection lshParameterFinding computeStatistics exactCloneDetection printOutClones printOutClones_distinctFiles printPairs

createVectorsBinary_SOURCES = createSignatureVectors.C createVectorsBinary.C vectorCompression.C

findClones_SOURCES = findClones.C callLSH.C computerangesFunc.h computerangesFunc.C vectorCompression.h vectorCompression.C lsh.C lsh.h lshParameterFinding.h
findClones_CPPFLAGS = -DABS_BUILDDIR="\"$(ABS_BUILDDIR)\""

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

# Andreas is looking into whether any of these files are actually necessary or if they
# can be removed from the source tree. [Robb Matzke, 2013-03-12]
EXTRA_DIST = 					\
	callLSH.h				\
	collateByHash.C				\
	createCloneDetectionVectorsBinary.h	\
	createSignatureVectors.h		\
	createVectors.C				\
	findExactClones.h			\
	lsh/BasicDefinitions.h			\
	lsh/BucketHashing.C			\
	lsh/BucketHashing.h			\
	lsh/Geometry.C				\
	lsh/Geometry.h				\
	lsh/GlobalVars.C			\
	lsh/GlobalVars.h			\
	lsh/LocalitySensitiveHashing.C		\
	lsh/LocalitySensitiveHashing.h		\
	lsh/NearNeighbors.C			\
	lsh/NearNeighbors.h			\
	lsh/Random.C				\
	lsh/Random.h				\
	lsh/SelfTuning.C			\
	lsh/SelfTuning.h			\
	lsh/SettingUpLSH.C			\
	lsh/SettingUpLSH.h			\
	lsh/headers.h				\
	lsh/lsh2.C				\
	lsh2/BasicDefinitions.h			\
	lsh2/BucketHashing.cpp			\
	lsh2/BucketHashing.h			\
	lsh2/Geometry.cpp			\
	lsh2/Geometry.h				\
	lsh2/GlobalVars.cpp			\
	lsh2/GlobalVars.h			\
	lsh2/LocalitySensitiveHashing.cpp	\
	lsh2/LocalitySensitiveHashing.h		\
	lsh2/NearNeighbors.cpp			\
	lsh2/NearNeighbors.h			\
	lsh2/Random.cpp				\
	lsh2/Random.h				\
	lsh2/SelfTuning.cpp			\
	lsh2/SelfTuning.h			\
	lsh2/enumBuckets.cpp			\
	lsh2/headers.h				\
	lsh2/lsh2.C

endif
endif

clean-local:
	rm -f 

