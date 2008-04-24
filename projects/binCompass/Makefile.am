include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_BINARY_ANALYSIS_SUPPORT

SUBDIRS = analyses graphanalyses

INCLUDES = $(ROSE_INCLUDES) -I$(builddir)

bin_PROGRAMS = binCompassMain 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

binCompassMain_SOURCES = binCompassMain.cpp  
binCompassMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

check-local: binCompassMain 
	cp $(srcdir)/buffer2.bin .
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -printTree -cfa ****************************************************"
	./binCompassMain buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -callgraph  ****************************************************"
	./binCompassMain buffer2.bin -cfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter ****************************************************"
	./binCompassMain buffer2.bin -dfa -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa -inter -gml****************************************************"
	./binCompassMain buffer2.bin -dfa -inter -gml >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -dfa ****************************************************"
	./binCompassMain buffer2.bin -dfa >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkAST -inter ****************************************************"
	./binCompassMain buffer2.bin -checkAST -inter >output
	@echo " "
	@echo " *** 	./binCompassMain buffer2.bin -checkGraph -inter ****************************************************"
	./binCompassMain buffer2.bin -checkGraph -inter > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

EXTRA_DIST = buffer2.bin

binCompassMain.o: binCompassMain.cpp

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
	rm -f analyses/*.la analyses/*.lo analyses/*.o graphanalyses/*.o graphanalyses/*.lo

endif
