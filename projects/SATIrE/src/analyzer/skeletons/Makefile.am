EXTRA_DIST = main.C-template main.h-template \
			 main-support.C-template main-support.h-template \
			 satire_dataflow_analysis_implementation.h-template \
			 satire_dataflow_analysis_implementation.C-template \
             Makefile-template.in newanalysis.in newcombinedanalysis.in \
			 undonewanalysis runtests-template

dist_pkgdata_DATA = main.C-template main.h-template main-support.C-template main-support.h-template Makefile-template runtests-template prefixed_support_funcs.h-template satire_dataflow_analysis_implementation.h-template satire_dataflow_analysis_implementation.C-template

bin_SCRIPTS= newanalysis  undonewanalysis newcombinedanalysis
CLEANFILES = newanalysis Makefile-template newcombinedanalysis

# necessary to get the right path for the skeletons..
# TODO: replace this with AC_SUBST()

do_subst= sed -e 's,[@]pkgdatadir[@],$(pkgdatadir),g' \
              -e 's,[@]bindir[@],$(bindir),g' \
              -e 's,[@]pagdir[@],$(PAGDIR),g' \
              -e 's,[@]roseinclude[@],$(ROSE_INCLUDE_DIR),g' \
              -e 's,[@]roselib[@],$(ROSE_LIB_DIR),g' \
              -e 's,[@]boostinclude[@],$(BOOST_INCLUDE_DIR),g' \
              -e 's,[@]boostlib[@],$(BOOST_LIB_DIR),g' \
	      -e 's,[@]plinclude[@],$(SWI_PROLOG_INCLUDE_DIR),g' \
              -e 's|[@]pllibs[@]|$(SWI_PROLOG_LIBS)|g' \
              -e 's|[@]plldadd[@]|$(SWI_PROLOG_LDADD)|g' \
              -e 's,[@]satireinclude[@],$(pkgincludedir),g' \
              -e 's,[@]satirelib[@],$(libdir),g' \
              -e 's,[@]cc[@],$(CC),g' \
              -e 's,[@]cxx[@],$(CXX),g'

newanalysis: newanalysis.in Makefile
	$(do_subst) < $(srcdir)/newanalysis.in > newanalysis \
	&& $(do_subst) < $(srcdir)/newcombinedanalysis.in > newcombinedanalysis\
	&& chmod +x newanalysis newcombinedanalysis

newcombinedanalysis: newanalysis # dummy dep. for make -j

Makefile-template: Makefile-template.in Makefile
	$(do_subst) < $(srcdir)/Makefile-template.in > Makefile-template

