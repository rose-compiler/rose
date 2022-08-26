#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/Disassembler/Mips.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/Diagnostics.h>

#include <Sawyer/Synchronization.h>

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

// Mutex for the disasassembler registry
static SAWYER_THREAD_TRAITS::Mutex registryMutex;

// List of registered disassembler factories (protected by registryMutex)
static std::vector<Base::Ptr> registry;

// Initialize the disassembler registry with ROSE's built-in disassembler factories.
static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(Null::instance());
#ifdef ROSE_ENABLE_ASM_AARCH32
    registry.push_back(Aarch32::instanceA32());
    registry.push_back(Aarch32::instanceT32());
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
    registry.push_back(Aarch64::instance());
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
    registry.push_back(Aarch32::instance());
#endif
    registry.push_back(Disassembler::Powerpc::instance(powerpc_32, ByteOrder::ORDER_MSB));
    registry.push_back(Disassembler::Powerpc::instance(powerpc_32, ByteOrder::ORDER_LSB));
    registry.push_back(Disassembler::Powerpc::instance(powerpc_64, ByteOrder::ORDER_MSB));
    registry.push_back(Disassembler::Powerpc::instance(powerpc_64, ByteOrder::ORDER_LSB));
    registry.push_back(Disassembler::M68k::instance(m68k_freescale_isab));
    registry.push_back(Disassembler::Mips::instance());
    registry.push_back(Disassembler::X86::instance(2)); /*16-bit*/
    registry.push_back(Disassembler::X86::instance(4)); /*32-bit*/
    registry.push_back(Disassembler::X86::instance(8)); /*64-bit*/
}

static boost::once_flag initFlag = BOOST_ONCE_INIT;

static void
initRegistry() {
    boost::call_once(&initRegistryHelper, initFlag);
}

// Thread safe by virtue of lookup(SgAsmGenericHeader*).
Base::Ptr
lookup(SgAsmInterpretation *interp) {
    initRegistry();
    ASSERT_not_null(interp);

    // Find a disassembler for each header in the interpretation
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    if (headers.empty())
        throw Exception("no file headers from which to choose disassembler");
    typedef Sawyer::Container::Map<Base::Ptr, size_t> DisassemblerCounts;
    DisassemblerCounts disassemblerCounts;
    for (size_t i = 0; i < headers.size(); ++i) {
        Base::Ptr candidate;
        try {
            candidate = lookup(headers[i]);
        } catch (const Exception&) {
        }
        ++disassemblerCounts.insertMaybe(candidate, 0);
    }

    // Choose the best disassembler based on how often it matched.
    Base::Ptr bestDisassembler;
    if (disassemblerCounts.size() == 1) {
        bestDisassembler = disassemblerCounts.least();
    } else if (disassemblerCounts.size() > 1) {
        mlog[WARN] <<"ambiguous disassemblers for file headers\n";
        size_t bestCount = 0;
        for (const DisassemblerCounts::Node &node: disassemblerCounts.nodes()) {
            if (Base::Ptr disassembler = node.key()) {
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

Base::Ptr
lookup(SgAsmGenericHeader *header) {
    initRegistry();

    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
        if ((*factory)->canDisassemble(header))
            return *factory;
    }
    throw Exception("no disassembler for architecture");
}

Base::Ptr
lookup(const std::string &name) {
    initRegistry();
    if (name == "list") {
        std::cout <<"The following ISAs are supported:\n";
        for (const std::string &name: isaNames())
            std::cout <<"  " <<name <<"\n";
        exit(0);
    } else {
        SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
        for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
            if (name == (*factory)->name())
                return *factory;
        }
        throw Exception("invalid ISA name \"" + StringUtility::cEscape(name) + "\"; use \"list\" to get a list of valid names");
    }
}

std::vector<std::string>
isaNames() {
    initRegistry();
    std::vector<std::string> names;
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
        for (const Base::Ptr &factory: registry)
            names.push_back(factory->name());
    }

    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}

void
registerFactory(const Base::Ptr &factory) {
    initRegistry();
    ASSERT_not_null(factory);
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(factory);
}

bool
deregisterFactory(const Base::Ptr &factory) {
    ASSERT_not_null(factory);
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        if (*iter == factory) {
            registry.erase(std::next(iter).base());
            return true;
        }
    }
    return false;
}

std::vector<Base::Ptr>
registeredFactories() {
    std::vector<Base::Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registry.size());
    for (const Base::Ptr &factory: registry)
        retval.push_back(factory);
    return retval;
}

const std::string&
name(const Base::Ptr &disassembler) {
    return disassembler->name();
}

} // namespace
} // namespace
} // namespace
#endif
