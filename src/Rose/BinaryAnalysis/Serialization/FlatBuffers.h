#ifndef ROSE_BinaryAnalysis_Serialization_FlatBuffers_H
#define ROSE_BinaryAnalysis_Serialization_FlatBuffers_H

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <rosePublicConfig.h>

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

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

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
    explicit Serializer(const P2::PartitionerConstPtr&);
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

    /** Write encoding to a stream. Valid only after @ref save has been called.  */
    void write(std::ostream&) const;

    /** Write buffer to a file. Valid only after @ref save has been called. */
    void write(const boost::filesystem::path&) const;

  private:
    P2::PartitionerConstPtr                         partitioner_;
    std::vector<char>                               bytes_;
    std::unique_ptr<flatbuffers::FlatBufferBuilder> builder_;

  protected:
    Handle<Architecture> architecture(const BinaryAnalysis::Architecture::BaseConstPtr& arch);
    Handle<Instruction>  instruction(const SgAsmInstruction* const& insn);
    Handle<BasicBlock>   basicBlock(const P2::BasicBlockPtr& bb);
    Handle<Function>     function(const P2::FunctionPtr& f);
    Handle<CfgEdge>      cfgEdge(const P2::ControlFlowGraph::Edge& e);
    Handle<Cfg>          cfg(const P2::ControlFlowGraph& cfg);
    Handle<Segment>      segment(const BinaryAnalysis::MemoryMap::Super::Node& seg);
    Handle<MemoryMap>    mmap(const BinaryAnalysis::MemoryMap& map);
    std::pair<Handle<InstructionList>, Handle<BasicBlockList>>
                         instructionsBasicBlocks(const std::vector<P2::BasicBlockPtr>& bbs);
    Handle<FunctionList> functions(const std::vector<P2::FunctionPtr>& funs);
    Handle<Root>         partitioner(/*partitioner_*/);
};

/** Loads a Partitioner2::Partitioner from a FlatBuffer.
 *
 * This class deserializes partitioner state from FlatBuffers format. By default, it uses
 * the instruction provider to reconstruct instructions from the memory map, which is
 * efficient for traditional binary formats (ELF, PE, etc.).
 *
 * For architectures that don't use memory maps (e.g., JVM), the deserializer can be
 * configured to prefer reconstructing instructions from stored bytes. This is controlled
 * via the @ref preferStoredBytes flag, which is automatically set based on the architecture.
 *
 * The deserialization process always maintains a fallback: if the primary method fails
 * (instruction provider or stored bytes), it automatically tries the alternative method.
 * This ensures robustness across different binary formats and edge cases.
 *
 * **IMPORTANT CFG RESTORATION BEHAVIOR:**
 * The deserializer treats the serialized CFG as ground truth and overrides the partitioner's
 * auto-generated CFG structure. The deserialization process:
 *
 * 1. Constructs basic blocks and functions through normal attachment, which creates CFG vertices
 * 2. Removes all discovered CFG edges from basic block vertices
 * 3. Restores edges from the serialized CFG with their exact types and targets
 *
 * This ensures the deserialized CFG exactly matches the original serialized CFG, including
 * edge types (E_FUNCTION_CALL vs. E_CALL_RETURN), edge counts, and graph structure. */
class Deserializer {
  public:
    Deserializer() = default;

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

    /** Set whether to prefer stored bytes over instruction provider.
     *
     * @param prefer If true, use stored bytes first; if false, use instruction provider first.
     *
     * This should be set to true for architectures that don't use memory maps (e.g., JVM). */
    void preferStoredBytes(bool prefer) { preferStoredBytes_ = prefer; }

    /** Materialize and return a new partitioner instance from the FlatBuffer data.
     *
     *  The algorithm is implemented as follows:
     *    1. Create a partitioner from the input architecture and memory map.
     *    2. Rebuild instructions via an instruction provider and discoverInstruction. Map from instruction address to
     * rebuilt instruction.
     *    3. Rebuild basic blocks by appending the constituent instructions (requires the instruction map from 2.).
     *    4. Rebuild functions by attaching basic blocks from step 3. Additionally add placeholder basic blocks for
     * empty functions.
     **/
    P2::PartitionerPtr load(const P2::BasePartitionerSettings& settings);
    // Load with default settings (useful in testing)
    P2::PartitionerPtr load();

  private:
    // Underlying bytes
    std::vector<char> bytes_;

    // Current partitioner
    P2::PartitionerPtr partitioner_;

    // Index ROSE instructions and basic blocks by start address.
    // This is needed because generally FlatBuffer structures use addresses as lightweight references.
    // For example, FlatBuffer basic blocks save their constituent instructions as a list of addresses.
    // We use external explicit maps (instead of the partitioner) so that the Deserializer can create detached
    // partitioner objects.
    std::unordered_map<Address, SgAsmInstruction*>   instructions_;
    std::unordered_map<Address, P2::BasicBlock::Ptr> basic_blocks_;

    /** Prefer stored bytes over instruction provider.
     *
     * When true, attempts to reconstruct instructions from stored bytes before
     * using the instruction provider. This is necessary for architectures like JVM
     * that don't use traditional memory maps.
     *
     * When false (default), uses the instruction provider first and falls back to
     * stored bytes only if the provider fails. This maintains backward compatibility
     * and efficiency for traditional binary formats (ELF, PE). */
    bool preferStoredBytes_ = false;

    /**
     * Create a ROSE memory map from a FlatBuffer memory map.
     * This function has no side effects and assumes that the input map is non-null.
     * ROSE memory maps are byte-addressable, so intervals are created with size equal to the buffer size.
     */
    BinaryAnalysis::MemoryMap::Ptr mmap(const MemoryMap* map) const;

    /**
     * Reconstruct instruction from stored bytes.
     *
     * Creates a temporary memory map containing the stored instruction bytes and uses the
     * partitioner's architecture to disassemble the instruction. This method is used as a
     * fallback when the instruction provider cannot reconstruct the instruction from the
     * main memory map (e.g., for architectures like JVM that don't use memory maps).
     *
     * @param addr The address of the instruction
     * @param bytes The stored instruction bytes from the FlatBuffer
     * @return The disassembled instruction, or nullptr if disassembly fails
     */
    SgAsmInstruction* disassembleFromBytes(Address addr, const flatbuffers::Vector<uint8_t>* bytes) const;

    /**
     * Deserialization factory methods. Each of these methods is responsible for updating deserialization state.
     * They each assume that the input FlatBuffer pointer is non-null.
     *
     * The basicBlock, function, and cfg factory methods additionally modify the partitioner by attaching the built
     * object (or updating the CFG structure).
     */

    // The instruction method makes no assumptions other than that the input pointer is non-null.
    void instruction(const Instruction* const& instr);
    // The basicBlock method assumes that all instructions have been discovered and that instructions_ is up-to-date.
    void basicBlock(const BasicBlock* const& bb);
    // The function method assumes that all basic blocks have been discovered and that basic_blocks_ is up-to-date.
    void function(const Function* const& fun);
    // The cfg method assumes that all functions and basic blocks have been discovered and that basic_blocks_ is
    // up-to-date.
    void cfg(const Cfg* const& cfg);
};

} // namespace FlatBuffers
} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
#endif
