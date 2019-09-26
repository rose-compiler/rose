#ifndef ROSE_Partitioner2_ModulesElf_H
#define ROSE_Partitioner2_ModulesElf_H

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Modules.h>

#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utility functions for ELF. */
namespace ModulesElf {

/** Reads ELF .eh_frames to find function entry addresses.
 *
 *  Performs an AST traversal rooted at the specified @p ast to find ELF .eh_frames sections and returns a sorted list of
 *  functions at unique starting addresses.  The functions are not attached to the CFG/AUM.
 *
 * @{ */
std::vector<Function::Ptr> findErrorHandlingFunctions(SgAsmElfFileHeader*);
std::vector<Function::Ptr> findErrorHandlingFunctions(SgAsmInterpretation*);
size_t findErrorHandlingFunctions(SgAsmElfFileHeader*, std::vector<Function::Ptr>&);
/** @} */

/** Reads ELF PLT sections and returns a list of functions.
 *
 * @{ */
std::vector<Function::Ptr> findPltFunctions(const Partitioner&, SgAsmElfFileHeader*);
std::vector<Function::Ptr> findPltFunctions(const Partitioner&, SgAsmInterpretation*);
size_t findPltFunctions(const Partitioner&, SgAsmElfFileHeader*, std::vector<Function::Ptr>&);
/** @} */

/** Get a list of all ELF sections by name.
 *
 *  Returns an empty list if the interpretation is null or it doesn't have any sections that match the specified name. */
std::vector<SgAsmElfSection*> findSectionsByName(SgAsmInterpretation*, const std::string&);

/** True if the function is an import.
 *
 *  True if the specified function is an import, whether it's actually been linked in or not. This is a weaker version of @ref
 *  isLinkedImport. */
bool isImport(const Partitioner&, const Function::Ptr&);

/** True if function is a linked import.
 *
 *  Returns true if the specified function is an import which has been linked to an actual function. This is a stronger version
 *  of @ref isImport. */
bool isLinkedImport(const Partitioner&, const Function::Ptr&);

/** True if function is a non-linked import.
 *
 *  Returns true if the specified function is an import function but has not been linked in yet. */
bool isUnlinkedImport(const Partitioner&, const Function::Ptr&);

/** True if named file is an ELF object file.
 *
 *  Object files usually have names with a ".o" extension, although this function actually tries to open the file and parse
 *  some ELF data structures to make that determination. */
bool isObjectFile(const boost::filesystem::path&);

/** True if named file is a static library archive.
 *
 *  Archives usually have names ending with a ".a" extension, although this function actually tries to open the file and parse
 *  the header to make that determination. */
bool isStaticArchive(const boost::filesystem::path&);

/** Boolean flag for @ref tryLink. */
namespace FixUndefinedSymbols {
/** Boolean flag for @ref tryLink. */
enum Boolean {
    NO,                                                 /**< Do not try to fix undefined symbols. */
    YES                                                 /**< Yes, try to fix undefined symbols. */
};
} // namespace

/** Try to run a link command.
 *
 *  The substring "%o" is replaced by the quoted output name, and the substring "%f" is replaced by the space separated list of
 *  quoted input names.  Bourne shell escape syntax is used. Returns true if the link command was successful, false otherwise. */
bool tryLink(const std::string &command, const boost::filesystem::path &outputName,
             std::vector<boost::filesystem::path> inputNames, Sawyer::Message::Stream &errors,
             FixUndefinedSymbols::Boolean fixUndefinedSymbols = FixUndefinedSymbols::YES);

/** Extract object files from a static archive.
 *
 *  Given the name of an archive file, extract the member files and return a list of object files. The files are extracted into
 *  a subdirectory of the specified @p directory. The name of the subdirectory is the same as the file name (not directory
 *  part) of the archive. */
std::vector<boost::filesystem::path>
extractStaticArchive(const boost::filesystem::path &directory, const boost::filesystem::path &archive);

/** Matches an ELF PLT entry.  The address through which the PLT entry branches is remembered. This address is typically an
 *  RVA which is added to the initial base address. */
struct PltEntryMatcher: public InstructionMatcher {
    rose_addr_t baseVa_;                                // base address for computing memAddress_
    rose_addr_t gotEntryVa_;                            // address through which an indirect branch branches
    size_t gotEntryNBytes_;                             // size of the global offset table entry in bytes
    rose_addr_t gotEntry_;                              // address read from the GOT if the address is mapped (or zero)
    size_t nBytesMatched_;                              // number of bytes matched for PLT entry

public:
    PltEntryMatcher(rose_addr_t base)
        : baseVa_(base), gotEntryVa_(0), gotEntryNBytes_(0), gotEntry_(0), nBytesMatched_(0) {}
    static Ptr instance(rose_addr_t base) { return Ptr(new PltEntryMatcher(base)); }
    virtual bool match(const Partitioner&, rose_addr_t anchor);

    /** Size of the PLT entry in bytes. */
    size_t nBytesMatched() const { return nBytesMatched_; }

    /** Address of the corresponding GOT entry. */
    rose_addr_t gotEntryVa() const { return gotEntryVa_; }

    /** Size of the GOT entry in bytes. */
    size_t gotEntryNBytes() const { return gotEntryNBytes_; }
    
    /** Value stored in the GOT entry. */
    rose_addr_t gotEntry() const { return gotEntry_; }

    // [Robb Matzke 2018-04-06]: deprecated: use gotEntryVa
    rose_addr_t memAddress() const { return gotEntryVa_; }
};

/** Build may-return white and black lists. */
void buildMayReturnLists(Partitioner&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
