include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# path to headers for ROSE, BUPC, and global.tld 
# BUPC_INCLUDE = $(ROSE_INCLUDES) -I$(srcdir)/../include 
BUPC_INCLUDE = -I$(srcdir)/../include 
BUPC_MACROS = 

ROSEUPCC_FLAGS = -rose:skipfinalCompileStep --edg:no_warnings 

EXTRA_DIST = barrier.upc bupcr_magic.c hello.upc shared_1.upc unshared_2.upc \
             extern.upc unshared_1.upc

#individual compilation targets
rose_barrier.c:../roseupcc
	../roseupcc -c $(ROSEUPCC_FLAGS) $(srcdir)/barrier.upc
rose_hello.c:../roseupcc
	../roseupcc -c $(ROSEUPCC_FLAGS) $(srcdir)/hello.upc
# various shared variables
rose_shared_1.c:../roseupcc
	../roseupcc -c $(ROSEUPCC_FLAGS) $(srcdir)/shared_1.upc
rose_shared_1.c.aa: rose_shared_1.c
	split -67 rose_shared_1.c rose_shared_1.c.
	test -f rose_shared_1.c.aa
	fold rose_shared_1.c.aa > rose_shared_1.c.aaf
	test -f rose_shared_1.c.ab
	fold rose_shared_1.c.ab > rose_shared_1.c.abf

rose_unshared_1.c:../roseupcc
	../roseupcc -c $(ROSEUPCC_FLAGS) $(srcdir)/unshared_1.upc
	split -60 rose_unshared_1.c rose_unshared_1.c.
	test -f rose_unshared_1.c.aa
	fold rose_unshared_1.c.aa > rose_unshared_1.c.aaf
	test -f rose_unshared_1.c.ab
	fold rose_unshared_1.c.ab > rose_unshared_1.c.abf

#unshared_1.upc refers to an extern variable defined in extern.upc
rose_extern.c:../roseupcc
	../roseupcc -c $(ROSEUPCC_FLAGS) $(srcdir)/extern.upc

# DQ (9/20/2010): This test code must be compiled as a UPC++ test using the "-rose:UPCxx" option.
# This is because the type checking is different with C++ as a base language for the UPC support.
# In general the typechecking is more strict so that UPC++ might fail and UPC pass, but this is 
# reversed for this example.
# unshared pointers
rose_unshared_2.c:../roseupcc
	../roseupcc -rose:UPCxx -c $(ROSEUPCC_FLAGS) $(srcdir)/unshared_2.upc

# generate global thread local data structure	
global.tld:
	cat *_obj.tld | sort -u -k 3,3rn -k 1,1 >global.tld
# Test linking phase processing: 
#   find per-file alloc/init functions from multiple files and 
#   insert calls of them into bupcr_magic.c	
#   current build dir is needed to include global.tld
rose_bupcr_magic.c:../roseupcc global.tld
	../roseupcc -c $(ROSEUPCC_FLAGS) -I. $(UPC_MACROS) $(BUPC_INCLUDE) $(srcdir)/bupcr_magic.c rose_unshared_1.c rose_extern.c

# check	rules
check-local:
	@echo "Test for ROSE UPC Translator."
	$(MAKE) rose_barrier.c
	$(MAKE) rose_hello.c
	$(MAKE) rose_shared_1.c
	$(MAKE) rose_shared_1.c.aa
	$(MAKE) rose_unshared_1.c
	$(MAKE) rose_unshared_1.c.aa
	$(MAKE) rose_extern.c
	$(MAKE) rose_unshared_2.c
	$(MAKE) rose_bupcr_magic.c
	@echo "*****************************************************************************************"
	@echo "****** ROSE/projects/UpcTranslator/tests: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************"

clean-local:
	rm -rf a.out *.o rose_* *.dot *.tld
