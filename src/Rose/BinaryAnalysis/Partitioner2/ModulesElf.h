#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesElf_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesElf_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>

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
std::vector<FunctionPtr> findErrorHandlingFunctions(SgAsmElfFileHeader*);
std::vector<FunctionPtr> findErrorHandlingFunctions(SgAsmInterpretation*);
size_t findErrorHandlingFunctions(SgAsmElfFileHeader*, std::vector<FunctionPtr>&);
/** @} */

/** Reads ELF PLT sections and returns a list of functions.
 *
 * @{ */
std::vector<FunctionPtr> findPltFunctions(const PartitionerPtr&, SgAsmElfFileHeader*);
std::vector<FunctionPtr> findPltFunctions(const PartitionerPtr&, SgAsmInterpretation*);
size_t findPltFunctions(const PartitionerPtr&, SgAsmElfFileHeader*, std::vector<FunctionPtr>&);
/** @} */

/** Information about the procedure lookup table. */
struct PltInfo {
    SgAsmGenericSection *section;
    size_t firstOffset;                                 /**< Byte offset w.r.t. section for first valid entry. */
    size_t entrySize;                                   /**< Size of each entry in bytes. */

    PltInfo()
        : section(NULL), firstOffset(0), entrySize(0) {}
};

/** Find information about the PLT. */
PltInfo findPlt(const PartitionerConstPtr&, SgAsmGenericSection*, SgAsmElfFileHeader*);

/** Get a list of all ELF sections by name.
 *
 *  Returns an empty list if the interpretation is null or it doesn't have any sections that match the specified name. */
std::vector<SgAsmElfSection*> findSectionsByName(SgAsmInterpretation*, const std::string&);

/** True if the function is an import.
 *
 *  True if the specified function is an import, whether it's actually been linked in or not. This is a weaker version of @ref
 *  isLinkedImport. */
bool isImport(const PartitionerConstPtr&, const FunctionPtr&);

/** True if function is a linked import.
 *
 *  Returns true if the specified function is an import which has been linked to an actual function. This is a stronger version
 *  of @ref isImport. */
bool isLinkedImport(const PartitionerConstPtr&, const FunctionPtr&);

/** True if function is a non-linked import.
 *
 *  Returns true if the specified function is an import function but has not been linked in yet. */
bool isUnlinkedImport(const PartitionerConstPtr&, const FunctionPtr&);

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
/** Boolean flag for @ref Rose::BinaryAnalysis::Partitioner2::ModulesElf::tryLink. */
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
    // These data members are generally optional, and filled in as they're matched.
    Address gotVa_ = 0;                                 // address of global offset table
    Address gotEntryVa_ = 0;                            // address through which an indirect branch branches
    size_t gotEntryNBytes_ = 0;                         // size of the global offset table entry in bytes
    Address gotEntry_ = 0;                              // address read from the GOT if the address is mapped (or zero)
    size_t nBytesMatched_ = 0;                          // number of bytes matched for PLT entry
    Address functionNumber_ = 0;                        // function number argument for the dynamic linker (usually a push)
    Address pltEntryAlignment_ = 1;                     // must PLT entries be aligned, and by how much?

public:
    explicit PltEntryMatcher(Address gotVa)
        : gotVa_(gotVa) {}
    static Ptr instance(Address gotVa) {
        return Ptr(new PltEntryMatcher(gotVa));
    }
    virtual bool match(const PartitionerConstPtr&, Address anchor);

    /** Address of global offset table. */
    Address gotVa() const { return gotVa_; }

    /** Size of the PLT entry in bytes. */
    size_t nBytesMatched() const { return nBytesMatched_; }

    /** Alignment of PLT entries w.r.t. the beginning of the PLT section. */
    Address pltEntryAlignment() const { return pltEntryAlignment_; }

    /** Address of the corresponding GOT entry. */
    Address gotEntryVa() const { return gotEntryVa_; }

    /** Size of the GOT entry in bytes. */
    size_t gotEntryNBytes() const { return gotEntryNBytes_; }
    
    /** Value stored in the GOT entry. */
    Address gotEntry() const { return gotEntry_; }

private:
    SgAsmInstruction* matchNop(const PartitionerConstPtr&, Address va);
    SgAsmInstruction* matchPush(const PartitionerConstPtr&, Address var, Address &n /*out*/);
    SgAsmInstruction* matchDirectJump(const PartitionerConstPtr&, Address va);
    SgAsmInstruction* matchIndirectJump(const PartitionerConstPtr&, Address va, Address &indirectVa /*out*/,
                                        size_t &indirectNBytes /*out*/);
    SgAsmInstruction* matchIndirectJumpEbx(const PartitionerConstPtr&, Address va, Address &offsetFromEbx /*out*/,
                                           size_t &indirectNBytes /*out*/);
    SgAsmInstruction* matchAarch64Adrp(const PartitionerConstPtr&, Address va, Address &value /*out*/);
    SgAsmInstruction* matchAarch64Ldr(const PartitionerConstPtr&, Address va, Address &indirectVa /*in,out*/,
                                      Address &indirectNBytes /*out*/);
    SgAsmInstruction* matchAarch64Add(const PartitionerConstPtr&, Address va);
    SgAsmInstruction* matchAarch64Br(const PartitionerConstPtr&, Address va);
    SgAsmInstruction* matchAarch32CopyPcToIp(const PartitionerConstPtr&, Address va, uint32_t &result);
    SgAsmInstruction* matchAarch32AddConstToIp(const PartitionerConstPtr&, Address va, uint32_t &addend);
    SgAsmInstruction* matchAarch32IndirectBranch(const PartitionerConstPtr&, Address va, uint32_t &addend);
};

/** Build may-return white and black lists. */
void buildMayReturnLists(const PartitionerPtr&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
