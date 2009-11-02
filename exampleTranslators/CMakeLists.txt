
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(documentedExamples)
add_subdirectory(defaultTranslator)
add_subdirectory(DOTGenerator)
add_subdirectory(PDFGenerator)
add_subdirectory(AstCopyReplTester)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#     documentedExamples \
#     defaultTranslator  \
#     DOTGenerator       \
#     PDFGenerator       \
#     AstCopyReplTester
#
## DQ (9/11/2008): We can add these later but these are older QRose examples.
## Current examples are in developersScratchSpace/Dan/QRose_tests.
##    graphicalUserInterfaceExamples
#
