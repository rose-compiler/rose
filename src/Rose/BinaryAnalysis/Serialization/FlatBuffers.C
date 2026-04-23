#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Serialization/FlatBuffers.h>

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
    Partitioner2::PartitionerPtr
    loadPartitioner(const std::vector<char>& data, Serialization::ProgressCallback progress) override {

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

    for (const auto& node : map.nodes())
        segments.push_back(segment(node));

    const auto fb_segs = builder_->CreateVector(segments);

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

    // Save major structures.
    auto fbArch = architecture(partitioner_->architecture());
    auto fbMap  = mmap(*partitioner_->memoryMap());
    auto fbCfg  = cfg(partitioner_->cfg());

    const auto instrs_bbs = instructionsBasicBlocks(partitioner_->basicBlocks());
    const auto funs       = functions(partitioner_->functions());

    return FB::CreateRoot(*builder_, fbArch, fbMap, instrs_bbs.first, instrs_bbs.second, funs, fbCfg);
}

// Top-level serializer methods

Serializer::Serializer(const P2::PartitionerConstPtr& p) :
  partitioner_(p), builder_{std::make_unique<flatbuffers::FlatBufferBuilder>(1024 * 1024)} {}

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
Deserializer::mmap(const FB::MemoryMap* map, const size_t& object_width) const {
    auto ba_map = BinaryAnalysis::MemoryMap::instance();
    ba_map->byteOrder(fromFBEndianness(map->endianness()));

    for (auto seg : *map->segments()) {
        if (!seg)
            continue;

        const auto bytes = seg->bytes();
        auto       buf   = BinaryAnalysis::MemoryMap::AllocatingBuffer::instance(bytes->size());

        buf->write(bytes->data(), 0, bytes->size());

        BinaryAnalysis::MemoryMap::Segment ba_seg(
          buf, 0, fromAccessibilityMask(seg->accessibility()), seg->name()->str()
        );

        auto interval = AddressInterval::hull(
          seg->address(), seg->address() + bytes->size() * object_width
        );

        ba_map->insert(interval, ba_seg);
    }

    return ba_map;
}

void
Deserializer::instruction(const Instruction* const& fb_instr) {

    const auto        instr_addr = fb_instr->address();
    const auto        instr_prov = partitioner_->instructionProvider();
    SgAsmInstruction* insn       = instr_prov.at(instr_addr); // Automatically disassembles the address

    instructions_[instr_addr] = insn;
}

void
Deserializer::basicBlock(const BasicBlock* const& fb_bb) {

    const auto bb_addr = *fb_bb->addresses()->begin();
    auto       bb      = P2::BasicBlock::instance(bb_addr, partitioner_);

    for (const auto& instr_addr : *fb_bb->addresses())
        bb->append(partitioner_, instructions_[instr_addr]);

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
Deserializer::cfg(const Cfg* const& fb_cfg) {}

P2::PartitionerPtr
Deserializer::load() {
    if (!verify())
        throw Exception("invalid flatbuffer data for Partitioner");

    const FB::Root* root = FB::GetRoot(bytes_.data());
    ASSERT_not_null(root);

    const auto arch_name    = root->architecture()->name()->str();
    const auto default_arch = "ppc32-be";

    auto arch =
      BinaryAnalysis::Architecture::findByName(arch_name)
        .orElse([arch_name](const auto& e) -> auto {
            std::cerr << "Unknown architecture, defaulting to PowerPC: " << arch_name << std::endl;
            return Sawyer::makeError(e);
        })
        .orElse(BinaryAnalysis::Architecture::findByName(default_arch))
        .orThrow(Exception("Unknown or missing architecture name: " + arch_name + " (default: " + default_arch + ")"));
    auto map = mmap(root->memory_map(), arch->bitsPerWord());

    // Create a fresh partitioner.
    partitioner_ = P2::Partitioner::instance(arch, map);

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