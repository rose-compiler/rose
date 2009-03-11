include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "    Test ROSEs parsing of file names from        "
	@echo "#################################################"
	@echo "** The 'roseTranslator -c file -o obj' should compile and not disassemble obj **"
	$(CXX)  -c $(srcdir)/test1.C -o test1.o 
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1.C -o test1.o 
	sh $(srcdir)/doesFilesExist.sh rose_test1.C
	sh $(srcdir)/filesNotExist.sh  rose_test1.o.s
	$(MAKE) clean

	@echo	"** The 'roseTranslator obj' should produce an executable and not disassemble obj **"
	$(CXX) -c $(srcdir)/test1.C -o test1.o 
	$(TEST_TRANSLATOR) -rose:verbose 0 test1.o 
	sh $(srcdir)/doesFilesExist.sh a.out
	sh $(srcdir)/filesNotExist.sh  rose_test1.o.s
	$(MAKE) clean

	@echo "** The 'roseTranslator -rose:binary obj' should disassemble obj **"
	$(CXX) -c $(srcdir)/test1.C -o test1.o 
	$(TEST_TRANSLATOR) -rose:binary -rose:verbose 0 test1.o 
	sh $(srcdir)/doesFilesExist.sh rose_test1.o.s
	sh $(srcdir)/filesNotExist.sh  a.out
	$(MAKE) clean

	@echo "** The 'roseTranslator -o exe source' should compile source and not disassemble obj **"
	$(CXX) -o test1 $(srcdir)/test1.C
	$(TEST_TRANSLATOR) -rose:verbose 0 -o test1 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh rose_test1.C
	sh $(srcdir)/filesNotExist.sh  rose_test1.s
	$(MAKE) clean

	@echo	"** The 'roseTranslator exe source' should compile source and disassemble exe **"
	$(CXX) -o test1 $(srcdir)/test1.C
	$(TEST_TRANSLATOR) -rose:verbose 0 test1 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh rose_test1.C rose_test1.s
	$(MAKE) clean

	@echo	"** The 'roseTranslator source' should compile source **"
	$(TEST_TRANSLATOR) -rose:verbose 0 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh a.out rose_test1.C
	$(MAKE) clean

	@echo "** The 'roseTranslator exe'    should disassemble exe **"
	$(CXX) $(srcdir)/test1.C
	$(TEST_TRANSLATOR) -rose:verbose 0 a.out
	sh $(srcdir)/doesFilesExist.sh rose_a.out.s
	$(MAKE) clean

	@echo "** The 'roseTranslator -c src exe'   should compile src and disassemble exe **"
	$(CXX) -o test1 $(srcdir)/test1.C
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1.C test1
	sh $(srcdir)/doesFilesExist.sh rose_test1.C rose_test1.s
	sh $(srcdir)/filesNotExist.sh  a.out
	$(MAKE) clean

	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/CompilerOptionTests/testGenerateSourceFileNames: make check rule complete (terminated normally)   *"
	@echo "*****************************************************************************************************************"


EXTRA_DIST = doesFilesExist.sh  filesNotExist.sh test1.C 

clean-local:
	rm -f *.o test1 *.s rm *.out rose_*.C *.C.pdf *.dot core local_test-isystemOption.C *.dump *.new

