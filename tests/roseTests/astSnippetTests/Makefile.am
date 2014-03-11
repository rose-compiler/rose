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
# Tests
###############################################################################################################################
C_INJECTION_TEST = $(srcdir)/injectSnippet.conf
JAVA_INJECTION_TEST = $(srcdir)/injectSnippetJava.conf
STORELOAD_TEST = $(srcdir)/storeLoad.conf
EXTRA_DIST += $(C_INJECTION_TEST) $(JAVA_INJECTION_TEST) $(STORELOAD_TEST)

#----------------------------------------------------------------------------------------------------
# test1*.passed are for injection snippets1.c into specimen1.c in various ways.
TEST_TARGETS += test1a.passed test1b.passed test1c.passed test1d.passed test1e.passed
test1a.passed: injectSnippet snippets1.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.c,::basic	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test1b.passed: injectSnippet snippets1.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.c,::decrement	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test1c.passed: injectSnippet snippets1.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.c,::swap	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test1d.passed: injectSnippet snippets1.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.c,::basic	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test1e.passed: injectSnippet snippets1.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.c,::basic	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=beginning	\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test2*.passed are for injection snippets2.c into specimen1.c in various ways.
TEST_TARGETS += test2a.passed test2b.passed test2c.passed test2d.passed
test2a.passed: injectSnippet snippets2.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.c,::count	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test2b.passed: injectSnippet snippets2.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.c,::count	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@

test2c.passed: injectSnippet snippets2.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.c,::count	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=no		\
		$(C_INJECTION_TEST) $@

test2d.passed: injectSnippet snippets2.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.c,::count	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint1		\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=no		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test3*.passed are for injection snippets3.c into specimen1.c in various ways.
TEST_TARGETS += test3a.passed test3b.passed
test3a.passed: injectSnippet snippets3.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="include files [$@]"	\
		SNIPPET=snippets3.c,::print	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint2		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=yes		\
		$(C_INJECTION_TEST) $@
test3b.passed: injectSnippet snippets3.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="include files [$@]"	\
		SNIPPET=snippets3.c,::print	\
		SPECIMEN=specimen1.c		\
		IPOINT=::ipoint2		\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		COPY_RELATED_THINGS=no		\
		$(C_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# test4*.passed are for injection snippets5.c into specimen1.c in various ways.
TEST_TARGETS += test4a.passed test4b.passed test4c.passed test4d.passed
test4a.passed: injectSnippet snippets5.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test4b.passed: injectSnippet snippets5.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test4c.passed: injectSnippet snippets5.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test4d.passed: injectSnippet snippets5.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.c,::swap		\
		SPECIMEN=specimen1.c			\
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
test5a.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test5b.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test5c.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test5d.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint1			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test5e.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint3			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test5f.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.c,::addWithError	\
		SPECIMEN=specimen1.c			\
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
test6a.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test6b.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		COPY_RELATED_THINGS=yes			\
		$(C_INJECTION_TEST) $@

test6c.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=specimen1.c			\
		IPOINT=::ipoint2			\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=no			\
		$(C_INJECTION_TEST) $@

test6d.passed: injectSnippet snippets6.c specimen1.c $(C_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.c,::allocate_string	\
		SPECIMEN=specimen1.c			\
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
test7a.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load heap string [$@]" \
		METHOD=HeapString \
		SPECIMEN=specimen2.c \
		COPY_RELATED_THINGS=yes \
		$(STORELOAD_TEST) $@

test7b.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load struct string [$@]" \
		METHOD=StructString \
		SPECIMEN=specimen2.c \
		COPY_RELATED_THINGS=yes \
		$(STORELOAD_TEST) $@

test7c.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load heap string [$@]" \
		METHOD=HeapString \
		SPECIMEN=specimen2.c \
		COPY_RELATED_THINGS=no \
		$(STORELOAD_TEST) $@

test7d.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load struct string [$@]" \
		METHOD=StructString \
		SPECIMEN=specimen2.c \
		COPY_RELATED_THINGS=no \
		$(STORELOAD_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava1.passed are for injection Snippets1.java into Specimen1.java in various ways.
TEST_TARGETS += testJava1a.passed testJava1b.passed testJava1c.passed testJava1d.passed
testJava1a.passed: injectSnippet Snippets1.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=Snippets1.java,Snippets1.basic	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=stmts			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

testJava1b.passed: injectSnippet Snippets1.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="basic insertion [$@]"			\
		SNIPPET=Snippets1.java,Snippets1.decrement	\
		SPECIMEN=Specimen1.java				\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=body				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		COPY_RELATED_THINGS=yes				\
		$(JAVA_INJECTION_TEST) $@

testJava1c.passed: injectSnippet Snippets1.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=Snippets1.java,Snippets1.swap	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=stmts			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

testJava1d.passed: injectSnippet Snippets1.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="basic insertion [$@]"		\
		SNIPPET=Snippets1.java,Snippets1.basic	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=body			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava2.passed are for injection Snippets2.java into Specimen1.java in various ways.
TEST_TARGETS += testJava2a.passed testJava2b.passed testJava2c.passed testJava2d.passed
testJava2a.passed: injectSnippet Snippets2.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=Snippets2.java,Snippets2.count	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=stmts			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

testJava2b.passed: injectSnippet Snippets2.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="global entities [$@]"		\
		SNIPPET=Snippets2.java,Snippets2.count	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=body			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

testJava2c.passed: injectSnippet Snippets2.java Specimen1b.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)									\
		TITLE="global entities [$@]"						\
		SNIPPET=Snippets2.java,Snippets2.count					\
		SPECIMEN=Specimen1b.java						\
		IPOINT=Specimen1b.ipoint1						\
		INSERT_MECHANISM=stmts							\
		RECURSIVE_INSERTION=yes							\
		COPY_DEFINITIONS=no							\
		COPY_RELATED_THINGS=no							\
		$(JAVA_INJECTION_TEST) $@

testJava2d.passed: injectSnippet Snippets2.java Specimen1b.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)									\
		TITLE="global entities [$@]"						\
		SNIPPET=Snippets2.java,Snippets2.count					\
		SPECIMEN=Specimen1b.java						\
		IPOINT=Specimen1b.ipoint1						\
		INSERT_MECHANISM=body							\
		RECURSIVE_INSERTION=yes							\
		COPY_DEFINITIONS=no							\
		COPY_RELATED_THINGS=no							\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava3*.passed are for injection Snippets3.java into Specimen1.java in various ways.
TEST_TARGETS += testJava3a.passed
testJava3a.passed: injectSnippet Snippets3.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="imports [$@]"			\
		SNIPPET=Snippets3.java,Snippets3.bigint	\
		SPECIMEN=Specimen1.java			\
		IPOINT=Specimen1.ipoint1		\
		INSERT_MECHANISM=stmts			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		COPY_RELATED_THINGS=yes			\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava4*.passed are for injection Snippets5.java into Specimen1.java in various ways.
TEST_TARGETS += testJava4a.passed testJava4b.passed testJava4c.passed
testJava4a.passed: injectSnippet Snippets5.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=Specimen1.java						\
		IPOINT=Speciment1.ipoint2					\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		COPY_RELATED_THINGS=yes						\
		$(JAVA_INJECTION_TEST) $@

testJava4b.passed: injectSnippet Snippets5.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=Specimen1.java						\
		IPOINT=Speciment1.ipoint3					\
		INSERT_MECHANISM=stmts						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		COPY_RELATED_THINGS=yes						\
		$(JAVA_INJECTION_TEST) $@

testJava4c.passed: injectSnippet Snippets5.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)								\
		TITLE="parameterized types [$@]"				\
		DISABLED="Java snippets do not support parameterized types"	\
		SNIPPET=Snippets5.java,Snippets5.swap				\
		SPECIMEN=Specimen1.java						\
		IPOINT=Speciment1.ipoint2					\
		INSERT_MECHANISM=body						\
		RECURSIVE_INSERTION=yes						\
		COPY_DEFINITIONS=no						\
		COPY_RELATED_THINGS=yes						\
		$(JAVA_INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
# testJava5*.passed are for injection Snippets6.java into Specimen1.java in various ways.
TEST_TARGETS += testJava5a.passed testJava5c.passed
testJava5a.passed: injectSnippet Snippets6.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="recursive invocation [$@]"		\
		SNIPPET=Snippets6.java,Snippets6.addWithError	\
		SPECIMEN=Specimen1.java				\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=yes				\
		COPY_DEFINITIONS=no				\
		COPY_RELATED_THINGS=no				\
		$(JAVA_INJECTION_TEST) $@

testJava5c.passed: injectSnippet Snippets6.java Specimen1.java $(JAVA_INJECTION_TEST)
	@$(RTH_RUN)						\
		TITLE="recursive invocation [$@]"		\
		SNIPPET=Snippets6.java,Snippets6.addWithError	\
		SPECIMEN=Specimen1.java				\
		IPOINT=Specimen1.ipoint1			\
		INSERT_MECHANISM=stmts				\
		RECURSIVE_INSERTION=no				\
		COPY_DEFINITIONS=yes				\
		COPY_RELATED_THINGS=no				\
		$(JAVA_INJECTION_TEST) $@


# DQ (3/2/2014): Extra tests I have to build:
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