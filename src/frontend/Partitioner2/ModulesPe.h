#ifndef ROSE_Partitioner2_ModulesPe_H
#define ROSE_Partitioner2_ModulesPe_H

#include <Partitioner2/Function.h>
#include <Partitioner2/Modules.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesPe {

/** Index for PE import addresses.
 *
 *  This is a map from possible import addresses to the item in the Import Address Table (IAT) that describes the function that
 *  will be imported to that address. The map probably contains at least two addresses per function, the absolute address and a
 *  relative address, because we're not sure which is contained in the IAT -- it depends on whether a linker has run. */
typedef Sawyer::Container::Map<rose_addr_t, SgAsmPEImportItem*> ImportIndex;

/** Reads PE export sections to find functions.
 *
 *  Scans all PE export sections under the specified PE file header to obtain names and addresses for functions.  Returns a
 *  list of new, unique functions sorted by entry address.
 *
 * @{ */
std::vector<Function::Ptr> findExportFunctions(const Partitioner&, SgAsmPEFileHeader*);
std::vector<Function::Ptr> findExportFunctions(const Partitioner&, SgAsmInterpretation*);
size_t findExportFunctions(const Partitioner&, SgAsmInterpretation*, std::vector<Function::Ptr>&);
/** @} */

/** Reads PE import sections to find functions.
 *
 *  Scans all PE import sections in the specified interpretation to obtain addresses where imported functions will be mapped or
 *  have been mapped, depending on whether a dynamic linker has run.  The return value is a list of new, unique functions
 *  sorted by entry address.
 *
 * @{ */
std::vector<Function::Ptr> findImportFunctions(const Partitioner&, SgAsmPEFileHeader*);
std::vector<Function::Ptr> findImportFunctions(const Partitioner&, SgAsmInterpretation*);
size_t findImportFunctions(const Partitioner&, SgAsmPEFileHeader*, const ImportIndex&, std::vector<Function::Ptr>&);
/** @} */

/** Scans PE import sections to build an index.
 *
 * @{ */
ImportIndex getImportIndex(const Partitioner&, SgAsmPEFileHeader*);
ImportIndex getImportIndex(const Partitioner&, SgAsmInterpretation*);
size_t getImportIndex(const Partitioner&, SgAsmPEFileHeader*, ImportIndex&);
/** @} */

/** Update import address tables to reflect addresses of imported functions. */
void rebaseImportAddressTables(Partitioner &partitioner, const ImportIndex &index);

/** Names functions that look like they're thunks for imports.
 *
 *  An import thunk is an indirect unconditional jump through a read-only import address table to a virtual address where an
 *  external function will be dynamically linked.  The dynamic linking will have already taken place if a linker was used prior
 *  to disassembly. */
void nameImportThunks(const Partitioner&, SgAsmInterpretation*);

/** Build may-return white and black lists. */
void buildMayReturnLists(Partitioner&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
