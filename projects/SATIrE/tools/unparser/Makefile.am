EXTRA_DIST = unparse.pl \
	tests/array.c \
	tests/define.c \
	tests/for.c \
	tests/return.c \
	tests/termtests_gb.c \
	tests/typedef.h
	tests/char.c \
	tests/empty.h \
	tests/include.c \
	tests/simple.c \
	tests/test-literals.c \
	tests/union.c \
	tests/cnt.c \
	tests/enum.c \
	tests/long.c \
	tests/string.c \
	tests/test-stmt.c \
	tests/while.c \
	tests/comma.c \
	tests/float.c \
	tests/matmult.c \
	tests/struct.c \
	tests/test-stmt2.c \
	tests/comma.h \
	tests/foo.h \
	tests/pp.c \
	tests/struct1.c \
	tests/test-typdef.c 


if WITH_SWI_PROLOG

bin_PROGRAMS=unparse
unparse_SOURCES=

unparse: unparse.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o unparse -c $(srcdir)/unparse.pl

endif
