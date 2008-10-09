include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS =  gui

if ROSE_USE_QROSE

bin_PROGRAMS = BinQ 

BinQ_SOURCES = \
	BinQMain.C \
        BinQGui.C

      
BinQ_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 

EXTRA_DIST = 

clean:
	rm -f *.o BinQ

endif
