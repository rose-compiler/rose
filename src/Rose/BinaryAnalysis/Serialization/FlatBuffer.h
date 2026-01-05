#ifndef ROSE_BinaryAnalysis_Serialization_FlatBuffer_H
#define ROSE_BinaryAnalysis_Serialization_FlatBuffer_H

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <sage3basic.h>

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <utility>
#include <vector>

#include <flatbuffers/flatbuffers.h>

#include <Rose/BinaryAnalysis/Serialization/FlatBufferSchema.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Serialization {
namespace FlatBuffers {

static constexpr uint32_t version = 1;

/** This class encapsulates the state needed to serialize (save) a Partitioner2::Partitioner to a
 *  FlatBuffer encoding.
 *
 * The intended usage is as follows:
 * ```c++
 *   void savePartitioner(Partitioner2::PartitionerPtr partitioner) {
 *      auto serializer = FlatBuffer::Serializer(partitioner);
 *
 *      // First create and save the encoding.
 *      serializer.save();
 *      // Next, get the encoding via the buffer();
 *      const std::vector<char> encoding = serializer.buffer();
 *      // Do something with the encoding
 *      // ...
 *      // Alternatively, after save(), use a convenience method to save to a file like so:
 *      boost::filesystem::path output_path = "/some/output/path";
 *      serializer.write(output_path);
 *   }
 * ```
 * */
class Serializer {
  public:
    explicit Serializer(const Partitioner2::PartitionerConstPtr&);
    ~Serializer() = default;

    Serializer(const Serializer&)            = delete;
    Serializer& operator=(const Serializer&) = delete;

    template <typename T> using Handle = flatbuffers::Offset<T>;

    /** Serialize the partitioner into the internal FlatBuffer.
     *  This function must be called before @ref buffer can be called because it freezes the internal FlatBuffer state.
     */
    void save();

    /** Pointer and size view into the internal FlatBuffer.
     *
     *  Valid only after @ref save has been called. */
    std::pair<const uint8_t*, size_t> buffer() const;

    /** Write encoding to a stream. Valid  */
    void write(std::ostream&) const;

    /** Write buffer to a file. */
    void write(const boost::filesystem::path&) const;

  private:
    Partitioner2::PartitionerConstPtr               partitioner_;
    std::vector<char>                               bytes_;
    std::unique_ptr<flatbuffers::FlatBufferBuilder> builder_;

  protected:
    Handle<Instruction> instruction(const SgAsmInstruction*& insn);
    Handle<BasicBlock>  basicBlock(const Partitioner2::BasicBlockPtr& bb);
    Handle<Function>    function(const Partitioner2::FunctionPtr& f);
    Handle<CfgEdge>     cfgEdge(const Partitioner2::ControlFlowGraph::Edge& e);
    Handle<Cfg>         cfg(const Partitioner2::ControlFlowGraph& cfg);
    Handle<Segment>     segment(const BinaryAnalysis::MemoryMap::Super::Node& seg);
    Handle<MemoryMap>   mmap(const BinaryAnalysis::MemoryMap& map);
    Handle<Root>        partitioner(/*partitioner_*/);
};

/** Loads a Partitioner2::Partitioner from a FlatBuffer. */
class Deserializer {
  public:
    Deserializer();
    ~Deserializer();

    Deserializer(const Deserializer&)            = delete;
    Deserializer& operator=(const Deserializer&) = delete;
    Deserializer(Deserializer&&)                 = default;
    Deserializer& operator=(Deserializer&&)      = default;

    /** Initialize a loader from a file. */
    static Deserializer fromFile(const boost::filesystem::path&);

    /** Initialize a loader from a stream (reads until EOF). */
    static Deserializer fromStream(std::istream&);

    /** Initialize a loader from an existing buffer, taking ownership of the buffer. */
    static Deserializer fromBytes(std::vector<char>&&);

    /** Initialize a loader from an existing buffer without ownership of the buffer. */
    static Deserializer fromBytes(const std::vector<char>&);

    /** Verify that the loaded bytes are a valid FlatBuffer for the Partitioner root type. */
    bool verify() const;

    /** Materialize and return a new partitioner instance from the FlatBuffer data. */
    Partitioner2::PartitionerPtr load() const;

  private:
    std::vector<char> bytes_;

  protected:
    BinaryAnalysis::MemoryMap::Ptr mmap(const MemoryMap* map) const;
};

} // namespace FlatBuffers
} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
