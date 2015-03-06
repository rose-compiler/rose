
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = logger

$(top_builddir)/src/frontend/DLX/lib/libDLX-core.la:
	make -C $(top_builddir)/src/frontend/DLX/lib/core libDLX-core.la

$(builddir)/../lib/dlx/logger/libDLX-logger.la:
	make -C $(builddir)/../lib/dlx/logger libDLX-logger.la

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

$(top_builddir)/src/backend/MDCG/lib/libMDCG.la:
	$(MAKE) -C $(top_builddir)/src/backend/MDCG/lib libMDCG.la

$(builddir)/../lib/mdcg/logger/libMDCG-logger.la:
	make -C $(builddir)/../lib/mdcg/logger libMDCG-logger.la

logger_SOURCES = \
  logger.cpp
logger_CXXFLAGS = \
  -DLIBLOGGER_INC_PATH=\"$(srcdir)/../include/\" \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
logger_LDADD= \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(builddir)/../lib/dlx/logger/libDLX-logger.la \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(builddir)/../lib/mdcg/logger/libMDCG-logger.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

