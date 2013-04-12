include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =  gui
bin_PROGRAMS =

if ROSE_USE_SQLITE_DATABASE

# FIXME: We should not have to depend on all of SSL just to compute MD5 sums.  We should be using libgcrypt instead. This
# would also avoid the SSL/Java conflict mentioned in the ROSE manual. [RPM 2013-04-11]
if ROSE_USE_SSL_SUPPORT

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

# Scans the specimen with sliding windows and populates the "vectors" and "function_ids" tables.
bin_PROGRAMS += createVectorsBinary
createVectorsBinary_SOURCES = createSignatureVectors.C createVectorsBinary.C vectorCompression.C

# Reads the database created by createVectorsBinary and finds windows that are similar, inserting them into the "clusters" table.
bin_PROGRAMS += findClones
findClones_SOURCES = \
	findClones.C callLSH.C computerangesFunc.h computerangesFunc.C vectorCompression.h vectorCompression.C \
	lsh.C lsh.h lshParameterFinding.h
ABS_BUILDDIR = @abs_builddir@
findClones_CPPFLAGS = -DABS_BUILDDIR="\"$(ABS_BUILDDIR)\""

# Computes how much overlap there is between pairs of similar functions.  Reads the "clusters" and "vectors" table and
# creates a new table called cluster_pairs.
bin_PROGRAMS += computeClusterPairs
computeClusterPairs_SOURCES = computeClusterPairs.C

# These are undocumented as to what they do
bin_PROGRAMS += createGML createGML2 createGML3 createGML4
createGML_SOURCES  = createGML.C 
createGML2_SOURCES = createGML2.C
createGML3_SOURCES = createGML3.C
createGML4_SOURCES = createGML4.C

# These are undocumented as to what they do
bin_PROGRAMS += createDOT createDOT2
createDOT_SOURCES  = createDOT.C 
createDOT2_SOURCES = createDOT2.C

# Example code on how to use RoseBin
bin_PROGRAMS += diffBinary
diffBinary_SOURCES = diffBinary.C 

# Used to compute grouping ranges for vectors. It only generates a set of ranges.
bin_PROGRAMS += doGrouping
doGrouping_SOURCES =  doGrouping.C computerangesFunc.C

# Find all exact disjoint sets
bin_PROGRAMS += findExactDisjointSets
findExactDisjointSets_SOURCES = findExactDisjointSets.C 

# Finds largest clone pairs (not clusters) by decoupling the clone clusters into a set of clone pairs and
# then optimistically merging adjacent clone pairs.
bin_PROGRAMS += findLargestClones
findLargestClones_SOURCES = findLargestClones.C 

# undocumented
bin_PROGRAMS += lshCloneDetection
lshCloneDetection_SOURCES = lsh.C lshCloneDetection.C lsh.h vectorCompression.h vectorCompression.C

# undocumented
bin_PROGRAMS += lshParameterFinding
lshParameterFinding_SOURCES = lshParameterFinding.C lsh.C lsh.h computerangesFunc.C vectorCompression.h vectorCompression.C

# undocumented
bin_PROGRAMS += computeStatistics
computeStatistics_SOURCES = computeStatistics.C 

# undocumented
bin_PROGRAMS += exactCloneDetection
exactCloneDetection_SOURCES = findExactClones.C lsh.C  lsh.h vectorCompression.h vectorCompression.C

# undocumented
bin_PROGRAMS += printOutClones
printOutClones_SOURCES = printOutClones.C 

# Generate the GML file for visualizing the largest clones
bin_PROGRAMS += printOutClones_distinctFiles 
printOutClones_distinctFiles_SOURCES = printOutClones_distinctFiles.C 

# undocumented
bin_PROGRAMS += printPairs
printPairs_SOURCES = printPairs.C 

endif
endif

