// Operations on specimen functions
#include <rose.h>

#include <BaseSemantics2.h>                             // ROSE
#include <bROwSE/FunctionUtil.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Modules.h>
#include <rose_getline.h>
#include <rose_strtoull.h>

using namespace Rose;
using namespace Rose::Diagnostics;

namespace bROwSE {

// Generates a unique path relative to the doc root (--docroot switch).
boost::filesystem::path
uniquePath(const std::string &extension) {
    return boost::filesystem::path("tmp") /  boost::filesystem::unique_path("ROSE-%%%%%%%%%%%%%%%%"+extension);
}

std::string
charToString(uint8_t ch) {
    switch (ch) {
        //case '\0': s = "\\0"; break; //too many of these clutter up the important stuff
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\t': return "\\t";
        case '\n': return "\\n";
        case '\v': return "\\v";
        case '\f': return "\\f";
        case '\r': return "\\r";
        default:
            if (isprint(ch))
                return std::string(1, ch);
    }
    return "";
}

// Count function size in bytes and cache as the function's ATTR_NBytes attribute
size_t
functionNBytes(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nBytes = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NBytes).assignTo(nBytes)) {
        nBytes = partitioner.functionExtent(function).size();
        function->setAttribute(ATTR_NBytes, nBytes);
    }
    return nBytes;
}

// Count number of instructions in function and cache as the function's ATTR_NInsns attribute
size_t
functionNInsns(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nInsns = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NInsns).assignTo(nInsns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockExists(bblockVa))
                nInsns += bblock->nInstructions();
        }
        function->setAttribute(ATTR_NInsns, nInsns);
    }
    return nInsns;
}

// Number of discontiguous regions in a function's definition.
size_t
functionNIntervals(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nIntervals = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NIntervals).assignTo(nIntervals)) {
        nIntervals = partitioner.functionExtent(function).nIntervals();
        function->setAttribute(ATTR_NIntervals, nIntervals);
    }
    return nIntervals;
}

// Number of discontiguous basic blocks
size_t
functionNDiscontiguousBlocks(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t retval = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NDiscontiguousBlocks).assignTo(retval)) {
        BOOST_FOREACH (rose_addr_t va, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(va)) {
                size_t nIntervals = partitioner.basicBlockInstructionExtent(bb).nIntervals();
                if (nIntervals > 1)
                    ++retval;
            }
        }
        function->setAttribute(ATTR_NDiscontiguousBlocks, retval);
    }
    return retval;
}

// Generate a GraphViz file describing a function's control flow graph and store the name of the file as the function's
// ATTR_CfgGraphVizFile attribute.  We also process the file through "dot" in order to lay out the graph. This can be time
// consuming but it's worth it because the GraphViz file is used as input to multiple other functions (such as producing a JPEG
// or SVG file, obtaining node coordinates for sensitive areas and tool tips, etc) which would otherwise have to duplicate the
// layout step.  Besides, if a different layout is needed later, producing the layout input is very fast.
boost::filesystem::path
functionCfgGraphvizFile(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path fileName;
    if (function && !function->optionalAttribute<boost::filesystem::path>(ATTR_CfgGraphVizFile).assignTo(fileName)) {
        fileName = uniquePath(".dot");
        boost::filesystem::path tmpName = uniquePath(".dot");
        std::ofstream out(tmpName.string().c_str());
        P2::GraphViz::CfgEmitter graphViz(partitioner);
        graphViz.defaultGraphAttributes().insert("overlap", "scale");
        graphViz.useFunctionSubgraphs(false);
        graphViz.showInstructions(true);
        graphViz.showInstructionAddresses(false);
        graphViz.strikeNoopSequences(true);
        graphViz.defaultNodeAttributes().insert("fontsize", "10");
        graphViz.defaultEdgeAttributes().insert("fontsize", "10");
        graphViz.emitFunctionGraph(out, function);
        out.close();
        std::string dotCmd = "dot " + tmpName.string() + " > " + fileName.string();
        if (system(dotCmd.c_str())) {
            std::ofstream o(fileName.string().c_str());   // on failure, make sure file exists
            o <<"digraph {\n1 [ label=\"failure\" ];\n}\n";
        }
        function->setAttribute(ATTR_CfgGraphVizFile, fileName);

    }
    return fileName;
}

// Generate an image of the function's CFG
boost::filesystem::path
functionCfgImage(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path imageName;
    if (function && !function->optionalAttribute<boost::filesystem::path>(ATTR_CfgImage).assignTo(imageName)) {
        boost::filesystem::path srcName = functionCfgGraphvizFile(partitioner, function);
        if (srcName.empty())
            return boost::filesystem::path();

        // Most modern browsers can display SVG (vector graphics) efficiently, and dot can generate them very quickly from our
        // inputs since we've already run the time-consuming layout algorithms, and they're small (compared to JPEG) for
        // efficient transport from server to browser.
        imageName = uniquePath(".svg");
        std::string dotCmd = "dot -Tsvg -o" + imageName.string() + " " + srcName.string();
        if (0!=system(dotCmd.c_str())) {
            mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";

            // The dot-to-svg translator in GraphViz-2.26.3 has a buffer overflow problem that's triggered by basic blocks that
            // have a large number of instructions.  So if that fails, try generating a JPEG file instead.  The unfortunate
            // thing with this is that they might get scaled so small as to be unusable.
            imageName = uniquePath(".jpg");
            std::string dotCmd = "dot -Tjpg -o" + imageName.string() + " " + srcName.string();
            if (0!=system(dotCmd.c_str())) {
                mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";
                return boost::filesystem::path();
            }
        }
        function->setAttribute(ATTR_CfgImage, imageName);
    }
    return imageName;
}

// Obtain coordinates for all the vertices in a CFG GraphViz file and cache them as the ATTR_CfgVertexCoords attribute of the
// function.
CfgVertexCoords
functionCfgVertexCoords(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    // Things that need to be cleaned up on error
    struct Locals {
        FILE *coordFile;
        char *line;
        size_t linesz;
        Locals(): coordFile(NULL), line(NULL), linesz(0) {}
        ~Locals() {
            if (coordFile)
                fclose(coordFile);
            if (line)
                free(line);
        }
    } my;

    CfgVertexCoords coords;
    if (function && !function->optionalAttribute<CfgVertexCoords>(ATTR_CfgVertexCoords).assignTo(coords)) {
        boost::filesystem::path sourcePath = functionCfgGraphvizFile(partitioner, function);
        if (sourcePath.empty())
            throw std::runtime_error("CFG not available");
        std::string dotCmd = "dot -Timap_np -o/proc/self/fd/1 " + sourcePath.string();
        my.coordFile = popen(dotCmd.c_str(), "r");
        size_t linenum = 0;
        while (rose_getline(&my.line, &my.linesz, my.coordFile)>0) {
            ++linenum;
            if (!strncmp(my.line, "base referer", 12)) {
                // file always starts with this line; ignore it
            } else if (!strncmp(my.line, "rect ", 5)) {
                // example: "rect 0x401959 73,5 215,53"
                char *s=my.line+5, *t=NULL;
                Box box;

                errno = 0;
                rose_addr_t va = rose_strtoull(s, &t, 0);
                if (t==s || errno!=0) {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = t;
                errno = 0;
                box.x = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=',') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = ++t;
                errno = 0;
                box.y = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=' ') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }
                
                s = t;
                errno = 0;
                int x2 = strtol(s, &t, 0);
                if (t==s || errno!=0 || *t!=',') {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                s = ++t;
                errno = 0;
                int y2 = strtol(s, &t, 0);
                if (t==s || errno!=0) {
                    throw std::runtime_error("problem parsing line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }

                if (box.x > x2 || box.y > y2) {
                    throw std::runtime_error("invalid box coords at line "+StringUtility::numberToString(linenum)+": "+
                                             StringUtility::trim(my.line));
                }
                box.dx = x2 - box.x + 1;
                box.dy = y2 - box.y + 1;
                coords.insert(va, box);
            }
        }
        function->setAttribute(ATTR_CfgVertexCoords, coords);
    }
    return coords;
}

// Generate a function call graph and cache it on the partitioner.
P2::FunctionCallGraph*
functionCallGraph(P2::Partitioner &partitioner) {
    P2::FunctionCallGraph *cg = NULL;
    if (!partitioner.optionalAttribute<P2::FunctionCallGraph*>(ATTR_CallGraph).assignTo(cg)) {
        cg = new P2::FunctionCallGraph(partitioner.functionCallGraph(P2::AllowParallelEdges::YES));
        partitioner.setAttribute(ATTR_CallGraph, cg);
    }
    return cg;
}

// Calculate the number of places from whence a function is called and cache it in the function.
size_t
functionNCallers(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nCallers = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NCallers).assignTo(nCallers)) {
        P2::FunctionCallGraph *cg = functionCallGraph(partitioner);
        ASSERT_not_null(cg);
        nCallers = cg->nCallsIn(function);
        function->setAttribute(ATTR_NCallers, nCallers);
    }
    return nCallers;
}

// Calculate the number of calls which this function makes and cache it in this function.
size_t
functionNCallees(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nCallees = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NCallees).assignTo(nCallees)) {
        P2::FunctionCallGraph *cg = functionCallGraph(partitioner);
        ASSERT_not_null(cg);
        nCallees = cg->nCallsOut(function);
        function->setAttribute(ATTR_NCallees, nCallees);
    }
    return nCallees;
}

// Calculates the number of function return edges and caches it in the function
size_t
functionNReturns(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nReturns = 0;
    if (function && !function->optionalAttribute<size_t>(ATTR_NReturns).assignTo(nReturns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bblockVa);
            if (vertex != partitioner.cfg().vertices().end()) {
                BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->outEdges()) {
                    if (edge.value().type() == P2::E_FUNCTION_RETURN)
                        ++nReturns;
                }
            }
        }
        function->setAttribute(ATTR_NReturns, nReturns);
    }
    return nReturns;
}

// Calculates whether a function may return to the caller and caches it in the function.  The possible values are:
MayReturn
functionMayReturn(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    MayReturn result = MAYRETURN_UNKNOWN;
    if (function && !function->optionalAttribute<MayReturn>(ATTR_MayReturn).assignTo(result)) {
        bool mayReturn = false;
        if (partitioner.functionOptionalMayReturn(function).assignTo(mayReturn))
            result = mayReturn ? MAYRETURN_YES : MAYRETURN_NO;
        function->setAttribute(ATTR_MayReturn, result);
    }
    return result;
}

// Obtain stack delta or the special value SgAsmInstruction::INVALID_STACK_DELTA if unknown or not computed
int64_t
functionStackDelta(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    int64_t result = SgAsmInstruction::INVALID_STACK_DELTA;
    if (function && !function->optionalAttribute<int64_t>(ATTR_StackDelta).assignTo(result)) {
        SValuePtr delta = partitioner.functionStackDelta(function);
        if (delta && delta->is_number() && delta->get_width()<=64)
            result = IntegerOps::signExtend2<uint64_t>(delta->get_number(), delta->get_width(), 64);
        function->setAttribute(ATTR_StackDelta, result);
    }
    return result;
}

// Obtain the ROSE AST for a function
SgAsmFunction *
functionAst(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    SgNode *result = NULL;
    if (function && !function->optionalAttribute<SgNode*>(ATTR_Ast).assignTo(result)) {
        result = P2::Modules::buildFunctionAst(partitioner, function, P2::AstConstructionSettings::permissive());
        function->setAttribute(ATTR_Ast, result);
    }
    return isSgAsmFunction(result);
}

struct MaxInterproceduralDepth: P2::DataFlow::InterproceduralPredicate {
    size_t maxDepth;
    explicit MaxInterproceduralDepth(size_t n): maxDepth(n) {}
    bool operator()(const P2::ControlFlowGraph&, const P2::ControlFlowGraph::ConstEdgeIterator&, size_t depth) {
        return depth <= maxDepth;
    }
};

// Perform a data-flow analysis on a function.
FunctionDataFlow
functionDataFlow(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    using namespace Rose::BinaryAnalysis;
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;

    FunctionDataFlow result;
    if (function && !function->optionalAttribute<FunctionDataFlow>(ATTR_DataFlow).assignTo(result)) {
        // Dataflow doesn't work unless we have instruction semantics
        BaseSemantics::RiscOperatorsPtr ops = partitioner.newOperators();
        ASSERT_not_null(ops);
        if (BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops)) {
            // For now, use the same inter-procedural depth that we used when computing stack deltas.  We should probably
            // allow the user to adjust this on a per-function basis.
            MaxInterproceduralDepth ipPredicate(partitioner.stackDeltaInterproceduralLimit());
    
            // Build the CFG for this one function.
            P2::ControlFlowGraph::VertexIterator startVertex = partitioner.findPlaceholder(function->address());
            ASSERT_require(startVertex != partitioner.cfg().vertices().end());
            P2::DataFlow::DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), startVertex, ipPredicate);

            // Dataflow
            P2::DataFlow::TransferFunction xfer(cpu);
            P2::DataFlow::MergeFunction merge(cpu);
            typedef DataFlow::Engine<P2::DataFlow::DfCfg, BaseSemantics::StatePtr,
                                     P2::DataFlow::TransferFunction, P2::DataFlow::MergeFunction> Engine;
            Engine engine(dfCfg, xfer, merge);
            engine.maxIterations(10*dfCfg.nVertices()); // arbitrary
            try {
                engine.runToFixedPoint(0 /*startVertex*/, xfer.initialState());
            } catch (const BaseSemantics::Exception &e) {
                P2::mlog[ERROR] <<e <<"\n";             // probably no semantics for instruction
                result.error = e.what();
            } catch (const DataFlow::NotConverging &e) {
                P2::mlog[ERROR] <<e.what() <<"\n";      // probably iteration limit exceeded
                result.error = e.what();

            }

            result.dfCfg = dfCfg;
            result.initialStates = engine.getInitialStates();
            result.finalStates = engine.getFinalStates();
            function->setAttribute(ATTR_DataFlow, result);
        }
    }
    return result;
}

// Retrieve calling convention analysis on a function.
Rose::BinaryAnalysis::CallingConvention::Definition::Ptr
functionCallingConvention(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    using namespace Rose::BinaryAnalysis;
    CallingConvention::Definition::Ptr ccdef;

    if (function==NULL || !function->callingConventionAnalysis().hasResults())
        return ccdef;                                   // analysis was never run and is too expensive to run here.


    if (NULL == (ccdef = function->attributeOrElse(ATTR_CallConvDef, ccdef))) {
        const CallingConvention::Dictionary &archConventions = partitioner.instructionProvider().callingConventions();
        CallingConvention::Dictionary conventions = function->callingConventionAnalysis().match(archConventions);
        if (!conventions.empty()) {
            ccdef = conventions.front();
            function->setAttribute(ATTR_CallConvDef, ccdef);
        }
    }

    return ccdef;
}

} // namespace
