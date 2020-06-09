include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

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
	hashMapUnitTests			\
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
	traceUnitTests				\
	mesgUnitTests				\
	markupUnitTests				\
	cmdUnitTests

if ROSE_HAVE_BOOST_SERIALIZATION_LIB
sawyer_checkers += serializationUnitTests
endif

if ROSE_USE_SQLITE_DATABASE
sawyer_checkers += databaseUnitTests
endif

# Unfortunately, automake doesn't let us use variables here, so we need to list them again.
noinst_PROGRAMS +=				\
        attributeUnitTests			\
        optionalUnitTests			\
        listUnitTests				\
        mapUnitTests				\
	hashMapUnitTests			\
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
	traceUnitTests				\
	mesgUnitTests				\
	markupUnitTests				\
	cmdUnitTests

if ROSE_HAVE_BOOST_SERIALIZATION_LIB
noinst_PROGRAMS += serializationUnitTests
endif

if ROSE_USE_SQLITE_DATABASE
noinst_PROGRAMS += databaseUnitTests
endif

# Each checker depends on one source file, but automake doesn't let us use patterns so we list them yet again
attributeUnitTests_SOURCES       = attributeUnitTests.C
optionalUnitTests_SOURCES        = optionalUnitTests.C
listUnitTests_SOURCES            = listUnitTests.C
mapUnitTests_SOURCES             = mapUnitTests.C
hashMapUnitTests_SOURCES	 = hashMapUnitTests.C
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
traceUnitTests_SOURCES		 = traceUnitTests.C
ptrUnitTests_SOURCES		 = ptrUnitTests.C
mesgUnitTests_SOURCES		 = mesgUnitTests.C
markupUnitTests_SOURCES		 = markupUnitTests.C
cmdUnitTests_SOURCES		 = cmdUnitTests.C
serializationUnitTests_SOURCES	 = serializationUnitTests.C
databaseUnitTests_SOURCES	 = databaseUnitTests.C

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

check-local: $(TEST_TARGETS)
