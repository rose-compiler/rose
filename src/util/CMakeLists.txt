
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(stringSupport)
add_subdirectory(commandlineProcessing)
add_subdirectory(support)
add_subdirectory(graphs)

###############################################################################################################################
#Generating "rose_paths.C"
#We may want to generate it in a different way in the future
###############################################################################################################################
file( REMOVE ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C )
file( WRITE  ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "\#include <string> \n\n" )
#file( APPEND ${ROSE_TOP_BINARY_DIR}/test.txt we have ${CMAKE_Fortran_COMPILER} )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* Use the same header file to declare these variables as is used to reference them so that they will be globally available (and not local). */\n" )
file( APPEND  ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "\#include \"rose_paths.h\" \n\n" )

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* These paths will be absolute or relative depending on how the configure script is called (called with an absolute or relative path). */\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_GFORTRAN_PATH          = \"${CMAKE_Fortran_COMPILER}\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_TOP_SRCDIR    = \"${ROSE_TOP_SRC_DIR}\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_TOP_BUILDDIR  = \"${ROSE_TOP_BINARY_DIR}\";\n" )
#FIXME: may be a different directory at times. Not sure what the semantics are.
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_PREFIX        = \"${ROSE_TOP_BINARY_DIR}\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_DATADIR       = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_BINDIR        = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_INCLUDEDIR    = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_INFODIR       = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_LIBDIR        = \"/*deprecated*/\";" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_LIBEXECDIR    = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_LOCALSTATEDIR = \"/*deprecated*/\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_MANDIR        = \"/*deprecated*/\";\n\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* This will always be an absolute path, while paths above are dependent on how the configure script is called \(called with an absolute or relative path\). */\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR = \"${ROSE_TOP_SRC_DIR}\";\n\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* Additional interesting data to provide. */\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_CONFIGURE_DATE     = \"@configure_date@\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_BUILD_OS  = \"${CMAKE_HOST_SYSTEM_VERSION}\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_AUTOMAKE_BUILD_CPU = \"${CMAKE_HOST_SYSTEM_PROCESSOR}\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "const std::string ROSE_OFP_VERSION_STRING = \"@ROSE_OFP_VERSION_NUMBER@\";\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* Numeric form of ROSE version -- assuming ROSE version x.y.zL \(where */\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* x, y, and z are numbers, and L is a single lowercase letter from a to j\), */\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C "/* the numeric value is x * 1000000 + y * 10000 + z * 100 + \(L - 'a'\). */\n" )
#	@@echo "@PACKAGE_VERSION@" | sed 's/\([0-9]\)\([a-z]\)/\1_\2/g' | tr .a-j _0-9 | awk -F_ '{printf "%02d%02d%02d%02d\n", $$1, $$2, $$3, $$4}' | sed 's/^0*//' | sed 's/.*/const int ROSE_NUMERIC_VERSION = \0;/' >> src/util/rose_paths.C
########### next target ###############


########### install files ###############

install(FILES  FormatRestorer.h setup.h processSupport.h rose_paths.h DESTINATION ${INCLUDE_INSTALL_DIR})
#set( graphs DGBaseGraphImpl.C SCCAnalysis.C  TransAnalysis.C  TypedFusion.C )

#add_library(rose_util_lib STATIC ${rose_util_src}    )
