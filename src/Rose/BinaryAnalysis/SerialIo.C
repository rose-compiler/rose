// This file implements the top-level serial I/O API in ROSE

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/SerialFrame.h>

#include <AstSerialization.h>

#ifdef ROSE_ENABLE_FLATBUFFERS
#include <Rose/BinaryAnalysis/Serialization/FlatBuffers.h>
#endif

#include <utility>
#include <vector>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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

namespace {
std::vector<SerialIo::SerializationRegistration>&
serializationRegistry() {
    static std::vector<SerialIo::SerializationRegistration> reg;
    return reg;
}
} // namespace

void
SerialIo::registerSerialization(SerializationRegistration r) {
    serializationRegistry().push_back(std::move(r));
}

const Sawyer::Optional<SerialIo::SerializationRegistration>
SerialIo::findSerialization(Serialization::Format f) {
    for (const auto& r : serializationRegistry()) {
        if (r.format == f)
            return r;
    }
    return {};
}

void
SerialIo::init() {}

SerialIo::~SerialIo() {}

Serialization::Savable
SerialIo::userSavable(unsigned offset) {
    unsigned retval = Serialization::USER_DEFINED + offset;
    ASSERT_require(retval >= Serialization::USER_DEFINED && retval <= Serialization::USER_DEFINED_LAST);
    return (Serialization::Savable)retval;
}

Serialization::Format
SerialIo::format() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return format_;
}

void
SerialIo::format(Serialization::Format fmt) {
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
SerialIo::progress(const Progress::Ptr& p) {
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

Serialization::Savable
SerialIo::objectType() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return objectType_;
}

void
SerialIo::objectType(Serialization::Savable t) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    objectType_ = t;
}

void
SerialIo::close() {
    if (isOpen()) {
        setIsOpen(false);
        if (Progress::Ptr p = progress())
            p->update(1.0);
        progressBar_.value(progressBar_.domain().second);
    }
}

Partitioner2::PartitionerPtr
SerialIo::Deserializer::loadPartitioner(const std::vector<char>& data, Serialization::ProgressCallback progress) {
    Partitioner2::BasePartitionerSettings settings;
    return loadPartitioner(data, settings, progress);
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
SerialOutput::open(const boost::filesystem::path& fileName) {
    if (isOpen())
        close();

    objectType(Serialization::ERROR); // in case of exception

    // Create a callback for SerialFrame that wraps our progress indicator
    auto progressCB = makeProgressCB("serializing");

    try {
        // Initialize the container frame - it will handle the file opening
        frame_ = std::make_unique<Serialization::SerialFrame>();
        frame_->openForWrite(fileName, progressCB);
        frame_->writeFileHeader();

        // Look up and create the appropriate backend for the format
        const auto serializers = findSerialization(format());
        if (!serializers) {
            throw Exception("no backend registered for format " + boost::lexical_cast<std::string>(format()));
        }
        serializer_ = serializers->serializer();
        if (!serializer_) {
            throw Exception("failed to create serializer for format " + boost::lexical_cast<std::string>(format()));
        }

        setIsOpen(true);
        objectType(Serialization::NO_OBJECT);
    } catch (const Exception& e) {
        throw;
    } catch (...) {
        throw Exception("failed to open for writing: file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }
}

void
SerialOutput::savePartitioner(const Partitioner2::Partitioner::ConstPtr& partitioner) {
    if (!isOpen())
        throw Exception("cannot save partitioner when no file is open");

    ASSERT_not_null(serializer_);
    ASSERT_not_null(frame_);

    // Create a progress callback for the serializer
    auto progress = makeProgressCB("serializing");

    // Get serialized payload from the serializer
    std::vector<char> payload = serializer_->savePartitioner(partitioner, progress);

    // Create a FrameRecord with appropriate metadata
    Serialization::FrameRecord frameRecord(Serialization::PARTITIONER, format());
    frameRecord.payload(payload);

    // Write the frame record
    frame_->writeFrameRecord(frameRecord);
    objectType(Serialization::PARTITIONER);
    return;
}

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
void
SerialOutput::saveAstHelper(SgNode* ast) {
    if (ast) {
        SgNode* oldParent = ast->get_parent();
        try {
            ast->set_parent(NULL);
            saveObject(Serialization::AST, ast);
            ast->set_parent(oldParent);
        } catch (...) {
            ast->set_parent(oldParent);
            throw;
        }
    } else {
        saveObject(Serialization::AST, ast);
    }
}

void
SerialOutput::saveAst(SgAsmNode* ast) {
    saveAstHelper(ast);
}

void
SerialOutput::saveAst(SgBinaryComposite* ast) {
    saveAstHelper(ast);
}
#endif

void
SerialOutput::close() {
    if (isOpen() && objectType() != Serialization::END_OF_DATA && objectType() != Serialization::ERROR) {
        if (serializer_)
            serializer_.reset();

        if (frame_) {
            Serialization::FrameRecord frameRecord(Serialization::END_OF_DATA, format());
            frame_->writeFrameRecord(frameRecord);
            frame_->close();
            frame_.reset();
        }

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
SerialInput::open(const boost::filesystem::path& fileName) {
    if (isOpen())
        close();

    objectType(Serialization::ERROR); // in case of exception

    auto progressCB = makeProgressCB("deserializing");

    try {
        // Initialize the container frame - it will handle the file opening
        frame_ = std::make_unique<Serialization::SerialFrame>();

        // Open the file and read the header, but handle header errors specially
        frame_->openForRead(fileName, progressCB);

        try {
            frame_->readAndVerifyFileHeader();
            // File opened successfully as a framed container
            mlog[INFO] << "File " << fileName << " opened as a framed RBA container\n";
        } catch (const Serialization::Exception& e) {
            // The file doesn't have a valid RBA header
            mlog[WARN] << "File " << fileName << " doesn't have a valid RBA container header: " << e.what() << "\n";

            // Clean up and reset so we don't leave a half-initialized state
            frame_->close();
            frame_.reset();

            throw; // Re-throw to be caught by the outer try-catch
        }

        // Look up and create the appropriate backend for the format
        const auto serializers = findSerialization(format());
        if (!serializers) {
            throw Exception("no backend registered for format " + boost::lexical_cast<std::string>(format()));
        }
        deserializer_ = serializers->deserializer();
        if (!deserializer_) {
            throw Exception("failed to create deserializer for format " + boost::lexical_cast<std::string>(format()));
        }

        setIsOpen(true);
        objectType(Serialization::PARTITIONER);
    } catch (const Exception& e) {
        throw;
    } catch (...) {
        throw Exception("failed to open for reading: file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    }
}

void
SerialInput::advanceObjectType() {
    ASSERT_require(isOpen());
    ASSERT_require(frame_);

    try {
        // Read the next frame record
        Serialization::FrameRecord rec = frame_->readFrameRecord();

        // Update the object type based on the record
        objectType(rec.objectType());
    } catch (const std::exception& e) {
        mlog[WARN] << "Failed to read next object type: " << e.what() << "\n";
        objectType(Serialization::ERROR);
    }
}

Serialization::FrameRecord
SerialInput::readAndValidateRecord(Serialization::Savable expectedType) {
    ASSERT_not_null(frame_);
    auto rec = frame_->readFrameRecord();

    // Validate object type
    if (rec.objectType() != expectedType) {
        throw Exception(
          "unexpected object type (expected " + boost::lexical_cast<std::string>(expectedType) + " but found " +
          boost::lexical_cast<std::string>(rec.objectType()) + ")"
        );
    }

    // Validate format
    if (rec.format() != format()) {
        mlog[WARN] << "serialization format mismatch (expected " << format() << " but found " << rec.format() << ")"
                   << std::endl;
    }

    // Check ROSE version compatibility
    checkCompatibility(rec.roseVersion());

    return rec;
}

Serialization::ProgressCallback
SerialIo::makeProgressCB(const std::string& phase) {

    return [this, phase](size_t current, Sawyer::Optional<size_t> total, const std::string& innerPhase) {
        std::string name = phase + "-" + innerPhase;

        if (total) {
            progressBar_.value(current, *total);
        } else {
            progressBar_.value(current);
        }
        progressBar_.prefix(name);

        double prog_total = total ? *total : NAN;
        if (auto p = progress()) {
            p->update(Progress::Report(current, prog_total));
        }
    };
}

Partitioner2::Partitioner::Ptr
SerialInput::loadPartitioner() {
    if (!isOpen())
        throw Exception("cannot load partitioner when no file is open");

    ASSERT_not_null(deserializer_);
    ASSERT_not_null(frame_);

    // Read and validate the frame record
    auto frameRecord = readAndValidateRecord(Serialization::PARTITIONER);

    // Create a progress callback for the deserializer
    auto progress = makeProgressCB("deserializing");

    // Get the payload and deserialize it using the deserializer
    const auto& payload = frameRecord.payload();
    auto        p       = deserializer_->loadPartitioner(payload, progress);

    objectType(Serialization::END_OF_DATA);
    return p;
}

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
SgNode*
SerialInput::loadAst() {
    return loadObject<SgNode*>(Serialization::AST);
}
#endif

void
SerialInput::close() {
    if (isOpen()) {
        if (deserializer_)
            deserializer_.reset();

        if (frame_) {
            frame_->close();
            frame_.reset();
        }

        SerialIo::close();
    }
}

void
SerialInput::checkCompatibility(const std::string& fileVersion) {
    const std::string roseVersion = ROSE_PACKAGE_VERSION;
    if (roseVersion == fileVersion)
        return;

    std::vector<std::string> fileParts = Rose::StringUtility::split('.', fileVersion);
    std::vector<std::string> roseParts = Rose::StringUtility::split('.', roseVersion);

    // ROSE uses a dotted quad for the version number, as in W.X.Y.Z where W is zero, X is the major version, Y is the
    // minor version, and Z is the patch version. Backward compatibility is ensured when W and X are the same for the
    // file and the ROSE library and ROSE's Y.Z is greater than or equal to the file's Y.Z.
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

    if (fileParts[0] == roseParts[0] && fileParts[1] == roseParts[1] &&
        (fileParts[2] <= roseParts[2] || (fileParts[2] == roseParts[2] && fileParts[3] <= roseParts[3]))) {
        if (fileVersion != roseVersion)
            mlog[WARN] << "RBA file version " << fileVersion << " is being read by ROSE version " << roseVersion
                       << "\n";
    } else {
        throw Exception("ROSE library " + roseVersion + " cannot read file version " + fileVersion);
    }
}

} // namespace BinaryAnalysis
} // namespace Rose

#endif
