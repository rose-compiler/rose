include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE

bin_PROGRAMS = astEquivalence runIDA

astEquivalence_SOURCES = \
	astEquivalenceGui.C \
        astEquivalenceMainGui.C

      
astEquivalence_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

runIDA_SOURCES = \
        runIDA.C 
runIDA_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
 
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 

EXTRA_DIST = 


endif
