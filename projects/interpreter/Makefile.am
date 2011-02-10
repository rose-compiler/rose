include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = \
     $(ROSE_INCLUDES)  -Wall $(MAPLE_INCLUDES) $(LIBFFI_CFLAGS)

bin_PROGRAMS = coreInterpreter mpiInterpreter smtInterpreter test_smtlib
if ROSE_USE_MAPLE
bin_PROGRAMS += mapleInterpreter
endif
if ROSE_USE_LIBFFI
bin_PROGRAMS += extcallInterpreter
endif

noinst_PROGRAMS = coreTest smtTest

noinst_LIBRARIES = libinterpreter.a libsmtlib.a
if ROSE_USE_MAPLE
noinst_LIBRARIES += libmaplepp.a
endif

libinterpreter_a_SOURCES = interp_core.C typeLayoutStore.C interp_mpi.C interp_smt.C
if ROSE_USE_MAPLE
libinterpreter_a_SOURCES += interp_maple.C
endif
if ROSE_USE_LIBFFI
libinterpreter_a_SOURCES += interp_extcall.C
endif

libsmtlib_a_SOURCES = smtlib.C

libmaplepp_a_SOURCES = maple++.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl $(LIBLTDL) libinterpreter.a

coreInterpreter_SOURCES = main_core.C

extcallInterpreter_SOURCES = main_extcall.C
extcallInterpreter_LDADD = $(LDADD) $(LIBFFI_LIBS)

mapleInterpreter_SOURCES = main_maple.C
mapleInterpreter_LDADD = $(LDADD) libmaplepp.a $(MAPLE_LIBS)

mpiInterpreter_SOURCES = main_mpi.C

smtInterpreter_SOURCES = main_smt.C
smtInterpreter_LDADD = $(LDADD) libsmtlib.a

TEST_INPUTS = \
    testInput/core/agginit.c \
    testInput/core/anonUnionCtor.C \
    testInput/core/anonUnionStruct.c \
    testInput/core/arrays.C \
    testInput/core/boolcast.C \
    testInput/core/casting.C \
    testInput/core/condExp.C \
    testInput/core/constructors.C \
    testInput/core/globals.c \
    testInput/core/initunion.c \
    testInput/core/malloc.c \
    testInput/core/mdarr.c \
    testInput/core/nestedUnion.c \
    testInput/core/pointersFunctions.C \
    testInput/core/realloc.c \
    testInput/core/varargs.c \
    testInput/extcall/hello.c \
    testInput/smt/simpleIntegerOverflow.c \
    testInput/smt/abs.c \
    testInput/smt/absBug.c \
    testInput/smt/excludedMiddle.c \
    testInput/smt/reverse.c \
    testInput/smt/mkbvvar.c \
    testInput/md5.c

EXTRA_DIST = interp_core.h interp_extcall.h interp_maple.h interp_mpi.h interp_smt.h maple++.h typeLayoutStore.h smtlib.h $(TEST_INPUTS)

interp_core.o: interp_core.C interp_core.h
main_core.o: main_core.C interp_core.h
test_core.o: test_core.C interp_core.h

interp_extcall.o: interp_extcall.C interp_core.h interp_extcall.h
main_extcall.o: main_extcall.C interp_core.h interp_extcall.h

interp_maple.o: interp_maple.C interp_core.h interp_maple.h maple++.h
main_maple.o: main_maple.C interp_core.h interp_maple.h maple++.h

interp_mpi.o: interp_mpi.C interp_core.h interp_mpi.h
main_mpi.o: main_mpi.C interp_core.h interp_mpi.h

interp_smt.o: interp_smt.C interp_core.h interp_smt.h smtlib.h
main_smt.o: main_smt.C interp_core.h interp_smt.h smtlib.h
test_smt.o: test_smt.C interp_core.h interp_smt.h smtlib.h

smtlib.o: smtlib.C smtlib.h
test_smtlib.o: test_smtlib.C smtlib.h

test_smtlib_SOURCES = test_smtlib.C
test_smtlib_LDADD = libsmtlib.a

coreTest_SOURCES = test_core.C

smtTest_SOURCES = test_smt.C
smtTest_LDADD = $(LDADD) libsmtlib.a

check-local: $(TEST_INPUTS)
	./test_smtlib
	./coreTest -interp:expectedReturnValue 12 $(srcdir)/testInput/core/agginit.c
	./coreTest -interp:expectedReturnValue 0 $(srcdir)/testInput/core/boolcast.C
	./coreTest -interp:expectedReturnValue 3 $(srcdir)/testInput/core/casting.C
	./coreTest -interp:expectedReturnValue 2 $(srcdir)/testInput/core/condExp.C
	./coreTest -interp:expectedReturnValue 91 $(srcdir)/testInput/core/pointersFunctions.C
	./coreTest -interp:expectedReturnValue 20 $(srcdir)/testInput/core/constructors.C
	./coreTest -interp:expectedReturnValue 59 $(srcdir)/testInput/core/arrays.C
	./coreTest -interp:expectedReturnValue 42 $(srcdir)/testInput/core/globals.c
#Liao 2/9/2011. Mac OS X 10.6: builtin functions have no definition.     
if !OS_MACOSX
	./coreTest -interp:expectedReturnStr $$'\xb4\xc3\x03\x45\x04\x5f\x6c\x35\x36\xef\x43\x59\xef\xfc\x34\xd8' $(srcdir)/testInput/md5.c
endif	
	./coreTest -interp:expectedReturnValue 4 $(srcdir)/testInput/core/nestedUnion.c
	./coreTest -interp:expectUndefinedReturnValue $(srcdir)/testInput/core/anonUnionStruct.c
	./coreTest -interp:expectedReturnValue 42 $(srcdir)/testInput/core/anonUnionCtor.C
	./coreTest -interp:expectedReturnValue 0 $(srcdir)/testInput/core/initunion.c
	./coreTest -interp:expectedReturnValue 10 $(srcdir)/testInput/core/varargs.c
	./coreTest -interp:expectedReturnValue 78 $(srcdir)/testInput/core/mdarr.c
if !OS_MACOSX
#	DQ (1/12/2010): This appears to fail under Mac OSX (at least version 10.5).
	./coreTest -interp:expectedReturnValue 3 $(srcdir)/testInput/core/malloc.c
endif
	./coreTest -interp:expectedReturnValue 66 $(srcdir)/testInput/core/realloc.c
if ROSE_USE_LIBFFI
	./extcallInterpreter $(srcdir)/testInput/extcall/hello.c ; test $$? = 123
endif
if ROSE_USE_TEST_SMT_SOLVER
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure $(srcdir)/testInput/smt/simpleIntegerOverflow.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure $(srcdir)/testInput/smt/absBug.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput/smt/abs.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput/smt/excludedMiddle.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput/smt/reverse.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure -DSMT $(srcdir)/testInput/md5.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput/smt/mkbvvar.c
endif
	@echo "***************************************************************************************"
	@echo "****** ROSE/projects/interpreter: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************"
