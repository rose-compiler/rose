
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(simpleTranslatorExamples)
add_subdirectory(AstRewriteExamples)
add_subdirectory(dataBaseExamples)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#     simpleTranslatorExamples \
#     AstRewriteExamples \
#     dataBaseExamples
#
## DQ (1/2/2007): Most of these examples have been moved to the ROSE/tutorial directory.
## And additional problem is that the absolute paths for the files exceed 100 characters
## which is a posix limit not enforced by GNU tar 1.14 (2004) and so it is a hard limit 
## for us.
## astProcessingExamples
#
