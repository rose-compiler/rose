include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

########################################################################################################################
# Sawyer Unit Tests

# Sawyer unit tests. These are copied from the Sawyer release at https://github.com/matzke1/sawyer by
# the script in $ROSE/src/util/Sawyer/updateFromGithub.sh
# The order of this list should be from most basic to most advanced (dependencies checked before uses)
sawyer_checkers =				\
	attributeUnitTests			\
	optionalUnitTests			\
	listUnitTests				\
	mapUnitTests				\
	setUnitTests				\
        distinctListUnitTests			\
	intervalUnitTests			\
	bitvecTests				\
	denseIntegerSetUnitTests		\
	addressMapUnitTests			\
        graphUnitTests				\
	indexedGraphDemo			\
	graphIsomorphismTests			\
	intervalSetMapUnitTests			\
	ptrUnitTests				\
	mesgUnitTests				\
	markupUnitTests				\
	cmdUnitTests

# Unfortunately, automake doesn't let us use variables here, so we need to list them again.
noinst_PROGRAMS +=				\
        attributeUnitTests			\
        optionalUnitTests			\
        listUnitTests				\
        mapUnitTests				\
        setUnitTests				\
        distinctListUnitTests			\
        intervalUnitTests			\
        bitvecTests				\
        denseIntegerSetUnitTests		\
        addressMapUnitTests			\
        graphUnitTests				\
        indexedGraphDemo			\
        graphIsomorphismTests			\
        intervalSetMapUnitTests			\
	ptrUnitTests				\
	mesgUnitTests				\
	markupUnitTests				\
	cmdUnitTests

# Each checker depends on one source file, but automake doesn't let us use patterns so we list them yet again
attributeUnitTests_SOURCES       = attributeUnitTests.C
optionalUnitTests_SOURCES        = optionalUnitTests.C
listUnitTests_SOURCES            = listUnitTests.C
mapUnitTests_SOURCES             = mapUnitTests.C
setUnitTests_SOURCES             = setUnitTests.C
distinctListUnitTests_SOURCES    = distinctListUnitTests.C
intervalUnitTests_SOURCES        = intervalUnitTests.C
bitvecTests_SOURCES              = bitvecTests.C
denseIntegerSetUnitTests_SOURCES = denseIntegerSetUnitTests.C
addressMapUnitTests_SOURCES      = addressMapUnitTests.C
graphUnitTests_SOURCES           = graphUnitTests.C
indexedGraphDemo_SOURCES         = indexedGraphDemo.C
graphIsomorphismTests_SOURCES    = graphIsomorphismTests.C
intervalSetMapUnitTests_SOURCES  = intervalSetMapUnitTests.C
ptrUnitTests_SOURCES		 = ptrUnitTests.C
mesgUnitTests_SOURCES		 = mesgUnitTests.C
markupUnitTests_SOURCES		 = markupUnitTests.C
cmdUnitTests_SOURCES		 = cmdUnitTests.C

# Test targets
sawyer_targets = $(addsuffix .passed, $(sawyer_checkers))
TEST_TARGETS += $(sawyer_targets)

# Test rules
$(sawyer_targets): %.passed: %
	@$(RTH_RUN)						\
		TITLE="Sawyer unit test [$@]"			\
		CMD="$$(pwd)/$^"				\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-sawyer
check-sawyer: $(SAWYER_TEST_TARGETS)


###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-smoke-local: $(TEST_TARGETS)
check-nonsmoke-local:

check-local: check-smoke-local
check-all-local: check-smoke-local check-nonsmoke-local

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
