#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Serialization/SerialFrame.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

#include <Rose/StringUtility/Escape.h>

#include <fcntl.h>
#include <unistd.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Synchronization.h>

#include <iostream>

using namespace Rose::BinaryAnalysis;

namespace Rose {
namespace BinaryAnalysis {
namespace Serialization {

namespace {
static const std::uint32_t RBA_MAGIC   = 0x52424100; // "RBA\0" - versioned by metadata
static const std::uint32_t RBA_VERSION = 1;

static const FileHeader EXPECTED_HDR{RBA_MAGIC, RBA_VERSION};

void
throwIoError(const std::string& msg, const boost::filesystem::path& fileName) {
    throw Serialization::Exception(msg + " \"" + StringUtility::cEscape(fileName.string()) + "\"");
}
} // namespace

SerialFrame::SerialFrame() : mode_(Mode::Closed), fd_(-1), inArchive_(nullptr), outArchive_(nullptr) {}

SerialFrame::~SerialFrame() {
    try {
        close();
    } catch (...) {
        // suppress
    }
}

void
SerialFrame::openForWrite(const boost::filesystem::path& fileName, ProgressCallback ioCb) {
    if (mode_ != Mode::Closed)
        close();

    progress_ = ioCb;
    if (fileName == "-") {
        fd_ = 1; // stdout
    } else {
        fd_ = ::open(fileName.string().c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);
        if (fd_ == -1)
            throwIoError("cannot create or truncate file", fileName);
    }

    mode_ = Mode::Writing;
    open(fileName);
}

void
SerialFrame::openForRead(const boost::filesystem::path& fileName, ProgressCallback ioCb) {
    if (mode_ != Mode::Closed)
        close();

    progress_ = ioCb;
    if (fileName == "-") {
        fd_ = 0; // stdin
    } else {
        fd_ = ::open(fileName.string().c_str(), O_RDONLY);
        if (fd_ == -1)
            throwIoError("cannot open for reading file", fileName);
    }

    mode_ = Mode::Reading;
    open(fileName);
}

void
SerialFrame::closeInput() {
    if (inArchive_)
        inArchive_.reset();
    if (inStream_.is_open())
        inStream_.close();
    if (inDevice_.is_open())
        inDevice_.close();
}

void
SerialFrame::closeOutput() {
    if (outArchive_)
        outArchive_.reset();
    if (outStream_.is_open())
        outStream_.close();
    if (outDevice_.is_open())
        outDevice_.close();
}

void
SerialFrame::open(const boost::filesystem::path& fileName) {

    if (mode_ == Mode::Reading) {
        inDevice_.open(fd_, boost::iostreams::never_close_handle);
        inStream_.open(inDevice_);
        if (!inStream_.is_open())
            throwIoError("failed to open boost input stream", fileName);

        inArchive_ = std::make_unique<boost::archive::binary_iarchive>(inStream_);

    } else if (mode_ == Mode::Writing) {
        outDevice_.open(fd_, boost::iostreams::never_close_handle);
        outStream_.open(outDevice_);
        if (!outStream_.is_open())
            throwIoError("failed to open boost output stream", fileName);

        outArchive_ = std::make_unique<boost::archive::binary_oarchive>(outStream_);
    } else {
        throwIoError("Unexpected mode in boost device open", fileName);
    }
}

void
SerialFrame::close() {
    if (mode_ == Mode::Reading)
        closeInput();
    if (mode_ == Mode::Writing)
        closeOutput();

    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    mode_     = Mode::Closed;
    progress_ = ProgressCallback();
}

void
SerialFrame::ensureWriting() const {
    if (mode_ != Mode::Writing)
        throw Serialization::Exception("SerialFrame is not in writing mode");
    if (!outDevice_.is_open())
        throw Serialization::Exception("SerialFrame output device is not initialized");
    if (!outStream_.is_open())
        throw Serialization::Exception("SerialFrame output stream is not open");
    if (!outArchive_)
        throw Serialization::Exception("SerialFrame output archive is not initialized");
}

void
SerialFrame::ensureReading() const {
    if (mode_ != Mode::Reading)
        throw Serialization::Exception("SerialFrame is not in reading mode");
    if (!inDevice_.is_open())
        throw Serialization::Exception("SerialFrame input device is not initialized");
    if (!inStream_.is_open())
        throw Serialization::Exception("SerialFrame input stream is not open");
    if (!inArchive_)
        throw Serialization::Exception("SerialFrame input archive is not initialized");
}

void
SerialFrame::writeFileHeader() {
    ensureWriting();
    (*outArchive_) & EXPECTED_HDR;
}

void
SerialFrame::readAndVerifyFileHeader() {
    ensureReading();
    FileHeader hdr;
    (*inArchive_) & hdr;
    if (hdr.magic != EXPECTED_HDR.magic)
        throw Serialization::Exception("invalid RBA container magic");
    if (hdr.version != EXPECTED_HDR.version)
        throw Serialization::Exception("unsupported RBA container version");
}

void
SerialFrame::startWriteWorker(SerialFrame* saver, const std::vector<char>& data) {
    saver->writeWorker(data);
}

void
SerialFrame::writeWorker(const std::vector<char>& data) {
    (*outArchive_) & data;
}

void
SerialFrame::writeBlob(const std::vector<char>& data) {
    ensureWriting();

    std::string                 errorMessage;
    boost::thread               worker(startWriteWorker, this, data);
    boost::chrono::milliseconds timeout((unsigned)(1000 * Sawyer::ProgressBarSettings::minimumUpdateInterval()));

    while (!worker.try_join_for(timeout)) {
        off_t cur = ::lseek(fd_, 0, SEEK_CUR);
        if (progress_)
            progress_(cur, data.size(), "writing");
    }

    if (progress_)
        progress_(data.size(), data.size(), "writing");
}

std::vector<char>
SerialFrame::readBlob() {
    ensureReading();
    std::vector<char> data;
    (*inArchive_) & data;
    if (progress_)
        progress_(data.size(), data.size(), "reading");
    return data;
}

void
SerialFrame::writeRecord(const RecordHeader& header, const std::vector<char>& payload) {
    ensureWriting();

    // Serialize header into a temporary buffer using Boost.
    std::vector<char> meta_bytes;
    {
        auto                                          meta_sink = boost::iostreams::back_inserter(meta_bytes);
        boost::iostreams::stream<decltype(meta_sink)> meta_stream(meta_sink);
        boost::archive::binary_oarchive               meta_archive(meta_stream);

        meta_archive & header;
    }

    // Write metadata blob then payload blob.
    writeBlob(meta_bytes);
    writeBlob(payload);
}

Serialization::Record
SerialFrame::readRecord() {
    ensureReading();

    // Read metadata blob and restore header.
    std::vector<char> meta_bytes = readBlob();
    if (meta_bytes.empty()) {
        // An empty metadata blob is invalid; treat as END_OF_DATA with no payload.
        Record r;
        r.header.objectType  = Serialization::END_OF_DATA;
        r.header.payloadSize = 0;
        return r;
    }

    RecordHeader header;
    {
        boost::iostreams::stream<boost::iostreams::array_source> meta_stream(meta_bytes.data(), meta_bytes.size());
        boost::archive::binary_iarchive                          meta_archive(meta_stream);
        meta_archive & header;
    }

    // Read payload blob.
    std::vector<char> payload = readBlob();
    if (header.payloadSize != payload.size())
        header.payloadSize = payload.size();

    Record r;
    r.header  = std::move(header);
    r.payload = std::move(payload);
    return r;
}

void
SerialFrame::writeFrameRecord(const FrameRecord& frameRecord) {
    writeRecord(frameRecord.toRecord().header, frameRecord.toRecord().payload);
}

Serialization::FrameRecord
SerialFrame::readFrameRecord() {
    return FrameRecord(readRecord());
}

} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
