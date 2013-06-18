# Binary clone detection.  Finds semantically similar functions based on fuzz testing. See Andreas and Earl for details.
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
noinst_PROGRAMS=

noinst_LTLIBRARIES = libCloneDetection.la
libCloneDetection_la_SOURCES = CloneDetectionLib.C
libCloneDetection_la_CPPFLAGS = $(ROSE_INCLUDES)
CloneDetectionLib.o: CloneDetectionLib.h
CloneDetectionLib.lo: CloneDetectionLib.h


noinst_PROGRAMS += 00-create-schema
00_create_schema_SOURCES = 00-create-schema.C CloneDetectionSchema.C
00_create_schema_CPPFLAGS = $(ROSE_INCLUDES)
00_create_schema_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST += Schema.sql

noinst_PROGRAMS += 01-generate-inputs
01_generate_inputs_SOURCES = 01-generate-inputs.C
01_generate_inputs_CPPFLAGS = $(ROSE_INCLUDES)
01_generate_inputs_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 01-add-functions
01_add_functions_SOURCES = 01-add-functions.C
01_add_functions_CPPFLAGS = $(ROSE_INCLUDES)
01_add_functions_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 02-get-pending-tests
02_get_pending_tests_SOURCES = 02-get-pending-tests.C
02_get_pending_tests_CPPFLAGS = $(ROSE_INCLUDES)
02_get_pending_tests_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 03-run-tests
03_run_tests_SOURCES = 03-run-tests.C
03_run_tests_CPPFLAGS = $(ROSE_INCLUDES)
03_run_tests_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 04-bulk-load
04_bulk_load_SOURCES = 04-bulk-load.C
04_bulk_load_CPPFLAGS = $(ROSE_INCLUDES)
04_bulk_load_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 04-func-similarity
04_func_similarity_SOURCES = 04-func-similarity.C
04_func_similarity_CPPFLAGS = $(ROSE_INCLUDES)
04_func_similarity_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 05-clusters-from-pairs
05_clusters_from_pairs_SOURCES = 05-clusters-from-pairs.C
05_clusters_from_pairs_CPPFLAGS = $(ROSE_INCLUDES)
05_clusters_from_pairs_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 06-list-clusters
06_list_clusters_SOURCES = 06-list-clusters.C
06_list_clusters_CPPFLAGS = $(ROSE_INCLUDES)
06_list_clusters_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 06-list-function
06_list_function_SOURCES = 06-list-function.C
06_list_function_CPPFLAGS = $(ROSE_INCLUDES)
06_list_function_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 06-list-ogroup
06_list_ogroup_SOURCES = 06-list-ogroup.C
06_list_ogroup_CPPFLAGS = $(ROSE_INCLUDES)
06_list_ogroup_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += 99-grants
99_grants_SOURCES = 99-grants.C
99_grants_CPPFLAGS = $(ROSE_INCLUDES)
99_grants_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

## noinst_PROGRAMS += CloneDetection
## CloneDetection_SOURCES = CloneDetection.C
## CloneDetection_CPPFLAGS = $(ROSE_INCLUDES)
## CloneDetection_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
##
##noinst_PROGRAMS += clusters_from_pairs
##clusters_from_pairs_SOURCES = clusters_from_pairs.C
##clusters_from_pairs_CPPFLAGS = $(ROSE_INCLUDES)
##clusters_from_pairs_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
##
##noinst_PROGRAMS += show_results
##show_results_SOURCES = show_results.C
##show_results_CPPFLAGS = $(ROSE_INCLUDES)
##show_results_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
##
##noinst_PROGRAMS += call_graph_clones
##call_graph_clones_SOURCES = call_graph_clones.C
##call_graph_clones_CPPFLAGS = $(ROSE_INCLUDES)
##call_graph_clones_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)
##
##noinst_PROGRAMS += compare_outputs
##compare_outputs_SOURCES = compare_outputs.C
##compare_outputs_CPPFLAGS = $(ROSE_INCLUDES)
##compare_outputs_LDADD = $(BOOST_LDFLAGS) libCloneDetection.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)


MOSTLYCLEANFILES += CloneDetectionSchema.C
CloneDetectionSchema.C: Schema.sql
	@echo "  GEN     $@"
	@echo "namespace CloneDetection {" 			>$@
	@echo "const char *schema =" 				>>$@
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

