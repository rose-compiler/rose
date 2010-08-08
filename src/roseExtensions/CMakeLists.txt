
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(sqlite3x)
add_subdirectory(dataStructureTraversal)
add_subdirectory(highLevelGrammar)
add_subdirectory(roseHPCToolkit)
#add_subdirectory(qtWidgets)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#   sqlite3x \
#   SQLiteConnection \
#   databaseConnection \
#   dataStructureTraversal \
#   highLevelGrammar \
#   roseHPCToolkit \
#   qtWidgets
