#include <sage3basic.h>
#include <BinarySerialIo.h>
#include <Partitioner2/Partitioner.h>
#include <BaseSemantics2.h>
#include <Registers.h>
#include <boost/serialization/shared_ptr.hpp>

#ifdef ROSE_SUPPORTS_SERIAL_IO
#include <fcntl.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#endif



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

#ifndef ROSE_SUPPORTS_SERIAL_IO
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
SerialOutput::savePartitioner(const Partitioner2::Partitioner &partitioner) {
    saveObject(PARTITIONER, partitioner);
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
#ifndef ROSE_SUPPORTS_SERIAL_IO
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

#ifndef ROSE_SUPPORTS_SERIAL_IO
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
#ifdef ROSE_SUPPORTS_SERIAL_IO
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

Partitioner2::Partitioner
SerialInput::loadPartitioner() {
    Partitioner2::Partitioner partitioner;
    loadObject(PARTITIONER, partitioner);
    return boost::move(partitioner);
}

SgNode*
SerialInput::loadAst() {
    return loadObject<SgNode*>(AST);
}

void
SerialInput::close() {
    if (isOpen()) {
#ifndef ROSE_SUPPORTS_SERIAL_IO
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

} // namespace
} // namespace
