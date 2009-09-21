// This file is read by rose.h, if the USE_ROSE_DWARF_SUPPORT is set to 1
// by the use of --with-dwarf=<path> in the ROSE configure command line.

#include <dwarf.h>
#include <libdwarf.h>

// Main function to read dwarf.
// void build_dwarf_IR_nodes(Dwarf_Debug dbg, SgAsmInterpretation* asmInterpretation);

void readDwarf ( SgAsmGenericFile* asmFile );


