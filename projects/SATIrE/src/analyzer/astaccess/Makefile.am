AM_CPPFLAGS = -I$(ROSE_INCLUDE_DIR) \
    -I$(BOOSTDIR)/include \
    -I$(top_builddir)/src/satire \
    -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
    -I$(top_srcdir)/src/analyzer/astaccess/satire \
    -I$(top_srcdir)/src/analyzer/astnumbering/satire \
    -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
    -I$(top_srcdir)/src/analyzer/support/satire \
    -I$(top_srcdir)/src/analyzer/annotation/satire \
    -I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
    -I$(top_srcdir)/src/analyzer/provided/aslan/satire \
    -I$(top_srcdir)/src/aral/src/satire \
    -I$(top_srcdir)/src/termite/satire \
    -I$(top_builddir)/src/termite/satire

if WITH_PAG
AM_CPPFLAGS += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
AM_CPPFLAGS += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

SYNDIR=$(top_builddir)/src/analyzer/astaccess/satire

AM_CPPFLAGS += -I$(SYNDIR)

noinst_LTLIBRARIES = libAstAccess.la
nobase_include_HEADERS = satire/ast_support_funcs.h \
                         satire/iface.h \
                         satire/other.h \
                         satire/o_ExpressionId.h \
                         satire/o_TypeId.h \
                         satire/o_VariableId.h \
			 satire/o_ContextInfo.h \
                         satire/pag_support.h \
                         satire/pignodelist.h \
                         satire/syn_typedefs.h \
                         satire/syntree.h \
                         satire/syntree.c

libAstAccess_la_SOURCES = pag_support.C

BUILT_SOURCES = $(SYNDIR)/syntree.h  $(SYNDIR)/syntree.c $(SYNDIR)/syntree.cxx
CLEANFILES =    $(SYNDIR)/syntree.h  $(SYNDIR)/syntree.c $(SYNDIR)/syntree.cxx \
                      pig_temp.h        pig_temp.c \
                     pig_temp2.h       pig_temp2.c 

PIGDIR = $(top_builddir)/src/pig
PIG = $(PIGDIR)/pig

lib_LTLIBRARIES = satire/libsyntree.la libpagdatatypes.la
satire_libsyntree_la_SOURCES = \
	$(SYNDIR)/syntree.cxx
libpagdatatypes_la_SOURCES = \
	o_ExpressionId.C o_TypeId.C o_VariableId.C o_ContextInfo.C

# Does PAG expect those really to be in the same directory as the
# the includes, our could we put them into $(prefix)/share/ ?
nobase_include_HEADERS+= satire/edges satire/syn satire/pagoptions \
                           satire/pagoptions.set satire/pagoptions.optla
#dist_pkgdata_DATA = edges syn pagoptions

EXTRA_DIST = rose.pig #syn edges pagoptions

# This trick won't work with a newer automake any more -- adrian
#syntree.o: satire/syntree.c satire/syntree.h
#	$(CXXCOMPILE) -c -o syntree.o -Isatire satire/syntree.c	 

$(PIG):
	$(MAKE) -C $(PIGDIR) pig

satire/syntree.c: $(SYNDIR)/syntree.c
satire/syntree.h: $(SYNDIR)/syntree.h

# An ugly hack indeed, but haven't found a better way 
# to confine those header files into include/satire
$(SYNDIR)/syntree.c: $(top_srcdir)/src/analyzer/astaccess/satire/syn $(top_srcdir)/src/analyzer/astaccess/rose.pig $(PIG)
	$(PIG) -ip $(top_srcdir)/src/analyzer/astaccess/satire/syn \
	           $(top_srcdir)/src/analyzer/astaccess/rose.pig \
	&& mkdir -p satire \
	&& mv syntree.h syntree.c satire/ 

$(SYNDIR)/syntree.h: $(SYNDIR)/syntree.c # dummy dependency for parallel builds

$(SYNDIR)/syntree.cxx: $(SYNDIR)/syntree.c
	cp $< $@
