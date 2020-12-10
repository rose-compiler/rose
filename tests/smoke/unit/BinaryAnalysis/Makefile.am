include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

########################################################################################################################

noinst_PROGRAMS += registerDescriptor
registerDescriptor_SOURCES = registerDescriptor.C

TEST_TARGETS += registerDescriptor.passed
registerDescriptor.passed: $(top_srcdir)/scripts/test_exit_status registerDescriptor
	@$(RTH_RUN)					\
		TITLE="RegisterDescriptor class [$@]"	\
		CMD="./registerDescriptor"		\
		$< $@

########################################################################################################################

noinst_PROGRAMS += smtSolver
smtSolver_SOURCES = smtSolver.C

TEST_TARGETS += smtSolver.passed
smtSolver.passed: $(top_srcdir)/scripts/test_exit_status smtSolver
	@$(RTH_RUN)					\
		TITLE="SMT solver classes [$@]"	\
		CMD="./smtSolver"			\
		$< $@

########################################################################################################################

noinst_PROGRAMS += peekPoke
peekPoke_SOURCES = peekPoke.C

TEST_TARGETS += peekPoke.passed
peekPoke.passed: $(top_srcdir)/scripts/test_exit_status peekPoke
	@$(RTH_RUN)					\
		TITLE="peek/poke semantic states [$@]"	\
		CMD="./peekPoke"			\
		$< $@

########################################################################################################################

noinst_PROGRAMS += testSourceLocations
testSourceLocations_SOURCES = testSourceLocations.C

TEST_TARGETS += testSourceLocations.passed
testSourceLocations.passed: $(top_srcdir)/scripts/test_exit_status testSourceLocations
	@$(RTH_RUN)					\
		TITLE="source to address mapping [$@]"	\
		CMD="./testSourceLocations"		\
		$< $@

###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
