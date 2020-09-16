// Library for all bat toolstState

#include <rose.h>
#include <BaseSemanticsState.h>                         // rose
#include <BinaryUnparserBase.h>                         // rose
#include <CommandLine.h>                                // rose
#include <Partitioner2/Partitioner.h>                   // rose
#include <rose_strtoull.h>                              // rose

#include <batSupport.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/FileSystem.h>

using namespace Sawyer::Message::Common;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace Bat {

bool
checkRoseVersionNumber(const std::string &need) {
    std::vector<std::string> needParts = Rose::StringUtility::split('.', need);
    std::vector<std::string> haveParts = Rose::StringUtility::split('.', ROSE_PACKAGE_VERSION);

    for (size_t i=0; i < needParts.size() && i < haveParts.size(); ++i) {
        unsigned needN = boost::lexical_cast<unsigned>(needParts[i]);
        unsigned haveN = boost::lexical_cast<unsigned>(haveParts[i]);
        if (needN != haveN)
            return needN < haveN;
    }

    // E.g., need = "1.2" and have = "1.2.x", or vice versa
    return true;
}

void
checkRoseVersionNumber(const std::string &need, Sawyer::Message::Stream &fatal) {
    if (!checkRoseVersionNumber(need)) {
        fatal <<"insufficient ROSE version (need " <<need <<" but have only " <<ROSE_PACKAGE_VERSION <<")\n";
        exit(1);
    }
}

Sawyer::CommandLine::Switch
stateFileFormatSwitch(SerialIo::Format &fmt) {
    return Sawyer::CommandLine::Switch("state-format", 'f')
        .argument("fmt", Sawyer::CommandLine::enumParser<SerialIo::Format>(fmt)
                  ->with("binary", SerialIo::BINARY)
                  ->with("text", SerialIo::TEXT)
                  ->with("xml", SerialIo::XML))
        .doc("Format of the binary analysis state file. The choices are:"

             "@named{binary}{Use a custom binary format that is small and fast but not portable.}"

             "@named{text}{Use a custom text format that is medium size and portable.}"

             "@named{xml}{Use an XML format that is verbose and portable. This format can also be transcribed "
             "using the rose-xml2json tool (or other tools) to JSON.}");
}

void
checkRbaOutput(const boost::filesystem::path &name, Sawyer::Message::Facility &mlog) {
    if (name != "-") {
        std::ofstream out(name.native().c_str());
        if (!out) {
            mlog[FATAL] <<"cannot open " <<name <<" for output\n";
            exit(1);
        }
    } else if (isatty(1)) {
        mlog[FATAL] <<"refusing to write binary data to a terminal\n";
        exit(1);
    }
}

std::vector<P2::Function::Ptr>
selectFunctionsByNameOrAddress(const std::vector<P2::Function::Ptr> &functions, const std::set<std::string> &nameSet,
                               std::set<std::string> &unmatched /*in,out*/) {
    std::vector<P2::Function::Ptr> retval;
    if (nameSet.empty())
        return retval;

    // Try to convert each name to an address
    std::vector<std::string> names(nameSet.begin(), nameSet.end());
    typedef std::vector<Sawyer::Optional<rose_addr_t> > AddressPerName;
    AddressPerName vas;
    BOOST_FOREACH (const std::string &name, names) {
        errno = 0;
        const char *s = name.c_str();
        char *rest = NULL;
        rose_addr_t va = rose_strtoull(s, &rest, 0);
        if (0 == errno && *rest == '\0') {
            vas.push_back(va);
        } else {
            vas.push_back(Sawyer::Nothing());
        }
    }

    // Get the list of matching functions
    std::vector<bool> hits(names.size(), false);;
    BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
        ASSERT_not_null(function);

        bool matched = false;
        for (size_t i=0; i<names.size(); ++i) {
            if (function->name() == names[i] || function->demangledName() == names[i] ||
                (vas[i] && *vas[i] == function->address())) {
                hits[i] = true;
                if (!matched)
                    retval.push_back(function);
                // don't break, or else there might be errors reported. E.g., if the only function in the partitioner has the
                // name "foo" and the demangled name "bar" and nameSet contains both "foo" and "bar", then if we break once
                // we matched "foo" we would report that "bar" was unmatched.
            }
        }
    }

    // Names that never matched
    for (size_t i = 0; i < names.size(); ++i) {
        if (!hits[i])
            unmatched.insert(names[i]);
    }

    return retval;
}

std::vector<P2::Function::Ptr>
selectFunctionsByNameOrAddress(const std::vector<P2::Function::Ptr> &functions, const std::set<std::string> &nameSet,
                               Sawyer::Message::Stream &errors) {
    std::set<std::string> unmatched;
    std::vector<P2::Function::Ptr> retval = selectFunctionsByNameOrAddress(functions, nameSet, unmatched);
    if (!unmatched.empty() && errors) {
        BOOST_FOREACH (const std::string &name, unmatched)
            errors <<"name or address \"" <<Rose::StringUtility::cEscape(name) <<"\" didn't match any functions\n";
    }
    return retval;
}

std::vector<P2::Function::Ptr>
selectFunctionsContainingInstruction(const P2::Partitioner &partitioner, const std::set<rose_addr_t> &insnVas) {
    std::vector<P2::Function::Ptr> retval;

    BOOST_FOREACH (rose_addr_t insnVa, insnVas) {
        std::vector<P2::Function::Ptr> found = partitioner.functionsOverlapping(insnVa);
        BOOST_FOREACH (const P2::Function::Ptr &f, found)
            P2::insertUnique(retval, f, P2::sortFunctionsByAddress);
    }
    return retval;
}

// Self test to check that RBA files can be written and read
struct CheckRbaIo: Rose::CommandLine::SelfTest {
    std::string name() const { return "RBA I/O"; }
    bool operator()() {
        Sawyer::FileSystem::TemporaryFile tempRbaFile;
        tempRbaFile.stream().close();

        {
            SerialOutput::Ptr output = SerialOutput::instance();
            output->format(SerialIo::BINARY);
            output->open(tempRbaFile.name());
            P2::Partitioner partitioner;
            output->savePartitioner(partitioner);
        }

        {
            SerialInput::Ptr input = SerialInput::instance();
            input->format(SerialIo::BINARY);
            input->open(tempRbaFile.name());
            P2::Partitioner partitioner = input->loadPartitioner();
        }

        return true; // failure would have thrown an exception by now
    }
};

void
registerSelfTests() {
    Rose::CommandLine::insertSelfTest<CheckRbaIo>();
}

std::pair<std::string, std::string>
pathEndpointFunctionNames(const FeasiblePath &fpAnalysis, const P2::CfgPath &path, size_t lastVertexIdx) {
    // Find the starting function and the function in which the flaw occurs
    const P2::Partitioner &partitioner = fpAnalysis.partitioner();
    std::string firstName, lastName;
    P2::CfgPath::Vertices vertices = path.vertices();
    if (!vertices.empty()) {
        if (vertices.front()->value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertices.front()->value().bblock();
            P2::Function::Ptr f = partitioner.functionsOwningBasicBlock(bb->address()).front();
            firstName = f->name();
        } else if (vertices.front()->value().type() == P2::V_USER_DEFINED) {
            const FeasiblePath::FunctionSummary &summary = fpAnalysis.functionSummary(vertices.front()->value().address());
            firstName = summary.name;
        }

        ASSERT_require(lastVertexIdx < vertices.size());
        if (vertices.size() == 1) {
            lastName = firstName;
        } else if (vertices[lastVertexIdx]->value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertices[lastVertexIdx]->value().bblock();
            P2::Function::Ptr f = partitioner.functionsOwningBasicBlock(bb->address()).front();
            lastName = f->name();
        } else if (vertices[lastVertexIdx]->value().type() == P2::V_INDETERMINATE) {
            lastName = "intedeterminate";
        } else if (vertices[lastVertexIdx]->value().type() == P2::V_USER_DEFINED) {
            const FeasiblePath::FunctionSummary &summary = fpAnalysis.functionSummary(vertices[lastVertexIdx]->value().address());
            lastName = summary.name;
        }
    }
    return std::make_pair(firstName, lastName);
}

void
printPath(std::ostream &out, const FeasiblePath &fpAnalysis, const P2::CfgPath &path, const SmtSolver::Ptr &solver,
          const BS::RiscOperatorsPtr &cpu, SgAsmInstruction *lastInsn, ShowStates::Flag showStates) {
    const P2::Partitioner &partitioner = fpAnalysis.partitioner();

    // We need to prevent the output of two or more paths from being interleaved
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    // Figure out how many instructions to list in the path. We want to show the path up to and including the last occurrence
    // of lastInsn.
    std::pair<size_t, size_t> lastIndices = path.lastInsnIndex(lastInsn);
    size_t lastVertexIdx = lastIndices.first;
    size_t lastInsnIdx = lastIndices.second;
    out <<"    path has " <<Rose::StringUtility::plural(lastInsnIdx+1, "steps") <<"\n";

    // Find the starting function and the function in which the flaw occurs
    std::pair<std::string, std::string> endpointNames = pathEndpointFunctionNames(fpAnalysis, path, lastVertexIdx);
    if (!endpointNames.first.empty())
        out <<"    starting in function \"" <<Rose::StringUtility::cEscape(endpointNames.first) <<"\"\n";
    if (!endpointNames.second.empty())
        out <<"    ending in in function \"" <<Rose::StringUtility::cEscape(endpointNames.second) <<"\"\n";

    // List the path
    Rose::BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
    P2::CfgPath::Vertices vertices = path.vertices();
    for (size_t vertexIdx=0, insnIdx=0; vertexIdx <= lastVertexIdx; ++vertexIdx) {
#if 1 // DEBUGGING [Robb Matzke 2020-02-27]
        if (vertexIdx > 0)
            out <<"      edge " <<partitioner.edgeName(path.edges()[vertexIdx-1]) <<"\n";
#endif

        P2::ControlFlowGraph::ConstVertexIterator vertex = vertices[vertexIdx];
        if (vertex->value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertex->value().bblock();
            P2::Function::Ptr function = partitioner.functionsOwningBasicBlock(bb->address()).front();
            out <<"    vertex #" <<vertexIdx <<" in " <<function->printableName() <<"\n";
            if (bb->sourceLocation())
                out <<"    at " <<bb->sourceLocation() <<"\n";
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                std::string idxStr = (boost::format("%-6d") % insnIdx++).str();
                out <<"      #" <<idxStr <<" " <<unparser->unparse(partitioner, insn) <<"\n";
                if (insn->semanticFailure()) {
                    out <<"            semantic error caused by revious instruction\n"
                        <<"            previous insn ignored; semantic state may be invalid from this point on\n";
                }
                if (insnIdx > lastInsnIdx)
                    break;
            }
        } else if (vertex->value().type() == P2::V_INDETERMINATE) {
            out <<"    vertex #" <<vertexIdx <<" at indeterminate location\n";
            std::string idxStr = (boost::format("%-6d") % insnIdx++).str();
            out <<"      #" <<idxStr <<" indeterminate\n";
        } else if (vertex->value().type() == P2::V_USER_DEFINED) {
            const FeasiblePath::FunctionSummary &summary = fpAnalysis.functionSummary(vertex->value().address());
            out <<"    vertex #" <<vertexIdx <<" in " <<Rose::StringUtility::cEscape(summary.name) <<"\n";
            std::string idxStr = (boost::format("%-6d") % insnIdx++).str();
            out <<"      #" <<idxStr <<" summary\n";
        } else {
            ASSERT_not_reachable("unknown path vertex type: " + boost::lexical_cast<std::string>(vertex->value().type()));
        }

        // Show virtual machine state after each vertex
        if (ShowStates::YES == showStates) {
            BS::StatePtr state;
            if (vertexIdx == lastVertexIdx) {
                if (cpu)
                    state = cpu->currentState();
                if (state)
                    out <<"      state at detected operation:\n";
            } else {
                state = fpAnalysis.pathPostState(path, vertexIdx);
                if (state)
                    out <<"      state after vertex #" <<vertexIdx <<":\n";
            }

            if (state) {
                BS::Formatter fmt;
                fmt.set_register_dictionary(fpAnalysis.partitioner().instructionProvider().registerDictionary());
                fmt.set_line_prefix("        ");
                out <<(*state+fmt);
            } else {
                out <<"      no virtual state calculated at the end of vertex #" <<vertexIdx <<"\n";
            }
        }
    }

    // Show evidence from solver
    out <<"    evidence from SMT solver:\n";
    if (solver) {
        std::vector<std::string> names = solver->evidenceNames();
        if (names.empty()) {
            out <<"      none (trivial solution?)\n";
        } else {
            BOOST_FOREACH (const std::string &name, names) {
                if (SymbolicExpr::Ptr value = solver->evidenceForName(name)) {
                    out <<"      " <<name <<" = " <<*value <<"\n";
                } else {
                    out <<"      " <<name <<" = unknown\n";
                }
            }
        }
    } else {
        out <<"      no solver\n";
    }
}

P2::ControlFlowGraph::ConstVertexIterator
vertexForInstruction(const P2::Partitioner &partitioner, const std::string &nameOrVa) {
    const char *s = nameOrVa.c_str();
    char *rest;
    errno = 0;
    rose_addr_t va = rose_strtoull(s, &rest, 0);
    if (*rest || errno!=0) {
        size_t nFound = 0;
        BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
            if (function->name() == nameOrVa) {
                va = function->address();
                ++nFound;
            }
        }
        if (0==nFound)
            return partitioner.cfg().vertices().end();
        if (nFound > 1)
            throw std::runtime_error("vertex \"" + Rose::StringUtility::cEscape(nameOrVa) + "\" is ambiguous");
    }
    return partitioner.instructionVertex(va);
}

P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner &partitioner,
                    const P2::ControlFlowGraph::ConstVertexIterator &source,
                    const P2::ControlFlowGraph::ConstVertexIterator &target) {
    if (source == partitioner.cfg().vertices().end() || target == partitioner.cfg().vertices().end())
        return partitioner.cfg().edges().end();         // sourceVa or targetVa is not an instruction starting address
    ASSERT_require(source->value().type() == P2::V_BASIC_BLOCK);
    ASSERT_require(target->value().type() == P2::V_BASIC_BLOCK);
    for (P2::ControlFlowGraph::ConstEdgeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target() == target)
            return edge;
    }
    return partitioner.cfg().edges().end();
}

P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner &partitioner, const std::string &sourceNameOrVa,
                    const std::string &targetNameOrVa) {
    return edgeForInstructions(partitioner,
                               vertexForInstruction(partitioner, sourceNameOrVa),
                               vertexForInstruction(partitioner, targetNameOrVa));
}

void
assignCallingConventions(const P2::Partitioner &partitioner) {
    const CallingConvention::Dictionary &ccDict = partitioner.instructionProvider().callingConventions();
    CallingConvention::Definition::Ptr defaultCc;
    if (!ccDict.empty())
        defaultCc = ccDict[0];
    partitioner.allFunctionCallingConventionDefinition(defaultCc);
}

size_t
PathSelector::operator()(const FeasiblePath &fpAnalysis, const P2::CfgPath &path, SgAsmInstruction *offendingInstruction) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    // Suppress all paths?  This is used when we want to create only summaries.
    if (suppressAll) {
        ++nSuppressed_;
        return 0;
    }

    // Suppress uninteresting paths.
    if (suppressUninteresting) {
        std::pair<size_t, size_t> lastIndices = path.lastInsnIndex(offendingInstruction);
        size_t lastVertexIdx = lastIndices.first;
        std::pair<std::string, std::string> endpointNames = pathEndpointFunctionNames(fpAnalysis, path, lastVertexIdx);
        if (endpointNames.first.empty() && !endpointNames.second.empty()) {
            // Interesting because the path starts in unnamed user code and ends in a function whose name we know from library
            // attribution results.
        } else {
            ++nSuppressed_;
            ++nUninteresting_;
            return 0;
        }
    }

    uint64_t hash = path.hash(offendingInstruction);

    // If a non-empty set of path hashes is specified, then suppress all but those that are present in the set.
    if (!requiredHashes.empty() && requiredHashes.find(hash) == requiredHashes.end()) {
        ++nSuppressed_;
        ++nWrongHashes_;
        return 0;
    }

    // It's possible for the same path to occur more than once. For instance, encountering a possible weakness doesn't
    // necessarily mean that the execution path terminates -- the model checker may continue trying to extend the path.
    // Therefore, in order to prevent printing redundant paths, we hash each path and print it only if we haven't seen the hash
    // before.
    if (suppressDuplicatePaths && !seenPaths_.insert(hash).second) {
        ++nSuppressed_;
        ++nDuplicatePaths_;
        return 0;
    }

    // The user might want to show only one path per end-point.
    if (suppressDuplicateEndpoints &&
        !seenEndpoints_.insert(path.backVertex()->value().optionalAddress().orElse((rose_addr_t)(-1))).second) {
        ++nSuppressed_;
        ++nDuplicateEndpoints_;
        return 0;
    }

    // Have we selected too many paths already?
    if (nSelected_ >= maxPaths) {
        ++nSuppressed_;
        ++nLimitExceeded_;
        return 0;
    }

    // Assume the path should be shown
    return ++nSelected_;
}

void
PathSelector::maybeTerminate() const {
    if (nSelected_ >= maxPaths)
        _exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction histograms
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus >= 201402L

InsnHistogram
computeInsnHistogram(const InstructionProvider &insns, const MemoryMap::Ptr &map) {
    InsnHistogram histogram;
    rose_addr_t va = 0;
    while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        const rose_addr_t aligned = alignUp(va, insns.instructionAlignment());
        if (va != aligned) {
            va = aligned;
        } else if (SgAsmInstruction *insn = insns[va]) {
            ++histogram[insn->get_mnemonic()];
            va += insn->get_size();
        } else {
            ++va;
        }
    }
    return histogram;
}

void
saveInsnHistogram(const InsnHistogram &histogram, const boost::filesystem::path &fileName) {
    auto io = SerialOutput::instance();
    io->format(SerialIo::XML);
    io->open(fileName);
    io->saveObject(SerialIo::USER_DEFINED, histogram);
}

InsnHistogram
loadInsnHistogram(const boost::filesystem::path &fileName) {
    auto io = SerialInput::instance();
    io->format(SerialIo::XML);
    io->open(fileName);
    return io->loadObject<InsnHistogram>(SerialIo::USER_DEFINED);
}

std::vector<InsnHistogram>
splitInsnHistogram(const InsnHistogram &histogram, size_t nParts) {
    ASSERT_require(nParts > 0);
    std::vector<std::pair<std::string, size_t>> records(histogram.begin(), histogram.end());
    std::sort(records.begin(), records.end(), [](auto &a, auto &b) { return a.second > b.second; });
    size_t partSize = (records.size() + nParts - 1) / nParts;
    std::vector<InsnHistogram> parts(nParts);
    for (size_t i = 0; i < records.size(); ++i)
        parts[i/partSize].insert(records[i]);
    return parts;
}

void
mergeInsnHistogram(InsnHistogram &histogram, const InsnHistogram &other) {
    for (auto &pair: other)
        histogram[pair.first] += pair.second;
}

double
compareInsnHistograms(const std::vector<InsnHistogram> &aParts, const InsnHistogram &b) {
    std::vector<InsnHistogram> bParts = splitInsnHistogram(b, aParts.size());
    size_t totalDiff = 0, maxDiff = 0;
    for (size_t i = 0; i < aParts.size(); ++i) {
        maxDiff += std::max(i - 0, (bParts.size()-1) - i) * aParts[i].size();
        for (const auto &record: aParts[i]) {
            const std::string &label = record.first;
            size_t foundAt = bParts.size() - 1;
            for (size_t j = 0; j < bParts.size(); ++j) {
                if (bParts[j].find(label) != bParts[j].end()) {
                    foundAt = j;
                    break;
                }
            }
            size_t diff = std::max(i, foundAt) - std::min(i, foundAt);
            totalDiff += diff;
        }
    }
    return 1.0 * totalDiff / maxDiff;
}

double
compareInsnHistograms(const InsnHistogram &a, const InsnHistogram &b, size_t nParts) {
    std::vector<InsnHistogram> aParts = splitInsnHistogram(a, nParts);
    return compareInsnHistograms(aParts, b);
}

void
printInsnHistogram(const InsnHistogram &histogram, std::ostream &out) {
    size_t runningTotal = 0, grandTotal = 0;
    for (auto pair: histogram)
        grandTotal += pair.second;
    std::vector<std::pair<std::string, size_t>> results(histogram.begin(), histogram.end());
    std::sort(results.begin(), results.end(), [](auto &a, auto &b) { return a.second > b.second; });
    std::cout <<"Instruction           N      N%   Total  Total%\n";
    for (auto pair: results) {
        runningTotal += pair.second;
        out <<(boost::format("%-15s\t%7d\t%7.3f\t%7d\t%7.3f\n")
               %pair.first %pair.second %(100.0 * pair.second / grandTotal)
               %runningTotal %(100.0 * runningTotal / grandTotal));
    }
}

#endif

} // namespace
