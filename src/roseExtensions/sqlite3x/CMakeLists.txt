
include_directories(${SQLITE3_INCLUDEDIR})

add_library(RoseSQLite3xDatabase
  sqlite3x_command.C sqlite3x_exception.C sqlite3x_transaction.C
  sqlite3x_connection.C  sqlite3x_reader.C
)
target_link_libraries(RoseSQLite3xDatabase ${SQLITE3_LIBRARIES})

########### install files ###############

install(FILES  sqlite3x.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_SQLITE_DATABASE
#INCLUDES = $(ROSE_INCLUDES)
##ACLOCAL_AMFLAGS = -I m4
#
##AM_CXXFLAGS = $(BOOST_CPPFLAGS) ${SQLITE3_CFLAGS}
##AM_LDFLAGS = $(BOOST_LDFLAGS)  $(BOOST_PROGRAM_OPTIONS_LIB) $(SQLITE3_LDFLAGS)
#
#
#
#libRoseSQLite3xDatabaseSources =  sqlite3x_command.C     sqlite3x_exception.C  sqlite3x_transaction.C \
#sqlite3x_connection.C  sqlite3x_reader.C
#noinst_LTLIBRARIES = libRoseSQLite3xDatabase.la
#libRoseSQLite3xDatabase_la_SOURCES = $(libRoseSQLite3xDatabaseSources)
#endif
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates libRoseSQLite3xDatabase.so
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = \
#   sqlite3x.h
#
