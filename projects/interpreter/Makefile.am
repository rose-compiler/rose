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
    testInput_core_agginit.c \
    testInput_core_arrays.C \
    testInput_core_boolcast.C \
    testInput_core_casting.C \
    testInput_core_condExp.C \
    testInput_core_constructors.C \
    testInput_core_pointersFunctions.C \
    testInput_extcall_hello.c \
    testInput_smt_simpleIntegerOverflow.c \
    testInput_smt_abs.c \
    testInput_smt_absBug.c \
    testInput_smt_excludedMiddle.c \
    testInput_smt_reverse.c \
    testInput_md5.c

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
	./coreTest -interp:expectedReturnValue 12 $(srcdir)/testInput_core_agginit.c
	./coreTest -interp:expectedReturnValue 0 $(srcdir)/testInput_core_boolcast.C
	./coreTest -interp:expectedReturnValue 3 $(srcdir)/testInput_core_casting.C
	./coreTest -interp:expectedReturnValue 2 $(srcdir)/testInput_core_condExp.C
	./coreTest -interp:expectedReturnValue 91 $(srcdir)/testInput_core_pointersFunctions.C
	./coreTest -interp:expectedReturnValue 20 $(srcdir)/testInput_core_constructors.C
	./coreTest -interp:expectedReturnValue 59 $(srcdir)/testInput_core_arrays.C
	./coreTest -interp:expectedReturnStr $$'\xb4\xc3\x03\x45\x04\x5f\x6c\x35\x36\xef\x43\x59\xef\xfc\x34\xd8' $(srcdir)/testInput_md5.c
if ROSE_USE_LIBFFI
	./extcallInterpreter $(srcdir)/testInput_extcall_hello.c ; test $$? = 123
endif
if ROSE_USE_TEST_SMT_SOLVER
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure $(srcdir)/testInput_smt_simpleIntegerOverflow.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure $(srcdir)/testInput_smt_absBug.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput_smt_abs.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput_smt_excludedMiddle.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" $(srcdir)/testInput_smt_reverse.c
	./smtTest -interp:smtSolver "$(TEST_SMT_SOLVER)" -interp:expectAssertionFailure -DSMT $(srcdir)/testInput_md5.c
endif
