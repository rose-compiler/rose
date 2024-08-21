#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

#include <Rose/BinaryAnalysis/Architecture/Amd64.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Architecture/Cil.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium4.h>
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc64.h>
#include <Rose/CommandLine/Parser.h>
#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>

#include <SgAsmGenericHeader.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmInterpretation.h>

#include <Sawyer/Synchronization.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <set>

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Sawyer::Message::Facility mlog;

// The following variables are protected by registryMutex
static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<Base::Ptr> registryList;
static std::vector<Base::Ptr> registryIds;
static boost::once_flag initFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);

    // Order is important -- lookup searches progress from the end toward the beginning.  Therefore, most specific and/or newest
    // architectures should be closer to the end.

    // ARM
#ifdef ROSE_ENABLE_ASM_AARCH32
    registryList.push_back(ArmAarch32::instance(ArmAarch32::InstructionSet::T32));
    registryList.push_back(ArmAarch32::instance(ArmAarch32::InstructionSet::A32));
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
    registryList.push_back(ArmAarch64::instance());
#endif

    // MIPS
    registryList.push_back(Mips32::instance(ByteOrder::ORDER_MSB));
    registryList.push_back(Mips32::instance(ByteOrder::ORDER_LSB));

    // Motorola
    registryList.push_back(Motorola68040::instance());
    registryList.push_back(NxpColdfire::instance());

    // PowerPC
    registryList.push_back(Powerpc32::instance(ByteOrder::ORDER_MSB));
    registryList.push_back(Powerpc32::instance(ByteOrder::ORDER_LSB));
    registryList.push_back(Powerpc64::instance(ByteOrder::ORDER_MSB));
    registryList.push_back(Powerpc64::instance(ByteOrder::ORDER_LSB));

    // x86
    registryList.push_back(Intel8086::instance());
    registryList.push_back(Intel8088::instance());
    registryList.push_back(Intel80286::instance());
    registryList.push_back(IntelI386::instance());
    registryList.push_back(IntelI486::instance());
    registryList.push_back(IntelPentium::instance());
    registryList.push_back(IntelPentiumii::instance());
    registryList.push_back(IntelPentiumiii::instance());
    registryList.push_back(IntelPentium4::instance());
    registryList.push_back(Amd64::instance());

    // Byte code
    registryList.push_back(Cil::instance());
    registryList.push_back(Jvm::instance());

    // Assign IDs to the registered architectures.
    registryIds.resize(registryList.size(), Base::Ptr());
    for (size_t i = 0; i < registryList.size(); ++i) {
        registryList[i]->registrationId(i);
        registryIds[i] = registryList[i];
    }
}

// Initialize the registry with ROSE's built-in architecture definitions.
static void
initRegistry() {
    boost::call_once(&initRegistryHelper, initFlag);
}

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Architecture");
        mlog.comment("defining hardware architectures");
    }
}

size_t
registerDefinition(const Base::Ptr &arch) {
    ASSERT_not_null(arch);
    initRegistry();

    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registryList.push_back(arch);

    // Assign a registration ID
    for (size_t i = 0; i < registryIds.size(); ++i) {
        if (!registryIds[i]) {
            arch->registrationId(i);
            registryIds[i] = arch;
        }
    }
    if (!arch->registrationId()) {
        arch->registrationId(registryIds.size());
        registryIds.push_back(arch);
    }

    ASSERT_require(arch->registrationId());
    return *arch->registrationId();
}

#ifdef __linux__
static std::string
nameVariation(const std::string &name, int variation) {
    switch (variation) {
        case 0:
            return name;
        case 1:
            return name + ".so";
        case 2:
            return "lib" + name;
        case 3:                                     // libfoo.so
            return "lib" + name + ".so";
        default:
            ASSERT_not_reachable("invalid variation");
    }
}

// Returns architecture names, error message, and/or warning message
static std::tuple<std::vector<std::string>, std::string, std::string>
loadSharedLibrary(const std::string &name) {
    const bool isFileName = name.find('/') != std::string::npos;
    std::vector<std::string> newArchNames;

    // Load the library or return an error
    std::string loadedName;
    void *so = nullptr;
    if (isFileName) {
        so = dlopen(name.c_str(), RTLD_LAZY);
        if (!so)
            return {newArchNames, dlerror(), ""};
        loadedName = name;
    } else {
        std::string firstError;
        for (int i = 0; i < 4 && !so; ++i) {
            loadedName = nameVariation(name, i);
            so = dlopen(loadedName.c_str(), RTLD_LAZY);
            if (!so && 0 == i)
                firstError = dlerror();
        }
        if (!so)
            return {newArchNames, firstError, ""};
    }

    // Call the library's main architecture registration function
    const std::set<std::string> oldArchNames = registeredNames();
    ASSERT_not_null(so);
    if (auto func = reinterpret_cast<void(*)()>(dlsym(so, "registerArchitectures"))) {
        func();
    } else {
        dlclose(so);
        return {
            newArchNames,
            "",
            "no \"registerArchitecture\" function in \"" + StringUtility::cEscape(loadedName) + "\""
        };
    }

    // Show what architectures were registered
    const std::set<std::string> allArchNames = registeredNames();
    std::set_difference(allArchNames.begin(), allArchNames.end(), oldArchNames.begin(), oldArchNames.end(),
                        std::inserter(newArchNames, newArchNames.begin()));
    if (newArchNames.empty()) {
        return {
            newArchNames,
            "",
            "library \"" + StringUtility::cEscape(loadedName) + "\" did not define any new architectures\n"
        };
    } else {
        return {newArchNames, "", ""};
    }
}
#endif

void
registerDefinition(const std::string &name) {
#ifdef __linux__
    namespace bfs = boost::filesystem;
    if (bfs::is_directory(name)) {
        for (bfs::directory_iterator dentry(name), end; dentry != end; ++dentry) {
            if (dentry->path().extension() == ".so")
                loadSharedLibrary(dentry->path().string()); // do not emit errors or warnings from directory expansion
        }
    } else {
        auto result = loadSharedLibrary(name);
        if (!std::get<1>(result).empty()) {
            mlog[ERROR] <<std::get<1>(result) <<"\n";
        } else if (!std::get<2>(result).empty()) {
            mlog[WARN] <<std::get<2>(result) <<"\n";
        } else if (!std::get<0>(result).empty()) {
            mlog[INFO] <<"architectures loaded from \"" <<StringUtility::cEscape(name) <<"\"\n";
            for (const std::string &archName: std::get<0>(result))
                mlog[INFO] <<"  " <<archName <<"\n";
        }
    }
#else
    mlog[ERROR] <<"loading architecture definitions from shared objects is not supported on this platform\n";
    mlog[ERROR] <<"registerDefinition name is " << name << "\n";
#endif
}

bool
deregisterDefinition(const Base::Ptr &arch) {
    if (arch && arch->registrationId()) {
        initRegistry();
        SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);

        ASSERT_require(arch->registrationId());
        ASSERT_require(*arch->registrationId() < registryIds.size());
        ASSERT_require(registryIds[*arch->registrationId()] == arch);
        registryIds[*arch->registrationId()] = Base::Ptr();
        arch->registrationId(Sawyer::Nothing());

        for (size_t i = registryList.size(); i > 0; --i) {
            if (registryList[i-1] == arch) {
                registryList.erase(registryList.begin() + (i-1));
                return true;
            }
        }
    }
    return false;
}

std::vector<Base::Ptr>                                  // intentionally not a const ref
registeredDefinitions() {
    initRegistry();
    std::vector<Base::Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registryList.size());
    for (size_t i = registryList.size(); i > 0; --i)
        retval.push_back(registryList[i-1]);
    return retval;
}

std::set<std::string>
registeredNames() {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    std::set<std::string> names;
    for (const Base::Ptr &arch: registryList)
        names.insert(arch->name());
    return names;
}

Sawyer::Result<Base::Ptr, NotFound>
findByName(const std::string &name) {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registryList.rbegin(); iter != registryList.rend(); ++iter) {
        ASSERT_not_null(*iter);
        if ((*iter)->matchesName(name))
            return Sawyer::makeOk(*iter);
    }
    return Sawyer::makeError(NotFound("architecture \"" + StringUtility::cEscape(name) + "\" not found"));
}

Sawyer::Result<Base::Ptr, NotFound>
findById(const size_t id) {
    if (id >= registryIds.size() || !registryIds[id]) {
        return Sawyer::makeError(NotFound("architecture ID " + boost::lexical_cast<std::string>(id) + " not found"));
    } else {
        return Sawyer::makeOk(registryIds[id]);
    }
}

Sawyer::Result<Base::Ptr, NotFound>
findByHeader(SgAsmGenericHeader *header) {
    ASSERT_not_null(header);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registryList.rbegin(); iter != registryList.rend(); ++iter) {
        ASSERT_not_null(*iter);
        if ((*iter)->matchesHeader(header))
            return Sawyer::makeOk(*iter);
    }
    return Sawyer::makeError(NotFound("no architecture found to handle " + std::string(header->formatName()) + " file header"));
}

std::pair<Base::Ptr, size_t>
findBestByInterpretation(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();

    Base::Ptr best;                                     // best matching architecture
    size_t bestN = 0;                                   // number of matches for best architecture

    for (const Base::Ptr &arch: registeredDefinitions()) {
        // How many headers does `arch` match?
        size_t nMatched = 0;
        for (SgAsmGenericHeader *header: headers) {
            if (arch->matchesHeader(header))
                ++nMatched;
        }

        // Return or remember
        if (nMatched == headers.size()) {
            return {arch, nMatched};
        } else if (nMatched > bestN) {
            best = arch;
            bestN = nMatched;
        }
    }

    return std::make_pair(best, bestN);
}

Sawyer::Result<Base::Ptr, NotFound>
findByInterpretation(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    const size_t nHeaders = interp->get_headers()->get_headers().size();
    auto best = findBestByInterpretation(interp);
    if (best.second == nHeaders) {
        ASSERT_not_null(best.first);
        return Sawyer::makeOk(best.first);
    } else if (best.second > 0) {
        return Sawyer::makeError(NotFound("architecture \"" + StringUtility::cEscape(best.first->name()) + "\" matches " +
                                          boost::lexical_cast<std::string>(best.second) + " of " +
                                          StringUtility::plural(nHeaders, "headers")));
    } else {
        return Sawyer::makeError(NotFound("no architecture matches any of the file headers"));
    }
}

const std::string&
name(const Base::ConstPtr &arch) {
    static const std::string empty;
    return arch ? arch->name() : empty;
}

const std::string&
name(const size_t id) {
    return name(findById(id).orDefault());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
newInstructionDispatcher(const std::string &name, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) {
    return findByName(name).orThrow()->newInstructionDispatcher(ops);
}

} // namespace
} // namespace
} // namespace

#endif
