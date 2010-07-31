
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  GlobalDatabaseConnectionMYSQL.h TableAccessMacros.h TableAccess.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found. 
## so I disable this if --with-binarysql is used!
#if ROSE_USE_BINARY_SQL
#else
#if ROSE_USE_MYSQL_DATABASE
#
## DQ (11/13/2003): Modified to use GNU specific -idirafter option to 
##                  correct set.h (the g++ set.h instead of the C++ mySQL set.h)
## INCLUDES = $(ROSE_INCLUDES) -I/usr/include/g++-3 -I@MySQL_path@/include -I@MySQL_path@/include/mysql
## INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql
#INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql -idirafter /home/thuerey1/local/include
#
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(top_builddir)/projects/dataBase/librosedatabase.a @MySQL_path@/lib/mysql/libmysqlclient.a @MySQL_path@/lib/libsqlplus.a -lz
#
#librosedatabaseSources = \
#     GlobalDatabaseConnection.C \
#     TableAccess.C
#
## lib_LIBRARIES = librosedatabase.a
## librosedatabase_a_SOURCES  = $(librosedatabaseSources)
## librosedatabase_a_DEPENDENCIES = $(librosedatabase_a_SOURCES)
#
#noinst_LTLIBRARIES = librosedatabase.la
#librosedatabase_la_SOURCES = $(librosedatabaseSources)
#
#include_HEADERS = GlobalDatabaseConnectionMYSQL.h TableAccessMacros.h TableAccess.h
#
#
## Test codes must be placed outside of the source directory (ROSE/src) into the ROSE/tests directory
##bin_PROGRAMS = testDataBase testEdges
##testDataBase_SOURCES = testDataBase.C
##testDataBase_DEPENDENCIES = 
##testEdges_SOURCES = testEdges.C
#
## define all test codes
## TESTCODES = testEdges.C
#
## Build the list of object files
## TEST_Objects = ${TESTCODES:.C=.o}
#
## EXTRA_DIST = $(TESTCODES)
#
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf
#
## /usr/casc/overture/MySQL/MySQL_Install/bin/mysql -u root -prose-dq mysql -e "DELETE FROM user WHERE user = ''; GRANT ALL ON db.* TO dquinlan@'%.llnl.gov' IDENTIFIED BY 'rosepwd'; FLUSH PRIVILEGES; SELECT * FROM user;"
## "@MySQL_username@" , "@MySQL_password@", "@MySQL_database_name@"
#
#check-db:
##	initial zap to clean up from any errors in previous runs
#	@echo "Cleanup any previously left state within MySQL (errors in previous runs)"
#	@MySQL_path@/bin/mysql_zap -f mysqld
##	Create a data directory
#	-rm -rf /tmp/mysqld-datadir
##	Remove database directory (used for makefile check rule only, should not be used by users!)
#	mkdir /tmp/mysqld-datadir
##	Instantiate administrative databases
#	@MySQL_path@/bin/mysql_install_db --datadir=/tmp/mysqld-datadir/
##	start the daemon
#	@MySQL_path@/bin/mysqld_safe --datadir=/tmp/mysqld-datadir/ &
#	sleep 1
##	set the root password
#	@MySQL_path@/bin/mysqladmin --user=root password rootpwd
##	connect to the database as root to perform access control
#	@MySQL_path@/bin/mysql --user=root --password=rootpwd mysql -e "DELETE FROM user WHERE user = ''; \
#        GRANT ALL ON *.* TO @MySQL_username@ IDENTIFIED BY '@MySQL_password@'; FLUSH PRIVILEGES;"
#	@echo "Run test code ..."
#	./testEdges
##	rm -f testCode.o
##	./testDataBase
##	$(MAKE) $(TEST_Objects)
##	pkill mysqld_safe
#	@echo "Cleanup use of MySQL ..."
#	@MySQL_path@/bin/mysql_zap -f mysqld
#
## This test must be done in the ROSE/tests directory tree (not in the ROSE/src directory tree)
## check-local: check-db
#
#old-check-local:
##	Create a data directory
#	mkdir /tmp/mysqld-datadir
##	Instantiate administrative databases
#	@MySQL_path@/bin/mysql_install_db --datadir=/tmp/mysqld-datadir/
##	start the daemon
#	@MySQL_path@/bin/mysqld_safe --datadir=/tmp/mysqld-datadir/ &
##	set the root password
#	@MySQL_path@/bin/mysqladmin --user=root password rootpwd
##	connect to the database as root to perform access control
#	@MySQL_path@/bin/mysql --user=root --password=rootpwd mysql -e "DELETE FROM user WHERE user = ''; GRANT ALL ON *.* TO @MySQL_username@ IDENTIFIED BY @MySQL_password@; FLUSH PRIVILEGES;"
#	./testEdges
##	rm -f testCode.o
##	./testDataBase
##	$(MAKE) $(TEST_Objects)
#	pkill mysqld_safe
#endif
#endif
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
