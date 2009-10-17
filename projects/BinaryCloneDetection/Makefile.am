include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use --with-libxml2 on configure command-line for ROSE to activate this


SUBDIRS =  gui

if ROSE_USE_SQLITE_DATABASE
# if HAVE_SSL
if ROSE_USE_SSL_SUPPORT

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)

bin_PROGRAMS = createVectorsBinary  printOutClones

include_HEADERS = createSignatureVectors.h  vectorCompression.h 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

printOutClones_SOURCES = printOutClones.C 

createVectorsBinary_SOURCES = createVectorsBinary.C createSignatureVectors.C vectorCompression.h vectorCompression.C

endif
endif

clean-local:
	rm -f 

