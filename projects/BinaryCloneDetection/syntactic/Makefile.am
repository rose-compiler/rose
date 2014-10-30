include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =  gui
bin_PROGRAMS =

# Liao 1/15/2014. Move this file outside of conditinoals since automake will scan _SOURCES unconditionally for dependencies
# which will bring in SyntacticSchema.C into the distribution package.
MOSTLYCLEANFILES =
MOSTLYCLEANFILES += SyntacticSchema.C
SyntacticSchema.C: SchemaDrop.sql SchemaCreate.sql
	@echo "  GEN     $@"
	@echo "const char *syntactic_schema_drop ="				 >$@
	@perl -pe 's/"/\\"/g; s/(.*)/    "$$1\\n"/' <$(srcdir)/SchemaDrop.sql	>>$@
	@echo ';'								>>$@
	@echo "const char *syntactic_schema_create ="				>>$@
	@perl -pe 's/"/\\"/g; s/(.*)/    "$$1\\n"/' <$(srcdir)/SchemaCreate.sql	>>$@
	@echo ';'								>>$@


if ROSE_USE_SQLITE_DATABASE

# FIXME: We should not have to depend on all of SSL just to compute MD5 sums.  We should be using libgcrypt instead. This
# would also avoid the SSL/Java conflict mentioned in the ROSE manual. [RPM 2013-04-11]
if ROSE_USE_SSL_SUPPORT

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

# (Re)creates the tables for syntactic clone detection
bin_PROGRAMS += createSchema
createSchema_SOURCES = createSignatureVectors.C SyntacticSchema.C createSchema.C vectorCompression.C
createSchema_LDADD = $(BOOST_LDFLAGS) ../semantic/libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Scans the specimen with sliding windows and populates the "vectors" and "function_ids" tables.
bin_PROGRAMS += createVectorsBinary
createVectorsBinary_SOURCES = createSignatureVectors.C SyntacticSchema.C createVectorsBinary.C vectorCompression.C
createVectorsBinary_LDADD = $(BOOST_LDFLAGS) ../semantic/libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Scans the specimen with sliding windows and populates the "vectors" and "function_ids" tables.
bin_PROGRAMS += createVectorsSource
createVectorsSource_SOURCES = createSignatureVectors.C createVectorsSource.C  SyntacticSchema.C vectorCompression.C
createVectorsSource_LDADD = $(BOOST_LDFLAGS) ../semantic/libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Reads the database created by createVectorsBinary and finds windows that are similar, inserting them into the "clusters" table.
bin_PROGRAMS += findClones
findClones_SOURCES = \
	findClones.C callLSH.C computerangesFunc.h computerangesFunc.C vectorCompression.h vectorCompression.C \
	lsh.C lsh.h lshParameterFinding2.C
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
lshCloneDetection_SOURCES = lsh.C lshCloneDetection.C lsh.h vectorCompression.h vectorCompression.C findExactClones2.C

# undocumented
bin_PROGRAMS += lshParameterFinding
lshParameterFinding_SOURCES = lshParameterFinding.C lsh.C lsh.h computerangesFunc.C vectorCompression.h vectorCompression.C \
	lshParameterFinding2.C

# undocumented
bin_PROGRAMS += computeStatistics
computeStatistics_SOURCES = computeStatistics.C 

# undocumented
bin_PROGRAMS += exactCloneDetection
exactCloneDetection_SOURCES = findExactClones.C findExactClones2.C lsh.C  lsh.h vectorCompression.h vectorCompression.C

# undocumented
bin_PROGRAMS += printOutClones
printOutClones_SOURCES = printOutClones.C 

# Generate the GML file for visualizing the largest clones
bin_PROGRAMS += printOutClones_distinctFiles 
printOutClones_distinctFiles_SOURCES = printOutClones_distinctFiles.C 

# undocumented
bin_PROGRAMS += printPairs
printPairs_SOURCES = printPairs.C 

#-----------------------------------------------------------------------------------------------------------------------------
# Level 110-120: run evaluation metrics on pairwise binaries
bin_PROGRAMS += 110_compute_aggregate
110_compute_aggregate_SOURCES = 110-compute-aggregate.C FailureDetectionSchema.C 
110_compute_aggregate_CPPFLAGS = $(ROSE_INCLUDES)
110_compute_aggregate_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

MOSTLYCLEANFILES += FailureDetectionSchema.C
FailureDetectionSchema.C: compute-pairwise-precision.sql
	@echo "  GEN     $@"
	@echo "namespace FailureEvaluation {"  			>$@
	@echo "const char *failure_schema =" 				>>$@
	@perl -pe 's/"/\\"/g; s/(.*)/    "$$1\\n"/' <$< 	>>$@
	@echo ';' 						>>$@
	@echo "} //namespace" 					>>$@

# Andreas is looking into whether any of these files are actually necessary or if they
# can be removed from the source tree. [Robb Matzke, 2013-03-12]
EXTRA_DIST = 					\
        compute-pairwise-precision.sql          \
	callLSH.h				\
	collateByHash.C				\
	createCloneDetectionVectorsBinary.h	\
        createCloneDetectionVectorsSource.h     \
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

