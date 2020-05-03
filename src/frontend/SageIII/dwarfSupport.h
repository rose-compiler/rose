#include "rosePublicConfig.h"
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#ifdef ROSE_HAVE_DWARF_H
#include <dwarf.h>
#include <libdwarf.h>
#endif

// Main function to read dwarf.
// void build_dwarf_IR_nodes(Dwarf_Debug dbg, SgAsmInterpretation* asmInterpretation);

void readDwarf ( SgAsmGenericFile* asmFile );

#endif
