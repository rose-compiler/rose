include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =

DEFAULT_DATABASE       = 
DEFAULT_DATABASE_LOCAL = 

bin_PROGRAMS += rose-matrix-next-test
rose_matrix_next_test_SOURCES = matrixNextTest.C
rose_matrix_next_test_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_next_test_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_next_test_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-test-result
rose_matrix_test_result_SOURCES = matrixTestResult.C
rose_matrix_test_result_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_test_result_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_test_result_LDADD = $(ROSE_LIBS)

bin_PROGRAMS += rose-matrix-query
rose_matrix_query_SOURCES = matrixQueryTable.C
rose_matrix_query_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
rose_matrix_query_LDFLAGS = $(ROSE_RPATHS)
rose_matrix_query_LDADD = $(ROSE_LIBS)

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

