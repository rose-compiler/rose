#include "sage3basic.h"
#include "BinaryReturnValueUsed.h"
#include "BinaryControlFlow.h"
#include "DispatcherX86.h"
#include "NullSemantics2.h"
#include "WorkLists.h"

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>

namespace rose {
namespace BinaryAnalysis {
namespace ReturnValueUsed {

// FIXME[Robb P. Matzke 2014-02-18]: Remove this when all of BinaryAnalsis is inside of "rose"
using namespace InstructionSemantics2;

// FIXME[Robb P. Matzke 2014-02-18]: These should all be template parameters
typedef ControlFlow::BlockGraph Cfg;
typedef boost::graph_traits<Cfg>::vertex_descriptor CfgVertex;
typedef boost::graph_traits<Cfg>::vertex_iterator CfgVertexIterator;
typedef boost::graph_traits<Cfg>::edge_descriptor CfgEdge;
typedef boost::graph_traits<Cfg>::edge_iterator CfgEdgeIterator;
typedef boost::graph_traits<Cfg>::out_edge_iterator CfgOutEdgeIterator;
typedef std::pair<SgAsmBlock*, SgAsmBlock*> BlockPair;
typedef std::pair<SgAsmFunction*, SgAsmFunction*> FunctionPair;

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Returns the basic blocks for an edge.
static BlockPair edgeBlocks(const Cfg &cfg, CfgEdge edge) {
    SgAsmBlock *src_blk = get_ast_node(cfg, source(edge, cfg));
    SgAsmBlock *dst_blk = get_ast_node(cfg, target(edge, cfg));
    return std::make_pair(src_blk, dst_blk);
}

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Returns functions for an edge
static FunctionPair edgeFunctions(const Cfg &cfg, CfgEdge edge) {
    BlockPair blocks = edgeBlocks(cfg, edge);
    SgAsmFunction *src_func = SageInterface::getEnclosingNode<SgAsmFunction>(blocks.first);
    SgAsmFunction *dst_func = SageInterface::getEnclosingNode<SgAsmFunction>(blocks.second);
    return std::make_pair(src_func, dst_func);
}

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Returns true if this edge represents a function call.
static bool isFunctionCall(const Cfg &cfg, CfgEdge edge) {
    FunctionPair funcs = edgeFunctions(cfg, edge);
    return funcs.first!=NULL && funcs.second!=NULL && funcs.first!=funcs.second;
}

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Returns true if this edge is an intra-function edge (not a call)
static bool isIntraFunctionEdge(const Cfg &cfg, CfgEdge edge) {
    FunctionPair funcs = edgeFunctions(cfg, edge);
    return funcs.first!=NULL && funcs.second!=NULL && funcs.first==funcs.second;
}

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Return true if a call edge also has a return edge.  We're assuming that fixup_fcall_fret() has not been called for this
// graph.  In other words, if there's a function call edge for a function that returns, then the source vertex will also have
// an edge to another vertex in the caller function.
static bool callReturns(const Cfg &cfg, CfgEdge callEdge) {
    assert(isFunctionCall(cfg, callEdge));
    CfgVertex callerVertex = source(callEdge, cfg);
    CfgOutEdgeIterator ei, ei_end;
    for (boost::tie(ei, ei_end)=out_edges(callerVertex, cfg); ei!=ei_end; ++ei) {
        if (isIntraFunctionEdge(cfg, *ei))
            return true;
    }
    return false;
}

// FIXME[Robb P. Matzke 2014-02-18]: Make this a member of rose::BinaryAnalysis::BinaryControlFlow
// Given a function call edge for a function that returns, return the vertex for the function return point.  If there's more
// than one such vertex return just the first one.
static CfgVertex firstReturnVertex(const Cfg &cfg, CfgEdge callEdge) {
    assert(isFunctionCall(cfg, callEdge));
    assert(callReturns(cfg, callEdge));
    CfgVertex callerVertex = source(callEdge, cfg);
    CfgOutEdgeIterator ei, ei_end;
    for (boost::tie(ei, ei_end)=out_edges(callerVertex, cfg); ei!=ei_end; ++ei) {
        if (isIntraFunctionEdge(cfg, *ei))
            return target(*ei, cfg);
    }
    abort();                                            // not reachable
}

// Register state for the analysis. All we're interested in is whether the register that typically holds return values has
// be read before it was written, indicating that the called function probably returned a value that was used.
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;
class RegisterState: public BaseSemantics::RegisterState {
    bool wroteValue_;                                   // wrote a value to eax
    bool readUninitialized_;                            // read eax without first writing to eax
protected:
    RegisterState(const BaseSemantics::SValuePtr &protoval, const RegisterDictionary *regdict)
        : BaseSemantics::RegisterState(protoval, regdict), wroteValue_(false), readUninitialized_(false) {}
    RegisterState(const RegisterState &other)
        : BaseSemantics::RegisterState(other), wroteValue_(other.wroteValue_), readUninitialized_(other.readUninitialized_) {}
public:
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStatePtr(new RegisterState(protoval, regdict));
    }
    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        return instance(protoval, regdict);
    }
    virtual BaseSemantics::RegisterStatePtr clone() const ROSE_OVERRIDE {
        return RegisterStatePtr(new RegisterState(*this));
    }
    virtual void clear()ROSE_OVERRIDE {
        wroteValue_ = readUninitialized_ = false;
    }
    virtual void zero()ROSE_OVERRIDE {
        wroteValue_ = readUninitialized_ = false;
    }
    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg, BaseSemantics::RiscOperators*)ROSE_OVERRIDE {
        if (reg.get_major()==x86_regclass_gpr && reg.get_minor()==x86_gpr_ax && !wroteValue_)
            readUninitialized_ = true;
        return get_protoval()->undefined_(reg.get_nbits());
    }
    virtual void writeRegister(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr&,
                               BaseSemantics::RiscOperators*)ROSE_OVERRIDE {
        if (reg.get_major()==x86_regclass_gpr && reg.get_minor()==x86_gpr_ax)
            wroteValue_ = true;
    }
    virtual void print(std::ostream &o, BaseSemantics::Formatter&) const ROSE_OVERRIDE {
        if (readUninitialized_) {
            o <<"readUninitialized\n";
        } else if (wroteValue_) {
            o <<"wrote to EAX\n";
        } else {
            o <<"initial state\n";
        }
    }
    bool readUninitialized() {
        return readUninitialized_;
    }
};
    
// The actual analysis for a function call starting at a function call return point and terminating each path whenever we reach
// another function call.  Try to figure out if we ever read from EAX without first writing to it and return true if we do.
static bool returnValueUsed(const Cfg &cfg, CfgVertex startVertex, const RegisterDictionary *regdict) {
    BaseSemantics::SValuePtr protoval = NullSemantics::SValue::instance();
    RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryStatePtr memory = BaseSemantics::MemoryCellList::instance(protoval, protoval);
    BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
    BaseSemantics::RiscOperatorsPtr ops = NullSemantics::RiscOperators::instance(state);
    BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(ops);

    WorkList<CfgVertex> worklist(true);
    Map<CfgVertex, size_t> seen;
    worklist.push(startVertex);
    while (!worklist.empty()) {
        CfgVertex v = worklist.pop();
        seen[v] = 1;
        SgAsmBlock *bb = get_ast_node(cfg, v);
        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(bb);

        // "Run" the basic block
        bool failed = false;
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            try {
                dispatcher->processInstruction(insn);
                if (registers->readUninitialized())
                    return true;
            } catch (...) {
                failed = true;
                break;
            }
        }
        if (failed)
            continue;

        // Add new vertices to the work list, but only if none of the outgoing edges are function calls.
        bool isCall = false;
        CfgOutEdgeIterator ei, ei_end;
        for (boost::tie(ei, ei_end)=out_edges(v, cfg); ei!=ei_end && !isCall; ++ei)
            isCall = isFunctionCall(cfg, *ei);
        if (!isCall) {
            for (boost::tie(ei, ei_end)=out_edges(v, cfg); ei!=ei_end && !isCall; ++ei) {
                if (!seen.exists(v))
                    worklist.push(v);
            }
        }
    }
    return false;
}

void UsageCounts::update(bool used) {
    if (used) {
        ++nUsed;
    } else {
        ++nUnused;
    }
}

UsageCounts& UsageCounts::operator+=(const UsageCounts &other) {
    nUsed += other.nUsed;
    nUnused += other.nUnused;
    return *this;
}

static std::string canonicalName(SgAsmFunction *func) {
    std::string name = func->get_name();
    if (boost::ends_with(name, "@plt"))
        boost::erase_last(name, "@plt");
    return name;
}

// Create a new results by computing sums for equivalent names ("foo" and "foo@plt").
static Results combineLikeNames(SgAsmInterpretation *interp, const Results &separate) {

    // Index of functions by canonical name
    std::vector<SgAsmFunction*> funcList = SageInterface::querySubTree<SgAsmFunction>(interp);
    Map<std::string, std::vector<SgAsmFunction*> > funcMap;
    BOOST_FOREACH (SgAsmFunction *func, funcList)
        funcMap[canonicalName(func)].push_back(func);

    // Scan through the analysis results and sum by canonical function name
    Map<std::string, UsageCounts> byName;
    for (Results::const_iterator ri=separate.begin(); ri!=separate.end(); ++ri)
        byName[canonicalName(ri->first)] += ri->second;

    // Create a new return value for all the functions.
    Results retval;
    for (Map<std::string, UsageCounts>::iterator i=byName.begin(); i!=byName.end(); ++i) {
        BOOST_FOREACH (SgAsmFunction *func, funcMap[i->first])
            retval[func] = i->second;
    }

    return retval;
}

Results analyze(SgAsmInterpretation *interp) {
    assert(interp!=NULL);
    Results results;

    Cfg cfg = ControlFlow().build_block_cfg_from_ast<Cfg>(interp);
    CfgEdgeIterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(cfg); ei!=ei_end; ++ei) {
        SgAsmBlock *src_blk = get_ast_node(cfg, source(*ei, cfg));
        SgAsmBlock *dst_blk = get_ast_node(cfg, target(*ei, cfg));
        SgAsmFunction *src_func = SageInterface::getEnclosingNode<SgAsmFunction>(src_blk);
        SgAsmFunction *dst_func = SageInterface::getEnclosingNode<SgAsmFunction>(dst_blk);
        if (!src_func || !dst_func)
            continue;
        if (src_func != dst_func && callReturns(cfg, *ei))
            results[dst_func].update(returnValueUsed(cfg, firstReturnVertex(cfg, *ei), interp->get_registers()));
    }

    results = combineLikeNames(interp, results);        // optional
    return results;
}

} // namespace
} // namespace
} // namespace
