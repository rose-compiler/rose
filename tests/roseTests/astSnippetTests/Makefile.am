include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =
MOSTLYCLEANFILES =

AM_CPPFLAGS = $(ROSE_INCLUDES)


###############################################################################################################################
# Executables
###############################################################################################################################

noinst_PROGRAMS += injectSnippet
injectSnippet_SOURCES = injectSnippet.C snippetTests.C
injectSnippet_LDADD = $(ROSE_LIBS)

noinst_PROGRAMS += storeLoad
storeLoad_SOURCES = storeLoad.C snippetTests.C
storeLoad_LDADD = $(ROSE_LIBS)

noinst_PROGRAMS += listSnippets
listSnippets_SOURCES = listSnippets.C
listSnippets_LDADD = $(ROSE_LIBS)


###############################################################################################################################
# Small C Tests
###############################################################################################################################
C_INJECTION_TEST = $(srcdir)/injectSnippet.conf
STORELOAD_TEST = $(srcdir)/storeLoad.conf
EXTRA_DIST += $(C_INJECTION_TEST) $(STORELOAD_TEST)

#----------------------------------------------------------------------------------------------------
# test1*.passed are for injection snippets1.c into specimen1.c in various ways.
TEST_TARGETS += test1a.passed test1b.passed test1c.passed test1d.passed test1e.passed test1f.passed test1g.passed
EXTRA_DIST += SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c
test1a.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::basic		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="int c = 1"		\
		$(C_INJECTION_TEST) $@

test1b.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::decrement		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="--x"			\
		$(C_INJECTION_TEST) $@

test1c.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::swap		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="x = y"			\
		$(C_INJECTION_TEST) $@

test1d.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::basic		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="int c = 1"		\
		$(C_INJECTION_TEST) $@

test1e.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::basic		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=beginning		\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="int c = 1"		\
		$(C_INJECTION_TEST) $@

test1f.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::basic		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint4			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=beginning		\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="int c = 1"		\
		$(C_INJECTION_TEST) $@

test1g.passed: injectSnippet SmallSpecimensC/snippets1.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=snippets1.c,::basic		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint4			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="int c = 1"		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test2*.passed are for injection snippets2.c into specimen1.c in various ways.
TEST_TARGETS += test2a.passed test2b.passed
EXTRA_DIST += snippets2.c SmallSpecimensC/specimen1.c
test2a.passed: injectSnippet SmallSpecimensC/snippets2.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=snippets2.c,::count		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="++snippet_usage_counter"	\
		$(C_INJECTION_TEST) $@

test2b.passed: injectSnippet SmallSpecimensC/snippets2.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=snippets2.c,::count		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="++snippet_usage_counter"	\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test3*.passed are for injection snippets3.c into specimen1.c in various ways.
TEST_TARGETS += test3a.passed test3b.passed
EXTRA_DIST += SmallSpecimensC/snippets3.c SmallSpecimensC/specimen1.c
test3a.passed: injectSnippet SmallSpecimensC/snippets3.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="include files [$@]"		\
		SNIPPET=snippets3.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=yes			\
		CHECK_STRING="stupid example"		\
		$(C_INJECTION_TEST) $@

# Do all the work, but don't actually insert anything (insertion point is "none")
test3b.passed: injectSnippet SmallSpecimensC/snippets3.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="no insertion [$@]"		\
		SNIPPET=snippets3.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=none				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=yes			\
		CHECK_STRING="INSERT_HERE"		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test4*.passed are for injection snippets5.c into specimen1.c in various ways.
TEST_TARGETS += test4a.passed test4b.passed test4c.passed test4d.passed
EXTRA_DIST += SmallSpecimensC/snippets5.c SmallSpecimensC/specimen1.c
test4a.passed: injectSnippet SmallSpecimensC/snippets5.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="s1 = s2"			\
		$(C_INJECTION_TEST) $@

test4b.passed: injectSnippet SmallSpecimensC/snippets5.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="f1 = f2"			\
		$(C_INJECTION_TEST) $@

test4c.passed: injectSnippet SmallSpecimensC/snippets5.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="s1 = s2"			\
		$(C_INJECTION_TEST) $@

test4d.passed: injectSnippet SmallSpecimensC/snippets5.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=beginning		\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="s1 = s2"			\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test5*.passed are for injection snippets6.c into specimen1.c in various ways.
TEST_TARGETS += test5a.passed test5b.passed test5c.passed
EXTRA_DIST += SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c
test5a.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="= (random() %"		\
		$(C_INJECTION_TEST) $@

test5b.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="= (random() %"		\
		$(C_INJECTION_TEST) $@

test5c.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		CHECK_STRING="randomOffByOne(x)"	\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test6*.passed are for injection snippets6.c into specimen1.c in various ways.
TEST_TARGETS += test6a.passed test6b.passed
EXTRA_DIST += SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c
test6a.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		CHECK_STRING="= ((random() %"		\
		$(C_INJECTION_TEST) $@

test6b.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		CHECK_STRING="= ((random() %"		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test7*.passed are for injectioning snippets4 into specimen2 using the storeLoad driver. These tests inject
# two related snippets into the specimen in various ways.
TEST_TARGETS += test7a.passed test7b.passed 
EXTRA_DIST += SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c
test7a.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load heap string [$@]"	\
		METHOD=HeapString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		$(STORELOAD_TEST) $@

test7b.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load struct string [$@]"	\
		METHOD=StructString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		$(STORELOAD_TEST) $@

#----------------------------------------------------------------------------------------------------
# test8*.passed are for injecting snippets7 into specimen3
TEST_TARGETS += test8a.passed
EXTRA_DIST += SmallSpecimensC/snippets7.c SmallSpecimensC/specimen3.c

test8a.passed: injectSnippet SmallSpecimensC/snippets7.c SmallSpecimensC/specimen3.c $(C_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="missing preprocessor information [$@]"	\
		SNIPPET=snippets7.c,::weakness			\
		SPECIMEN=SmallSpecimensC/specimen3.c		\
		IPOINT=::main					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=beginning			\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		CHECK_STRING="output_value(argc)"		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test9*.passed are for injecting snippets8 into specimen8
TEST_TARGETS += test9a.passed test9b.passed
EXTRA_DIST += SmallSpecimensC/snippets8.c SmallSpecimensC/specimen8.c

test9a.passed: injectSnippet SmallSpecimensC/snippets8.c SmallSpecimensC/specimen8.c $(C_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="references to ctype macros [$@]"		\
		SNIPPET=snippets8.c,::weakness			\
		SPECIMEN=SmallSpecimensC/specimen8.c		\
		IPOINT=::main					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=beginning			\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		CHECK_STRING="__ctype_b_loc"			\
		$(C_INJECTION_TEST) $@

# without copying related things
test9b.passed: injectSnippet SmallSpecimensC/snippets8.c SmallSpecimensC/specimen8.c $(C_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="references to ctype macros [$@]"		\
		SNIPPET=snippets8.c,::weakness			\
		SPECIMEN=SmallSpecimensC/specimen8.c		\
		IPOINT=::main					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=beginning			\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="__ctype_b_loc"			\
		$(C_INJECTION_TEST) $@


###############################################################################################################################
# Small Java Tests
###############################################################################################################################
JAVA_INJECTION_TEST = $(srcdir)/injectSnippetJava.conf
EXTRA_DIST += $(JAVA_INJECTION_TEST)

#----------------------------------------------------------------------------------------------------
# testJava1.passed are for injection Snippets1.java into Specimen1.java in various ways.
TEST_TARGETS += testJava1a.passed testJava1b.passed testJava1c.passed testJava1d.passed
EXTRA_DIST += SmallSpecimensJava/Snippets1.java SmallSpecimensJava/Specimen1.java
testJava1a.passed: injectSnippet SmallSpecimensJava/Snippets1.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=Snippets1.java,Snippets1.basic		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava1b.passed: injectSnippet SmallSpecimensJava/Snippets1.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=Snippets1.java,Snippets1.decrement	\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=body				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava1c.passed: injectSnippet SmallSpecimensJava/Snippets1.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=Snippets1.java,Snippets1.swap		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava1d.passed: injectSnippet SmallSpecimensJava/Snippets1.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=Snippets1.java,Snippets1.basic		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=body				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava2.passed are for injection Snippets2.java into Specimen1.java in various ways.
TEST_TARGETS += testJava2a.passed testJava2b.passed
EXTRA_DIST += SmallSpecimensJava/Snippets2.java SmallSpecimensJava/Specimen1.java
testJava2a.passed: injectSnippet SmallSpecimensJava/Snippets2.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="global entities [$@]"			\
		SNIPPET=Snippets2.java,Snippets2.count		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava2b.passed: injectSnippet SmallSpecimensJava/Snippets2.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="global entities [$@]"			\
		SNIPPET=Snippets2.java,Snippets2.count		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=body				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava3*.passed are for injection Snippets3.java into Specimen1.java in various ways.
TEST_TARGETS += testJava3a.passed
EXTRA_DIST += SmallSpecimensJava/Snippets3.java SmallSpecimensJava/Specimen1.java
testJava3a.passed: injectSnippet SmallSpecimensJava/Snippets3.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="imports [$@]"				\
		SNIPPET=Snippets3.java,Snippets3.bigint		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava4*.passed are for injection Snippets5.java into Specimen1.java in various ways.
TEST_TARGETS += testJava4a.passed testJava4b.passed testJava4c.passed
EXTRA_DIST += SmallSpecimensJava/Snippets5.java SmallSpecimensJava/Specimen1.java
testJava4a.passed: injectSnippet SmallSpecimensJava/Snippets5.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=SmallSpecimensJava/Specimen1.java			\
		IPOINT=Speciment1.ipoint2					\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		$(JAVA_INJECTION_TEST) $@

testJava4b.passed: injectSnippet SmallSpecimensJava/Snippets5.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=SmallSpecimensJava/Specimen1.java			\
		IPOINT=Speciment1.ipoint3					\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		$(JAVA_INJECTION_TEST) $@

testJava4c.passed: injectSnippet SmallSpecimensJava/Snippets5.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=SmallSpecimensJava/Specimen1.java			\
		IPOINT=Speciment1.ipoint2					\
		INSERT_MECHANISM=body						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava5*.passed are for injection Snippets6.java into Specimen1.java in various ways.
TEST_TARGETS += testJava5a.passed testJava5c.passed
EXTRA_DIST += SmallSpecimensJava/Snippets6.java SmallSpecimensJava/Specimen1.java
testJava5a.passed: injectSnippet SmallSpecimensJava/Snippets6.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="recursive invocation [$@]"		\
		SNIPPET=Snippets6.java,Snippets6.addWithError	\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava5c.passed: injectSnippet SmallSpecimensJava/Snippets6.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="recursive invocation [$@]"		\
		SNIPPET=Snippets6.java,Snippets6.addWithError	\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava6*.passed are for injection Snippets7.java into Specimen2.java in various ways.
TEST_TARGETS += testJava6a.passed
EXTRA_DIST += SmallSpecimensJava/Snippets7.java SmallSpecimensJava/Specimen2.java
testJava6a.passed: injectSnippet SmallSpecimensJava/Snippets7.java SmallSpecimensJava/Specimen2.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="Sasha's java test [$@]"			\
		SNIPPET=Snippets7.java,Snippets7.function0	\
		SPECIMEN=SmallSpecimensJava/Specimen2.java	\
		IPOINT=Specimen2.woah				\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava7*.passed are for injection Exotic*.java into Specimen1.java in various ways.
TEST_TARGETS += testJava7a.passed testJava7b.passed
EXTRA_DIST += SmallSpecimensJava/Exotic1.java SmallSpecimensJava/Exotic2.java SmallSpecimensJava/Specimen1.java

testJava7a.passed: injectSnippet SmallSpecimensJava/Exotic1.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="Exotitc types [$@]"			\
		DISABLED="Requires Java SE 7"			\
		SNIPPET=Exotic1.java,Exotic1.snippet		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		$(JAVA_INJECTION_TEST) $@

testJava7b.passed: injectSnippet SmallSpecimensJava/Exotic2.java SmallSpecimensJava/Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="Exotitc types [$@]"			\
		SNIPPET=Exotic2.java,Exotic2.snippet		\
		SPECIMEN=SmallSpecimensJava/Specimen1.java	\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava8*.passed are for injections with packages
TEST_TARGETS += testJava8a-parse.passed testJava8a-snippet.passed testJava8a.passed
EXTRA_DIST += SmallSpecimensJava/SnippetPackage/Snippet3.java SmallSpecimensJava/Specimen3/Specimen3.java

# frontend() only, for testing that we can compile the java snippet files as Java source
noinst_PROGRAMS += testParser
testParser_SOURCES = testParser.C
testParser_LDADD = $(ROSE_LIBS)

# We're having problems parsing SnippetPackage.Snippet3 as a snippet, so make sure we can at least parse it normally. The
# failure is "/tmp/dir-13971683045770/tmp-1319461204681189217.java:3: error: class Snippet3 is public, should be declared
# in a file named Snippet3.java", but the code *is* in Snippet3.java. [RPM 2014-04-11]

# This appears to be okay
testJava8a-parse.passed: SmallSpecimensJava/SnippetPackage/Snippet3.java testParser
	@$(RTH_RUN)						\
		TITLE="parse a java file with package [$@]"	\
		CMD="$(builddir)/testParser $(abspath $<)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# This fails, so it's a problem with how the Snippet class parses the snippet file. [RPM 2014-04-11]
testJava8a-snippet.passed: SmallSpecimensJava/SnippetPackage/Snippet3.java listSnippets testJava8a-parse.passed
	@$(RTH_RUN)							\
		TITLE="list snippets [$@]"				\
		DISABLED="fails sometimes in Jenkins (RPM 2014-04-11)"	\
		CMD="$(builddir)/listSnippets $(abspath $<)"		\
		$(top_srcdir)/scripts/test_exit_status $@

testJava8a.passed: testJava8a-snippet.passed \
injectSnippet SmallSpecimensJava/SnippetPackage/Snippet3.java SmallSpecimensJava/Specimen3/Specimen3.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)									\
		TITLE="packages [$@]"							\
		DISABLED="fails sometimes in Jenkins (RPM 2014-04-11)"			\
		SNIPPET=SnippetPackage/Snippet3.java,SnippetPackage.Snippet3.snippet	\
		SPECIMEN=SmallSpecimensJava/Specimen3/Specimen3.java			\
		IPOINT=Specimen3.Specimen3.ipoint1					\
		INSERT_MECHANISM=stmts							\
		RECURSIVE_INSERTION=no							\
		COPY_DEFINITIONS=yes							\
		$(JAVA_INJECTION_TEST) $@

###############################################################################################################################
# quicksort tests
###############################################################################################################################

noinst_PROGRAMS += quickSortInjector
quickSortInjector_SOURCES = quickSortInjector.C snippetTests.C
quickSortInjector_LDADD = $(ROSE_LIBS)

TEST_TARGETS += quickSortC.passed quickSortJava.passed reverseC.passed reverseJava.passed

quickSortC.passed: quickSortInjector QuickSortC/quicksort.c QuickSortC/errorSnippets.c
	@$(RTH_RUN)										\
		TITLE="quickSort C [$@]"							\
		CMD="$(abs_builddir)/quickSortInjector $(abs_srcdir)/QuickSortC/quicksort.c"	\
		USE_SUBDIR=yes									\
		$(top_srcdir)/scripts/test_exit_status $@

quickSortJava.passed: quickSortInjector QuickSortJava/QuickSort.java QuickSortJava/ErrorSnippets.java
	@$(RTH_RUN)											\
		TITLE="quickSort Java [$@]"								\
		CMD="$(abs_builddir)/quickSortInjector $(abs_srcdir)/QuickSortJava/QuickSort.java"	\
		USE_SUBDIR=yes										\
		$(top_srcdir)/scripts/test_exit_status $@

# These use quicksort, but they reverse the injection -- they treat quicksort as the snippet and inject it into
# the "reverse" specimen.  The only purpose is to test that we can inject larger, more complex snippets.

reverseJava.passed: injectSnippet QuickSortJava/QuickSort.java QuickSortJava/Reverse.java
	@$(RTH_RUN)								\
		TITLE="reversed quicksort injection [$@]"			\
		SNIPPET=QuickSortJava/QuickSort.java,QuickSort.nullSnippet	\
		SPECIMEN=QuickSortJava/Reverse.java				\
		IPOINT=Reverse.ipoint1						\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=no						\
		COPY_DEFINITIONS=yes						\
		$(JAVA_INJECTION_TEST) $@

reverseC.passed: injectSnippet QuickSortC/quicksort.c QuickSortC/reverse.c
	@$(RTH_RUN)								\
		TITLE="reversed quicksort injection [$@]"			\
		SNIPPET=QuickSortC/quicksort.c,::nullSnippet			\
		SPECIMEN=QuickSortC/reverse.c					\
		IPOINT=::main							\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=no						\
		COPY_DEFINITIONS=yes						\
		CHECK_STRING="insertSort"					\
		$(C_INJECTION_TEST) $@

###############################################################################################################################
# SS_JAVA_CWE tests.  These tests are only run if the "SS_JAVA_CWES/src" directory is present under the CWD and it appears to
# contain Java CWE implementations.
###############################################################################################################################

# Top level directory containing the Java CWE packages
SS_JAVA_CWE_ROOT = $(abspath SS_JAVA_CWES)
SS_JAVA_CWE_SRCDIR = $(SS_JAVA_CWE_ROOT)/src
SS_JAVA_CWE_LIBDIR = $(SS_JAVA_CWE_ROOT)/lib

# Those subdirectories of $(SS_JAVA_CWD_DIR) that contain CWE implementations
SS_JAVA_CWE_CATEGORIES = Error_Handling Injection Number_Handling Tainted_Data

# All the CWE_*.java files under those package directories.
SS_JAVA_CWES := $(foreach category, $(SS_JAVA_CWE_CATEGORIES), \
	$(shell find $(SS_JAVA_CWE_SRCDIR)/$(category) -name 'CWE_*.java' -printf '$(category)/%P\n' 2>/dev/null))

# Make one test target for each CWE file.  The name of the test target will be the package+class name with dots instead
# of slashes.  For instance, if the SS_JAVA_CWE full name is $(SS_JAVA_CWE_SRCDIR)/Error_Handling/CWE_209/CWE_209_0.java then
# the target will be named "Error_Handling.CWE_209.CWE_209_0.passed"
SS_JAVA_TEST_TARGETS = $(addsuffix .passed, $(subst /,.,$(basename $(SS_JAVA_CWES))))

# Some of the CWEs depend on a few support classes that need to be built. Fortunately, the SS_JAVA_CWES.tar file provides
# a makefile.
MOSTLYCLEANFILES += ss_java_support_built
ss_java_support_built:
	@echo "  MAKE   building SS_JAVA_CWE support jars [$@]"
	$(MAKE) -C $(SS_JAVA_CWE_ROOT)
	touch $@

# Class paths (use "-classpath PATH1:PATH2:..." not "-classpath PATH1 -classpath PATH2 ...")
SS_JAVA_CLASSPATH = -classpath $$(perl -e 'print join ":", @ARGV' $(wildcard $(SS_JAVA_CWE_LIBDIR)/*.jar))

# Some help if no SS_JAVA_CWES are present
.PHONY: check-ss-java-cwes-help
check-ss-java-cwes-help:
	@if [ "$(SS_JAVA_TEST_TARGETS)" = "" ]; then										\
		(														\
			echo;													\
		 	echo -n "Running \"make check-ss-java-cwes\" does't do anything because you have no Java CWEs ";	\
			echo -n "installed.  You must untar the SS_JAVA_CWES.tar.gz file into $(abs_builddir) and then ";	\
			echo "rerun \"make check-ss-java-cwes\".";								\
			echo;													\
		) |fold --spaces --width=120 |sed 's/^/**** /' >&2;								\
	fi

# Run each test
EXTRA_DIST += ss_java_cwes.conf
.PHONY: check-ss-java-cwes
check-ss-java-cwes: check-ss-java-cwes-help $(SS_JAVA_TEST_TARGETS)
$(SS_JAVA_TEST_TARGETS): %.passed: injectSnippet ss_java_cwes.conf ss_java_support_built
	@$(RTH_RUN)										\
		TITLE="Java CWE injection [$@]"							\
		USE_SUBDIR=yes									\
		SNIPPET_FILE="$(SS_JAVA_CWE_SRCDIR)/$(subst .,/,$*).java"			\
		SNIPPET_CLASS_NAME="$*"								\
		SNIPPET_FUNCTION_NAME="$$(echo $* |perl -pe 'chomp; $$_=lc((split /\./)[-1])')"	\
		SS_JAVA_CLASSPATH="$(SS_JAVA_CLASSPATH)"					\
		$(srcdir)/ss_java_cwes.conf $@


# DQ (3/19/2014): We are not ready to test these in the "make check" rule (not distributed with ROSE, so can't be run by Jenkins).
# RPM (3/20/2014): To reenable these tests, enable the first line (TEST_TARGETS) and delete the second (MOSTLYCLEANFILES).
# TEST_TARGETS += $(SS_JAVA_TEST_TARGETS)
MOSTLYCLEANFILES += $(SS_JAVA_TEST_TARGETS) $(SS_JAVA_TEST_TARGETS:.passed=.failed)

###############################################################################################################################
# Extra, optional snippet tests.  Any .c or .java file that's found in $(srcdir)/ExtraSnippets will treated as a snippet file,
# and all void functions or methods that take no arguments will be treated as snippets.  The snippets will each be inserted
# in turn into its own target specimen.
###############################################################################################################################

INJECT_ALL_TEST = $(srcdir)/injectAllSnippets.conf
EXTRA_DIST += $(INJECT_ALL_TEST)

EXTRAS_SNIPPET_DIR = $(srcdir)/ExtraSnippets
EXTRA_DIST += $(EXTRAS_SNIPPET_DIR)/0_README $(INJECT_ALL_TEST)

EXTRAS_SNIPPET_FILES_C = $(notdir $(wildcard $(EXTRAS_SNIPPET_DIR)/*.c))
EXTRAS_TARGETS_C = $(addprefix extras_, $(addsuffix .passed, $(EXTRAS_SNIPPET_FILES_C)))
EXTRAS_SPECIMEN_C = $(srcdir)/SmallSpecimensC/specimen1.c
TEST_TARGETS += $(EXTRAS_TARGETS_C)

$(EXTRAS_TARGETS_C): extras_%.passed: injectSnippet listSnippets $(EXTRAS_SNIPPET_DIR)/% $(EXTRAS_SPECIMEN_C) $(INJECT_ALL_TEST)
	@$(RTH_RUN)						\
		TITLE="extra snippet $* [$@]"			\
		SNIPPET_FILE="$(EXTRAS_SNIPPET_DIR)/$*"		\
		SPECIMEN="$(EXTRAS_SPECIMEN_C)"			\
		IPOINT="::ipoint1"				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		COMPILER_FLAGS="-I$(EXTRAS_SNIPPET_DIR)"	\
		$(INJECT_ALL_TEST) $@

# funky name because *_JAVA is apparently reserved by automake
EXTRAS_SNIPPET_FILES_J = $(notdir $(wildcard $(EXTRAS_SNIPPET_DIR)/*.java))
EXTRAS_TARGETS_J = $(addprefix extras_, $(addsuffix .passed, $(EXTRAS_SNIPPET_FILES_J)))
EXTRAS_SPECIMEN_J = $(srcdir)/SmallSpecimensJava/Specimen1.java
TEST_TARGETS += $(EXTRAS_TARGETS_J)

$(EXTRAS_TARGETS_J): \
extras_%.passed: injectSnippet listSnippets $(EXTRAS_SNIPPET_DIR)/% $(EXTRAS_SPECIMEN_J) $(INJECT_ALL_TEST)
	@$(RTH_RUN)						\
		TITLE="extra snippet $* [$@]"			\
		SNIPPET_FILE="$(EXTRAS_SNIPPET_DIR)/$*"		\
		SPECIMEN="$(EXTRAS_SPECIMEN_J)"			\
		IPOINT="Specimen1.ipoint1"			\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		$(INJECT_ALL_TEST) $@


.phony: check-extras
check-extra-snippets: $(EXTRAS_TARGETS_C) $(EXTRAS_TARGETS_J)


###############################################################################################################################
# DQ (3/2/2014): Extra tests I have to build (these are small enough to be visualized for more detailed debugging):
###############################################################################################################################
# ./injectSnippet --test:snippet=snippet2014_01.foo --test:ipoint-function=foobar --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=no --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_01.c
# ./injectSnippet --test:snippet=snippet2014_03.foo --test:ipoint-function=foobar --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=no --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_03.c
# ./injectSnippet --test:snippet=snippet2014_04.swap --test:ipoint-function=ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_04.c
# ./injectSnippet --test:snippet=snippet2014_05.count --test:ipoint-function=ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_05.c
# ./injectSnippet --test:snippet=snippet2014_06.print --test:ipoint-function=ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_06.c
# ./injectSnippet --test:snippet=snippet2014_07.print --test:ipoint-function=ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_07.c
# ./injectSnippet --test:snippet=snippet2014_08.print --test:ipoint-function=ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_08.c
# ./injectSnippet --test:snippet=snippet2014_10.c,::storeStructString --test:copy-related-things=no --test:ipoint-function=::ipoint1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=yes --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/specimen2014_10.c

# To run Java:
# ./injectSnippet --test:snippet=$(src_dir)/SnippetGroup1.java,SnippetGroup1.snippet1 --test:ipoint-function=HelloWorld.target1 --test:insert-mechanism=stmts --test:locdecls-position=end --test:recursive=no --test:copy-definitions=no -rose:skipfinalCompileStep $(src_dir)/HelloWorld.java $(src_dir)/SnippetGroup1.java

#FIXME: These targets all need dependencies, otherwise "make clean; make local_debug_tests" will fail. [RPM 2014-03-18]

TEST_TARGETS += test_1.passed
test_1.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_01.c,::foo			\
		SPECIMEN=SmallSpecimensC/specimen2014_01.c	\
		IPOINT=::foobar					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="y = x"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_2.passed
test_2.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_02.c,::foo			\
		SPECIMEN=SmallSpecimensC/specimen2014_02.c	\
		IPOINT=::foobar					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="foo2()"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_3.passed
test_3.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_03.c,::foo			\
		SPECIMEN=SmallSpecimensC/specimen2014_03.c	\
		IPOINT=::foobar					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="y = x"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_4.passed
test_4.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_04.c,::swap			\
		SPECIMEN=SmallSpecimensC/specimen2014_04.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="x = y"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_5.passed
test_5.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_05.c,::count		\
		SPECIMEN=SmallSpecimensC/specimen2014_05.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="++snippet_usage_counter"		\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_6.passed
test_6.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_06.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen2014_06.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="fputs(x,stderr);"			\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_7.passed
test_7.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_07.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen2014_07.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="fputs(x,"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_8.passed
test_8.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_08.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen2014_08.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="struct __FILE ;"			\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_9.passed
test_9.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_09.c,::allocate_string	\
		SPECIMEN=SmallSpecimensC/specimen2014_09.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="heap_storage"			\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_10.passed
test_10.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_10.c,::storeStructString	\
		SPECIMEN=SmallSpecimensC/specimen2014_10.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="storage . const_string_member"	\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_11.passed
test_11.passed:
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_11.c,::foo			\
		SPECIMEN=SmallSpecimensC/specimen2014_11.c	\
		IPOINT=::foobar					\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="y = x"				\
		$(C_INJECTION_TEST) $@

TEST_TARGETS += test_12.passed
test_12.passed: injectSnippet
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=snippet2014_12.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen2014_12.c	\
		IPOINT=::ipoint1				\
		INSERT_MECHANISM=stmts				\
		LOCDECLS_POSITION=end				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		CHECK_STRING="int value = "			\
		$(C_INJECTION_TEST) $@

test_java_1.passed:
	./injectSnippet \
		--test:snippet=$(srcdir)/SnippetGroup1.java,SnippetGroup1.snippet1 \
		--test:ipoint-function=HelloWorld.target1 --test:insert-mechanism=stmts --test:locdecls-position=end \
		--test:recursive=no --test:copy-definitions=no -rose:skipfinalCompileStep $(srcdir)/HelloWorld.java


# TEST_TARGETS += testJava2014_01.passed 
# EXTRA_DIST += SmallSpecimensJava/Snippet2014_01.java SmallSpecimensJava/Specimen2014_01.java
testJava2014_01.passed: injectSnippet SmallSpecimensJava/Snippet2014_01.java SmallSpecimensJava/Specimen2014_01.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)							\
		TITLE="basic insertion [$@]"				\
		SNIPPET=Snippet2014_01.java,Snippet2014_01.basic	\
		SPECIMEN=SmallSpecimensJava/Specimen2014_01.java	\
		IPOINT=Specimen2014_01.ipoint1				\
		INSERT_MECHANISM=stmts					\
		RECURSIVE_INSERTION=no					\
		COPY_DEFINITIONS=no					\
		$(JAVA_INJECTION_TEST) $@



# DQ: Tests that I can use to vizualize the whole AST (for debugging).
local_debug_tests: $(addsuffix .passed, test_1 test_2 test_3 test_4 test_5 test_6 test_7 test_8 test_9 test_10 test_11 test_12)

###############################################################################################################################
# Automake boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
	@echo "**************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astSnippetTests: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************"

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
