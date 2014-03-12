include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =

INCLUDES = $(ROSE_INCLUDES)


###############################################################################################################################
# Executables
###############################################################################################################################

noinst_PROGRAMS += injectSnippet
injectSnippet_SOURCES = injectSnippet.C snippetTests.C
injectSnippet_LDADD = $(ROSE_LIBS)

noinst_PROGRAMS += storeLoad
storeLoad_SOURCES = storeLoad.C snippetTests.C
storeLoad_LDADD = $(ROSE_LIBS)


###############################################################################################################################
# Small C Tests
###############################################################################################################################
C_INJECTION_TEST = $(srcdir)/injectSnippet.conf
STORELOAD_TEST = $(srcdir)/storeLoad.conf
EXTRA_DIST += $(C_INJECTION_TEST) $(STORELOAD_TEST)

#----------------------------------------------------------------------------------------------------
# test1*.passed are for injection snippets1.c into specimen1.c in various ways.
TEST_TARGETS += test1a.passed test1b.passed test1c.passed test1d.passed test1e.passed
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test2*.passed are for injection snippets2.c into specimen1.c in various ways.
TEST_TARGETS += test2a.passed test2b.passed test2c.passed test2d.passed
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test2c.passed: injectSnippet SmallSpecimensC/snippets2.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=snippets2.c,::count		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test2d.passed: injectSnippet SmallSpecimensC/snippets2.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=snippets2.c,::count		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
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
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@
test3b.passed: injectSnippet SmallSpecimensC/snippets3.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="include files [$@]"		\
		SNIPPET=snippets3.c,::print		\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test5*.passed are for injection snippets6.c into specimen1.c in various ways.
TEST_TARGETS += test5a.passed test5b.passed test5c.passed test5d.passed test5e.passed test5f.passed
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test5d.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test5e.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test5f.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test6*.passed are for injection snippets6.c into specimen1.c in various ways.
TEST_TARGETS += test6a.passed test6b.passed test6c.passed test6d.passed
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
		COPY_RELATED_THINGS=yes			\
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
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test6c.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test6d.passed: injectSnippet SmallSpecimensC/snippets6.c SmallSpecimensC/specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=SmallSpecimensC/specimen1.c	\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test7*.passed are for injectioning snippets4 into specimen2 using the storeLoad driver. These tests inject
# two related snippets into the specimen in various ways.
TEST_TARGETS += test7a.passed test7b.passed test7c.passed test7d.passed
EXTRA_DIST += SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c
test7a.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load heap string [$@]"	\
		METHOD=HeapString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		COPY_RELATED_THINGS=yes			\
		$(STORELOAD_TEST) $@

test7b.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load struct string [$@]"	\
		METHOD=StructString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		COPY_RELATED_THINGS=yes			\
		$(STORELOAD_TEST) $@

test7c.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load heap string [$@]"	\
		METHOD=HeapString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		COPY_RELATED_THINGS=no			\
		$(STORELOAD_TEST) $@

test7d.passed: storeLoad SmallSpecimensC/snippets4.c SmallSpecimensC/specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN)					\
		TITLE="store/load struct string [$@]"	\
		METHOD=StructString			\
		SPECIMEN=SmallSpecimensC/specimen2.c	\
		COPY_RELATED_THINGS=no			\
		$(STORELOAD_TEST) $@

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
		COPY_RELATED_THINGS=yes				\
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
		COPY_RELATED_THINGS=yes				\
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
		COPY_RELATED_THINGS=yes				\
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
		COPY_RELATED_THINGS=yes				\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava2.passed are for injection Snippets2.java into Specimen1.java in various ways.
TEST_TARGETS += testJava2a.passed testJava2b.passed testJava2c.passed testJava2d.passed
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
		COPY_RELATED_THINGS=yes				\
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
		COPY_RELATED_THINGS=yes				\
		$(JAVA_INJECTION_TEST) $@

testJava2c.passed: injectSnippet SmallSpecimensJava/Snippets2.java SmallSpecimensJava/Specimen1b.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="global entities [$@]"			\
		SNIPPET=Snippets2.java,Snippets2.count		\
		SPECIMEN=SmallSpecimensJava/Specimen1b.java	\
		IPOINT=Specimen1b.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		COPY_RELATED_THINGS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava2d.passed: injectSnippet SmallSpecimensJava/Snippets2.java SmallSpecimensJava/Specimen1b.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="global entities [$@]"			\
		SNIPPET=Snippets2.java,Snippets2.count		\
		SPECIMEN=SmallSpecimensJava/Specimen1b.java	\
		IPOINT=Specimen1b.ipoint1			\
		INSERT_MECHANISM=body				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		COPY_RELATED_THINGS=no				\
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
		COPY_RELATED_THINGS=yes				\
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
		COPY_RELATED_THINGS=yes						\
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
		COPY_RELATED_THINGS=yes						\
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
		COPY_RELATED_THINGS=yes						\
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
		COPY_RELATED_THINGS=no				\
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
		COPY_RELATED_THINGS=no				\
		$(JAVA_INJECTION_TEST) $@

###############################################################################################################################
# quicksort tests
###############################################################################################################################

noinst_PROGRAMS += quickSortInjector
quickSortInjector_SOURCES = quickSortInjector.C snippetTests.C
quickSortInjector_LDADD = $(ROSE_LIBS)

TEST_TARGETS += quickSort.passed
quickSort.passed: quickSortInjector QuickSortC/quicksort.c QuickSortC/errorSnippets.c
	@$(RTH_RUN)										\
		TITLE="quickSort C [$@]"							\
		CMD="$(abs_builddir)/quickSortInjector $(abs_srcdir)/QuickSortC/quicksort.c"	\
		SUBDDIR=yes									\
		$(top_srcdir)/scripts/test_exit_status $@

###############################################################################################################################
# DQ (3/2/2014): Extra tests I have to build:
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

###############################################################################################################################
# Automake boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)