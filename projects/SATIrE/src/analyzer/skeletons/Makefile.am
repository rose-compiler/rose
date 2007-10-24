EXTRA_DIST = main.C-template  main.h-template  main-support.C-template  Makefile-template.in \
             newanalysis.in undonewanalysis 

dist_pkgdata_DATA = main.C-template  main.h-template  main-support.C-template  Makefile-template

bin_SCRIPTS= newanalysis  undonewanalysis 
CLEANFILES = newanalysis Makefile-template

# necessary to get the right path for the skeletons..
# TODO: replace this with AC_SUBST()

do_subst= sed -e 's,[@]pkgdatadir[@],$(pkgdatadir),g' \
              -e 's,[@]pagdir[@],$(PAGDIR),g' \
              -e 's,[@]roseinclude[@],$(ROSE_INCLUDE_DIR),g' \
              -e 's,[@]roselib[@],$(ROSE_LIB_DIR),g' \
              -e 's,[@]satireinclude[@],$(pkgincludedir),g' \
              -e 's,[@]satirelib[@],$(libdir),g'

newanalysis: newanalysis.in Makefile
	$(do_subst) < $(srcdir)/newanalysis.in > newanalysis
	chmod +x newanalysis

Makefile-template: Makefile-template.in Makefile
	$(do_subst) < $(srcdir)/Makefile-template.in > Makefile-template

