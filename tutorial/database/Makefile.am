include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found. 
# so I disable this if --with-binarysql is used!
if ROSE_USE_BINARY_SQL
else
if ROSE_USE_MYSQL_DATABASE
# DQ (11/13/2003): Modified to use GNU specific -idirafter option to 
#                  correct set.h (the g++ set.h instead of the C++ mySQL set.h)
# INCLUDES = $(ROSE_INCLUDES) -I/usr/include/g++-3 -I@MySQL_path@/include -I@MySQL_path@/include/mysql
# INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql
INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql 

# LDADD    = $(ROSE_LIBS) $(top_builddir)/projects/dataBase/librosedatabase.a @MySQL_path@/lib/mysql/libmysqlclient.a @MySQL_path@/lib/libsqlplus.a -lm -lfl -lz

# Note: stlgraph.h stlDOTgraph.h are include temporarily here so that the 
#       tutorial codes will compile. Later they will be removed.
# include_HEADERS = stlgraph.h stlDOTgraph.h DOTGraphInterface.h DOTSubgraphRepresentation.h DOTSubgraphRepresentation.C

bin_PROGRAMS = \
     customtable_example \
     databasegraph_example \
     rosedb_example \
     simplecallgraph_example

customtable_example_SOURCES     = customtable_example.C
databasegraph_example_SOURCES   = databasegraph_example.C
rosedb_example_SOURCES          = rosedb_example.C
simplecallgraph_example_SOURCES = simplecallgraph_example.C

DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

# milki (06/23/2010) MYSQL database support moved to src/roseExtensions
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(top_builddir)/projects/dataBase/librosedatabase.a @MySQL_path@/lib/mysql/libmysqlclient.a @MySQL_path@/lib/libsqlplus.a -lz
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# DEPENDENCIES = \
#     $(top_builddir)/QueryLibs/NameQuery/libnamequery.a \
#     $(top_builddir)/QueryLibs/NumberQuery/libnumberquery.a \
#     $(top_builddir)/src/librose.a \
#     $(top_builddir)/EDG/src/libedg.a \
#     $(top_builddir)/util/libroseutil.a \
#     $(top_builddir)/SAGE/libsage3.a \
#     $(top_builddir)/PDF/libpdf.a \
#     $(top_builddir)/AST_RewriteMechanism/librewrite.a \
#     $(top_builddir)/projects/dataBase/librosedatabase.a

# Add extra option to compile boost with g++
# milki (06/23/2010) Increased depth due to template instantiation
# errors
#AM_CXXFLAGS = -ftemplate-depth-30
AM_CXXFLAGS = -ftemplate-depth-32

check-local:
	rm -f testCode.o
# initial zap to clean up from any errors in previous runs
	@echo "Cleanup any previously left state within MySQL (errors in previous runs)"
	@MySQL_path@/bin/mysql_zap -f mysqld
# Create a data directory
	-rm -rf /tmp/mysqld-datadir
# Remove database directory (used for makefile check rule only, should not be used by users!)                                                                     mkdir /tmp/mysqld-datadir
# Instantiate administrative databases
	@MySQL_path@/bin/mysql_install_db --datadir=/tmp/mysqld-datadir/              # start the daemoni
	@MySQL_path@/bin/mysqld_safe --datadir=/tmp/mysqld-datadir/ &
	sleep 10
# set the root password
	@MySQL_path@/bin/mysqladmin --user=root password rootpwd
# connect to the database as root to perform access control
	@MySQL_path@/bin/mysql --user=root --password=rootpwd mysql -e "DELETE FROM mysql.user WHERE user = '@MySQL_username@@@MySQL_server@'; \
	      GRANT ALL ON *.* TO @MySQL_username@@@MySQL_server@ IDENTIFIED BY '@MySQL_password@'; FLUSH PRIVILEGES;"
	@echo "Run test code ..."
	./customtable_example
	./rosedb_example
	./databasegraph_example
# milki (07/12/2010) Commented out bad test in preperation for mysql
# deprecation
# $(MAKE) $(TEST_Objects)
	@echo "Cleaning use of MySQL ..."
	@MySQL_path@/bin/mysql_zap -f mysqld

# define all test codes
TESTCODES = simplecallgraphTestProgram.c

else

# define all test codes
TESTCODES = 

endif
endif
# Could not initialize the data base and need to work with Nils to check this out
#     simplecallgraph_testprogram.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

EXTRA_DIST = CMakeLists.txt $(TESTCODES)

CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf

ROSE_FLAGS =

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

# $(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN simplecallgraph_example
$(TEST_Objects): FORCE_TEST_CODES_TO_RUN simplecallgraph_example
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./simplecallgraph_example -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/simplecallgraphTestProgram.C
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)















