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
INJECTION_TEST = $(srcdir)/injectSnippet.conf
STORELOAD_TEST = $(srcdir)/storeLoad.conf
EXTRA_DIST += $(INJECTION_TEST) $(STORELOAD_TEST)

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test1a.passed test1b.passed test1c.passed test1d.passed test1e.passed
test1a.passed: injectSnippet snippets1.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.basic		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

test1b.passed: injectSnippet snippets1.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.decrement	\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

test1c.passed: injectSnippet snippets1.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.swap		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

test1d.passed: injectSnippet snippets1.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.basic		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

test1e.passed: injectSnippet snippets1.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="basic insertion [$@]"	\
		SNIPPET=snippets1.basic		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=beginning	\
		RECURSIVE_INSERTION=no		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test2a.passed test2b.passed
test2a.passed: injectSnippet snippets2.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.count		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

test2b.passed: injectSnippet snippets2.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="global entities [$@]"	\
		SNIPPET=snippets2.count		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=body		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test3a.passed
test3a.passed: injectSnippet snippets3.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)				\
		TITLE="include files [$@]"	\
		SNIPPET=snippets3.print		\
		SPECIMEN=specimen1.c		\
		IPOINT=ipoint1			\
		INSERT_MECHANISM=stmts		\
		LOCDECLS_POSITION=end		\
		RECURSIVE_INSERTION=yes		\
		COPY_DEFINITIONS=no		\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test4a.passed test4b.passed test4c.passed test4d.passed
test4a.passed: injectSnippet snippets5.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.swap			\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint2				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test4b.passed: injectSnippet snippets5.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.swap			\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint3				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test4c.passed: injectSnippet snippets5.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.swap			\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint2				\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test4d.passed: injectSnippet snippets5.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="parameterized types [$@]"	\
		SNIPPET=snippets5.swap			\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint2				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=beginning		\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test5a.passed test5b.passed test5c.passed
test5a.passed: injectSnippet snippets6.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.addWithError		\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint1				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test5b.passed: injectSnippet snippets6.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.addWithError		\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint3				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test5c.passed: injectSnippet snippets6.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="recursive invocation [$@]"	\
		SNIPPET=snippets6.addWithError		\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint1				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test6a.passed test6b.passed
test6a.passed: injectSnippet snippets6.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.allocate_string	\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint2				\
		INSERT_MECHANISM=stmts			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=yes			\
		COPY_DEFINITIONS=no			\
		$(INJECTION_TEST) $@

test6b.passed: injectSnippet snippets6.c specimen1.c $(INJECTION_TEST)
	@$(RTH_RUN)					\
		TITLE="variable renaming [$@]"		\
		SNIPPET=snippets6.allocate_string	\
		SPECIMEN=specimen1.c			\
		IPOINT=ipoint2				\
		INSERT_MECHANISM=body			\
		LOCDECLS_POSITION=end			\
		RECURSIVE_INSERTION=no			\
		COPY_DEFINITIONS=yes			\
		$(INJECTION_TEST) $@

#----------------------------------------------------------------------------------------------------
TEST_TARGETS += test7a.passed test7b.passed
test7a.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load heap string [$@]" \
		METHOD=HeapString \
		SPECIMEN=specimen2.c \
		$(STORELOAD_TEST) $@

test7b.passed: storeLoad snippets4.c specimen2.c $(STORELOAD_TEST)
	@$(RTH_RUN) \
		TITLE="store/load struct string [$@]" \
		METHOD=StructString \
		SPECIMEN=specimen2.c \
		$(STORELOAD_TEST) $@

###############################################################################################################################
# Automake boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)