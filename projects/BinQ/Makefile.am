include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS =  gui

if ROSE_USE_QT

bin_PROGRAMS = BinQ 
#nobin_PROGRAMS = test

MOC_SOURCES = moc_slide.C  moc_MyBars.C

#test_SOURCES = test.C

BinQ_SOURCES = \
	BinQMain.C \
	BinQSupport.C \
	BinQGui.C \
	slide.C \
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
	BinQAbstract.C \
	BinQbatch.C \
	ForbiddenFunctionCall.C \
	BinQinteractive.C \
	NullAfterFree.C \
	MallocAndFree.C \
	InitPointerToNull.C \
	ComplexityMetric.C \
	DwarfFileInfo.C

nodist_BinQ_SOURCES = $(MOC_SOURCES) 

# This section was added by Gabriel and generates moc files 
# tps: I added the sed line that includes the rose.h on top of the generated file
if QROSE_USE_WINDOWS
moc_%.C: $(srcdir)/%.h 
	echo $(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.C: $(srcdir)/%.h 
	 $(MOC) -o $@ $<
endif 


BinQ_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS)

#test_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 

# DQ (4/20/2009): Added valgrind support...
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind

# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =


# DQ (1/15/2009): Added demo rule to support ROSE reviews.

# Show the source code used to build the binary used for some of the demos (below).
demo_show_source_code:
	cat $(srcdir)/buffer2.c

# This demo runs a predefined (hard coded) collection of analysis algorithms as part of the startup
demo_withAnalysisRunInBatchMode:
	cat $(srcdir)/buffer2.c
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin --batch

# This demo will show the represntation of the binary before any analysis has been done.
demo_withoutAnalysisRunInBatchMode:
	cat $(srcdir)/buffer2.c
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin

# Show the source code used to build the binary used for the more complex buffer overflow (below).
demo_show_rted_example_source_code:
	cat  $(srcdir)/bufftest.c

# This demo is of the buffer overflow that we can not detect (partial word length overlow).
demo_rted_example:
	cat  $(srcdir)/bufftest.c
	$(VALGRIND) ./BinQ -a $(srcdir)/bufftest.bin --batch

demo_rose_vs_IDAPro:
#	./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv  
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin.32bit -b $(srcdir)/buffer2-tsv  

demo_diff:
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin  


demo2:
	cat $(srcdir)/buffer2.c
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c --batch

demo3:
	$(VALGRIND) ./BinQ -a $(srcdir)/linux-ls --batch

test_1:
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin  

test_2:
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv  

test_3:
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c  

test_4:
	$(VALGRIND) ./BinQ -a $(srcdir)/linux-ls

test_fileASTFile:
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -save test.ast --batch


check-local:
	$(VALGRIND) ./BinQ -a $(srcdir)/win_buffer.exe --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/win_buffer.exe-tsv --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin --test
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin $(srcdir)/buffer2b.bin --test
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin --test
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv --test 
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c --test 
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin $(srcdir)/buffer2b.bin --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2b.bin --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2-tsv --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/buffer2.bin -b $(srcdir)/buffer2.c --test --batch
	$(VALGRIND) ./BinQ -a $(srcdir)/linux-ls --test
	$(VALGRIND) ./BinQ -a $(srcdir)/linux-ls --test --batch
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"


CLEANFILES = 


clean:
	rm -f *.o BinQ *.s *.dump *.txt moc_*.C 

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
             DwarfFileInfo.h \
             buffer2.bin bufftest.c buffer2.c buffer2b.bin buffer2.bin.32bit linux-ls \
             buffer2-tsv/address_comments_1.tsv buffer2-tsv/address_references_1.tsv buffer2-tsv/basic_blocks_1.tsv buffer2-tsv/callgraph_1.tsv \
             buffer2-tsv/control_flow_graph_1.tsv buffer2-tsv/expression_substitutions_1.tsv buffer2-tsv/expression_tree_1.tsv buffer2-tsv/functions_1.tsv \
             buffer2-tsv/instructions_1.tsv buffer2-tsv/modules.tsv buffer2-tsv/operand_expressions_1.tsv buffer2-tsv/operand_strings_1.tsv \
             buffer2-tsv/operand_tuples_1.tsv win_buffer.exe win_buffer.cpp win_buffer.exe-tsv


