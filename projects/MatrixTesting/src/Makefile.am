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
rose_matrix_test_result_SOURCES = matrixTestResult.C
rose_matrix_test_result_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_test_result_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_test_result_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-query
rose_matrix_query_SOURCES = matrix-query.C
rose_matrix_query_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_query_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_query_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-attachments
rose_matrix_attachments_SOURCES = matrixAttachments.C
rose_matrix_attachments_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_attachments_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_attachments_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-errors
rose_matrix_errors_SOURCES = matrixErrors.C
rose_matrix_errors_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_errors_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_errors_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-delete-test
rose_matrix_delete_test_SOURCES = matrixDeleteTest.C
rose_matrix_delete_test_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_delete_test_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_delete_test_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-dependencies
rose_matrix_dependencies_SOURCES = matrix-dependencies.C
rose_matrix_dependencies_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_dependencies_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_dependencies_LDADD = libmatrixTools.a $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-status
rose_matrix_status_SOURCES = matrix-status.C
rose_matrix_status_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_status_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_status_LDADD = libmatrixTools.a $(ROSE_LIBS)

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

if ROSE_USE_WT

bin_PROGRAMS += rose-matrix-web-server-http
rose_matrix_web_server_http_SOURCES = matrixWebServer.C
rose_matrix_web_server_http_CPPFLAGS = $(ROSE_INCLUDES) -I$(WT_PREFIX)/include -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_web_server_http_LDFLAGS = $(ROSE_RPATHS) -R $(WT_PREFIX)/lib
rose_matrix_web_server_http_LDADD = -L$(WT_PREFIX)/lib -lwt -lwthttp -lwtdbo -lwtdbopostgres $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-web-server-fcgi
rose_matrix_web_server_fcgi_SOURCES = matrixWebServer.C
rose_matrix_web_server_fcgi_CPPFLAGS = $(ROSE_INCLUDES) -I$(WT_PREFIX)/include -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE_LOCAL)"' -DUSING_FASTCGI
rose_matrix_web_server_fcgi_LDFLAGS = $(ROSE_RPATHS) -R $(WT_PREFIX)/lib
rose_matrix_web_server_fcgi_LDADD = -L$(WT_PREFIX)/lib -lwt -lwtfcgi -lwtdbo -lwtdbopostgres $(ROSE_LIBS)

endif

