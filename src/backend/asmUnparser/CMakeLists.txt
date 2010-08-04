
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library(  roseAsmUnparser STATIC ${roseAsmUnparser_SRC} )
#add_dependencies( roseAsmUnparser ROSETTA )
########### install files ###############

# tps: (11/25/2009): This seems wrong
#install(FILES  unparseAsm.h DESTINATION src/frontend/PHPFrontend/CMakeLists.txt )
install(FILES  AsmUnparser.h AsmUnparser_compat.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## This will cause the dynamic library to be built (as libroseDisassemblers.so)
#noinst_LTLIBRARIES = libroseAsmUnparser.la
#
#libroseAsmUnparser_la_DEPENDENCIES =
#libroseAsmUnparser_la_SOURCES = unparseX86Asm.C unparseArmAsm.C unparsePowerpcAsm.C unparseAsm.C
#
#dist_include_HEADERS = unparseAsm.h
#
#include_HEADERS = 
