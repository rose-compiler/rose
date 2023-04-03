include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =

DEFAULT_DATABASE       = 
DEFAULT_DATABASE_LOCAL =

lib_LIBRARIES = libmatrixTools.a
libmatrixTools_a_SOURCES = matrixTools.C
libmatrixTools_a_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS += rose-matrix-next-test
rose_matrix_next_test_SOURCES = matrix-next-test.C
rose_matrix_next_test_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_next_test_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_next_test_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-test-result
rose_matrix_test_result_SOURCES = matrix-test-result.C
rose_matrix_test_result_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_test_result_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_test_result_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-query
rose_matrix_query_SOURCES = matrix-query.C
rose_matrix_query_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_query_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_query_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-attachments
rose_matrix_attachments_SOURCES = matrix-attachments.C
rose_matrix_attachments_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_attachments_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_attachments_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-errors
rose_matrix_errors_SOURCES = matrix-errors.C
rose_matrix_errors_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_errors_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_errors_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-dependencies
rose_matrix_dependencies_SOURCES = matrix-dependencies.C
rose_matrix_dependencies_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_dependencies_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_dependencies_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-slave-config
rose_matrix_slave_config_SOURCES = matrix-slave-config.C
rose_matrix_slave_config_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_slave_config_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_slave_config_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-slave-health
rose_matrix_slave_health_SOURCES = matrix-slave-health.C
rose_matrix_slave_health_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_slave_health_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_slave_health_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-phases
rose_matrix_phases_SOURCES = matrix-phases.C
rose_matrix_phases_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_phases_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_phases_LDADD = libmatrixTools.a $(ROSE_LIBS)

# These headers are not installed
not_installed_headers = matrixTools.h
