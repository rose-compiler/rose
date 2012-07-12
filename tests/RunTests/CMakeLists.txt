
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(A++Tests)
add_subdirectory(AstDeleteTests)
add_subdirectory(FortranTests)
add_subdirectory(PythonTests)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## SUBDIRS = @optional_AxxPxxSpecificTest_subdirs@
## we can't make this @optional_AxxPxxSpecificTest_subdirs@
## since the substitution could be NULL which causes a problem
## for automake generated shell code placed in Makefile.
#
## Comment out P++ tests until they compile properly
## SUBDIRS = A++Tests P++Tests
#SUBDIRS = A++Tests AstDeleteTests
#
