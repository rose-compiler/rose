// Operations on specimen functions
#include <bROwSE/FunctionUtil.h>
#include <Partitioner2/GraphViz.h>
#include <rose_getline.h>
#include <rose_strtoull.h>

using namespace rose::Diagnostics;

namespace bROwSE {

// Generates a unique path relative to the doc root (--docroot switch).
static boost::filesystem::path
uniquePath(const std::string &extension) {
    return boost::filesystem::path("tmp") /  boost::filesystem::unique_path("ROSE-%%%%%%%%%%%%%%%%"+extension);
}

// Count function size in bytes and cache as the function's ATTR_NBYTES attribute
size_t
functionNBytes(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nBytes = 0;
    if (function && !function->attr<size_t>(ATTR_NBYTES).assignTo(nBytes)) {
        nBytes = partitioner.functionExtent(function).size();
        function->attr(ATTR_NBYTES, nBytes);
    }
    return nBytes;
}

// Count number of instructions in function and cache as the function's ATTR_NINSNS attribute
size_t
functionNInsns(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nInsns = 0;
    if (function && !function->attr<size_t>(ATTR_NINSNS).assignTo(nInsns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockExists(bblockVa))
                nInsns += bblock->nInstructions();
        }
        function->attr(ATTR_NINSNS, nInsns);
    }
    return nInsns;
}

// Generate a GraphViz file describing a function's control flow graph and store the name of the file as the function's
// ATTR_CFG_DOTFILE attribute.
boost::filesystem::path
functionCfgGraphvizFile(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path fileName;
    if (function && !function->attr<boost::filesystem::path>(ATTR_CFG_DOTFILE).assignTo(fileName)) {
        fileName = uniquePath(".dot");
        boost::filesystem::path tmpName = uniquePath(".dot");
        std::ofstream out(tmpName.string().c_str());
        P2::GraphViz graphViz;
#if 1 // DEBUGGING [Robb P. Matzke 2014-12-13]
        graphViz.showInstructions(true);
#endif
        graphViz.dumpCfgFunction(out, partitioner, function);
        out.close();
        std::string dotCmd = "dot " + tmpName.string() + " > " + fileName.string();
        system(dotCmd.c_str());
        function->attr(ATTR_CFG_DOTFILE, fileName);
    }
    return fileName;
}

// Generate a JPEG image of the function's CFG
boost::filesystem::path
functionCfgImage(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    boost::filesystem::path imageName;
    if (function && !function->attr<boost::filesystem::path>(ATTR_CFG_IMAGE).assignTo(imageName)) {
        boost::filesystem::path srcName = functionCfgGraphvizFile(partitioner, function);
        if (srcName.empty())
            return boost::filesystem::path();
        imageName = uniquePath(".jpg");
        std::string dotCmd = "dot -Tjpg -o" + imageName.string() + " " + srcName.native();
        if (0!=system(dotCmd.c_str())) {
            mlog[ERROR] <<"command failed: " <<dotCmd <<"\n";
            return boost::filesystem::path();
        }
        function->attr(ATTR_CFG_IMAGE, imageName);
    }
    return imageName;
}

// Obtain coordinates for all the vertices in a CFG GraphViz file and cache them as the ATTR_CFG_COORDS attribute of the
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
    if (function && !function->attr<CfgVertexCoords>(ATTR_CFG_COORDS).assignTo(coords)) {
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
        function->attr(ATTR_CFG_COORDS, coords);
    }
    return coords;
}

// Generate a function call graph and cache it on the partitioner.
P2::FunctionCallGraph*
functionCallGraph(P2::Partitioner &partitioner) {
    P2::FunctionCallGraph *cg = NULL;
    if (!partitioner.attr<P2::FunctionCallGraph*>(ATTR_CG).assignTo(cg)) {
        cg = new P2::FunctionCallGraph(partitioner.functionCallGraph());
        partitioner.attr(ATTR_CG, cg);
    }
    return cg;
}

// Calculate the number of places from whence a function is called and cache it in the function.
size_t
functionNCallers(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nCallers = 0;
    if (function && !function->attr<size_t>(ATTR_NCALLERS).assignTo(nCallers)) {
        P2::FunctionCallGraph *cg = functionCallGraph(partitioner);
        ASSERT_not_null(cg);
        nCallers = cg->nCallsIn(function);
        function->attr(ATTR_NCALLERS, nCallers);
    }
    return nCallers;
}

// Calculates the number of function return edges and caches it in the function
size_t
functionNReturns(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    size_t nReturns = 0;
    if (function && !function->attr<size_t>(ATTR_NRETURNS).assignTo(nReturns)) {
        BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
            P2::ControlFlowGraph::ConstVertexNodeIterator vertex = partitioner.findPlaceholder(bblockVa);
            if (vertex != partitioner.cfg().vertices().end()) {
                BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNode &edge, vertex->outEdges()) {
                    if (edge.value().type() == P2::E_FUNCTION_RETURN)
                        ++nReturns;
                }
            }
        }
        function->attr(ATTR_NRETURNS, nReturns);
    }
    return nReturns;
}

// Calculates whether a function may return to the caller and caches it in the function.  The possible values are:
MayReturn
functionMayReturn(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    MayReturn result = MAYRETURN_UNKNOWN;
    if (function && !function->attr<MayReturn>(ATTR_MAYRETURN).assignTo(result)) {
        bool mayReturn = false;
        if (partitioner.functionOptionalMayReturn(function).assignTo(mayReturn))
            result = mayReturn ? MAYRETURN_YES : MAYRETURN_NO;
        function->attr(ATTR_MAYRETURN, result);
    }
    return result;
}

// Obtain stack delta or the special value SgAsmInstruction::INVALID_STACK_DELTA if unknown or not computed
int64_t
functionStackDelta(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    using namespace rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    int64_t result = SgAsmInstruction::INVALID_STACK_DELTA;
    if (function && !function->attr<int64_t>(ATTR_STACKDELTA).assignTo(result)) {
        SValuePtr delta = partitioner.functionStackDelta(function);
        if (delta && delta->is_number() && delta->get_width()<=64)
            result = IntegerOps::signExtend2<uint64_t>(delta->get_number(), delta->get_width(), 64);
        function->attr(ATTR_STACKDELTA, result);
    }
    return result;
}

} // namespace
