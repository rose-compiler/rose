
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(abstractHandle)
add_subdirectory(astDiagnostics)
add_subdirectory(astProcessing)
add_subdirectory(astMatching)
add_subdirectory(astQuery)
add_subdirectory(astRewriteMechanism)
add_subdirectory(astUtil)
add_subdirectory(binaryAnalyses)
add_subdirectory(programAnalysis)
add_subdirectory(programTransformation)
########### next target   ###############

#add_library( midend_static STATIC ${midend_src}  )
#add_dependencies( midend_static ROSETTA midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary )

# tps (12/17/2009) : Experiments to create (small) dynamic libraries.
# The following libraries require dllexport and dllimport in WINDOWS!
#add_library( midend_shared SHARED ${midend_src}  )
#add_dependencies( midend_shared ROSETTA midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary )

#target_link_libraries(midend_static 
#     midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary
# )
 
########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#        abstractHandle \
#        astDiagnostics \
#        astInlining \
#        astOutlining \
#        astProcessing \
#        astMatching \
#        astQuery \
#        astRewriteMechanism \
#        astUtil \
#        binaryAnalyses \
#        loopProcessing \
#        ompLowering \
#        programAnalysis \
#        programTransformation
#
## DQ (7/25/2008):
## This file is deliberately excluded from a distribution so that the
## "make docs" rule (at the top level directory) can query the file to
## determin if this is a distribution.
## EXTRA_DIST = midend.docs
