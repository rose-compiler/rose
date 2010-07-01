include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The testing of the side effect analysis depends upon the side effect analysis,
# which depends upon the database support within ROSE, which depends upon the use
# of MySQL, which is an external (and optional) piece of the ROSE puzzle.

# tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found.
# so I disable this if --with-binarysql is used!
if ROSE_USE_BINARY_SQL
else
if ROSE_USE_MYSQL_DATABASE

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
# CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = testSideEffect
testSideEffect_SOURCES = testSideEffect.C
# testSideEffect_DEPENDENCIES = TestDriver.o
testSideEffect_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = testClasses1.C  testClasses2.C  testClasses3.C  testGlobal1.C  testGlobal2.C \
	           testLoop.C  testPtr1.C  testPtr2.C  testRef.C  testShadow.C

TESTCODES =

# to deactivate tests comment out check-local
alt-check-local:
	./testSideEffect $(srcdir)/testClasses1.C
	./testSideEffect $(srcdir)/testClasses2.C
	./testSideEffect $(srcdir)/testClasses3.C
	./testSideEffect $(srcdir)/testGlobal1.C
	./testSideEffect $(srcdir)/testGlobal2.C
	./testSideEffect $(srcdir)/testLoop.C
	./testSideEffect $(srcdir)/testPtr1.C
	./testSideEffect $(srcdir)/testPtr2.C
	./testSideEffect $(srcdir)/testRef.C
	./testSideEffect $(srcdir)/testShadow.C

# DQ (8/26/2004):
# This test currently fails because the data base is not setup before
# the test is run.  We should likely use the same technique for setting
# up the database as in the test code used in the database test directory.
# This can be fixed later after discussion with Brian White.
check-local:	testSideEffect
	@echo "Cleanup any previously left state within MySQL (errors in previous runs)"
	@MySQL_path@/bin/mysql_zap -f mysqld
# Create a data directory
	-rm -rf /tmp/mysqld-datadir
# Remove database directory (used for makefile check rule only, should not be used by users!)
	mkdir /tmp/mysqld-datadir
# Instantiate administrative databases
	@MySQL_path@/bin/mysql_install_db --datadir=/tmp/mysqld-datadir/
# start the daemon
	@MySQL_path@/bin/mysqld_safe --datadir=/tmp/mysqld-datadir/ &
	sleep 10
# set the root password
	@MySQL_path@/bin/mysqladmin --user=root password rootpwd
# connect to the database as root to perform access control
	@MySQL_path@/bin/mysql --user=root --password=rootpwd mysql -e "DELETE FROM mysql.user WHERE user = '@MySQL_username@@@MySQL_server@'; \
	      GRANT ALL ON *.* TO @MySQL_username@@@MySQL_server@ IDENTIFIED BY '@MySQL_password@'; FLUSH PRIVILEGES;"
	@echo "Run test code ..."
	./testSideEffect $(srcdir)/testGlobal1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal1.outx
	./testSideEffect $(srcdir)/testGlobal2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal2.outx
	./testSideEffect $(srcdir)/testRef.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testRef.outx
	./testSideEffect $(srcdir)/testPtr1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testPtr1.outx
	./testSideEffect $(srcdir)/testClasses1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses1.outx
	./testSideEffect $(srcdir)/testClasses2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses2.outx
	./testSideEffect $(srcdir)/testClasses3.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses3.outx
	./testSideEffect $(srcdir)/testLoop.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testLoop.outx
	@echo "Side-Effect Tests Passed"
	@echo "Cleanup use of MySQL ..."
	@MySQL_path@/bin/mysql_zap -f mysqld

# endif for section of Makefile.am which is dependent upon the optional database support
endif
endif
clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB






