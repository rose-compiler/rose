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

###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
