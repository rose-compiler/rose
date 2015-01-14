
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_BACKSTROKE2

SUBDIRS=\
	src \
	tests


EXTRA_DIST=\
	README

faketarget:
	src/runtime/isstacktest.C
	src/runtime/runtime.C
	src/runtime/runtime.h
	src/runtime/runtime_macros.h
	src/runtime/testruntimesystem.C
	tests/array1/checkarrayoperators.cpp
	tests/io/couttest1.C
	tests/linked-list/900_create_insert_delall.C
	tests/linked-list/901_create_insert_delall.C
	tests/linked-list/902_create_insert_delall.C
	tests/linked-list/rose_900_create_insert_delall.C
	tests/linked-list/rose_900_create_insert_delall_mod2.C
	tests/linked-list/rose_901_create_insert_delall.C
	tests/linked-list/rose_902_create_insert_delall.C
	tests/linked-list/test_2014_09_26_rose_900_create_insert_delall.C
	tests/linked-list/test_rose_900_create_insert_delall_mod.C
	tests/ross-event/rose_simple2-event-funcs-modified-2.c
	tests/ross-event/rose_simple2-event-funcs-modified.c
	tests/ross-event/rose_simple2-event-funcs.c
	tests/ross-event/runtime.C
	tests/ross-event/runtime.h
	tests/ross-event/runtime_macros.h
	tests/ross-event/simple2-bs.c
	tests/ross-event/simple2-event-funcs.c
	tests/ross-event/simple2.c
	tests/vector1/RevStack.h
	tests/vector1/rev_rtest3.C
	tests/vector1/rev_rtest4.C
	tests/vector1/rev_rtest4.cpp.C
	tests/vector1/rev_rtest4.cpppure.C
	tests/vector1/revcheck.C
	tests/vector1/revtest.C
	tests/vector1/rose_rev_rtest4.C
	tests/vector1/rose_rev_rtest4.cpp.C
	tests/vector1/rose_rev_rtest4.cpppure.C
	tests/vector1/rose_rev_rtest4.cpppure2.C
	tests/vector1/rtest1.C
	tests/vector1/rtest2.C
#end of faketarget

endif
