include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use --with-libxml2 on configure command-line for ROSE to activate this

if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES)  $(SQLITE_DATABASE_INCLUDE)

bin_PROGRAMS = printOutClones

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

printOutClones_SOURCES = printOutClones.C 

endif

clean-local:
	rm -f 

