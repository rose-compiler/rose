include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Added INTEP PIN support to collect dynamic binary analysis info.
if ROSE_USE_INTEL_PIN
#  Use the path specified on the configuration command line (e.g. --with-Intel_Pin=<path>)
#   INTEL_PIN_PATH          = @IntelPin_path@
#   INTEL_PIN_LIB_PATHS     = -L$(INTEL_PIN_PATH)/intel64/lib -L$(INTEL_PIN_PATH)/extras/xed2-intel64/lib -L$(INTEL_PIN_PATH)/intel64/lib-ext
#   INTEL_PIN_INCLUDE_PATHS = -I$(INTEL_PIN_PATH)/source/include -I$(INTEL_PIN_PATH)/source/include/gen -I$(INTEL_PIN_PATH)/extras/xed2-intel64/include
#   INTEL_PIN_TOOLS_PATH    = $(INTEL_PIN_PATH)/source/tools/SimpleExamples

   INTEL_PIN_SUPPORT = IntelPinSupport.C
endif

# INCLUDES = $(ROSE_INCLUDES)
INCLUDES = $(ROSE_INCLUDES) $(INTEL_PIN_INCLUDE_PATHS)

# This will cause the dynamic library to be built (as libroseBinaryFormats.so)
noinst_LTLIBRARIES = libroseBinaryFormats.la

libroseBinaryFormats_la_DEPENDENCIES =
libroseBinaryFormats_la_SOURCES = \
   $(INTEL_PIN_SUPPORT) \
   DataConversion.C ExtentMap.C Hexdump.C Interpretation.C MemoryMap.C Rva.C \
   GenericDynamicLinking.C GenericFile.C GenericFormat.C GenericHeader.C GenericSection.C GenericString.C \
   PeExport.C PeFileHeader.C PeImport.C PeSection.C PeStringTable.C PeSymbolTable.C \
   ElfDynamicLinking.C ElfErrorFrame.C ElfFileHeader.C ElfNote.C ElfRelocation.C ElfSection.C ElfSectionTable.C \
   ElfSegmentTable.C ElfStringTable.C ElfSymbolTable.C ElfSymbolVersion.C \
   ExecDOS.C ExecNE.C ExecLE.C ExecGeneric.C PeSectionTable.C

include_HEADERS = DataConversion.h IntelPinSupport.h MemoryMap.h

# Make sure that this is distributed even if ROSE was not configured using: -with-IntelPin=<path>
EXTRA_DIST = CMakeLists.txt IntelPinSupport.C
