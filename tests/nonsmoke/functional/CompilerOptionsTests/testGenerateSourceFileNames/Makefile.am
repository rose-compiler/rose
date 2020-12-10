include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if !OS_MACOSX

TEST_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator $(ROSE_FLAGS)

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

# This rule is run after automake's internal check rule (which we don't want to use)
test:
	@echo "#################################################"
	@echo "    Test ROSE's parsing of file names from       "
	@echo "#################################################"
	@echo "** The 'roseTranslator -c file -o obj' should compile and not disassemble obj **"
	$(CXX)  -c $(srcdir)/test1.C -o test1.o 
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1.C -o test1.o 
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.C
	sh $(srcdir)/filesNotExist.sh  ./rose_test1.o.s
	$(MAKE) clean
	@echo	"** The 'roseTranslator obj' should produce an executable and not disassemble obj **"
	$(CXX) -c $(srcdir)/test1.C -o test1.o 
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 test1.o 
	sh $(srcdir)/doesFilesExist.sh ./a.out
	sh $(srcdir)/filesNotExist.sh  ./rose_test1.o.s
	$(MAKE) clean
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler.
	@echo "** The 'roseTranslator -rose:binary obj' should disassemble obj **"
	$(CXX) -c $(srcdir)/test1.C -o test1.o 
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:binary -rose:verbose 0 test1.o 
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.o.s
	sh $(srcdir)/filesNotExist.sh  ./a.out
	$(MAKE) clean
endif
endif
# DQ (10/20/2010): Unclear what shuld be done when using C/C++ only language configuration.
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
	@echo "** The 'roseTranslator -o exe source' should compile source and not disassemble obj **"
	$(CXX) -o test1 $(srcdir)/test1.C
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 -o test1 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.C
	sh $(srcdir)/filesNotExist.sh  ./rose_test1.s
	$(MAKE) clean
endif
# DQ (10/20/2010): Unclear what shuld be done when using C/C++ only language configuration.
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler.
	@echo	"** The 'roseTranslator exe source' should compile source and disassemble exe **"
	$(CXX) -o test1 $(srcdir)/test1.C
# DQ (11/5/2016): Failing test using address sanitizer (will let Robb look into this).
if !ROSE_USE_SANITIZER
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 test1 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.C ./rose_test1.s
endif
	$(MAKE) clean
endif
endif
	@echo	"** The 'roseTranslator source' should compile source **"
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh ./a.out ./rose_test1.C
	$(MAKE) clean
# DQ (10/20/2010): Unclear what shuld be done when using C/C++ only language configuration.
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler.
	@echo "** The 'roseTranslator exe'    should disassemble exe **"
	$(CXX) $(srcdir)/test1.C
# DQ (11/5/2016): Failing test using address sanitizer (will let Robb look into this).
if !ROSE_USE_SANITIZER
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 a.out
	sh $(srcdir)/doesFilesExist.sh ./rose_a.out.s
endif
	$(MAKE) clean
endif
endif
# DQ (10/20/2010): Unclear what shuld be done when using C/C++ only language configuration.
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler.
	@echo "** The 'roseTranslator -c src exe'   should compile src and disassemble exe **"
	$(CXX) -o test1 $(srcdir)/test1.C
# DQ (11/5/2016): Failing test using address sanitizer (will let Robb look into this).
if !ROSE_USE_SANITIZER
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1.C test1
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.C ./rose_test1.s
	sh $(srcdir)/filesNotExist.sh  ./a.out
endif
	$(MAKE) clean
endif
endif
	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/CompilerOptionTests/testGenerateSourceFileNames: make check rule complete (terminated normally)   *"
	@echo "*****************************************************************************************************************"

conditional-check-local:
	$(MAKE) test
	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/CompilerOptionTests/testGenerateSourceFileNames: ############# TESTS COMMENTED OUT ############   *"
	@echo "*****************************************************************************************************************"

else
conditional-check-local:
endif

EXTRA_DIST = doesFilesExist.sh  filesNotExist.sh test1.C

check-local: conditional-check-local

clean-local:
	rm -f *.o test1 *.s rm *.out rose_*.C *.C.pdf *.dot core local_test-isystemOption.C *.dump *.new


