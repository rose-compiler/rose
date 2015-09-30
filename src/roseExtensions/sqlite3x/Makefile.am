include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST = CMakeLists.txt

if ROSE_USE_SQLITE_DATABASE

AM_CPPFLAGS = $(ROSE_INCLUDES)

# The SQLite3x library, a C++ wrapper around the libsqlite3 C API.  This is an old version of libsqlite3x, but the
# API changed in newer versions, so we're sticking with the old version.  We've also made a few changes to it ourselves.
noinst_LTLIBRARIES = libRoseSQLite3xDatabase.la
libRoseSQLite3xDatabase_la_SOURCES = \
	sqlite3x_command.C sqlite3x_exception.C  sqlite3x_transaction.C sqlite3x_connection.C  sqlite3x_reader.C
pkginclude_HEADERS = sqlite3x.h

# SQLite3 is missing some common string and arithmetic functions, but we can provide them through a dynamic plugin.
lib_LTLIBRARIES = libsqlitefunctions.la
libsqlitefunctions_la_SOURCES = extension-functions.c

endif

