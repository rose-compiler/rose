include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS =  gui

if ROSE_USE_QT

bin_PROGRAMS = BinQ 

BinQ_SOURCES = \
	BinQMain.C \
	BinQSupport.C \
	BinQGui.C \
	slide.C \
	Clone.C \
	AlignFunctions.C \
	BinCallGraph.C \
	BinControlFlowAnalysis.C \
	BinDataFlowAnalysis.C \
	BufferOverflow.C \
	InterruptAnalysis.C




BinQ_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


check-local:
	./BinQ --fileA $(srcdir)/buffer2.bin --fileB $(srcdir)/buffer2b.bin --test 
	./BinQ --fileA $(srcdir)/buffer2.bin --fileB $(srcdir)/buffer2-tsv --test 
	./BinQ --fileA $(srcdir)/buffer2.bin --fileB $(srcdir)/buffer2.c --test 
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"


CLEANFILES = 


clean:
	rm -f *.o BinQ *.s *.dump

endif

EXTRA_DIST = BinQGui.h \
             BinQSupport.h \
             AlignFunctions.h \
             BinAnalyses.h \
             slide.h \
             disks.xpm  \
             folder.xpm \
             icons.h \
             Item.h \
             Clone.h \
             BinCallGraph.h \
             BinControlFlowAnalysis.h \
             BinDataFlowAnalysis.h \
             BufferOverflow.h \
             InterruptAnalysis.h

