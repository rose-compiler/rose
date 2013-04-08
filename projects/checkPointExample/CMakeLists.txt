include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/11/2004): Commented out to avoid compilation (link fails with older Linux OS)
# bin_PROGRAMS = eigenvalue
# eigenvalue_SOURCES = eigenvalue.C main.C simple_matvec.C

# LDADD = $(top_builddir)/src/3rdPartyLibraries/checkPointLibrary/libckpt.so
LDADD = $(LIBS_WITH_RPATH) -lckpt -ldl

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB


