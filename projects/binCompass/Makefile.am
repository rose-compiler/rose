include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = buffer2.bin

SUBDIRS = analyses graphanalyses

AM_CPPFLAGS = $(ROSE_INCLUDES) -I. $(LTDLINCL)

bin_PROGRAMS = binCompassMain 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl $(LIBLTDL)

binCompassMain_SOURCES = binCompassMain.cpp  

# DQ (4/20/2009): Added valgrind support...
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind

# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

check-local: binCompassMain$(EXEEXT)
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -printTree -cfa ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -callgraph  ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -dfa -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter -gml****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -dfa -inter -gml >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -dfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkAST -inter ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -checkAST -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkGraph -inter ****************************************************"
	$(VALGRIND) ./binCompassMain$(EXEEXT) $(srcdir)/buffer2.bin -checkGraph -inter > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

CLEANFILES = *.dot dfg.dot thomas.dot cfg.dot unparsed.s dfg.gml out/buffer2.bin.PrintAsmInstruction.out out/buffer2.bin.PrintAsmFunctions.out output out/*

binCompassMain.o: binCompassMain.cpp

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
	rm -f analyses/*.la analyses/*.lo analyses/*.o graphanalyses/*.o graphanalyses/*.lo
