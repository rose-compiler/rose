# Binary clone detection.  Finds semantically similar functions based on fuzz testing. See Andreas and Earl for details.
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
noinst_PROGRAMS=

noinst_PROGRAMS += CloneDetection
EXTRA_DIST += Schema.sql			
CloneDetection_SOURCES = CloneDetection.C CloneDetectionOutputs.C CloneDetectionProgress.C CloneDetectionSchema.C
CloneDetection_CPPFLAGS = $(ROSE_INCLUDES)
CloneDetection_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += clusters_from_pairs
clusters_from_pairs_SOURCES = clusters_from_pairs.C
clusters_from_pairs_CPPFLAGS = $(ROSE_INCLUDES)
clusters_from_pairs_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += show_results
show_results_SOURCES = show_results.C
show_results_CPPFLAGS = $(ROSE_INCLUDES)
show_results_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += call_graph_clones
call_graph_clones_SOURCES = call_graph_clones.C CloneDetectionProgress.C
call_graph_clones_CPPFLAGS = $(ROSE_INCLUDES)
call_graph_clones_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += compare_outputs
compare_outputs_SOURCES = compare_outputs.C CloneDetectionOutputs.C CloneDetectionProgress.C
compare_outputs_CPPFLAGS = $(ROSE_INCLUDES)
compare_outputs_LDADD = $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)


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

