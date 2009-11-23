
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(roseExecFormats_SRC
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecGeneric.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecDOS.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecELF.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecPE.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecNE.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/ExecLE.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/Loader.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/LoaderELF.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/LoaderELFObj.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/LoaderPE.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/MemoryMap.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/ExecFormats/Interpretation.C
   )

add_library( roseExecFormats STATIC ${roseExecFormats_SRC} )
add_dependencies( roseExecFormats ROSETTA )

########### install files ###############

install(FILES  IntelPinSupport.h Loader.h LoaderELF.h LoaderELFObj.h LoaderPE.h MemoryMap.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## Added INTEP PIN support to collect dynamic binary analysis info.
#if ROSE_USE_INTEL_PIN
##  Use the path specified on the configuration command line (e.g. --with-Intel_Pin=<path>)
##   INTEL_PIN_PATH          = @IntelPin_path@
##   INTEL_PIN_LIB_PATHS     = -L$(INTEL_PIN_PATH)/intel64/lib -L$(INTEL_PIN_PATH)/extras/xed2-intel64/lib -L$(INTEL_PIN_PATH)/intel64/lib-ext
##   INTEL_PIN_INCLUDE_PATHS = -I$(INTEL_PIN_PATH)/source/include -I$(INTEL_PIN_PATH)/source/include/gen -I$(INTEL_PIN_PATH)/extras/xed2-intel64/include
##   INTEL_PIN_TOOLS_PATH    = $(INTEL_PIN_PATH)/source/tools/SimpleExamples
#
#   INTEL_PIN_SUPPORT = IntelPinSupport.C
#endif
#
## INCLUDES = $(ROSE_INCLUDES)
#INCLUDES = $(ROSE_INCLUDES) $(INTEL_PIN_INCLUDE_PATHS)
#
## This will cause the dynamic library to be built (as libroseExecFormats.so)
#noinst_LTLIBRARIES = libroseExecFormats.la
#
#libroseExecFormats_la_DEPENDENCIES =
#libroseExecFormats_la_SOURCES = \
#   $(INTEL_PIN_SUPPORT) \
#   ExecGeneric.C ExecDOS.C ExecELF.C ExecPE.C ExecNE.C ExecLE.C \
#   Loader.C LoaderELF.C LoaderELFObj.C LoaderPE.C \
#   MemoryMap.C Interpretation.C
#
#include_HEADERS = IntelPinSupport.h Loader.h LoaderELF.h LoaderELFObj.h LoaderPE.h MemoryMap.h
#
## Make sure that this is distributed even if ROSE was not configured using: -with-IntelPin=<path>
#EXTRA_DIST = IntelPinSupport.C
