include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =

DEFAULT_DATABASE = postgresql://rose:fcdc7b4207660a1372d0cd5491ad856e@www.hoosierfocus.com/rose_matrix

bin_PROGRAMS += matrixNextTest
matrixNextTest_SOURCES = matrixNextTest.C
matrixNextTest_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
matrixNextTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS += matrixTestResult
matrixTestResult_SOURCES = matrixTestResult.C
matrixTestResult_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
matrixTestResult_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS += matrixQueryTable
matrixQueryTable_SOURCES = matrixQueryTable.C
matrixQueryTable_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
matrixQueryTable_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS += matrixAttachments
matrixAttachments_SOURCES = matrixAttachments.C
matrixAttachments_CPPFLAGS = $(ROSE_INCLUDES) -DDEFAULT_DATABASE='"$(DEFAULT_DATABASE)"'
matrixAttachments_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
