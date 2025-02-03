#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

// This file implements the top-level serial I/O API in ROSE, but also serves as a place to put serialization functions that have
// more dependencies than we want to include into header files. For instance, if class `T` has a data member that's a smart pointer
// to type `P` then header file "T.h" only needs a forward delcaraton for `P`, but `T::serialize` needs the definition of `P`.
// Declarations are cheap (`class P;`) but definitions are expensive (`#include <P.h>`). And since these are occuring in a header
// file ("T.h") they get multiplied by the number of translation units that directly or indirectly include the header. Therefore,
// when it's possible to eliminate P's definition from T.h by moving `T::serialize` to this file (SerialIo.C) we do so.
//
// In other words, here's what we're trying to accomplish. The old code:
//
//    +-------------------------------------------------------------------------------------------
//    |// T.h
//    |#include <Rose/BasicTypes.h>                     // forward decls for most common things
//    |#include <P.h>                                   // expensive definition for `P`
//    |#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
//    |#include <boost/serialization/access.hpp>
//    |#include <boost/serialization/nvp.hpp>
//    |#endif
//    |
//    |class T {
//    |    PPtr p_;                                     // smart pointer to `P`
//    |
//    |#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
//    |private:
//    |    friend class boost::serialization::access;
//    |
//    |    template<class S>
//    |    void serialize(S &s, const unsigned /*version*/) {
//    |        s & BOOST_SERIALIZATION_NVP(p_);         // needs definition of `P`
//    |    }
//    |#endif
//    |
//    |    ....
//    |};
//    +-------------------------------------------------------------------------------------------
//
// We'd rather do this in order to elimiate the `#include <P.h>` from "T.h":
//
//    +-------------------------------------------------------------------------------------------
//    |// T.h
//    |#include <Rose/BasicTypes.h>                     // forward decls for most common things
//    |#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
//    |#include <boost/serialization/access.hpp>
//    |#endif
//    |
//    |class P;                                         // cheap declaration of `P`
//    |
//    |class T {
//    |    PPtr p_;                                     // smart pointer to `P`
//    |
//    |#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
//    |private:
//    |    friend class boost::serialization::access;
//    |
//    |    template<class S>
//    |    void serialize(S &s, const unsigned /*version*/);
//    |#endif
//    |
//    |    T();                                         // move implementation to T.C
//    |    ~T();                                        // move implementation to T.C
//    |    ....
//    |};
//    +-------------------------------------------------------------------------------------------
//
//    +-------------------------------------------------------------------------------------------
//    |// SerialIo.C
//    |
//    |#include <P.h>                                   // expensive definition for `P`
//    |
//    |template<class S>
//    |void T::serialize(S &s, const unsigned /*version*/) {
//    |    s & BOOST_SERIALIZATION_NVP(p_);             // needs definition of `P`
//    |}
//    |
//    |ROSE_SERIALIZATION_INSTANTIATE(T);
//    +-------------------------------------------------------------------------------------------

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionProvider.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch64.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/AddressUsageMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Semantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/SplitJoin.h>
#include <AstSerialization.h>                           // rose

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <fcntl.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#endif

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SerialIo_initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&SerialIo::mlog, "Rose::BinaryAnalysis::SerialIo");
        SerialIo::mlog.comment("reading/writing serialized analysis states");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialIo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility SerialIo::mlog;

void
SerialIo::init() {}

SerialIo::~SerialIo() {}

SerialIo::Savable
SerialIo::userSavable(unsigned offset) {
    unsigned retval = USER_DEFINED + offset;
    ASSERT_require(retval >= USER_DEFINED && retval <= USER_DEFINED_LAST);
    return (Savable)retval;
}

SerialIo::Format
SerialIo::format() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return format_;
}

void
SerialIo::format(Format fmt) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (fmt != format_) {
        if (isOpen_)
            throw Exception("cannot change format while file is attached");
        format_ = fmt;
    }
}

Progress::Ptr
SerialIo::progress() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return progress_;
}

void
SerialIo::progress(const Progress::Ptr &p) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    progress_ = p;
}

bool
SerialIo::isOpen() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return isOpen_;
}

void
SerialIo::setIsOpen(bool b) {
    // Locking is only to make isOpen thread-safe
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    isOpen_ = b;
}

SerialIo::Savable
SerialIo::objectType() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return objectType_;
}

void
SerialIo::objectType(Savable t) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    objectType_ = t;
}

void
SerialIo::close() {
    if (isOpen()) {
        if (::close(fd_) == -1 && EIO == errno)
            throw Exception("I/O error when closing file");
        fd_ = -1;
        setIsOpen(false);
        if (Progress::Ptr p = progress())
            p->update(1.0);
        progressBar_.value(progressBar_.domain().second);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialOutput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SerialOutput::~SerialOutput() {
    try {
        close();
    } catch (...) {
    }
}

void
SerialOutput::open(const boost::filesystem::path &fileName) {
    if (isOpen())
        close();

#ifndef ROSE_ENABLE_BOOST_SERIALIZATION
    ROSE_UNUSED(fileName);
    throw Exception("binary state files are not supported in this configuration");
#else
    objectType(ERROR); // in case of exception

    // Open, create, or truncate the output file
    if (fileName == "-") {
        fd_ = 1; // standard output on Unix-like systems
    } else if ((fd_ = ::open(fileName.string().c_str(), O_RDWR|O_TRUNC|O_CREAT, 0666)) == -1) {
        throw Exception("cannot create or truncate file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }

    // Wrap the file descriptor in an std::ostream interface and then a boost::archive.
    try {
        device_.open(fd_, boost::iostreams::never_close_handle);
        file_.open(device_);
        if (!file_.is_open())
            throw Exception("failed to open boost stream for file \"" + StringUtility::cEscape(fileName.string()) + "\"");

        switch (format()) {
            case BINARY:
                binary_archive_ = new boost::archive::binary_oarchive(file_);
                break;
            case TEXT:
                text_archive_ = new boost::archive::text_oarchive(file_);
                break;
            case XML:
                xml_archive_ = new boost::archive::xml_oarchive(file_);
                break;
        }

        if (Progress::Ptr p = progress())
            p->update(Progress::Report("saving", 0.0));
        progressBar_.value(0, 0, 0);

        setIsOpen(true);
        objectType(NO_OBJECT);
    } catch (const Exception &e) {
        throw;
    } catch (...) {
        throw Exception("failed to open for writing: file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }
#endif
}

void
SerialOutput::savePartitioner(const Partitioner2::Partitioner::ConstPtr &partitioner) {
    const Partitioner2::Partitioner *raw = partitioner.getRawPointer();
    saveObject(PARTITIONER, raw);
}

void
SerialOutput::saveAstHelper(SgNode *ast) {
    if (ast) {
        SgNode *oldParent = ast->get_parent();
        try {
            ast->set_parent(NULL);
            saveObject(AST, ast);
            ast->set_parent(oldParent);
        } catch (...) {
            ast->set_parent(oldParent);
            throw;
        }
    } else {
        saveObject(AST, ast);
    }
}

void
SerialOutput::saveAst(SgAsmNode *ast) {
    saveAstHelper(ast);
}

void
SerialOutput::saveAst(SgBinaryComposite *ast) {
    saveAstHelper(ast);
}

void
SerialOutput::close() {
    if (isOpen() && objectType() != END_OF_DATA && objectType() != ERROR) {
#ifndef ROSE_ENABLE_BOOST_SERIALIZATION
        throw Exception("binary state files are not supported in this configuration");
#else
        Savable endMarker = END_OF_DATA;
        switch (format()) {
            case BINARY:
                *binary_archive_ <<BOOST_SERIALIZATION_NVP(endMarker);
                delete binary_archive_;
                binary_archive_ = NULL;
                break;
            case TEXT:
                *text_archive_ <<BOOST_SERIALIZATION_NVP(endMarker);
                delete text_archive_;
                text_archive_ = NULL;
                break;
            case XML:
                *xml_archive_ <<BOOST_SERIALIZATION_NVP(endMarker);
                delete xml_archive_;
                xml_archive_ = NULL;
                break;
        }
        file_.close();
#endif
        SerialIo::close();
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialInput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SerialInput::~SerialInput() {
    try {
        close();
    } catch (...) {
    }
}

void
SerialInput::open(const boost::filesystem::path &fileName) {
    if (isOpen())
        close();

#ifndef ROSE_ENABLE_BOOST_SERIALIZATION
    ROSE_UNUSED(fileName);
    throw Exception("binary state files are not supported in this configuration");
#else
    objectType(ERROR); // in case of exception

    // Open low-level file for read-only
    if (fileName == "-") {
        fd_ = 0; // standard input on Unix-like systems
    } else if ((fd_ = ::open(fileName.string().c_str(), O_RDONLY)) == -1) {
        throw Exception("cannot open for reading file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }

    // File size is for progress reporting, so it's okay if we don't have a size
    struct stat sb;
    if (fstat(fd_, &sb) != -1)
        fileSize_ = sb.st_size;

    // Wrap the file descriptor in an std::ostream interface and then a boost::archive.
    try {
        device_.open(fd_, boost::iostreams::never_close_handle);
        file_.open(device_);
        if (!file_.is_open())
            throw Exception("failed to open boost stream for file \"" + StringUtility::cEscape(fileName.string()) + "\"");

        switch (format()) {
            case BINARY:
                binary_archive_ = new boost::archive::binary_iarchive(file_);
                break;
            case TEXT:
                text_archive_ = new boost::archive::text_iarchive(file_);
                break;
            case XML:
                xml_archive_ = new boost::archive::xml_iarchive(file_);
                break;
        }

        if (Progress::Ptr p = progress())
            p->update(Progress::Report("loading", 0.0));
        progressBar_.value(0, 0, fileSize_);

        setIsOpen(true);
        advanceObjectType();
    } catch (const Exception &e) {
        throw;
    } catch (...) {
        throw Exception("failed to open for reading: file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }
#endif
}

void
SerialInput::advanceObjectType() {
    ASSERT_require(isOpen());
    Savable typeId = NO_OBJECT;
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    switch (format()) {
        case BINARY:
            *binary_archive_ >>typeId;
            break;
        case TEXT:
            *text_archive_ >>typeId;
            break;
        case XML:
            *xml_archive_ >>BOOST_SERIALIZATION_NVP(typeId);
            break;
    }
#endif
    objectType(typeId);
}

Partitioner2::Partitioner::Ptr
SerialInput::loadPartitioner() {
    Partitioner2::Partitioner *raw = nullptr;
    loadObject(PARTITIONER, raw);
    return Partitioner2::Partitioner::Ptr(raw);
}

SgNode*
SerialInput::loadAst() {
    return loadObject<SgNode*>(AST);
}

void
SerialInput::close() {
    if (isOpen()) {
#ifndef ROSE_ENABLE_BOOST_SERIALIZATION
        throw Exception("binary state files are not supported in this configuration");
#else
        switch (format()) {
            case BINARY:
                delete binary_archive_;
                binary_archive_ = NULL;
                break;
            case TEXT:
                delete text_archive_;
                text_archive_ = NULL;
                break;
            case XML:
                delete xml_archive_;
                xml_archive_ = NULL;
                break;
        }

        file_.close();
        fileSize_ = 0;
#endif
        SerialIo::close();
    }
}

void
SerialInput::checkCompatibility(const std::string &fileVersion) {
    const std::string roseVersion = ROSE_PACKAGE_VERSION;
    if (roseVersion == fileVersion)
        return;

    std::vector<std::string> fileParts = Rose::StringUtility::split('.', fileVersion);
    std::vector<std::string> roseParts = Rose::StringUtility::split('.', roseVersion);

    // ROSE uses a dotted quad for the version number, as in W.X.Y.Z where W is zero, X is the major version, Y is the minor
    // version, and Z is the patch version. Backward compatibility is ensured when W and X are the same for the file and the
    // ROSE library and ROSE's Y.Z is greater than or equal to the file's Y.Z.
    //
    // Examples:
    //       File Version           ROSE library version   ROSE library can read the file?
    //       0.11.87.0              0.11.87.0              Yes
    //       0.11.87.0              0.11.87.1              Yes
    //       0.11.87.0              0.11.88.0              Yes
    //       0.11.87.0              0.12.0.0               No
    //       0.11.87.0              1.0.0.0                No
    //
    ASSERT_require2(roseParts.size() == 4, roseVersion);
    if (fileParts.size() != 4)
        throw Exception("invalid file version string \"" + StringUtility::cEscape(fileVersion) + "\"");

    if (fileParts[0] == roseParts[0] &&
        fileParts[1] == roseParts[1] &&
        (fileParts[2] <= roseParts[2] ||
         (fileParts[2] == roseParts[2] && fileParts[3] <= roseParts[3]))) {
        if (fileVersion != roseVersion)
            mlog[WARN] <<"RBA file version " <<fileVersion <<" is being read by ROSE version " <<roseVersion <<"\n";
    } else {
        throw Exception("ROSE library " + roseVersion + " cannot read file version " + fileVersion);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros to help instantiate serialization functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ROSE_SERIALIZATION_INSTANTIATE(CLASS)                                                                                  \
    template void CLASS::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, unsigned);               \
    template void CLASS::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, unsigned);               \
    template void CLASS::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, unsigned);                   \
    template void CLASS::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, unsigned);                   \
    template void CLASS::serialize<boost::archive::xml_oarchive>(boost::archive::xml_oarchive&, unsigned);                     \
    template void CLASS::serialize<boost::archive::xml_iarchive>(boost::archive::xml_iarchive&, unsigned);

#define ROSE_SERIALIZATION_INSTANTIATE_SAVE(CLASS)                                                                             \
    template void CLASS::save<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, unsigned) const;              \
    template void CLASS::save<boost::archive::text_oarchive>(boost::archive::text_oarchive&, unsigned) const;                  \
    template void CLASS::save<boost::archive::xml_oarchive>(boost::archive::xml_oarchive&, unsigned) const;

#define ROSE_SERIALIZATION_INSTANTIATE_LOAD(CLASS)                                                                             \
    template void CLASS::load<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, unsigned);                    \
    template void CLASS::load<boost::archive::text_iarchive>(boost::archive::text_iarchive&, unsigned);                        \
    template void CLASS::load<boost::archive::xml_iarchive>(boost::archive::xml_iarchive&, unsigned);

#define ROSE_SERIALIZATION_INSTANTIATE_SPLIT(CLASS)                                                                            \
    ROSE_SERIALIZATION_INSTANTIATE_SAVE(CLASS)                                                                                 \
    ROSE_SERIALIZATION_INSTANTIATE_LOAD(CLASS)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::CallingConvention::Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace CallingConvention {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void Definition::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(name_);
    s & BOOST_SERIALIZATION_NVP(comment_);
    s & BOOST_SERIALIZATION_NVP(bitsPerWord_);
    s & BOOST_SERIALIZATION_NVP(nonParameterInputs_);
    s & BOOST_SERIALIZATION_NVP(inputParameters_);
    s & BOOST_SERIALIZATION_NVP(outputParameters_);
    s & BOOST_SERIALIZATION_NVP(stackParameterOrder_);
    s & BOOST_SERIALIZATION_NVP(stackPointerRegister_);
    s & BOOST_SERIALIZATION_NVP(nonParameterStackSize_);
    s & BOOST_SERIALIZATION_NVP(stackAlignment_);
    s & BOOST_SERIALIZATION_NVP(stackDirection_);
    s & BOOST_SERIALIZATION_NVP(stackCleanup_);
    s & BOOST_SERIALIZATION_NVP(thisParameter_);
    s & BOOST_SERIALIZATION_NVP(calleeSavedRegisters_);
    s & BOOST_SERIALIZATION_NVP(scratchRegisters_);
    s & BOOST_SERIALIZATION_NVP(returnAddressLocation_);
    s & BOOST_SERIALIZATION_NVP(instructionPointerRegister_);

    if (auto arch = architecture_.lock()) {
        std::string archName = arch->name();
        s & BOOST_SERIALIZATION_NVP(archName);
    } else {
        std::string archName;
        s & BOOST_SERIALIZATION_NVP(archName);
        architecture_ = Architecture::findByName(archName).orThrow();
    }
}

ROSE_SERIALIZATION_INSTANTIATE(Definition);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::ConcreteLocation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
ConcreteLocation::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(reg_);
    s & BOOST_SERIALIZATION_NVP(va_);
    s & BOOST_SERIALIZATION_NVP(regdict_);
}

ROSE_SERIALIZATION_INSTANTIATE(ConcreteLocation);
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::InstructionProvider
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
InstructionProvider::save(S &s, const unsigned /*version*/) const {
    roseAstSerializationRegistration(s);            // so we can save instructions through SgAsmInstruction base ptrs
    s <<BOOST_SERIALIZATION_NVP(memMap_);

    const size_t mapSize = insnMap_.size();
    s <<BOOST_SERIALIZATION_NVP(mapSize);
    for (const auto &insn: insnMap_.values())
        saveAst(s, insn);

    ASSERT_not_null(architecture_);
    const std::string architecture = architecture_->name();
    s <<BOOST_SERIALIZATION_NVP(architecture);
}

template<class S>
void
InstructionProvider::load(S &s, const unsigned version) {
    ASSERT_always_require(version >= 2);
    roseAstSerializationRegistration(s);
    s >>BOOST_SERIALIZATION_NVP(memMap_);

    size_t mapSize;
    s >>BOOST_SERIALIZATION_NVP(mapSize);
    for (size_t i = 0; i < mapSize; ++i) {
        SgNode *node = restoreAst(s);
        auto insn = isSgAsmInstruction(node);
        ASSERT_require(!node || insn);
        insnMap_.insert(insn->get_address(), insn);
    }

    std::string architecture;
    s >>BOOST_SERIALIZATION_NVP(architecture);
    architecture_ = Architecture::findByName(architecture).orThrow();

    if (memMap_) {
        disassembler_ = architecture_->newInstructionDecoder();
        ASSERT_not_null(disassembler_);
    }
}

ROSE_SERIALIZATION_INSTANTIATE_SPLIT(InstructionProvider);
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
AddressUsageMap::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(map_);
}

ROSE_SERIALIZATION_INSTANTIATE(AddressUsageMap);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
AddressUser::serialize(S &s, const unsigned version) {
    transferAst(s, insn_);
    s & BOOST_SERIALIZATION_NVP(bblocks_);
    if (version < 1) {
        ASSERT_not_reachable("Rose::BinaryAnalysis::Partitioner2::AddressUser version 0 no longer supported");
    } else {
        s & BOOST_SERIALIZATION_NVP(dblock_);
    }
}

ROSE_SERIALIZATION_INSTANTIATE(AddressUser);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::AddressUsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
AddressUsers::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(users_);
}

ROSE_SERIALIZATION_INSTANTIATE(AddressUsers);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::AstConstructionSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
AstConstructionSettings::serialize(S &s, unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(allowEmptyGlobalBlock);
    s & BOOST_SERIALIZATION_NVP(allowFunctionWithNoBasicBlocks);
    s & BOOST_SERIALIZATION_NVP(allowEmptyBasicBlocks);
    s & BOOST_SERIALIZATION_NVP(copyAllInstructions);
}

ROSE_SERIALIZATION_INSTANTIATE(AstConstructionSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::BasePartitionerSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
BasePartitionerSettings::serialize(S &s, const unsigned version) {
    s & BOOST_SERIALIZATION_NVP(usingSemantics);
    s & BOOST_SERIALIZATION_NVP(checkingCallBranch);
    s & BOOST_SERIALIZATION_NVP(basicBlockSemanticsAutoDrop);
    if (version >= 1)
        s & BOOST_SERIALIZATION_NVP(ignoringUnknownInsns);
}

ROSE_SERIALIZATION_INSTANTIATE(BasePartitionerSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
BasicBlock::serializeCommon(S &s, const unsigned version) {
    //s & boost::serialization::base_object<Sawyer::Attribute::Storage<> >(*this); -- not saved
    s & BOOST_SERIALIZATION_NVP(isFrozen_);
    s & BOOST_SERIALIZATION_NVP(startVa_);
    s & BOOST_SERIALIZATION_NVP(comment_);

    size_t nInsns = insns_.size();                  // zero when loading
    s & BOOST_SERIALIZATION_NVP(nInsns);
    insns_.resize(nInsns);                          // no-op when saving
    for (size_t i = 0; i < nInsns; ++i)
        transferAst(s, insns_[i]);

    s & BOOST_SERIALIZATION_NVP(insns_);
    if (version < 3)
        s & boost::serialization::make_nvp("dispatcher_", semantics_.dispatcher);
    s & boost::serialization::make_nvp("operators_", semantics_.operators);
    s & boost::serialization::make_nvp("initialState_", semantics_.initialState);
    s & boost::serialization::make_nvp("usingDispatcher_", semantics_.usingDispatcher);
    s & boost::serialization::make_nvp("optionalPenultimateState_", semantics_.optionalPenultimateState);
    s & BOOST_SERIALIZATION_NVP(dblocks_);
    s & BOOST_SERIALIZATION_NVP(insnAddrMap_);
    s & BOOST_SERIALIZATION_NVP(successors_);
    s & BOOST_SERIALIZATION_NVP(ghostSuccessors_);
    s & BOOST_SERIALIZATION_NVP(isFunctionCall_);
    s & BOOST_SERIALIZATION_NVP(isFunctionReturn_);
    s & BOOST_SERIALIZATION_NVP(mayReturn_);
    if (version >= 1)
        s & BOOST_SERIALIZATION_NVP(popsStack_);
    if (version >= 2)
        s & BOOST_SERIALIZATION_NVP(sourceLocation_);
}

template<class S>
void
BasicBlock::save(S &s, const unsigned version) const {
    const_cast<BasicBlock*>(this)->serializeCommon(s, version);
    if (version >= 3) {
        // Save only the dispatcher name and its operators, which should be sufficient to load it later.
        std::string archName;
        InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops;
        if (semantics_.dispatcher) {
            archName = Architecture::name(semantics_.dispatcher->architecture());
            ops = semantics_.dispatcher->operators();
        }
        s & BOOST_SERIALIZATION_NVP(archName);
        s & BOOST_SERIALIZATION_NVP(ops);
    }
}

template<class S>
void
BasicBlock::load(S &s, const unsigned version) {
    serializeCommon(s, version);
    if (version >= 3) {
        // Restore the dispatcher from its saved name and operators.
        std::string archName;
        InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops;
        s & BOOST_SERIALIZATION_NVP(archName);
        s & BOOST_SERIALIZATION_NVP(ops);
        if (!archName.empty())
            semantics_.dispatcher = Architecture::newInstructionDispatcher(archName, ops);
    }
}

ROSE_SERIALIZATION_INSTANTIATE(BasicBlock);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::BasicBlockSuccessor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
BasicBlockSuccessor::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(expr_);
    s & BOOST_SERIALIZATION_NVP(type_);
    s & BOOST_SERIALIZATION_NVP(confidence_);
}

ROSE_SERIALIZATION_INSTANTIATE(BasicBlockSuccessor);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::CfgEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
CfgEdge::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(type_);
    s & BOOST_SERIALIZATION_NVP(confidence_);
}

ROSE_SERIALIZATION_INSTANTIATE(CfgEdge);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::CfgVertex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
CfgVertex::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(type_);
    s & BOOST_SERIALIZATION_NVP(startVa_);
    s & BOOST_SERIALIZATION_NVP(bblock_);
    s & BOOST_SERIALIZATION_NVP(owningFunctions_);
}

ROSE_SERIALIZATION_INSTANTIATE(CfgVertex);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::DataBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
DataBlock::serialize(S &s, const unsigned version) {
    // s & boost::serialization::base_object<Sawyer::Attribute::Storage>(*this); -- not serialized
    s & BOOST_SERIALIZATION_NVP(isFrozen_);
    s & BOOST_SERIALIZATION_NVP(startVa_);
    if (version >= 1) {
        s & BOOST_SERIALIZATION_NVP(type_);
        s & BOOST_SERIALIZATION_NVP(comment_);
    } else if (S::is_loading::value) {
        size_t nBytes = 0;
        s & boost::serialization::make_nvp("size_", nBytes);
        type_ = SageBuilderAsm::buildTypeVector(nBytes, SageBuilderAsm::buildTypeU8());
    }
    if (version < 2) {
        ASSERT_not_reachable("Rose::BinaryAnalysis::Partitioner2::DataBlock version 2 is no longer supported");
    } else {
        s & BOOST_SERIALIZATION_NVP(attachedBasicBlockOwners_);
        s & BOOST_SERIALIZATION_NVP(attachedFunctionOwners_);
    }
    if (version >= 3)
        s & BOOST_SERIALIZATION_NVP(sourceLocation_);
}

ROSE_SERIALIZATION_INSTANTIATE(DataBlock);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::DisassemblerSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
DisassemblerSettings::serialize(S &s, unsigned version) {
    if (version >= 1)
        s & BOOST_SERIALIZATION_NVP(doDisassemble);
    s & BOOST_SERIALIZATION_NVP(isaName);
}

ROSE_SERIALIZATION_INSTANTIATE(DisassemblerSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::Engine::Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
Engine::Settings::serialize(S &s, unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(loader);
    s & BOOST_SERIALIZATION_NVP(disassembler);
    s & BOOST_SERIALIZATION_NVP(partitioner);
    s & BOOST_SERIALIZATION_NVP(engine);
    s & BOOST_SERIALIZATION_NVP(astConstruction);
}

ROSE_SERIALIZATION_INSTANTIATE(Engine::Settings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::EngineSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
EngineSettings::serialize(S &s, unsigned /*version*/) {
    s & BOOST_SERIALIZATION_NVP(configurationNames);
    s & BOOST_SERIALIZATION_NVP(exitOnError);
}

ROSE_SERIALIZATION_INSTANTIATE(EngineSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
Function::serialize(S &s, const unsigned version) {
    //s & boost::serialization::base_object<Sawyer::Attribute::Storage<> >(*this); -- not stored
    s & BOOST_SERIALIZATION_NVP(entryVa_);
    s & BOOST_SERIALIZATION_NVP(name_);
    s & BOOST_SERIALIZATION_NVP(demangledName_);
    s & BOOST_SERIALIZATION_NVP(comment_);
    s & BOOST_SERIALIZATION_NVP(reasons_);
    s & BOOST_SERIALIZATION_NVP(bblockVas_);
    s & BOOST_SERIALIZATION_NVP(dblocks_);
    s & BOOST_SERIALIZATION_NVP(isFrozen_);
    s & BOOST_SERIALIZATION_NVP(ccAnalysis_);
    s & BOOST_SERIALIZATION_NVP(ccDefinition_);
    s & BOOST_SERIALIZATION_NVP(stackDeltaAnalysis_);
    s & BOOST_SERIALIZATION_NVP(stackDeltaOverride_);
    if (version >= 1)
        s & BOOST_SERIALIZATION_NVP(reasonComment_);
    if (version >= 2)
        s & BOOST_SERIALIZATION_NVP(sourceLocation_);
}

ROSE_SERIALIZATION_INSTANTIATE(Function);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::LoaderSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
LoaderSettings::serialize(S &s, unsigned version) {
    s & BOOST_SERIALIZATION_NVP(deExecuteZerosThreshold);
    s & BOOST_SERIALIZATION_NVP(deExecuteZerosLeaveAtFront);
    s & BOOST_SERIALIZATION_NVP(deExecuteZerosLeaveAtBack);
    s & BOOST_SERIALIZATION_NVP(memoryDataAdjustment);
    s & BOOST_SERIALIZATION_NVP(memoryIsExecutable);
    if (version >= 1) {
        s & BOOST_SERIALIZATION_NVP(envEraseNames);
        s & BOOST_SERIALIZATION_NVP(envInsert);

        // There's no serialization for boost::regex, so we do it ourselves.
        std::vector<std::string> reStrings;
        for (const boost::regex &re: envErasePatterns)
            reStrings.push_back(re.str());
        s & BOOST_SERIALIZATION_NVP(reStrings);
        if (envErasePatterns.empty()) {
            for (const std::string &reStr: reStrings)
                envErasePatterns.push_back(boost::regex(reStr));
        }
    }
}

ROSE_SERIALIZATION_INSTANTIATE(LoaderSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::Partitioner
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
Partitioner::serializeCommon(S &s, const unsigned version) {
    s.template register_type<InstructionSemantics::SymbolicSemantics::SValue>();
    s.template register_type<InstructionSemantics::SymbolicSemantics::RiscOperators>();
#ifdef ROSE_ENABLE_ASM_AARCH64
    s.template register_type<InstructionSemantics::DispatcherAarch64>();
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
    s.template register_type<InstructionSemantics::DispatcherAarch32>();
#endif
    s.template register_type<InstructionSemantics::DispatcherX86>();
    s.template register_type<InstructionSemantics::DispatcherM68k>();
    s.template register_type<InstructionSemantics::DispatcherPowerpc>();
    s.template register_type<SymbolicExpression::Interior>();
    s.template register_type<SymbolicExpression::Leaf>();
    s.template register_type<Z3Solver>();
    s.template register_type<Semantics::SValue>();
    s.template register_type<Semantics::MemoryListState>();
    s.template register_type<Semantics::MemoryMapState>();
    s.template register_type<Semantics::RegisterState>();
    s.template register_type<Semantics::State>();
    s.template register_type<Semantics::RiscOperators>();
    s & BOOST_SERIALIZATION_NVP(settings_);
    // s & config_;                         -- FIXME[Robb P Matzke 2016-11-08]
    s & BOOST_SERIALIZATION_NVP(instructionProvider_);
    s & BOOST_SERIALIZATION_NVP(memoryMap_);
    s & BOOST_SERIALIZATION_NVP(cfg_);
    // s & vertexIndex_;                    -- initialized by rebuildVertexIndices
    s & BOOST_SERIALIZATION_NVP(aum_);
    // s & BOOST_SERIALIZATION_NVP(solver_); -- not saved/restored in order to override from command-line
    s & BOOST_SERIALIZATION_NVP(functions_);
    s & BOOST_SERIALIZATION_NVP(autoAddCallReturnEdges_);
    s & BOOST_SERIALIZATION_NVP(assumeFunctionsReturn_);
    s & BOOST_SERIALIZATION_NVP(stackDeltaInterproceduralLimit_);
    s & BOOST_SERIALIZATION_NVP(addressNames_);
    if (version >= 1)
        s & BOOST_SERIALIZATION_NVP(sourceLocations_);
    s & BOOST_SERIALIZATION_NVP(semanticMemoryParadigm_);
    // s & unparser_;                       -- not saved; restored from disassembler
    // s & cfgAdjustmentCallbacks_;         -- not saved/restored
    // s & basicBlockCallbacks_;            -- not saved/restored
    // s & functionPrologueMatchers_;       -- not saved/restored
    // s & functionPaddingMatchers_;        -- not saved/restored
    // s & undiscoveredVertex_;             -- initialized by rebuildVertexIndices
    // s & indeterminateVertex_;            -- initialized by rebuildVertexIndices
    // s & nonexistingVertex_;              -- initialized by rebuildVertexIndices
    if (version >= 2)
        s & BOOST_SERIALIZATION_NVP(elfGotVa_);
    // s & progress_;                       -- not saved/restored
    // s & cfgProgressTotal_;               -- not saved/restored
}

template<class S>
void
Partitioner::save(S &s, const unsigned version) const {
    const_cast<Partitioner*>(this)->serializeCommon(s, version);
    if (version >= 3)
        saveAst(s, interpretation_);
    if (version >= 4) {
        ASSERT_not_null(architecture_);
        std::string architecture = architecture_->name();
        s & BOOST_SERIALIZATION_NVP(architecture);
    }
}

template<class S>
void
Partitioner::load(S &s, const unsigned version) {
    serializeCommon(s, version);
    if (version >= 3) {
        SgNode *node = restoreAst(s);
        interpretation_ = isSgAsmInterpretation(node);
        ASSERT_require(!node || interpretation_);
    }
    if (version >= 4) {
        std::string architecture;
        s & BOOST_SERIALIZATION_NVP(architecture);
        architecture_ = Architecture::findByName(architecture).orThrow();
    }
    rebuildVertexIndices();
}

ROSE_SERIALIZATION_INSTANTIATE_SPLIT(Partitioner);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::PartitionerSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
PartitionerSettings::serialize(S &s, unsigned version) {
    s & BOOST_SERIALIZATION_NVP(base);
    s & BOOST_SERIALIZATION_NVP(functionStartingVas);
    s & BOOST_SERIALIZATION_NVP(followingGhostEdges);
    s & BOOST_SERIALIZATION_NVP(discontiguousBlocks);
    s & BOOST_SERIALIZATION_NVP(maxBasicBlockSize);
    if (version >= 6)
        s & BOOST_SERIALIZATION_NVP(ipRewrites);
    s & BOOST_SERIALIZATION_NVP(findingFunctionPadding);
    s & BOOST_SERIALIZATION_NVP(findingDeadCode);
    s & BOOST_SERIALIZATION_NVP(peScramblerDispatcherVa);
    if (version >= 2) {
        s & BOOST_SERIALIZATION_NVP(findingIntraFunctionCode);
    } else {
        bool temp = false;
        if (S::is_saving::value)
            temp = findingIntraFunctionCode > 0;
        s & boost::serialization::make_nvp("findingIntraFunctionCode", temp);
        if (S::is_loading::value)
            findingIntraFunctionCode = temp ? 10 : 0; // arbitrary number of passes
    }
    s & BOOST_SERIALIZATION_NVP(findingIntraFunctionData);
    s & BOOST_SERIALIZATION_NVP(findingInterFunctionCalls);
    if (version >= 4)
        s & BOOST_SERIALIZATION_NVP(findingFunctionCallFunctions);
    if (version >= 5) {
        s & BOOST_SERIALIZATION_NVP(findingEntryFunctions);
        s & BOOST_SERIALIZATION_NVP(findingErrorFunctions);
        s & BOOST_SERIALIZATION_NVP(findingImportFunctions);
        s & BOOST_SERIALIZATION_NVP(findingExportFunctions);
        s & BOOST_SERIALIZATION_NVP(findingSymbolFunctions);
    }
    s & BOOST_SERIALIZATION_NVP(interruptVector);
    s & BOOST_SERIALIZATION_NVP(doingPostAnalysis);
    s & BOOST_SERIALIZATION_NVP(doingPostFunctionMayReturn);
    s & BOOST_SERIALIZATION_NVP(doingPostFunctionStackDelta);
    s & BOOST_SERIALIZATION_NVP(doingPostCallingConvention);
    s & BOOST_SERIALIZATION_NVP(doingPostFunctionNoop);
    s & BOOST_SERIALIZATION_NVP(functionReturnAnalysis);
    if (version >= 3)
        s & BOOST_SERIALIZATION_NVP(functionReturnAnalysisMaxSorts);
    s & BOOST_SERIALIZATION_NVP(findingDataFunctionPointers);
    s & BOOST_SERIALIZATION_NVP(findingCodeFunctionPointers);
    s & BOOST_SERIALIZATION_NVP(findingThunks);
    s & BOOST_SERIALIZATION_NVP(splittingThunks);
    s & BOOST_SERIALIZATION_NVP(semanticMemoryParadigm);
    if (version >= 8) {
        s & BOOST_SERIALIZATION_NVP(namingConstants);
    } else if (typename S::is_loading()) {
        bool b;
        s & boost::serialization::make_nvp("namingConstants", b);
        if (b) {
            namingConstants = AddressInterval::whole();
        } else {
            namingConstants = AddressInterval();
        }
    }
    if (version >= 7) {
        s & BOOST_SERIALIZATION_NVP(namingStrings);
    } else if (typename S::is_loading()) {
        bool b;
        s & boost::serialization::make_nvp("namingStrings", b);
        if (b) {
            namingStrings = AddressInterval::whole();
        } else {
            namingStrings = AddressInterval();
        }
    }
    s & BOOST_SERIALIZATION_NVP(demangleNames);
    if (version >= 1) {
        s & BOOST_SERIALIZATION_NVP(namingSyscalls);

        // There is no support for boost::filesystem serialization due to arguments by the maintainers over who has
        // responsibility, so we do it the hard way.
        std::string temp;
        if (S::is_saving::value)
            temp = syscallHeader.string();
        s & boost::serialization::make_nvp("syscallHeader", temp);
        if (S::is_loading::value)
            syscallHeader = temp;
    }
}

ROSE_SERIALIZATION_INSTANTIATE(PartitionerSettings);
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {
namespace Semantics {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class Super>
template<class S>
void
MemoryState<Super>::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    s & BOOST_SERIALIZATION_NVP(map_);
    s & BOOST_SERIALIZATION_NVP(addressesRead_);
    s & BOOST_SERIALIZATION_NVP(enabled_);
}

ROSE_SERIALIZATION_INSTANTIATE(MemoryListState);        // MemoryState<IS::SymbolicSemantics::MemoryListState>
ROSE_SERIALIZATION_INSTANTIATE(MemoryMapState);         // MemoryState<IS::SymbolicSemantics::MemoryMapState>
#endif

} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Partitioner2 {
namespace Semantics {

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
template<class S>
void
RiscOperators::serialize(S &s, const unsigned /*version*/) {
    s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
}

ROSE_SERIALIZATION_INSTANTIATE(RiscOperators);
#endif

} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class versions must be at global scope
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::AddressUser, 1);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::BasePartitionerSettings, 1);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::BasicBlock, 3);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::DisassemblerSettings, 1);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::Function, 2);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::LoaderSettings, 1);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::Partitioner, 4);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::PartitionerSettings, 8);

BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryListState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryMapState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators);
#endif

#endif
