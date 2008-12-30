include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS =  gui

if ROSE_USE_QT

bin_PROGRAMS = BinQ 

BinQ_SOURCES = \
	BinQMain.C \
	BinQSupport.C \
	BinQGui.C \
	slide.C \
	slide_moc.cpp \
        LCS.C   \
	Clone.C \
        FunctionDiff.C \
	AlignFunctions.C \
	BinCallGraph.C \
	BinControlFlowAnalysis.C \
	BinDataFlowAnalysis.C \
	BufferOverflow.C \
	InterruptAnalysis.C \
	BinDynamicInfo.C \
	MyBars.C \
	MyBars_moc.cpp \
	BinQAbstract.C \
	BinQbatch.C \
	ForbiddenFunctionCall.C \
	BinQinteractive.C \
	NullAfterFree.C \
	MallocAndFree.C \
	InitPointerToNull.C \
	ComplexityMetric.C \
	DwarfFileInfo.C




BinQ_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.

test_1:
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin  

test_2:
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv  

test_3:
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c  

test_4:
	./BinQ -a $(srcdir)/linux-ls


check-local:
	./BinQ -a $(srcdir)/buffer2.bin --test
	./BinQ -a $(srcdir)/buffer2.bin $(srcdir)/buffer2b.bin --test
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin --test
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv --test 
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c --test 
	./BinQ -a $(srcdir)/buffer2.bin --test --batch
	./BinQ -a $(srcdir)/buffer2.bin $(srcdir)/buffer2b.bin --test --batch
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin --test --batch
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv --test --batch
	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c --test --batch
	./BinQ -a $(srcdir)/linux-ls --test
	./BinQ -a $(srcdir)/linux-ls --test --batch
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"


CLEANFILES = 


clean:
	rm -f *.o BinQ *.s *.dump *.txt

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
             FunctionDiff.h \
             LCS.h \
             BinCallGraph.h \
             BinControlFlowAnalysis.h \
             BinDataFlowAnalysis.h \
             BufferOverflow.h \
             InterruptAnalysis.h \
             BinDynamicInfo.h \
             MyBars.h \
             BinQAbstract.h \
             BinQbatch.h \
             ForbiddenFunctionCall.h \
             BinQinteractive.h \
             NullAfterFree.h \
             MallocAndFree.h \
             InitPointerToNull.h \
             ComplexityMetric.h \
             DwarfFileInfo.h

