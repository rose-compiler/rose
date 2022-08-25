#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/Disassembler/Mips.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/Diagnostics.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Sawyer::Message::Facility mlog;

void initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Disassembler");
        mlog.comment("decoding machine language instructions");
    }
}

/* Mutex for class-wide operations (such as adjusting Base::disassemblers) */
static boost::mutex class_mutex;

/* List of disassembler subclasses (protect with class_mutex) */
static std::vector<Base*> disassemblers;

/* Initialize the class. Thread safe. */
static void
initclassHelper() {
#ifdef ROSE_ENABLE_ASM_AARCH32
    registerSubclass(Aarch32::instanceA32());
    registerSubclass(Aarch32::instanceT32());
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
    registerSubclass(new Aarch64());
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
    registerSubclass(new Aarch32());
#endif
    registerSubclass(new Disassembler::Powerpc(powerpc_32, ByteOrder::ORDER_MSB));
    registerSubclass(new Disassembler::Powerpc(powerpc_32, ByteOrder::ORDER_LSB));
    registerSubclass(new Disassembler::Powerpc(powerpc_64, ByteOrder::ORDER_MSB));
    registerSubclass(new Disassembler::Powerpc(powerpc_64, ByteOrder::ORDER_LSB));
    registerSubclass(new Disassembler::M68k(m68k_freescale_isab));
    registerSubclass(new Disassembler::Mips());
    registerSubclass(new Disassembler::X86(2)); /*16-bit*/
    registerSubclass(new Disassembler::X86(4)); /*32-bit*/
    registerSubclass(new Disassembler::X86(8)); /*64-bit*/
}

static boost::once_flag initFlag = BOOST_ONCE_INIT;

static void
initclass() {
    boost::call_once(&initclassHelper, initFlag);
}

// Thread safe by virtue of lookup(SgAsmGenericHeader*).
Base *
lookup(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);

    // Find a disassembler for each header in the interpretation
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    if (headers.empty())
        throw Exception("no file headers from which to choose disassembler");
    typedef Sawyer::Container::Map<Base*, size_t> DisassemblerCounts;
    DisassemblerCounts disassemblerCounts;
    for (size_t i=0; i<headers.size(); i++) {
        Base *candidate = NULL;
        try {
            candidate = lookup(headers[i]);
        } catch (const Exception&) {
        }
        ++disassemblerCounts.insertMaybe(candidate, 0);
    }

    // Choose the best disassembler based on how often it matched.
    Base *bestDisassembler = NULL;
    if (disassemblerCounts.size() == 1) {
        bestDisassembler = disassemblerCounts.least();
    } else if (disassemblerCounts.size() > 1) {
        mlog[WARN] <<"ambiguous disassemblers for file headers\n";
        size_t bestCount = 0;
        for (const DisassemblerCounts::Node &node: disassemblerCounts.nodes()) {
            if (Base *disassembler = node.key()) {
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

Base *
lookup(SgAsmGenericHeader *header) {
    initclass();
    Base *retval = NULL;

    boost::lock_guard<boost::mutex> lock(class_mutex);
    for (size_t i=disassemblers.size(); i>0 && !retval; --i) {
        if (disassemblers[i-1]->canDisassemble(header))
            retval = disassemblers[i-1];
    }

    if (retval)
        return retval;
    throw Exception("no disassembler for architecture");
}

Base *
lookup(const std::string &name) {
    Base *retval = NULL;
    if (name == "list") {
        std::cout <<"The following ISAs are supported:\n";
        for (const std::string &name: isaNames())
            std::cout <<"  " <<name <<"\n";
        exit(0);
    } else if (name == "a32") {
#ifdef ROSE_ENABLE_ASM_AARCH32
        retval = Aarch32::instanceA32();
#else
        throw Exception(name + " disassembler is not enabled in this ROSE configuration");
#endif
    } else if (name == "t32") {
#ifdef ROSE_ENABLE_ASM_AARCH32
        retval = Aarch32::instanceT32();
#else
        throw Exception(name + " disassembler is not enabled in this ROSE configuration");
#endif
    } else if (name == "a64") {
#ifdef ROSE_ENABLE_ASM_AARCH64
        retval = new Aarch64();
#else
        throw Exception(name + " disassembler is not enabled in this ROSE configuration");
#endif
    } else if (name == "ppc32-be") {
        retval = new Disassembler::Powerpc(powerpc_32, ByteOrder::ORDER_MSB);
    } else if (name == "ppc32-le") {
        retval = new Disassembler::Powerpc(powerpc_32, ByteOrder::ORDER_LSB);
    } else if (name == "ppc64-be") {
        retval = new Disassembler::Powerpc(powerpc_64, ByteOrder::ORDER_MSB);
    } else if (name == "ppc64-le") {
        retval = new Disassembler::Powerpc(powerpc_64, ByteOrder::ORDER_MSB);
    } else if (name == "mips-be") {
        retval = new Disassembler::Mips(ByteOrder::ORDER_MSB);
    } else if (name == "mips-le") {
        retval = new Disassembler::Mips(ByteOrder::ORDER_LSB);
    } else if (name == "i286") {
        retval = new Disassembler::X86(2);
    } else if (name == "i386") {
        retval = new Disassembler::X86(4);
    } else if (name == "amd64") {
        retval = new Disassembler::X86(8);
    } else if (name == "m68040") {
        retval = new Disassembler::M68k(m68k_68040);
    } else if (name == "coldfire") {
        retval = new Disassembler::M68k(m68k_freescale_emacb);
    } else if (name == "null") {
        retval = new Disassembler::Null;
    } else {
        throw std::runtime_error("invalid ISA name \"" + StringUtility::cEscape(name) + "\"; use --isa=list");
    }
    ASSERT_not_null(retval);
    retval->name(name);
    return retval;
}

std::vector<std::string>
isaNames() {
    std::vector<std::string> v;
    v.push_back("amd64");
#ifdef ROSE_ENABLE_ASM_AARCH32
    v.push_back("a32");                                 // AArch32 A32
    v.push_back("t32");                                 // AArch32 T32
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
    v.push_back("a64");                                 // AArch64 A64
#endif
    v.push_back("coldfire");
    v.push_back("i286");
    v.push_back("i386");
    v.push_back("m68040");
    v.push_back("mips-be");
    v.push_back("mips-le");
    // v.push_back("null"); -- intentionally undocumented
    v.push_back("ppc32-be");
    v.push_back("ppc32-le");
    v.push_back("ppc64-be");
    v.push_back("ppc64-le");
    return v;
}

void
registerSubclass(Base *factory) {
    boost::lock_guard<boost::mutex> lock(class_mutex);
    ASSERT_not_null(factory);
    disassemblers.push_back(factory);
}

const std::string&
name(const Base *disassembler) {
    return disassembler->name();
}

} // namespace
} // namespace
} // namespace
#endif
