# Binary clone detection.  Finds semantically similar functions based on fuzz testing. See Andreas and Earl for details.
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
noinst_PROGRAMS=
TEST_TARGETS=

noinst_LTLIBRARIES = libCloneDetection.la
libCloneDetection_la_SOURCES = CloneDetectionLib.C
libCloneDetection_la_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/BinaryCloneDetection/syntactic/
CloneDetectionLib.o: CloneDetectionLib.h
CloneDetectionLib.lo: CloneDetectionLib.h

#-----------------------------------------------------------------------------------------------------------------------------
# Level 00-09: initializing database (non-incremental)
noinst_PROGRAMS += 00-create-schema
00_create_schema_SOURCES = 00-create-schema.C CloneDetectionSchema.C
00_create_schema_CPPFLAGS = $(ROSE_INCLUDES)
00_create_schema_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST += Schema.sql

#-----------------------------------------------------------------------------------------------------------------------------
# Level 10-19: initializing database (incremental)
noinst_PROGRAMS += 10-generate-inputs
10_generate_inputs_SOURCES = 10-generate-inputs.C
10_generate_inputs_CPPFLAGS = $(ROSE_INCLUDES)
10_generate_inputs_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 11-add-functions
11_add_functions_SOURCES = 11-add-functions.C compute_signature_vector.C $(top_srcdir)/projects/BinaryCloneDetection/syntactic/vectorCompression.C
11_add_functions_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/BinaryCloneDetection/syntactic/
11_add_functions_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 14-bulk-load
14_bulk_load_SOURCES = 14-bulk-load.C
14_bulk_load_CPPFLAGS = $(ROSE_INCLUDES)
14_bulk_load_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-----------------------------------------------------------------------------------------------------------------------------
# Level 20-29: running tests
noinst_PROGRAMS += 20-get-pending-tests
20_get_pending_tests_SOURCES = 20-get-pending-tests.C
20_get_pending_tests_CPPFLAGS = $(ROSE_INCLUDES)
20_get_pending_tests_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 25-run-tests
25_run_tests_SOURCES = 25-run-tests.C compute_signature_vector.C $(top_srcdir)/projects/BinaryCloneDetection/syntactic/vectorCompression.C
25_run_tests_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/BinaryCloneDetection/syntactic/
25_run_tests_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-----------------------------------------------------------------------------------------------------------------------------
# Level 30-39: analysing test results
noinst_PROGRAMS += 31-func-similarity-worklist
31_func_similarity_worklist_SOURCES = 31-func-similarity-worklist.C
31_func_similarity_worklist_CPPFLAGS = $(ROSE_INCLUDES)
31_func_similarity_worklist_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 31-split-2dworklist
31_split_2dworklist_SOURCES = 31-split-2dworklist.C
31_split_2dworklist_CPPFLAGS = $(ROSE_INCLUDES)
31_split_2dworklist_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 32-func-similarity
32_func_similarity_SOURCES = 32-func-similarity.C $(top_srcdir)/projects/BinaryCloneDetection/syntactic/vectorCompression.C
32_func_similarity_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/BinaryCloneDetection/syntactic/
32_func_similarity_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-----------------------------------------------------------------------------------------------------------------------------
# Level 80-89: database maintenance
noinst_PROGRAMS += 85-delete
85_delete_SOURCES = 85-delete.C
85_delete_CPPFLAGS = $(ROSE_INCLUDES)
85_delete_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-----------------------------------------------------------------------------------------------------------------------------
# Level 90-99: querying results
noinst_PROGRAMS += 90-list-clusters
90_list_clusters_SOURCES = 90-list-clusters.C
90_list_clusters_CPPFLAGS = $(ROSE_INCLUDES)
90_list_clusters_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 90-list-function
90_list_function_SOURCES = 90-list-function.C
90_list_function_CPPFLAGS = $(ROSE_INCLUDES)
90_list_function_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 90-list-ogroup
90_list_ogroup_SOURCES = 90-list-ogroup.C
90_list_ogroup_CPPFLAGS = $(ROSE_INCLUDES)
90_list_ogroup_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 99-grants
99_grants_SOURCES = 99-grants.C
99_grants_CPPFLAGS = $(ROSE_INCLUDES)
99_grants_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-----------------------------------------------------------------------------------------------------------------------------
# Level 100-109: analysing api similarity
noinst_PROGRAMS += 101-api-similarity-worklist
101_api_similarity_worklist_SOURCES = 101-api-similarity-worklist.C
101_api_similarity_worklist_CPPFLAGS = $(ROSE_INCLUDES)
101_api_similarity_worklist_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 105-api-similarity
105_api_similarity_SOURCES = 105-api-similarity.C
105_api_similarity_CPPFLAGS = $(ROSE_INCLUDES)
105_api_similarity_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 109_drop_indexes
109_drop_indexes_SOURCES = 109-drop-indexes.C
109_drop_indexes_CPPFLAGS = $(ROSE_INCLUDES)
109_drop_indexes_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST += compute-pairwise-precision.sql



#-----------------------------------------------------------------------------------------------------------------------------
# Level 110-120: run evaluation metrics on pairwise binaries
noinst_PROGRAMS += 110_compute_aggregate
110_compute_aggregate_SOURCES = 110-compute-aggregate.C FailureDetectionSchema.C 
110_compute_aggregate_CPPFLAGS = $(ROSE_INCLUDES)
110_compute_aggregate_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST += compute-pairwise-precision.sql


noinst_PROGRAMS += 115_syscall_usage
115_syscall_usage_SOURCES = 115-syscall-usage.C linux_syscalls.C  
115_syscall_usage_CPPFLAGS = $(ROSE_INCLUDES)
115_syscall_usage_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)




#-----------------------------------------------------------------------------------------------------------------------------
# Built-in functions (used when not linking specimens with the C library)
builtins.so: builtins-string.c builtins-stdio.c
	$(CC) $(CFLAGS32) -Wall -fPIC -shared -o $@ $<

#-----------------------------------------------------------------------------------------------------------------------------
MOSTLYCLEANFILES += CloneDetectionSchema.C
CloneDetectionSchema.C: Schema.sql
	@echo "  GEN    $@"
	@echo "namespace CloneDetection {" 			>$@
	@echo "const char *schema =" 				>>$@
	@perl -pe 's/"/\\"/g; s/(.*)/    "$$1\\n"/' <$< 	>>$@
	@echo ';' 						>>$@
	@echo "} //namespace" 					>>$@

MOSTLYCLEANFILES += FailureDetectionSchema.C
FailureDetectionSchema.C: compute-pairwise-precision.sql
	@echo "  GEN     $@"
	@echo "namespace FailureEvaluation {"  			>$@
	@echo "const char *failure_schema =" 				>>$@
	@perl -pe 's/"/\\"/g; s/(.*)/    "$$1\\n"/' <$< 	>>$@
	@echo ';' 						>>$@
	@echo "} //namespace" 					>>$@

CloneDetection_SPECIMENS =			\
	tests/externalsort.C			\
	tests/qsort.C				\
	tests/quicksort.C			\
	tests/shellsort.C			\
	tests/simple001.C			\
	tests/simple002.C

#-----------------------------------------------------------------------------------------------------------------------------
# A very simple test to ensure that basic things are working

if ROSE_USE_SQLITE_DATABASE
TEST_TARGETS += basic.passed
endif

EXTRA_DIST += basic.conf

basic.passed: basic.conf 00-create-schema 10-generate-inputs 11-add-functions 20-get-pending-tests 25-run-tests \
		31-func-similarity-worklist 32-func-similarity 90-list-function
	@$(RTH_RUN) $< $@

#-----------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)

