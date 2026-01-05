#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Serialization/FlatBuffer.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/FlatBufferSchema.h>
#include <Rose/StringUtility/Escape.h>

#include <unistd.h>

#include <sage3basic.h>

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

namespace Rose {
namespace BinaryAnalysis {

class FlatBufferSerializer final: public SerialIo::Serializer {
  public:
    std::vector<char> savePartitioner(
      const Partitioner2::PartitionerConstPtr& partitioner, Serialization::ProgressCallback progress
    ) override {

        Serialization::FlatBuffers::Serializer saver(partitioner);
        saver.save();

        auto buf = saver.buffer();
        ASSERT_require(buf.first != nullptr);
        const size_t nBytes = buf.second;

        // Report initial progress
        if (progress)
            progress(0, nBytes, "flatbuffers");

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
    Partitioner2::PartitionerPtr
    loadPartitioner(const std::vector<char>& data, Serialization::ProgressCallback progress) override {

        // Report initial progress
        if (progress)
            progress(0, data.size(), "flatbuffer");

        // Create and verify the loader
        auto loader = Serialization::FlatBuffers::Deserializer::fromBytes(std::move(data));
        if (!loader.verify())
            throw Serialization::Exception("invalid FlatBuffer partitioner data");

        // Report completion
        if (progress)
            progress(data.size(), data.size(), "flatbuffer");

        return loader.load();
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
toFBEdgePurpose(P2::EdgeType t) {
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

Serializer::Handle<FB::Instruction>
Serializer::instruction(const SgAsmInstruction*& insn) {
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

    // JS: Why not directly save basic blocks instead of instructions?

    // std::vector<FB::BasicBlockRef> bbs;
    // bbs.reserve(f->nBasicBlocks());
    // for (Address va : f->basicBlockAddresses())
    //     bbs.push_back(FB::BasicBlockRef(va));

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

    if (partitioner_->basicBlockExists(toVal.address())) {
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

    for (const auto& node : map.nodes())
        segments.push_back(segment(node));

    const auto fb_segs = builder_->CreateVector(segments);

    return FB::CreateMemoryMap(*builder_, endianness, fb_segs);
}

Serializer::Serializer(const P2::PartitionerConstPtr& p) :
  partitioner_(p), builder_{std::make_unique<flatbuffers::FlatBufferBuilder>(1024 * 1024)} {}

Serializer::Handle<FB::Root>
Serializer::partitioner() {
    // Architecture info, currently unused (but will be used in the future)
    ASSERT_not_null(partitioner_->architecture());
    const auto archName   = partitioner_->architecture()->name();
    auto       fbArchName = builder_->CreateString(archName);

    const auto functions = partitioner_->functions();

    // Save major structures.
    auto fbMap = mmap(*partitioner_->memoryMap());
    auto fbCfg = cfg(partitioner_->cfg());

    // TODO: instructions, functions, and basic blocks

    return FB::CreateRoot(*builder_, fbMap, 0, 0, 0, fbCfg);
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

Deserializer::Deserializer() {}

Deserializer::~Deserializer() {}

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

    for (auto seg : *map->segments()) {
        if (!seg)
            continue;

        const auto bytes = seg->bytes();
        auto       buf   = BinaryAnalysis::MemoryMap::StaticBuffer::instance(bytes->data(), bytes->size());

        BinaryAnalysis::MemoryMap::Segment ba_seg(
          buf, 0, fromAccessibilityMask(seg->accessibility()), seg->name()->str()
        );

        auto interval = AddressInterval::hull(seg->address(), seg->address() + bytes->size());

        ba_map->insert(interval, ba_seg);
    }

    return ba_map;
}

P2::PartitionerPtr
Deserializer::load() const {
    if (!verify())
        throw Exception("invalid flatbuffer data for Partitioner");

    const FB::Root* root = FB::GetRoot(bytes_.data());
    ASSERT_not_null(root);

    // const auto arch_name = root->architecture_name();
    const auto arch_name = "ppc"; // TODO
    // if (!arch_name)
    //     throw Exception("serialized partitioner has no architecture name");

    // auto arch = Architecture::findByName(arch_name->str()).orThrow();
    auto arch = Architecture::findByName(arch_name).orThrow();
    auto map  = mmap(root->memory_map());

    // Create a fresh partitioner.
    auto partitioner = P2::Partitioner::instance(arch, map);

    // First recreate datablocks. They are relatively simple and are consumed "by reference" (vs by address),
    // so we save a map from datablock start address to datablock reference (pointer).

    using DataBlock = Rose::BinaryAnalysis::Partitioner2::DataBlock;

    std::unordered_map<Address, DataBlock::Ptr> data_blocks;

    // const auto fb_dbs = *root->data

    const auto fb_cfg = root->cfg();
    if (!fb_cfg)
        throw Exception("serialized partitioner has no CFG");

    // First, re-discover basic blocks, which (should) disassemble the contents
    // if (fb_cfg->basic_blocks()) {
    //     for (auto fbBb : *fb_cfg->basic_blocks()) {
    //         if (!fbBb)
    //             continue;
    //         auto bb = partitioner->discoverBasicBlock(fbBb->address());
    //         if (bb)
    //             partitioner->attachBasicBlock(bb);
    //     }
    // }

    // // Attach functions and placeholders.
    // if (fb_cfg->functions()) {
    //     for (auto fb_fun : *fb_cfg->functions()) {
    //         if (!fb_fun)
    //             continue;
    //         auto fun =
    //           P2::Function::instance(fb_fun->address(), fb_fun->name() ? fb_fun->name()->str() : std::string());

    //         // Add basic blocks to function
    //         if (fb_fun->basic_blocks()) {
    //             for (auto fb_bb : *fb_fun->basic_blocks())
    //                 fun->insertBasicBlock(fb_bb->address());
    //         }

    //         // Add datablocks to function
    //         if (fb_fun->data_blocks()) {
    //             for (auto fb_db : *fb_fun->data_blocks()) {
    //                 if (const auto db = data_blocks.at(fb_db->address()))
    //                     fun->insertDataBlock(db);
    //             }
    //         }

    //         partitioner->attachFunction(fun);
    //     }
    // }

    return partitioner;
}

} // namespace Serialization

} // namespace BinaryAnalysis
} // namespace Rose

#endif