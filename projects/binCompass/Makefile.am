include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = buffer2.bin

SUBDIRS = analyses graphanalyses

INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(LTDLINCL)

bin_PROGRAMS = binCompassMain 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl $(LIBLTDL)

binCompassMain_SOURCES = binCompassMain.cpp  

check-local: binCompassMain 
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -printTree -cfa ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -callgraph  ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -dfa -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter -gml****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -dfa -inter -gml >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -dfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkAST -inter ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -checkAST -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkGraph -inter ****************************************************"
	./binCompassMain $(srcdir)/buffer2.bin -checkGraph -inter > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

CLEANFILES = dfg.dot cfg.dot unparsed.s dfg.gml out/buffer2.bin.PrintAsmInstruction.out out/buffer2.bin.PrintAsmFunctions.out output out/*

binCompassMain.o: binCompassMain.cpp

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
	rm -f analyses/*.la analyses/*.lo analyses/*.o graphanalyses/*.o graphanalyses/*.lo
