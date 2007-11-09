INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/astaccess/satire

noinst_LIBRARIES = libAstAccess.a
nobase_include_HEADERS = satire/ast_support_funcs.h \
	                 satire/iface.h \
                         satire/other.h \
                         satire/pag_support.h \
                         satire/pignodelist.h \
                         satire/syn_typedefs.h \
                         satire/syntree.h \
                         satire/syntree.c

libAstAccess_a_SOURCES = pag_support.C

BUILT_SOURCES = satire/syntree.h  satire/syntree.c
CLEANFILES =    satire/syntree.h  satire/syntree.c \
                      pig_temp.h        pig_temp.c \
                     pig_temp2.h       pig_temp2.c 

PIG = ../../pig/pig

lib_LIBRARIES = satire/libsyntree.a
satire_libsyntree_a_SOURCES = satire/syntree.c

# Does PAG expect those really to be in the same directory as the
# the includes, our could we put them into $(prefix)/share/ ?
nobase_include_HEADERS+= satire/edges satire/syn satire/pagoptions
#dist_pkgdata_DATA = edges syn pagoptions

EXTRA_DIST = rose.pig #syn edges pagoptions

syntree.o: satire/syntree.c satire/syntree.h
	$(CXXCOMPILE) -c -o syntree.o -Isatire satire/syntree.c	 

$(PIG):
	$(MAKE) -C ../../pig

satire/syntree.h satire/syntree.c: $(top_srcdir)/src/analyzer/astaccess/satire/syn $(top_srcdir)/src/analyzer/astaccess/rose.pig $(PIG)
	$(PIG) -ip $(top_srcdir)/src/analyzer/astaccess/satire/syn $(top_srcdir)/src/analyzer/astaccess/rose.pig
        # An ugly hack indeed, but haven't found a better way to confine those header files into include/satire
	mkdir -p satire
	mv syntree.h syntree.c satire/
