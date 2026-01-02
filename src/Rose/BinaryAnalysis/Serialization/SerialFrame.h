#ifndef ROSE_BinaryAnalysis_Serialization_SerialFrame_H
#define ROSE_BinaryAnalysis_Serialization_SerialFrame_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Serialization {

struct RecordHeader {
    Serialization::Savable objectType = Serialization::NO_OBJECT;
    Serialization::Format  format     = Serialization::BINARY;
    std::string            roseVersion;
    std::uint64_t          payloadSize = 0;

    template <class Archive> void serialize(Archive& ar, const unsigned /*version*/) {
        ar & objectType;
        ar & format;
        ar & roseVersion;
        ar & payloadSize;
    }
};

struct FileHeader {
    std::uint32_t magic   = 0;
    std::uint32_t version = 0;

    template <class Archive> void serialize(Archive& ar, const unsigned /*version*/) {
        ar & magic;
        ar & version;
    }
};

struct Record {
    RecordHeader      header;
    std::vector<char> payload;
};

/** Helper class for metadata and payload handling.
 *
 * This class serves as a convenient abstraction for working with metadata and payloads
 * in the serialization system. It provides direct access to metadata fields,
 * methods to manipulate the payload, and serialization support.
 */
class FrameRecord {
  public:
    /** Default constructor creates an empty record. */
    FrameRecord() :
      objectType_(Serialization::NO_OBJECT), format_(Serialization::BINARY), roseVersion_(ROSE_PACKAGE_VERSION) {}

    /** Construct with specific objectType and format. */
    FrameRecord(Serialization::Savable objectType, Serialization::Format format) :
      objectType_(objectType), format_(format), roseVersion_(ROSE_PACKAGE_VERSION) {}

    /** Construct from a Record struct. */
    explicit FrameRecord(const Record& record) :
      objectType_(record.header.objectType), format_(record.header.format), roseVersion_(record.header.roseVersion),
      payload_(record.payload) {}

    /** Convert to a Record struct. */
    Record toRecord() const {
        Record record;
        record.header.objectType  = objectType_;
        record.header.format      = format_;
        record.header.roseVersion = roseVersion_;
        record.header.payloadSize = payload_.size();
        record.payload            = payload_;
        return record;
    }

    /** Get access to the payload data. */
    std::vector<char>& payload() { return payload_; }

    /** Get const access to the payload data. */
    const std::vector<char>& payload() const { return payload_; }

    /** Set the payload data. */
    void setPayload(const std::vector<char>& payload) { payload_ = payload; }

    /** Get the object type. */
    Serialization::Savable objectType() const { return objectType_; }

    /** Set the object type. */
    void objectType(Serialization::Savable type) { objectType_ = type; }

    /** Get the format. */
    Serialization::Format format() const { return format_; }

    /** Set the format. */
    void format(Serialization::Format fmt) { format_ = fmt; }

    /** Get the ROSE version. */
    std::string roseVersion() const { return roseVersion_; }

    /** Set the ROSE version. */
    void roseVersion(const std::string& ver) { roseVersion_ = ver; }

    /** Get the payload size. */
    std::uint64_t payloadSize() const { return payload_.size(); }

    /** Template method to serialize an object directly to this record's payload. */
    template <typename T> void serializeObject(const T& object) {
        std::ostringstream              oss;
        boost::archive::binary_oarchive oa(oss);
        oa << object;
        std::string serialized = oss.str();

        // Copy serialized data to payload
        payload_.resize(serialized.size());
        std::memcpy(payload_.data(), serialized.data(), serialized.size());
    }

    /** Template method to deserialize an object from this record's payload. */
    template <typename T> void deserializeObject(T& object) const {
        std::string                     serialized(reinterpret_cast<const char*>(payload_.data()), payload_.size());
        std::istringstream              iss(serialized);
        boost::archive::binary_iarchive ia(iss);
        ia >> object;
    }

    /** Boost serialization support. */
    template <class Archive> void serialize(Archive& ar, const unsigned /*version*/) {
        ar & objectType_;
        ar & format_;
        ar & roseVersion_;
        ar & payload_;
    }

  private:
    Serialization::Savable objectType_;  // Type of object stored in the payload
    Serialization::Format  format_;      // Format used for the payload
    std::string            roseVersion_; // ROSE version used to create the record
    std::vector<char>      payload_;     // Raw payload data
};

/** Framed container for SerialIo records.
 *
 * This class owns all raw I/O for the RBA container format and uses Boost
 * binary archives to encode/decode record metadata. SerialIo interacts with
 * this class using objectType/format/roseVersion/payloadBytes, and does not
 * call ::read/::write directly. */
class SerialFrame {
  public:
    using ProgressCallback = Serialization::ProgressCallback;

    SerialFrame();
    ~SerialFrame();

    // Open/close for writing or reading.
    void openForWrite(const boost::filesystem::path& fileName, ProgressCallback ioCb);
    void openForRead(const boost::filesystem::path& fileName, ProgressCallback ioCb);

    // Close boost streams and fd, adjust fd and mode if necessary
    void close();

  private:
    // Open boost streams depending on mode_ and fd_. This function assumes that mode_ and fd_
    // are properly set (by way of openForWrite() or openForRead()).
    void open(const boost::filesystem::path& fileName);

  public:
    // Container-level header.
    void writeFileHeader();
    void readAndVerifyFileHeader();

    // Record I/O.
    void   writeRecord(const RecordHeader& header, const std::vector<char>& payload);
    Record readRecord();

    // FrameRecord convenience methods
    void        writeFrameRecord(const FrameRecord& frameRecord);
    FrameRecord readFrameRecord();

  private:
    enum class Mode { Closed, Reading, Writing };

    Mode mode_;
    int  fd_;

    // Input I/O layer
    boost::iostreams::file_descriptor_source                           inDevice_;
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> inStream_;
    std::unique_ptr<boost::archive::binary_iarchive>                   inArchive_;

    // Output I/O layer
    boost::iostreams::file_descriptor_sink                           outDevice_;
    boost::iostreams::stream<boost::iostreams::file_descriptor_sink> outStream_;
    std::unique_ptr<boost::archive::binary_oarchive>                 outArchive_;

    ProgressCallback progress_;

    void closeInput();
    void closeOutput();
    void ensureWriting() const;
    void ensureReading() const;

    // Helpers to write/read a size-prefixed blob.
    void              writeBlob(const std::vector<char>& data);
    std::vector<char> readBlob();

    static void startWriteWorker(SerialFrame* saver, const std::vector<char>& data);
    void writeWorker(const std::vector<char>& data);
};

} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
