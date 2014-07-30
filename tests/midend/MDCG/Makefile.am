include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = model-to-dot

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

$(top_builddir)/src/backend/MDCG/lib/libMDCG.la:
	$(MAKE) -C $(top_builddir)/src/backend/MDCG/lib libMDCG.la

model_to_dot_SOURCES = model-to-dot.cpp
model_to_dot_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
model_to_dot_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:

clean-local:
	rm -f *.dat *.png *.dot

