#ifndef ROSE_BinaryAnalysis_Serialization_FlatbufferStorage_H
#define ROSE_BinaryAnalysis_Serialization_FlatbufferStorage_H

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <utility>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Serialization {

/** Saves a Partitioner2::Partitioner to a FlatBuffer. */
class FlatbufferPartitionerSaver {
  public:
    /** Construct a saver for an existing partitioner. */
    explicit FlatbufferPartitionerSaver(const Partitioner2::PartitionerConstPtr &);
    ~FlatbufferPartitionerSaver();

    FlatbufferPartitionerSaver(const FlatbufferPartitionerSaver &)            = delete;
    FlatbufferPartitionerSaver &operator=(const FlatbufferPartitionerSaver &) = delete;

    /** Serialize the partitioner into the internal FlatBuffer. */
    void save();

    /** Pointer + size view into the internal FlatBuffer.
     *
     *  Valid after @ref save has been called. */
    std::pair<const uint8_t *, size_t> buffer() const;

    /** Write buffer to a stream. */
    void write(std::ostream &) const;

    /** Write buffer to a file. */
    void write(const boost::filesystem::path &) const;

  private:
    Partitioner2::PartitionerConstPtr partitioner_;
    std::vector<uint8_t>              bytes_;
};

/** Loads a Partitioner2::Partitioner from a FlatBuffer. */
class FlatbufferPartitionerLoader {
  public:
    FlatbufferPartitionerLoader();
    ~FlatbufferPartitionerLoader();

    FlatbufferPartitionerLoader(const FlatbufferPartitionerLoader &)            = delete;
    FlatbufferPartitionerLoader &operator=(const FlatbufferPartitionerLoader &) = delete;
    FlatbufferPartitionerLoader(FlatbufferPartitionerLoader &&)                 = default;
    FlatbufferPartitionerLoader &operator=(FlatbufferPartitionerLoader &&)      = default;

    /** Initialize a loader from a file. */
    static FlatbufferPartitionerLoader fromFile(const boost::filesystem::path &);

    /** Initialize a loader from a stream (reads until EOF). */
    static FlatbufferPartitionerLoader fromStream(std::istream &);

    /** Initialize a loader from an existing buffer. */
    static FlatbufferPartitionerLoader fromBytes(std::vector<uint8_t>);

    /** Verify that the loaded bytes are a valid FlatBuffer for the Partitioner root type. */
    bool verify() const;

    /** Materialize and return a new partitioner instance from the FlatBuffer data. */
    Partitioner2::PartitionerPtr load() const;

  private:
    std::vector<uint8_t> bytes_;
};

} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
#endif
