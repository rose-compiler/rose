
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(customtable_example_SRCS customtable_example.C)

kde4_add_executable(customtable_example ${customtable_example_SRCS})

target_link_libraries(customtable_example ${KDE4_KDECORE_LIBS})

install(TARGETS customtable_example ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(databasegraph_example_SRCS databasegraph_example.C)

kde4_add_executable(databasegraph_example ${databasegraph_example_SRCS})

target_link_libraries(databasegraph_example ${KDE4_KDECORE_LIBS})

install(TARGETS databasegraph_example ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(rosedb_example_SRCS rosedb_example.C)

kde4_add_executable(rosedb_example ${rosedb_example_SRCS})

target_link_libraries(rosedb_example ${KDE4_KDECORE_LIBS})

install(TARGETS rosedb_example ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(simplecallgraph_example_SRCS simplecallgraph_example.C)

kde4_add_executable(simplecallgraph_example ${simplecallgraph_example_SRCS})

target_link_libraries(simplecallgraph_example ${KDE4_KDECORE_LIBS})

install(TARGETS simplecallgraph_example ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found. 
## so I disable this if --with-binarysql is used!
#if ROSE_USE_BINARY_SQL
#else
#if ROSE_USE_MYSQL_DATABASE
## DQ (11/13/2003): Modified to use GNU specific -idirafter option to 
##                  correct set.h (the g++ set.h instead of the C++ mySQL set.h)
## INCLUDES = $(ROSE_INCLUDES) -I/usr/include/g++-3 -I@MySQL_path@/include -I@MySQL_path@/include/mysql
## INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql
#INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql -idirafter /home/thuerey1/local/include
#
## LDADD    = $(ROSE_LIBS) $(top_builddir)/projects/dataBase/librosedatabase.a @MySQL_path@/lib/mysql/libmysqlclient.a @MySQL_path@/lib/libsqlplus.a -lm -lfl -lz
#
## Note: stlgraph.h stlDOTgraph.h are include temporarily here so that the 
##       tutorial codes will compile. Later they will be removed.
## include_HEADERS = stlgraph.h stlDOTgraph.h DOTGraphInterface.h DOTSubgraphRepresentation.h DOTSubgraphRepresentation.C
#
#bin_PROGRAMS = \
#     customtable_example \
#     databasegraph_example \
#     rosedb_example \
#     simplecallgraph_example
#
#customtable_example_SOURCES     = customtable_example.C
#databasegraph_example_SOURCES   = databasegraph_example.C
#rosedb_example_SOURCES          = rosedb_example.C
#simplecallgraph_example_SOURCES = simplecallgraph_example.C
#
#DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(top_builddir)/projects/dataBase/librosedatabase.a @MySQL_path@/lib/mysql/libmysqlclient.a @MySQL_path@/lib/libsqlplus.a -lz
#
## DEPENDENCIES = \
##     $(top_builddir)/QueryLibs/NameQuery/libnamequery.a \
##     $(top_builddir)/QueryLibs/NumberQuery/libnumberquery.a \
##     $(top_builddir)/src/librose.a \
##     $(top_builddir)/EDG/src/libedg.a \
##     $(top_builddir)/util/libroseutil.a \
##     $(top_builddir)/SAGE/libsage3.a \
##     $(top_builddir)/PDF/libpdf.a \
##     $(top_builddir)/AST_RewriteMechanism/librewrite.a \
##     $(top_builddir)/projects/dataBase/librosedatabase.a
#
## Add extra option to compile boost with g++
#AM_CXXFLAGS = -ftemplate-depth-30
#
#check-local:
#	rm -f testCode.o
## Could not initialize the data base and need to work with Nils to check this out
#	customtable_example
#	rosedb_example
#	databasegraph_example
#	@echo "Test not run due to database connection problem being worked on presently!"
#	$(MAKE) $(TEST_Objects)
#
## define all test codes
#TESTCODES = simplecallgraphTestProgram.c
#
#else
#
## define all test codes
#TESTCODES = 
#
#endif
#endif
## Could not initialize the data base and need to work with Nils to check this out
##     simplecallgraph_testprogram.C
#
## Build the list of object files
#TEST_Objects = ${TESTCODES:.C=.o}
#
#EXTRA_DIST = $(TESTCODES)
#
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf
#
#ROSE_FLAGS =
#
## This rule generates a dependency that forces the "make check" 
## test to always compile the test code
#FORCE_TEST_CODES_TO_RUN:
#
## $(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN simplecallgraph_example
#$(TEST_Objects): FORCE_TEST_CODES_TO_RUN simplecallgraph_example
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#	./simplecallgraph_example -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/simplecallgraphTestProgram.C
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
