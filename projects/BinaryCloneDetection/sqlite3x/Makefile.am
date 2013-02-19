ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = $(BOOST_CPPFLAGS) ${SQLITE3_CFLAGS}
AM_LDFLAGS = $(BOOST_LDFLAGS)  $(BOOST_PROGRAM_OPTIONS_LIB) $(SQLITE3_LDFLAGS)

libRoseSQLiteDatabaseSources =  sqlite3x_command.C     sqlite3x_exception.C  sqlite3x_transaction.C \
sqlite3x_connection.C  sqlite3x_reader.C


if DEV_LIBS
lib_LTLIBRARIES = libRoseSQLiteDatabase.la
else
noinst_LTLIBRARIES = libRoseSQLiteDatabase.la
endif
libRoseSQLiteDatabase_la_SOURCES = $(libRoseSQLiteDatabaseSources)


