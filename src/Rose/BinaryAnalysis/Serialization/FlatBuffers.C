#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Serialization/FlatBuffers.h>
#include <Rose/Diagnostics.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/FlatBufferSchema.h>
#include <Rose/StringUtility.h>
#include <Rose/StringUtility/Escape.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <unistd.h>

#include <Sawyer/Access.h>

#include <boost/filesystem.hpp>

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/verifier.h>

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

class FlatBufferSerializer final: public SerialIo::Serializer {
  public:
    std::vector<char> savePartitioner(
      const Partitioner2::PartitionerConstPtr& partitioner, Serialization::ProgressCallback progress
    ) override {

        // Report initial progress
        if (progress)
            progress(0, Sawyer::Nothing(), "flatbuffers");

        Serialization::FlatBuffers::Serializer saver(partitioner);
        saver.save();

        auto buf = saver.buffer();
        ASSERT_require(buf.first != nullptr);
        const size_t nBytes = buf.second;

        // Copy the buffer to a vector
        std::vector<char> payload(buf.first, buf.first + nBytes);

        // Report completion
        if (progress)
            progress(nBytes, nBytes, "flatbuffers");

        return payload;
    }
};

class FlatBufferDeserializer final: public SerialIo::Deserializer {
  public:
    Partitioner2::PartitionerPtr loadPartitioner(
      const std::vector<char>& data, const Partitioner2::BasePartitionerSettings& settings,
      Serialization::ProgressCallback progress
    ) override {

        // Report initial progress
        if (progress)
            progress(0, data.size(), "flatbuffers");

        // Create and verify the loader
        auto loader = Serialization::FlatBuffers::Deserializer::fromBytes(std::move(data));
        if (!loader.verify())
            throw Serialization::Exception("invalid FlatBuffer partitioner data");

        // Report completion
        if (progress)
            progress(data.size(), data.size(), "flatbuffers");

        return loader.load(settings);
    }
};

// Static initialization to register the FlatBuffers backends
namespace {
struct RegisterFlatBufferSerialization {
    RegisterFlatBufferSerialization() {
        SerialIo::registerSerialization({
          Serialization::FLATBUFFERS,                                  /* format */
          []() { return std::make_shared<FlatBufferDeserializer>(); }, /* deserializer factory */
          []() { return std::make_shared<FlatBufferSerializer>(); },   /* serializer factory */
        });
    }
} registerFlatBufferSerialization;
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization and Deserialization implementation logic
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Serialization {

namespace P2       = Rose::BinaryAnalysis::Partitioner2;
namespace FB       = Rose::BinaryAnalysis::Serialization::FlatBuffers;
using Serializer   = FB::Serializer;
using Deserializer = FB::Deserializer;

// Diagnostics
static Sawyer::Message::Facility mlog;

// Initialize diagnostics
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Serialization::FlatBuffers");
        mlog.comment("FlatBuffers serialization and deserialization");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers: enum conversions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static FB::Endianness
toFBEndianness(ByteOrder::Endianness order) {
    switch (order) {
    case ByteOrder::ORDER_LSB:
        return FB::Endianness::ORDER_LSB;
    case ByteOrder::ORDER_MSB:
        return FB::Endianness::ORDER_MSB;
    case ByteOrder::ORDER_UNSPECIFIED:
    default:
        return FB::Endianness::ORDER_UNSPECIFIED;
    }
}

static ByteOrder::Endianness
fromFBEndianness(FB::Endianness order) {
    switch (order) {
    case FB::Endianness::ORDER_LSB:
        return ByteOrder::ORDER_LSB;
    case FB::Endianness::ORDER_MSB:
        return ByteOrder::ORDER_MSB;
    case FB::Endianness::ORDER_UNSPECIFIED:
    default:
        return ByteOrder::ORDER_UNSPECIFIED;
    }
}

static FB::CfgEdgePurpose
toFBEdgePurpose(const P2::EdgeType& t) {
    switch (t) {
    case P2::EdgeType::E_NORMAL:
        return FB::CfgEdgePurpose::Normal;
    case P2::EdgeType::E_FUNCTION_CALL:
        return FB::CfgEdgePurpose::FunctionCall;
    case P2::EdgeType::E_FUNCTION_RETURN:
        return FB::CfgEdgePurpose::FunctionReturn;
    case P2::EdgeType::E_CALL_RETURN:
        return FB::CfgEdgePurpose::CallReturn;
    case P2::EdgeType::E_FUNCTION_XFER:
        return FB::CfgEdgePurpose::FunctionTransfer;
    case P2::EdgeType::E_USER_DEFINED:
        throw Exception("Cannot serialize user edges");
    default:
        throw Exception("Unhandled CFG edge type");
    }
}

static P2::EdgeType
fromFBEdgePurpose(const FB::CfgEdgePurpose& t) {
    switch (t) {
    case FB::CfgEdgePurpose::Normal:
        return P2::EdgeType::E_NORMAL;
    case FB::CfgEdgePurpose::FunctionCall:
        return P2::EdgeType::E_FUNCTION_CALL;
    case FB::CfgEdgePurpose::FunctionReturn:
        return P2::EdgeType::E_FUNCTION_RETURN;
    case FB::CfgEdgePurpose::CallReturn:
        return P2::EdgeType::E_CALL_RETURN;
    case FB::CfgEdgePurpose::FunctionTransfer:
        return P2::EdgeType::E_FUNCTION_XFER;
    default:
        throw Exception("Unhandled CFG edge type");
    }
}

static uint8_t
toAccessibilityMask(unsigned aBits) {
    // MemoryMap stores a bitmask in an unsigned.
    const Sawyer::Access::Access a = Sawyer::Access::Access(aBits);

    uint8_t m = 0;
    if (a & Sawyer::Access::EXECUTABLE)
        m |= static_cast<uint8_t>(FB::Accessibility::EXECUTABLE);
    if (a & Sawyer::Access::WRITABLE)
        m |= static_cast<uint8_t>(FB::Accessibility::WRITABLE);
    if (a & Sawyer::Access::READABLE)
        m |= static_cast<uint8_t>(FB::Accessibility::READABLE);
    if (a & Sawyer::Access::IMMUTABLE)
        m |= static_cast<uint8_t>(FB::Accessibility::IMMUTABLE);
    if (a & Sawyer::Access::PRIVATE)
        m |= static_cast<uint8_t>(FB::Accessibility::PRIVATE);
    return m;
}

static unsigned
fromAccessibilityMask(uint8_t m) {
    unsigned a = 0;
    if (m & static_cast<uint8_t>(FB::Accessibility::EXECUTABLE))
        a |= Sawyer::Access::EXECUTABLE;
    if (m & static_cast<uint8_t>(FB::Accessibility::WRITABLE))
        a |= Sawyer::Access::WRITABLE;
    if (m & static_cast<uint8_t>(FB::Accessibility::READABLE))
        a |= Sawyer::Access::READABLE;
    if (m & static_cast<uint8_t>(FB::Accessibility::IMMUTABLE))
        a |= Sawyer::Access::IMMUTABLE;
    if (m & static_cast<uint8_t>(FB::Accessibility::PRIVATE))
        a |= Sawyer::Access::PRIVATE;
    return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers: save individual objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Serializer::Handle<FB::Architecture>
Serializer::architecture(const BinaryAnalysis::Architecture::BaseConstPtr& arch) {
    ASSERT_not_null(arch);
    const auto name   = partitioner_->architecture()->name();
    auto       fbName = builder_->CreateString(name);

    return FB::CreateArchitecture(*builder_, fbName);
}

Serializer::Handle<FB::Instruction>
Serializer::instruction(const SgAsmInstruction* const& insn) {
    ASSERT_not_null(insn);
    const auto& raw     = insn->get_rawBytes();
    auto        fbBytes = builder_->CreateVector(reinterpret_cast<const uint8_t*>(raw.data()), raw.size());

    auto mnemonic = builder_->CreateString(insn->get_mnemonic());

    auto operands = 0;

    return FB::CreateInstruction(*builder_, insn->get_address(), fbBytes, mnemonic, operands);
}

Serializer::Handle<FB::BasicBlock>
Serializer::basicBlock(const P2::BasicBlockPtr& bb) {
    ASSERT_not_null(bb);

    std::vector<uint64_t> insns;
    insns.reserve(bb->nInstructions());
    for (SgAsmInstruction* insn : bb->instructions())
        insns.push_back(insn->get_address());

    auto fbInsns = builder_->CreateVector(insns);
    return FB::CreateBasicBlock(*builder_, fbInsns);
}

Serializer::Handle<FB::Function>
Serializer::function(const P2::FunctionPtr& f) {
    ASSERT_not_null(f);

    std::vector<uint64_t> insns;
    insns.reserve(f->nBasicBlocks());
    for (const auto& bb_va : f->basicBlockAddresses()) {
        if (const auto& bb = partitioner_->basicBlockExists(bb_va)) {
            for (const auto& isn : bb->instructions())
                insns.push_back(isn->get_address());
        }
    }

    const auto fb_insns = builder_->CreateVector(insns);
    const auto fb_name  = builder_->CreateString(f->name());

    return FB::CreateFunction(*builder_, f->address(), fb_insns, fb_name);
}

Serializer::Handle<FB::CfgEdge>
Serializer::cfgEdge(const P2::ControlFlowGraph::Edge& e) {
    const auto fromVal = e.source()->value();
    const auto toVal   = e.target()->value();

    FB::CfgEdgeTarget        target_type;
    Serializer::Handle<void> fb_target;

    if (toVal.optionalAddress() && partitioner_->basicBlockExists(toVal.address())) {
        target_type = FB::CfgEdgeTarget::AddressTarget;
        fb_target   = FB::CreateAddressTarget(*builder_, toVal.address()).Union();
    } else {
        target_type = FB::CfgEdgeTarget::IndeterminateTarget;
        fb_target   = FB::CreateIndeterminateTarget(*builder_).Union();
    }

    auto purpose = FB::CreateCfgEdgeData(*builder_, toFBEdgePurpose(e.value().type()));

    return FB::CreateCfgEdge(*builder_, fromVal.address(), target_type, fb_target, purpose);
}

Serializer::Handle<FB::Cfg>
Serializer::cfg(const P2::ControlFlowGraph& cfg) {

    std::vector<Serializer::Handle<FB::CfgEdge>> edges;

    for (const auto& e : cfg.edges())
        edges.push_back(cfgEdge(e));

    const auto fb_edges = builder_->CreateVector(edges);

    return FB::CreateCfg(*builder_, fb_edges);
}

Serializer::Handle<FB::Segment>
Serializer::segment(const BinaryAnalysis::MemoryMap::Super::Node& node) {
    const auto seg = node.value();
    const auto buf = seg.buffer();

    std::vector<uint8_t> data(buf->data(), buf->data() + buf->size());
    auto                 fb_bytes = builder_->CreateVector(data);

    const auto access = toAccessibilityMask(seg.accessibility());
    const auto name   = builder_->CreateString(seg.name());

    return FB::CreateSegment(*builder_, node.key().least(), fb_bytes, access, name);
}

Serializer::Handle<FB::MemoryMap>
Serializer::mmap(const BinaryAnalysis::MemoryMap& map) {
    auto endianness = toFBEndianness(map.byteOrder());

    std::vector<Serializer::Handle<FB::Segment>> segments;
    segments.reserve(map.size());

    mlog[TRACE] << "serializing memory map with " << map.nSegments() << " segments" << std::endl;

    for (const auto& node : map.nodes()) {
        mlog[TRACE] << "  segment at " << StringUtility::addrToString(node.key().least())
                    << " size=" << node.key().size() << " buffer_size=" << node.value().buffer()->size() << std::endl;
        segments.push_back(segment(node));
    }

    const auto fb_segs = builder_->CreateVector(segments);

    mlog[TRACE] << "created " << segments.size() << " FlatBuffer segments" << std::endl;

    return FB::CreateMemoryMap(*builder_, endianness, fb_segs);
}

std::pair<Serializer::Handle<FB::InstructionList>, Serializer::Handle<FB::BasicBlockList>>
Serializer::instructionsBasicBlocks(const std::vector<P2::BasicBlockPtr>& p_bbs) {

    std::vector<Handle<FB::Instruction>> instrs;
    std::vector<Handle<FB::BasicBlock>>  bbs;

    AddressIntervalSet seen_instrs;

    for (const auto& bb : p_bbs) {

        ASSERT_always_not_null2(bb, "Null basic block in serialization");

        std::vector<Address> bb_addrs;
        bb_addrs.reserve(bb->nInstructions());

        for (const auto& instr : bb->instructions()) {

            const auto instr_addr = instr->get_address();

            bb_addrs.emplace_back(instr_addr);

            if (!seen_instrs.contains(instr_addr)) {
                instrs.emplace_back(instruction(instr));
                seen_instrs.insert(instr_addr);
            }
        }

        bbs.emplace_back(basicBlock(bb));
    }

    const auto fb_instrs = FB::CreateInstructionList(*builder_, builder_->CreateVector(instrs));
    const auto fb_bbs    = FB::CreateBasicBlockList(*builder_, builder_->CreateVector(bbs));

    return std::make_pair(fb_instrs, fb_bbs);
}

Serializer::Handle<FB::FunctionList>
Serializer::functions(const std::vector<Partitioner2::FunctionPtr>& p_funs) {

    std::vector<Handle<FB::Function>> funs;

    for (const auto fun : p_funs)
        funs.emplace_back(function(fun));

    return FB::CreateFunctionList(*builder_, builder_->CreateVector(funs));
}

Serializer::Handle<FB::Root>
Serializer::partitioner() {

    mlog[TRACE] << "starting serialization" << std::endl;
    mlog[TRACE] << "memory map has " << partitioner_->memoryMap()->nSegments() << " segments" << std::endl;

    // Save major structures.
    auto fbArch = architecture(partitioner_->architecture());
    auto fbMap  = mmap(*partitioner_->memoryMap());
    auto fbCfg  = cfg(partitioner_->cfg());

    const auto instrs_bbs = instructionsBasicBlocks(partitioner_->basicBlocks());
    const auto funs       = functions(partitioner_->functions());

    mlog[TRACE] << "creating root with memory map" << std::endl;
    return FB::CreateRoot(*builder_, fbArch, fbMap, instrs_bbs.first, instrs_bbs.second, funs, fbCfg);
}

// Top-level serializer methods

Serializer::Serializer(const P2::PartitionerConstPtr& p) :
  partitioner_(p), builder_{std::make_unique<flatbuffers::FlatBufferBuilder>(1024 * 1024)} {
    initDiagnostics();
}

void
Serializer::save() {

    auto root = partitioner();
    builder_->Finish(root);

    bytes_.assign(builder_->GetBufferPointer(), builder_->GetBufferPointer() + builder_->GetSize());
}

std::pair<const uint8_t*, size_t>
Serializer::buffer() const {
    return {reinterpret_cast<const uint8_t*>(bytes_.data()), bytes_.size()};
}

void
Serializer::write(std::ostream& o) const {
    o.write(reinterpret_cast<const char*>(bytes_.data()), static_cast<std::streamsize>(bytes_.size()));
}

void
Serializer::write(const boost::filesystem::path& p) const {
    std::ofstream f(p.string().c_str(), std::ios::binary);
    if (!f)
        throw std::runtime_error("cannot open \"" + p.string() + "\" for writing");
    write(f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deserializer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Deserializer
Deserializer::fromFile(const boost::filesystem::path& p) {
    std::ifstream f(p.string().c_str(), std::ios::binary);
    if (!f)
        throw std::runtime_error("cannot open \"" + p.string() + "\" for reading");
    return fromStream(f);
}

Deserializer
Deserializer::fromStream(std::istream& in) {
    std::vector<char> bytes;
    in.seekg(0, std::ios::end);
    std::streampos end = in.tellg();
    in.seekg(0, std::ios::beg);
    if (end > 0) {
        bytes.resize(static_cast<size_t>(end));
        in.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    } else {
        // Non-seekable stream
        std::vector<char> buf(4096);
        while (in) {
            in.read(buf.data(), static_cast<std::streamsize>(buf.size()));
            auto n = in.gcount();
            if (n > 0)
                bytes.insert(bytes.end(), buf.begin(), buf.begin() + n);
        }
    }
    return fromBytes(std::move(bytes));
}

Deserializer
Deserializer::fromBytes(std::vector<char>&& bytes) {
    Deserializer loader;
    loader.bytes_ = std::move(bytes); // std::move on && is a true move
    return loader;
}

Deserializer
Deserializer::fromBytes(const std::vector<char>& bytes) {
    Deserializer loader;
    loader.bytes_ = std::move(bytes); // std::move on const& is a copy
    return loader;
}

bool
Deserializer::verify() const {
    flatbuffers::Verifier v(reinterpret_cast<const uint8_t*>(bytes_.data()), bytes_.size());
    return v.VerifyBuffer<FB::Root>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deserializer helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BinaryAnalysis::MemoryMap::Ptr
Deserializer::mmap(const FB::MemoryMap* map) const {
    auto ba_map = BinaryAnalysis::MemoryMap::instance();
    ba_map->byteOrder(fromFBEndianness(map->endianness()));

    mlog[TRACE] << "map=" << (map ? "valid" : "null") << std::endl;
    if (map) {
        mlog[TRACE] << "segments=" << (map->segments() ? "valid" : "null") << std::endl;
        if (map->segments()) {
            mlog[TRACE] << "deserializing " << map->segments()->size() << " segments" << std::endl;
        }
    }

    int segmentCount = 0;
    int skippedCount = 0;
    for (auto seg : *map->segments()) {
        segmentCount++;
        if (!seg || !seg->bytes()) {
            skippedCount++;
            mlog[WHERE] << "skipping segment #" << segmentCount;
            if (seg) {
                mlog[WHERE] << " at " << StringUtility::addrToString(seg->address());
                mlog[WHERE] << " (bytes=" << (seg->bytes() ? "valid" : "null") << ")";
            } else {
                mlog[WHERE] << " (seg=null)";
            }
            mlog[WHERE] << std::endl;
            continue;
        }

        const auto bytes = seg->bytes();
        auto       buf   = BinaryAnalysis::MemoryMap::AllocatingBuffer::instance(bytes->size());

        const auto seg_name = seg->name();

        buf->write(bytes->data(), 0, bytes->size());

        BinaryAnalysis::MemoryMap::Segment ba_seg(
          buf, 0, fromAccessibilityMask(seg->accessibility()), seg_name ? seg_name->str() : ""
        );

        // ROSE memory maps are byte-addressable, so the interval size equals the buffer size.
        // Use baseSize to create the interval with the correct size (avoids off-by-one from hull's inclusiveness)
        auto interval = AddressInterval::baseSize(seg->address(), bytes->size());
        mlog[TRACE] << "  deserializing segment at " << StringUtility::addrToString(seg->address())
                    << " size=" << bytes->size() << std::endl;
        ba_map->insert(interval, ba_seg);
    }

    mlog[TRACE] << "deserialized " << (segmentCount - skippedCount) << " segments, skipped " << skippedCount
                << std::endl;
    mlog[TRACE] << "final memory map has " << ba_map->nSegments() << " segments" << std::endl;

    return ba_map;
}

SgAsmInstruction*
Deserializer::disassembleFromBytes(Address addr, const flatbuffers::Vector<uint8_t>* bytes) const {
    // Create a temporary memory map with the stored bytes
    auto tempMap  = BinaryAnalysis::MemoryMap::instance();
    auto buf      = BinaryAnalysis::MemoryMap::StaticBuffer::instance(bytes->data(), bytes->size());
    auto interval = AddressInterval::baseSize(addr, bytes->size());
    tempMap->insert(
      interval, BinaryAnalysis::MemoryMap::Segment(
                  buf, 0, BinaryAnalysis::MemoryMap::READABLE | BinaryAnalysis::MemoryMap::EXECUTABLE, "temp"
                )
    );

    // Get disassembler from architecture
    auto disassembler = partitioner_->architecture()->newInstructionDecoder();

    // Disassemble from the temporary map
    try {
        return disassembler->disassembleOne(tempMap, addr);
    } catch (const Disassembler::Exception& e) {
        mlog[WHERE] << "failed to disassemble from stored bytes at " << StringUtility::addrToString(addr) << ": "
                    << e.what() << std::endl;
        return nullptr;
    }
}

void
Deserializer::instruction(const Instruction* const& fb_instr) {
    // Reconstruct instruction using either instruction provider or stored bytes.
    // The order is determined by preferStoredBytes_ flag:
    // - false (default): instruction provider first, stored bytes as fallback
    // - true (JVM, etc.): stored bytes first, instruction provider as fallback
    // This dual approach ensures compatibility across all architectures.

    const auto        instr_addr = fb_instr->address();
    SgAsmInstruction* insn       = nullptr;

    if (preferStoredBytes_) {
        // For architectures without memory maps (e.g., JVM), try stored bytes first
        const auto stored_bytes = fb_instr->bytes();
        if (stored_bytes && stored_bytes->size() > 0) {
            mlog[TRACE] << "reconstructing from stored bytes at " << StringUtility::addrToString(instr_addr) << " ("
                        << stored_bytes->size() << " bytes)" << std::endl;
            insn = disassembleFromBytes(instr_addr, stored_bytes);
            if (insn) {
                mlog[TRACE] << "  successfully created instruction, actual address: "
                            << StringUtility::addrToString(insn->get_address()) << std::endl;
            }
        }

        // Fall back to instruction provider if stored bytes failed
        if (!insn) {
            const auto instr_prov = partitioner_->instructionProvider();
            insn                  = instr_prov.at(instr_addr);
        }
    } else {
        // Default behavior: use instruction provider first
        const auto instr_prov = partitioner_->instructionProvider();
        insn                  = instr_prov.at(instr_addr);

        // Fall back to stored bytes if provider failed
        if (!insn) {
            const auto stored_bytes = fb_instr->bytes();
            if (stored_bytes && stored_bytes->size() > 0) {
                insn = disassembleFromBytes(instr_addr, stored_bytes);
            }
        }
    }

    if (!insn) {
        mlog[WHERE] << "bad instruction at " << StringUtility::addrToString(instr_addr) << std::endl;
        mlog[WHERE] << "memory map: " << *partitioner_->memoryMap() << std::endl;
        ASSERT_always_not_null2(
          insn, "Could not disassemble instruction at " + StringUtility::addrToString(instr_addr)
        );
    }

    instructions_[instr_addr] = insn;
}

void
Deserializer::basicBlock(const BasicBlock* const& fb_bb) {

    const auto bb_addr = *fb_bb->addresses()->begin();
    auto       bb      = P2::BasicBlock::instance(bb_addr, partitioner_);

    for (const auto& instr_addr : *fb_bb->addresses()) {
        ASSERT_always_require2(instructions_.count(instr_addr), "Missing instruction at address " + instr_addr);
        bb->append(partitioner_, instructions_[instr_addr]);
    }

    basic_blocks_[bb_addr] = bb;
    partitioner_->attachBasicBlock(bb);
}

void
Deserializer::function(const Function* const& fb_fun) {

    auto fun = P2::Function::instance(fb_fun->entry_addr(), fb_fun->name()->str());

    // For weird functions with no actual body, insert an empty basic block.
    // Normal functions (with actual bodies) will be handled in the main case.

    if (!basic_blocks_.count(fb_fun->entry_addr())) {
        auto bb = P2::BasicBlock::instance(fb_fun->entry_addr(), partitioner_);
        partitioner_->attachBasicBlock(bb);
        fun->insertBasicBlock(fb_fun->entry_addr());
    }

    for (const auto& instr_addr : *fb_fun->instructions()) {
        if (basic_blocks_.count(instr_addr)) {
            fun->insertBasicBlock(instr_addr);
        }
    }

    partitioner_->attachFunction(fun);
}

void
Deserializer::cfg(const Cfg* const& fb_cfg) {
    if (!fb_cfg || !fb_cfg->edges())
        return;

    mlog[TRACE] << "restoring CFG with " << fb_cfg->edges()->size() << " edges" << std::endl;

    // Phase 1: Ensure all vertices exist in the CFG
    // Collect all addresses that should have vertices
    std::unordered_set<Address> serializedAddresses;
    for (const auto* fb_edge : *fb_cfg->edges()) {
        // Source address always exists
        serializedAddresses.insert(fb_edge->src());

        // Target address (if not a special vertex)
        if (fb_edge->tgt_type() == FB::CfgEdgeTarget::AddressTarget) {
            const auto* addrTarget = fb_edge->tgt_as_AddressTarget();
            if (addrTarget) {
                serializedAddresses.insert(addrTarget->address());
            }
        }
    }

    // Create placeholders for any missing vertices
    for (const auto& addr : serializedAddresses) {
        partitioner_->insertPlaceholder(addr);
    }

    // Phase 2: Clear auto-generated edges from basic block vertices
    // Only clear from normal blocks, leave special vertices alone
    auto& cfg = partitioner_->cfg_;
    for (auto vertex = cfg.vertices().begin(); vertex != cfg.vertices().end(); ++vertex) {
        if (vertex->value().type() == P2::V_BASIC_BLOCK) {
            const auto& bblock = vertex->value().bblock();
            // Only clear edges from normal blocks (not empty, not placeholders)
            if (bblock && !bblock->isEmpty())
                cfg.clearOutEdges(vertex);
        }
    }

    mlog[TRACE] << "cleared auto-generated edges, restoring " << fb_cfg->edges()->size() << " serialized edges"
                << std::endl;

    // Phase 3: Restore serialized edges as ground truth
    size_t edgesRestored = 0;
    for (const auto* fb_edge : *fb_cfg->edges()) {
        // Find source vertex by address
        auto source = partitioner_->findPlaceholder(fb_edge->src());
        if (source == cfg.vertices().end()) {
            mlog[WARN] << "cannot find source vertex at " << StringUtility::addrToString(fb_edge->src())
                       << ", skipping edge" << std::endl;
            continue;
        }

        // Find or identify target vertex
        P2::ControlFlowGraph::VertexIterator target;
        if (fb_edge->tgt_type() == FB::CfgEdgeTarget::AddressTarget) {
            const auto* addrTarget = fb_edge->tgt_as_AddressTarget();
            if (addrTarget) {
                target = partitioner_->findPlaceholder(addrTarget->address());
                if (target == cfg.vertices().end()) {
                    mlog[WARN] << "cannot find target vertex at " << StringUtility::addrToString(addrTarget->address())
                               << ", skipping edge" << std::endl;
                    continue;
                }
            } else {
                mlog[WARN] << "null address target, skipping edge" << std::endl;
                continue;
            }
        } else if (fb_edge->tgt_type() == FB::CfgEdgeTarget::IndeterminateTarget) {
            target = partitioner_->indeterminateVertex();
        } else {
            mlog[WARN] << "unknown edge target type, skipping edge" << std::endl;
            continue;
        }

        // Create edge with serialized type and confidence
        const auto* edgeData = fb_edge->data();
        if (!edgeData) {
            mlog[WARN] << "missing edge data, skipping edge" << std::endl;
            continue;
        }

        P2::EdgeType edgeType = fromFBEdgePurpose(edgeData->purpose());
        P2::CfgEdge  edge(edgeType, P2::ASSUMED); // Default confidence

        // Insert the edge
        cfg.insertEdge(source, target, edge);
        edgesRestored++;
    }

    mlog[TRACE] << "restored " << edgesRestored << " CFG edges" << std::endl;

    // Phase 4: Ensure special vertex invariants are maintained
    size_t placeholdersFix = 0, nonexistingFixed = 0;
    for (auto vertex = cfg.vertices().begin(); vertex != cfg.vertices().end(); ++vertex) {
        if (vertex->value().type() != P2::V_BASIC_BLOCK)
            continue;

        const auto& bblock = vertex->value().bblock();

        if (!bblock) {
            // Pure placeholder - MUST have exactly 1 edge to undiscovered
            if (vertex->nOutEdges() == 0) {
                cfg.insertEdge(vertex, partitioner_->undiscoveredVertex());
                placeholdersFix++;
            } else if (vertex->nOutEdges() > 1) {
                cfg.clearOutEdges(vertex);
                cfg.insertEdge(vertex, partitioner_->undiscoveredVertex());
                placeholdersFix++;
                mlog[WARN] << "placeholder at " << StringUtility::addrToString(vertex->value().address())
                           << " had multiple edges, replaced with undiscovered edge" << std::endl;
            } else {
                auto edge = vertex->outEdges().begin();
                auto target = edge->target();
                if (target != partitioner_->undiscoveredVertex() && target != partitioner_->indeterminateVertex() &&
                    target != partitioner_->nonexistingVertex()) {
                    cfg.clearOutEdges(vertex);
                    cfg.insertEdge(vertex, partitioner_->undiscoveredVertex());
                    placeholdersFix++;
                    mlog[WARN] << "placeholder at " << StringUtility::addrToString(vertex->value().address())
                               << " had edge to non-special vertex" << std::endl;
                }
            }
        } else if (bblock->isEmpty()) {
            // Nonexisting block - MUST have exactly 1 edge to nonexisting
            if (vertex->nOutEdges() != 1 || vertex->outEdges().begin()->target() != partitioner_->nonexistingVertex()) {
                cfg.clearOutEdges(vertex);
                cfg.insertEdge(vertex, partitioner_->nonexistingVertex(), P2::CfgEdge(P2::E_USER_DEFINED));
                nonexistingFixed++;
            }
        }
    }

    mlog[TRACE] << "fixed " << placeholdersFix << " placeholders and " << nonexistingFixed << " nonexisting blocks"
                << std::endl;
}

P2::PartitionerPtr
Deserializer::load() {
    const Partitioner2::BasePartitionerSettings settings;
    return load(settings);
}

P2::PartitionerPtr
Deserializer::load(const P2::BasePartitionerSettings& settings) {
    initDiagnostics();

    if (!verify())
        throw Exception("invalid flatbuffer data for Partitioner");

    const FB::Root* root = FB::GetRoot(bytes_.data());
    ASSERT_not_null(root);

    mlog[TRACE] << "root=" << (root ? "valid" : "null") << std::endl;
    mlog[TRACE] << "root->memory_map()=" << (root->memory_map() ? "valid" : "null") << std::endl;

    const auto arch_name    = root->architecture()->name()->str();
    const auto default_arch = "ppc32-be";

    mlog[TRACE] << "deserializing as arch: " << arch_name << std::endl;

    auto arch =
      BinaryAnalysis::Architecture::findByName(arch_name)
        .orElse([arch_name](const auto& e) -> auto {
            mlog[WHERE] << "unknown architecture, defaulting to PowerPC: " << arch_name << std::endl;
            return Sawyer::makeError(e);
        })
        .orElse(BinaryAnalysis::Architecture::findByName(default_arch))
        .orThrow(Exception("Unknown or missing architecture name: " + arch_name + " (default: " + default_arch + ")"));

    // Set preference for architectures without memory maps
    if (arch_name == "jvm") {
        preferStoredBytes_ = true;
        mlog[TRACE] << "JVM architecture detected, preferring stored bytes" << std::endl;
    }

    mlog[TRACE] << "about to call mmap(root->memory_map())" << std::endl;
    auto map = mmap(root->memory_map());
    mlog[TRACE] << "after mmap, map has " << map->nSegments() << " segments" << std::endl;

    // Create a fresh partitioner.
    partitioner_ = P2::Partitioner::instance(arch, map);

    partitioner_->settings(settings);

    for (const auto& fb_instr : *root->instructions()->instructions())
        instruction(fb_instr);

    for (const auto& fb_bb : *root->basic_blocks()->basic_blocks())
        basicBlock(fb_bb);

    for (const auto& fb_fun : *root->functions()->functions())
        function(fb_fun);

    cfg(root->cfg());

    partitioner_->checkConsistency();

    return partitioner_;
}

} // namespace Serialization

} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif