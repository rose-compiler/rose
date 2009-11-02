
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  README DESTINATION ${INCLUDE_INSTALL_DIR} )
install(FILES  ColData.h  Connection.h  dbheaders.h  GlobalDatabaseConnection.h  Query.h  Result.h  Row.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_SQLITE_DATABASE
#INCLUDES = $(ROSE_INCLUDES)
#
### The grammar generator (ROSETTA) should use its own template repository
## CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libRoseSQLiteDatabaseSources =  Result.C GlobalDatabaseConnection.C Query.C Connection.C
#
#noinst_LTLIBRARIES = libRoseSQLiteDatabase.la
#libRoseSQLiteDatabase_la_SOURCES = $(libRoseSQLiteDatabaseSources)
#endif
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates libRoseSQLiteDatabase.so
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = \
#   ColData.h  Connection.h  dbheaders.h  GlobalDatabaseConnection.h  Query.h  Result.h  Row.h
#
#dist_noinst_DATA = README
