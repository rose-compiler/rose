EXTRA_DIST = main.C-template  main.h-template  main-support.C-template  main-support.h-template  \
             Makefile-template.in newanalysis.in undonewanalysis runtests-template newcombinedanalysis.in

dist_pkgdata_DATA = main.C-template  main.h-template  main-support.C-template  main-support.h-template  Makefile-template runtests-template prefixed_support_funcs.h-template

bin_SCRIPTS= newanalysis  undonewanalysis newcombinedanalysis
CLEANFILES = newanalysis Makefile-template newcombinedanalysis

# necessary to get the right path for the skeletons..
# TODO: replace this with AC_SUBST()

do_subst= sed -e 's,[@]pkgdatadir[@],$(pkgdatadir),g' \
		      -e 's,[@]bindir[@],$(bindir),g' \
              -e 's,[@]pagdir[@],$(PAGDIR),g' \
              -e 's,[@]roseinclude[@],$(ROSE_INCLUDE_DIR),g' \
              -e 's,[@]roselib[@],$(ROSE_LIB_DIR),g' \
              -e 's,[@]boostlib[@],$(BOOST_LIB_DIR),g' \
              -e 's,[@]satireinclude[@],$(pkgincludedir),g' \
              -e 's,[@]satirelib[@],$(libdir),g' \
              -e 's,[@]cxx[@],$(CXX),g'

newanalysis: newanalysis.in Makefile
	$(do_subst) < $(srcdir)/newanalysis.in > newanalysis
	$(do_subst) < $(srcdir)/newcombinedanalysis.in > newcombinedanalysis
	chmod +x newanalysis newcombinedanalysis

Makefile-template: Makefile-template.in Makefile
	$(do_subst) < $(srcdir)/Makefile-template.in > Makefile-template

