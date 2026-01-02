#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <Rose/BinaryAnalysis/Serialization/FlatbufferStorage.h>

#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Serialization/FlatbufferSchema.h>

#include <Rose/StringUtility/Escape.h>

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
namespace Serialization {

namespace P2  = Rose::BinaryAnalysis::Partitioner2;
namespace Fbs = Rose::BinaryAnalysis::Serialization::Flatbuffers;

static constexpr uint32_t kFlatbufferPartitionerVersion = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers: enum conversions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Fbs::Endianness
toFbsEndianness(ByteOrder::Endianness order) {
    switch (order) {
    case ByteOrder::ORDER_LSB:
        return Fbs::Endianness::ORDER_LSB;
    case ByteOrder::ORDER_MSB:
        return Fbs::Endianness::ORDER_MSB;
    case ByteOrder::ORDER_UNSPECIFIED:
    default:
        return Fbs::Endianness::ORDER_UNSPECIFIED;
    }
}

static ByteOrder::Endianness
fromFbsEndianness(Fbs::Endianness order) {
    switch (order) {
    case Fbs::Endianness::ORDER_LSB:
        return ByteOrder::ORDER_LSB;
    case Fbs::Endianness::ORDER_MSB:
        return ByteOrder::ORDER_MSB;
    case Fbs::Endianness::ORDER_UNSPECIFIED:
    default:
        return ByteOrder::ORDER_UNSPECIFIED;
    }
}

static Fbs::CFGVertexType
toFbsVertexType(P2::VertexType t) {
    switch (t) {
    case P2::VertexType::V_BASIC_BLOCK:
        return Fbs::CFGVertexType::V_BASIC_BLOCK;
    case P2::VertexType::V_UNDISCOVERED:
        return Fbs::CFGVertexType::V_UNDISCOVERED;
    case P2::VertexType::V_INDETERMINATE:
        return Fbs::CFGVertexType::V_INDETERMINIATE;
    case P2::VertexType::V_NONEXISTING:
        return Fbs::CFGVertexType::V_NONEXISTING;
    case P2::VertexType::V_USER_DEFINED:
    default:
        return Fbs::CFGVertexType::V_USER_DEFINED;
    }
}

static Fbs::CFGEdgeType
toFbsEdgeType(P2::EdgeType t) {
    switch (t) {
    case P2::EdgeType::E_NORMAL:
        return Fbs::CFGEdgeType::E_NORMAL;
    case P2::EdgeType::E_FUNCTION_CALL:
        return Fbs::CFGEdgeType::E_FUNCTION_CALL;
    case P2::EdgeType::E_FUNCTION_RETURN:
        return Fbs::CFGEdgeType::E_FUNCTION_RETURN;
    case P2::EdgeType::E_CALL_RETURN:
        return Fbs::CFGEdgeType::E_CALL_RETURN;
    case P2::EdgeType::E_FUNCTION_XFER:
        return Fbs::CFGEdgeType::E_FUNCTION_XFER;
    case P2::EdgeType::E_USER_DEFINED:
    default:
        return Fbs::CFGEdgeType::E_USER_DEFINED;
    }
}

static Fbs::CFGEdgeConfidence
toFbsEdgeConfidence(P2::Confidence c) {
    switch (c) {
    case P2::Confidence::ASSUMED:
        return Fbs::CFGEdgeConfidence::ASSUMED;
    case P2::Confidence::PROVED:
        return Fbs::CFGEdgeConfidence::PROVED;
    default:
        return Fbs::CFGEdgeConfidence::ASSUMED;
    }
}

static uint8_t
toAccessibilityMask(unsigned aBits) {
    // MemoryMap stores a bitmask in an unsigned.
    const Sawyer::Access::Access a = Sawyer::Access::Access(aBits);

    uint8_t m = 0;
    if (a & Sawyer::Access::EXECUTABLE)
        m |= static_cast<uint8_t>(Fbs::Accessibility::EXECUTABLE);
    if (a & Sawyer::Access::WRITABLE)
        m |= static_cast<uint8_t>(Fbs::Accessibility::WRITABLE);
    if (a & Sawyer::Access::READABLE)
        m |= static_cast<uint8_t>(Fbs::Accessibility::READABLE);
    if (a & Sawyer::Access::IMMUTABLE)
        m |= static_cast<uint8_t>(Fbs::Accessibility::IMMUTABLE);
    if (a & Sawyer::Access::PRIVATE)
        m |= static_cast<uint8_t>(Fbs::Accessibility::PRIVATE);
    return m;
}

static unsigned
fromAccessibilityMask(uint8_t m) {
    unsigned a = 0;
    if (m & static_cast<uint8_t>(Fbs::Accessibility::EXECUTABLE))
        a |= Sawyer::Access::EXECUTABLE;
    if (m & static_cast<uint8_t>(Fbs::Accessibility::WRITABLE))
        a |= Sawyer::Access::WRITABLE;
    if (m & static_cast<uint8_t>(Fbs::Accessibility::READABLE))
        a |= Sawyer::Access::READABLE;
    if (m & static_cast<uint8_t>(Fbs::Accessibility::IMMUTABLE))
        a |= Sawyer::Access::IMMUTABLE;
    if (m & static_cast<uint8_t>(Fbs::Accessibility::PRIVATE))
        a |= Sawyer::Access::PRIVATE;
    return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers: save individual objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static flatbuffers::Offset<Fbs::Instruction>
saveInstruction(flatbuffers::FlatBufferBuilder& b, SgAsmInstruction* insn) {
    ASSERT_not_null(insn);
    const auto& raw     = insn->get_rawBytes();
    auto        fbBytes = b.CreateVector(reinterpret_cast<const uint8_t*>(raw.data()), raw.size());
    return Fbs::CreateInstruction(b, insn->get_address(), fbBytes);
}

static flatbuffers::Offset<Fbs::BasicBlock>
saveBasicBlock(flatbuffers::FlatBufferBuilder& b, const P2::BasicBlockPtr& bb) {
    ASSERT_not_null(bb);

    std::vector<flatbuffers::Offset<Fbs::Instruction>> insns;
    insns.reserve(bb->nInstructions());
    for (SgAsmInstruction* insn : bb->instructions())
        insns.push_back(saveInstruction(b, insn));

    auto fbInsns = b.CreateVector(insns);
    return Fbs::CreateBasicBlock(b, bb->address(), fbInsns);
}

static flatbuffers::Offset<Fbs::SourceLocation>
saveSourceLocation(flatbuffers::FlatBufferBuilder& b, const SourceLocation& sl) {
    auto fbName = b.CreateString(sl.fileName().string());

    Fbs::Column               columnType = Fbs::Column::NONE;
    flatbuffers::Offset<void> fbColumn;

    if (sl.column()) {
        columnType = Fbs::Column::SomeColumn;
        fbColumn   = Fbs::CreateSomeColumn(b, *sl.column()).Union();
    } else {
        columnType = Fbs::Column::None;
        fbColumn   = Fbs::CreateNone(b).Union();
    }

    return Fbs::CreateSourceLocation(b, fbName, sl.line(), columnType, fbColumn);
}

static flatbuffers::Offset<Fbs::Function>
saveFunction(flatbuffers::FlatBufferBuilder& b, const P2::FunctionPtr& f) {
    ASSERT_not_null(f);

    std::vector<Fbs::BasicBlockRef> bbs;
    bbs.reserve(f->nBasicBlocks());
    for (Address va : f->basicBlockAddresses())
        bbs.push_back(Fbs::BasicBlockRef(va));

    std::vector<Fbs::DataBlockRef> dbs;
    dbs.reserve(f->nDataBlocks());
    for (const P2::DataBlockPtr& db : f->dataBlocks())
        dbs.push_back(Fbs::DataBlockRef(db->address()));

    auto fbName = b.CreateString(f->name());
    auto fbBbs  = b.CreateVectorOfStructs(bbs);
    auto fbDbs  = b.CreateVectorOfStructs(dbs);
    auto fbLoc  = saveSourceLocation(b, f->sourceLocation());

    return Fbs::CreateFunction(b, f->address(), fbName, fbBbs, fbDbs, fbLoc);
}

static flatbuffers::Offset<Fbs::CFGVertex>
saveCfgVertex(flatbuffers::FlatBufferBuilder& b, const P2::ControlFlowGraph::Vertex& v) {
    const auto& val = v.value();

    std::vector<Fbs::FunctionRef> owners;
    owners.reserve(val.nOwningFunctions());
    for (const auto& fun : val.owningFunctions().values())
        owners.push_back(Fbs::FunctionRef(fun->address()));

    auto fbOwners = b.CreateVectorOfStructs(owners);

    // For now, only serialize vertices that have a concrete address.
    return Fbs::CreateCFGVertex(b, val.address(), toFbsVertexType(val.type()), fbOwners);
}

static flatbuffers::Offset<Fbs::CFGEdge>
saveCfgEdge(flatbuffers::FlatBufferBuilder& b, const P2::ControlFlowGraph::Edge& e) {
    const auto fromVal = e.source()->value();
    const auto toVal   = e.target()->value();

    // Skip edges where endpoints don't have addresses.
    // (Special vertices don't have meaningful "addresses".)
    if (!fromVal.optionalAddress() || !toVal.optionalAddress())
        return {};

    const auto& val = e.value();
    return Fbs::CreateCFGEdge(
      b, fromVal.address(), toVal.address(), toFbsEdgeType(val.type()), toFbsEdgeConfidence(val.confidence())
    );
}

static flatbuffers::Offset<Fbs::CFG>
saveCfg(
  flatbuffers::FlatBufferBuilder& b, const P2::ControlFlowGraph& cfg, const std::vector<P2::FunctionPtr>& functions
) {
    std::vector<flatbuffers::Offset<Fbs::CFGVertex>> vertices;
    vertices.reserve(cfg.nVertices());

    std::vector<flatbuffers::Offset<Fbs::CFGEdge>> edges;
    edges.reserve(cfg.nEdges());

    std::vector<flatbuffers::Offset<Fbs::BasicBlock>> bbs;
    bbs.reserve(cfg.nVertices());

    for (const auto& v : cfg.vertices()) {
        if (v.value().optionalAddress())
            vertices.push_back(saveCfgVertex(b, v));
        if (v.value().bblock())
            bbs.push_back(saveBasicBlock(b, v.value().bblock()));
    }

    for (const auto& e : cfg.edges()) {
        auto fbEdge = saveCfgEdge(b, e);
        if (!fbEdge.IsNull())
            edges.push_back(fbEdge);
    }

    std::vector<flatbuffers::Offset<Fbs::Function>> fbFuns;
    fbFuns.reserve(functions.size());
    for (const auto& f : functions)
        fbFuns.push_back(saveFunction(b, f));

    auto fbVertices  = b.CreateVector(vertices);
    auto fbEdges     = b.CreateVector(edges);
    auto fbBbs       = b.CreateVector(bbs);
    auto fbFunctions = b.CreateVector(fbFuns);

    return Fbs::CreateCFG(b, fbVertices, fbEdges, fbBbs, fbFunctions);
}

static flatbuffers::Offset<Fbs::Interval>
saveInterval(flatbuffers::FlatBufferBuilder& b, const AddressInterval& interval) {
    if (interval.isEmpty())
        return Fbs::CreateInterval(b, 0, 0);
    return Fbs::CreateInterval(b, interval.least(), interval.greatest());
}

static flatbuffers::Offset<Fbs::Segment>
saveSegment(flatbuffers::FlatBufferBuilder& b, const MemoryMap::Super::Segment& seg) {
    const auto           buf = seg.buffer();
    std::vector<uint8_t> data(buf->data(), buf->data() + buf->size());
    auto                 fbData = b.CreateVector(data);
    return Fbs::CreateSegment(b, fbData, seg.offset(), toAccessibilityMask(seg.accessibility()));
}

static flatbuffers::Offset<Fbs::IntervalSegment>
saveIntervalSegment(flatbuffers::FlatBufferBuilder& b, const MemoryMap::Super::Node& node) {
    auto fbInterval = saveInterval(b, node.key());
    auto fbSeg      = saveSegment(b, node.value());
    return Fbs::CreateIntervalSegment(b, fbInterval, fbSeg);
}

static flatbuffers::Offset<Fbs::AddressMap>
saveAddressMap(flatbuffers::FlatBufferBuilder& b, const MemoryMap::Super& map) {
    std::vector<flatbuffers::Offset<Fbs::IntervalSegment>> entries;
    entries.reserve(map.size());
    for (const auto& node : map.nodes())
        entries.push_back(saveIntervalSegment(b, node));
    auto fbEntries = b.CreateVector(entries);
    return Fbs::CreateAddressMap(b, fbEntries);
}

static flatbuffers::Offset<Fbs::MemoryMap>
saveMemoryMap(flatbuffers::FlatBufferBuilder& b, const MemoryMap& map) {
    auto fbData = saveAddressMap(b, map);
    auto fbName = b.CreateString(map.name());
    return Fbs::CreateMemoryMap(b, fbData, toFbsEndianness(map.byteOrder()), fbName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loader helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static MemoryMap::Ptr
loadMemoryMap(const Fbs::MemoryMap* fbMap) {
    auto map = MemoryMap::instance();
    map->byteOrder(fromFbsEndianness(fbMap->endianness()));
    if (auto name = fbMap->name())
        map->name(name->str());

    const Fbs::AddressMap* fbAddrMap = fbMap->data();
    if (!fbAddrMap || !fbAddrMap->entries())
        return map;

    for (auto fbEntry : *fbAddrMap->entries()) {
        if (!fbEntry || !fbEntry->interval() || !fbEntry->segment())
            continue;

        const auto lo = fbEntry->interval()->lo();
        const auto hi = fbEntry->interval()->hi();
        if (hi < lo)
            continue;

        const auto where = AddressInterval::hull(lo, hi);
        const auto seg   = fbEntry->segment();

        std::vector<uint8_t> bytes;
        if (seg->data()) {
            bytes.assign(seg->data()->begin(), seg->data()->end());
        }

        auto               buf = MemoryMap::StaticBuffer::instance(bytes.data(), bytes.size());
        MemoryMap::Segment mmSeg(buf, seg->offset(), fromAccessibilityMask(seg->accessibility()), map->name());

        // Best-effort insert; clobber if needed to keep moving.
        try {
            map->insert(where, mmSeg);
        } catch (...) {
            map->erase(where);
            map->insert(where, mmSeg);
        }
    }

    return map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FlatbufferPartitionerSaver
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FlatbufferPartitionerSaver::FlatbufferPartitionerSaver(const P2::PartitionerConstPtr& p) : partitioner_(p) {}

FlatbufferPartitionerSaver::~FlatbufferPartitionerSaver() {}

void
FlatbufferPartitionerSaver::save() {
    ASSERT_not_null(partitioner_);

    flatbuffers::FlatBufferBuilder b;

    // Architecture name is required for load.
    ASSERT_not_null(partitioner_->architecture());
    const auto archName   = partitioner_->architecture()->name();
    auto       fbArchName = b.CreateString(archName);

    // Save major structures.
    auto fbMap     = saveMemoryMap(b, *partitioner_->memoryMap());
    auto functions = partitioner_->functions(); // distinct list
    auto fbCfg     = saveCfg(b, partitioner_->cfg(), functions);

    // AUM currently not serialized (leave empty for now).
    auto fbAum = Fbs::CreateAddressUsageMap(b, 0);

    auto fbRoot = Fbs::CreatePartitioner(b, kFlatbufferPartitionerVersion, fbArchName, fbCfg, fbMap, fbAum);
    b.Finish(fbRoot);

    bytes_.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
}

std::pair<const uint8_t*, size_t>
FlatbufferPartitionerSaver::buffer() const {
    return {reinterpret_cast<const uint8_t*>(bytes_.data()), bytes_.size()};
}

void
FlatbufferPartitionerSaver::write(std::ostream& o) const {
    o.write(reinterpret_cast<const char*>(bytes_.data()), static_cast<std::streamsize>(bytes_.size()));
}

void
FlatbufferPartitionerSaver::write(const boost::filesystem::path& p) const {
    std::ofstream f(p.string().c_str(), std::ios::binary);
    if (!f)
        throw std::runtime_error("cannot open \"" + p.string() + "\" for writing");
    write(f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FlatbufferPartitionerLoader
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FlatbufferPartitionerLoader::FlatbufferPartitionerLoader() {}

FlatbufferPartitionerLoader::~FlatbufferPartitionerLoader() {}

FlatbufferPartitionerLoader
FlatbufferPartitionerLoader::fromFile(const boost::filesystem::path& p) {
    std::ifstream f(p.string().c_str(), std::ios::binary);
    if (!f)
        throw std::runtime_error("cannot open \"" + p.string() + "\" for reading");
    return fromStream(f);
}

FlatbufferPartitionerLoader
FlatbufferPartitionerLoader::fromStream(std::istream& in) {
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

FlatbufferPartitionerLoader
FlatbufferPartitionerLoader::fromBytes(std::vector<char>&& bytes) {
    FlatbufferPartitionerLoader loader;
    loader.bytes_ = std::move(bytes); // std::move on && is a true move
    return loader;
}

FlatbufferPartitionerLoader
FlatbufferPartitionerLoader::fromBytes(const std::vector<char>& bytes) {
    FlatbufferPartitionerLoader loader;
    loader.bytes_ = std::move(bytes); // std::move on const& is a copy
    return loader;
}

bool
FlatbufferPartitionerLoader::verify() const {
    flatbuffers::Verifier v(reinterpret_cast<const uint8_t*>(bytes_.data()), bytes_.size());
    return v.VerifyBuffer<Fbs::Partitioner>();
}

P2::PartitionerPtr
FlatbufferPartitionerLoader::load() const {
    if (!verify())
        throw std::runtime_error("invalid flatbuffer data for Partitioner");

    const Fbs::Partitioner* root = Fbs::GetPartitioner(bytes_.data());
    ASSERT_not_null(root);

    const auto arch_name = root->architecture_name();
    if (!arch_name)
        throw std::runtime_error("flatbuffer partitioner has no architecture_name");

    auto arch = Architecture::findByName(arch_name->str()).orThrow();
    auto map  = loadMemoryMap(root->memory_map());

    // Create a fresh partitioner.
    auto partitioner = P2::Partitioner::instance(arch, map);

    // Recreate functions list first.
    const Fbs::CFG* fbCfg = root->cfg();
    if (!fbCfg)
        return partitioner;

    // Attach functions and placeholders.
    if (fbCfg->functions()) {
        for (auto fbFun : *fbCfg->functions()) {
            if (!fbFun)
                continue;
            auto fun = P2::Function::instance(fbFun->address(), fbFun->name() ? fbFun->name()->str() : std::string());
            if (fbFun->basic_blocks()) {
                for (auto bbref : *fbFun->basic_blocks())
                    fun->insertBasicBlock(bbref->address());
            }
            partitioner->attachFunction(fun);
        }
    }

    // Discover and attach basic blocks. This will re-disassemble based on map
    // contents.
    if (fbCfg->basic_blocks()) {
        for (auto fbBb : *fbCfg->basic_blocks()) {
            if (!fbBb)
                continue;
            auto bb = partitioner->discoverBasicBlock(fbBb->address());
            if (bb)
                partitioner->attachBasicBlock(bb);
        }
    }

    return partitioner;
}

} // namespace Serialization
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
