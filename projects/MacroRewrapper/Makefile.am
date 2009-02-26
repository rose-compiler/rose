include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_SQLITE_DATABASE

#bin_PROGRAMS = macroRewrapper 
bin_PROGRAMS =
macroRewrapper_SOURCES = \
	main.C
      
macroRewrapper_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt



INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 


endif

EXTRA_DIST = 

