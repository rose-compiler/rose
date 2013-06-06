#include "rosePublicConfig.h"

#ifdef ROSE_HAVE_DWARF_H
#include <dwarf.h>
#include <libdwarf.h>
#endif

// Main function to read dwarf.
// void build_dwarf_IR_nodes(Dwarf_Debug dbg, SgAsmInterpretation* asmInterpretation);

void readDwarf ( SgAsmGenericFile* asmFile );


