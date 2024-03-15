#ifndef ROSE_BinaryAnalysis_Dwarf_BasicTypes_H
#define ROSE_BinaryAnalysis_Dwarf_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

#include <Cxx_GrammarDeclarations.h>

#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {

/** Functions for DWARF debugging information.
 *
 *  DWARF is generated when source code is compiled using a compiler's debug mode. The DWARF information is represented in special
 *  sections in the binary ELF container. These sections are read using the open source "libdwarf" library. The parsed DWARF
 *  information is represented in ROSE by AST nodes whose type names begin with "SgAsmDwarf". */
namespace Dwarf {

/** Diagnostic facility for DWARF parsing. */
extern Sawyer::Message::Facility mlog;

/** Main function to parse DWARF information. */
void parse(SgAsmGenericFile*);

} // namespace
} // namespace
} // namespace

#endif
#endif
