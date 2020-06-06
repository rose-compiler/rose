#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <Disassembler.h>

#include <Assembler.h>
#include <AssemblerX86.h>
#include <AsmUnparser_compat.h>
#include <Diagnostics.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerArm.h>
#include <DisassemblerM68k.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <BinaryLoader.h>
#include <stringify.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <stdarg.h>

namespace Rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace StringUtility;

/* Mutex for class-wide operations (such as adjusting Disassembler::disassemblers) */
static boost::mutex class_mutex;

/* List of disassembler subclasses (protect with class_mutex) */
std::vector<Disassembler*> Disassembler::disassemblers;

/* Diagnostics */
Sawyer::Message::Facility Disassembler::mlog;

void Disassembler::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Disassembler");
        mlog.comment("decoding machine language instructions");
    }
}

void
Disassembler::Exception::print(std::ostream &o) const
{
    if (insn) {
        o <<"disassembly failed at " <<addrToString(ip)
          <<" [" <<unparseInstruction(insn) <<"]"
          <<": " <<what();
    } else if (ip>0) {
        o <<"disassembly failed at " <<addrToString(ip);
        if (!bytes.empty()) {
            for (size_t i=0; i<bytes.size(); i++) {
                o <<(i>0?", ":"[")
                  <<std::hex <<std::setfill('0') <<std::setw(2)
                  <<"0x" <<bytes[i]
                  <<std::dec <<std::setfill(' ') <<std::setw(1);
            }
            o <<"] at bit " <<bit;
        }
    } else {
        o <<what();
    }
}

std::ostream &
operator<<(std::ostream &o, const Disassembler::Exception &e)
{
    e.print(o);
    return o;
}

/* Initialize the class. Thread safe. */
void
Disassembler::initclassHelper()
{
#ifdef ROSE_ENABLE_ASM_A64
    registerSubclass(new DisassemblerArm(DisassemblerArm::ARCH_ARM64));
#endif
    registerSubclass(new DisassemblerPowerpc(powerpc_32, ByteOrder::ORDER_MSB));
    registerSubclass(new DisassemblerPowerpc(powerpc_32, ByteOrder::ORDER_LSB));
    registerSubclass(new DisassemblerPowerpc(powerpc_64, ByteOrder::ORDER_MSB));
    registerSubclass(new DisassemblerPowerpc(powerpc_64, ByteOrder::ORDER_LSB));
    registerSubclass(new DisassemblerM68k(m68k_freescale_isab));
    registerSubclass(new DisassemblerMips());
    registerSubclass(new DisassemblerX86(2)); /*16-bit*/
    registerSubclass(new DisassemblerX86(4)); /*32-bit*/
    registerSubclass(new DisassemblerX86(8)); /*64-bit*/
}

static boost::once_flag initFlag = BOOST_ONCE_INIT;

void
Disassembler::initclass() {
    boost::call_once(&initclassHelper, initFlag);
}

/* Class method to register a new disassembler subclass. Thread safe. */
void
Disassembler::registerSubclass(Disassembler *factory)
{
    boost::lock_guard<boost::mutex> lock(class_mutex);
    ASSERT_not_null(factory);
    disassemblers.push_back(factory);
}

/* Class method. Thread safe by virtue of lookup(SgAsmGenericHeader*). */
Disassembler *
Disassembler::lookup(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);

    // Find a disassembler for each header in the interpretation
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    if (headers.empty())
        throw Exception("no file headers from which to choose disassembler");
    typedef Sawyer::Container::Map<Disassembler*, size_t> DisassemblerCounts;
    DisassemblerCounts disassemblerCounts;
    for (size_t i=0; i<headers.size(); i++) {
        Disassembler *candidate = NULL;
        try {
            candidate = lookup(headers[i]);
        } catch (const Disassembler::Exception&) {
        }
        ++disassemblerCounts.insertMaybe(candidate, 0);
    }

    // Choose the best disassembler based on how often it matched.
    Disassembler *bestDisassembler = NULL;
    if (disassemblerCounts.size() == 1) {
        bestDisassembler = disassemblerCounts.least();
    } else if (disassemblerCounts.size() > 1) {
        mlog[WARN] <<"ambiguous disassemblers for file headers\n";
        size_t bestCount = 0;
        BOOST_FOREACH (const DisassemblerCounts::Node &node, disassemblerCounts.nodes()) {
            if (Disassembler *disassembler = node.key()) {
                mlog[WARN] <<"  " <<StringUtility::plural(node.value(), "file headers")
                            <<" using " <<disassembler->name() <<" disassember\n";
            } else {
                mlog[WARN] <<"  " <<StringUtility::plural(node.value(), "file headers")
                            <<" using no disassembler\n";
            }
            if (node.value() > bestCount && node.key() != NULL) {
                bestCount = node.value();
                bestDisassembler = node.key();
                mlog[WARN] <<"  selected " <<bestDisassembler->name() <<" disassembler\n";
            }
        }
    }

    if (!bestDisassembler)
        throw Exception("no disassembler for architecture");
    return bestDisassembler;
}

/* Class method. Thread safe. */
Disassembler *
Disassembler::lookup(SgAsmGenericHeader *header)
{
    initclass();
    Disassembler *retval = NULL;

    boost::lock_guard<boost::mutex> lock(class_mutex);
    for (size_t i=disassemblers.size(); i>0 && !retval; --i) {
        if (disassemblers[i-1]->canDisassemble(header))
            retval = disassemblers[i-1];
    }
    
    if (retval)
        return retval;
    throw Exception("no disassembler for architecture");
}

// Class method
std::vector<std::string>
Disassembler::isaNames() {
    std::vector<std::string> v;
    v.push_back("amd64");
#ifdef ROSE_ENABLE_ASM_A64
    v.push_back("a64");         // ARM AArch64 A64
#endif
    v.push_back("coldfire");
    v.push_back("i386");
    v.push_back("m68040");
    v.push_back("mips-be");
    v.push_back("mips-le");
    v.push_back("ppc32-be");
    v.push_back("ppc32-le");
    v.push_back("ppc64-be");
    v.push_back("ppc64-le");
    return v;
}

// Class method
Disassembler *
Disassembler::lookup(const std::string &name)
{
    Disassembler *retval = NULL;
    if (name == "list") {
        std::cout <<"The following ISAs are supported:\n";
        BOOST_FOREACH (const std::string &name, isaNames())
            std::cout <<"  " <<name <<"\n";
        exit(0);
    } else if (name == "a64") {
#ifdef ROSE_ENABLE_ASM_A64
        retval = new DisassemblerArm(DisassemblerArm::ARCH_ARM64);
#else
        throw Exception(name + " disassembler is not enabled in this ROSE configuration");
#endif
    } else if (name == "ppc32-be") {
        retval = new DisassemblerPowerpc(powerpc_32, ByteOrder::ORDER_MSB);
    } else if (name == "ppc32-le") {
        retval = new DisassemblerPowerpc(powerpc_32, ByteOrder::ORDER_LSB);
    } else if (name == "ppc64-be") {
        retval = new DisassemblerPowerpc(powerpc_64, ByteOrder::ORDER_MSB);
    } else if (name == "ppc64-le") {
        retval = new DisassemblerPowerpc(powerpc_64, ByteOrder::ORDER_MSB);
    } else if (name == "mips-be") {
        retval = new DisassemblerMips(ByteOrder::ORDER_MSB);
    } else if (name == "mips-le") {
        retval = new DisassemblerMips(ByteOrder::ORDER_LSB);
    } else if (name == "i386") {
        retval = new DisassemblerX86(4);
    } else if (name == "amd64") {
        retval = new DisassemblerX86(8);
    } else if (name == "m68040") {
        retval = new DisassemblerM68k(m68k_68040);
    } else if (name == "coldfire") {
        retval = new DisassemblerM68k(m68k_freescale_emacb);
    } else {
        throw std::runtime_error("invalid ISA name \"" + StringUtility::cEscape(name) + "\"; use --isa=list");
    }
    ASSERT_not_null(retval);
    retval->name(name);
    return retval;
}

/* Disassemble one instruction. */
SgAsmInstruction *
Disassembler::disassembleOne(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                             AddressSet *successors)
{
    MemoryMap::Ptr map = MemoryMap::instance();
    map->insert(AddressInterval::baseSize(buf_va, buf_size),
                MemoryMap::Segment::staticInstance(buf, buf_size, MemoryMap::READABLE|MemoryMap::EXECUTABLE,
                                                   "disassembleOne temp"));
    return disassembleOne(map, start_va, successors);
}

SgAsmInstruction *
Disassembler::find_instruction_containing(const InstructionMap &insns, rose_addr_t va)
{
    const size_t max_insns_size = 16;
    InstructionMap::const_iterator ii=insns.upper_bound(va);
    if (ii==insns.begin())
        return NULL;
    while (1) {
        --ii;
        ASSERT_require(ii->first <= va);
        if (ii->first + max_insns_size < va)
            return NULL;
        if (ii->first + ii->second->get_size() > va)
            return ii->second;
        if (ii==insns.begin())
            return NULL;
    }
}

/* Re-read instruction bytes from file if necessary in order to mark them as referenced. */
void
Disassembler::mark_referenced_instructions(SgAsmInterpretation *interp, const MemoryMap::Ptr &map, const InstructionMap &insns)
{
    unsigned char buf[32];
    SgAsmGenericFile *file = NULL;
    const SgAsmGenericFilePtrList &files = interp->get_files();
    bool was_tracking = false; // only valid when file!=NULL  (value here is to shut of used-before-defined warnings from GCC)
    MemoryMap::ConstNodeIterator si = map->nodes().end();

    /* Re-read each instruction so the file has a chance to track the reference. */
    try {
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmInstruction *insn = ii->second;
            ASSERT_require(insn->get_size()<=sizeof buf);
            rose_addr_t va = insn->get_address();
            size_t nbytes = insn->get_size();

            while (nbytes>0) {
                /* Find the memory map segment and the file that goes with that segment (if any) */
                if (si==map->nodes().end() || !si->key().isContaining(va)) {
                    if (file) {
                        file->set_tracking_references(was_tracking);
                        file = NULL;
                    }
                    si = map->at(va).findNode();
                    if (si==map->nodes().end()) {
                        /* This byte of the instruction is not mapped. Perhaps the next one is. */
                        ++va;
                        --nbytes;
                        continue;
                    }

                    /* Find the file that goes with this segment. */
                    for (size_t i=0; i<files.size(); i++) {
                        if (&(files[i]->get_data()[0]) == si->value().buffer()->data()) {
                            file = files[i];
                            was_tracking = file->get_tracking_references();
                            file->set_tracking_references(true);
                            break;
                        }
                    }
                }

                /* Read the file for its reference tracking side effect. */
                size_t sgmt_offset = va - si->key().least();
                size_t n = std::min(nbytes, (size_t)si->key().size()-sgmt_offset);
                if (file) {
                    size_t file_offset = si->value().offset() + sgmt_offset;
                    file->read_content(file_offset, buf, n, false);
                }
                nbytes -= n;
                va += n;
            }
        }
        if (file)
            file->set_tracking_references(was_tracking);
    } catch(...) {
        if (file)
            file->set_tracking_references(was_tracking);
        throw;
    }
}

/* Add last instruction's successors to returned successors. */
Disassembler::AddressSet
Disassembler::get_block_successors(const InstructionMap& insns, bool *complete)
{
    std::vector<SgAsmInstruction*> block;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        block.push_back(ii->second);
    Disassembler::AddressSet successors = block.front()->getSuccessors(block, complete);

    /* For the purposes of disassembly, assume that a CALL instruction eventually executes a RET that causes execution to
     * resume at the address following the CALL. This is true 99% of the time.  Higher software layers (e.g., Partitioner) may
     * make other assumptions, which is why this code is not in SgAsmX86Instruction::getSuccessors(). [RPM 2010-05-09] */
    rose_addr_t target, return_va;
    SgAsmInstruction *last_insn = block.back();
    if (last_insn->isFunctionCallSlow(block, &target, &return_va))
        successors.insert(return_va);

    return successors;
}

} // namespace
} // namespace

#endif
