
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(roseDisassemblers_SRC
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Partitioner.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Disassembler.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerArm.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerPowerpc.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerX86.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Assembler.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init.C
   )

add_library( roseDisassemblers ${roseDisassemblers_SRC} ) 
add_dependencies( roseDisassemblers ROSETTA  )

########### install files ###############

install(FILES  Partitioner.h  	Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h  	Assembler.h AssemblerX86.h AssemblerX86Init.h  DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## This will cause the dynamic library to be built (as libroseDisassemblers.so)
#noinst_LTLIBRARIES = libroseDisassemblers.la
#
#libroseDisassemblers_la_DEPENDENCIES =
#
#libroseDisassemblers_la_SOURCES = \
#	Partitioner.C \
#        Disassembler.C DisassemblerArm.C DisassemblerPowerpc.C DisassemblerX86.C \
#	Assembler.C AssemblerX86.C AssemblerX86Init.C
#
#include_HEADERS = \
#	Partitioner.h \
#	Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h \
#	Assembler.h AssemblerX86.h AssemblerX86Init.h 
#
#dist_include_HEADERS =
