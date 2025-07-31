// Library for all bat toolstState
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <rose.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/CommandLine.h>
#include <Rose/StringUtility.h>

#include <rose_strtoull.h>                              // rose
#include <stringify.h>                                  // rose

#include <batSupport.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <Sawyer/BitVector.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/FileSystem.h>

using namespace Sawyer::Message::Common;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Bat {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Null formatter produces no output.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputFormatter::Ptr
NullFormatter::instance() {
    return Ptr(new NullFormatter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plain text formatter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputFormatter::Ptr
PlainTextFormatter::instance() {
    return Ptr(new PlainTextFormatter);
}

void
PlainTextFormatter::title(std::ostream &out, const std::string &title) {
    out <<title;
}

void
PlainTextFormatter::pathNumber(std::ostream &out, size_t n) {
    out <<", path #" <<n;
}

void
PlainTextFormatter::pathHash(std::ostream &out, const std::string &hash) {
    out <<", hash " <<hash <<"\n";
}

void
PlainTextFormatter::ioMode(std::ostream &out, FeasiblePath::IoMode m, const std::string &what) {
    std::string s = boost::to_lower_copy(stringify::Rose::BinaryAnalysis::FeasiblePath::IoMode(m, ""));
    out <<"  " <<s;
    if (!what.empty())
        out <<" " <<what;
}

void
PlainTextFormatter::mayMust(std::ostream &out, FeasiblePath::MayOrMust mayMust, const std::string &what) {
    std::string s = FeasiblePath::MUST == mayMust ? "must" : "may";
    out <<" " <<s;
    if (!what.empty())
        out <<" " <<what;
    out <<"\n";
}

void
PlainTextFormatter::objectAddress(std::ostream &out, const SymbolicExpression::Ptr &address) {
    out <<"  for address " <<*address <<"\n";
}

void
PlainTextFormatter::finalInsn(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    out <<"  at instruction " <<partitioner->unparse(insn) <<"\n";
}

void
PlainTextFormatter::variable(std::ostream &out, const Variables::StackVariable &var) {
    out <<"  for " <<var <<"\n";
}

void
PlainTextFormatter::variable(std::ostream &out, const Variables::GlobalVariable &var) {
    out <<"  for " <<var <<"\n";
}

void
PlainTextFormatter::frameOffset(std::ostream &out, const Variables::OffsetInterval &i) {
    if (i.size() == 1) {
        out <<"  when accessing frame location " <<Variables::offsetStr(i.least()) <<"\n";
    } else {
        out <<"  when accessing frame locations " <<Variables::offsetStr(i.least())
            <<" through " <<Variables::offsetStr(i.greatest()) <<"\n";
    }
}

void
PlainTextFormatter::frameRelative(std::ostream &out, const Variables::StackVariable &var, const Variables::OffsetInterval &i,
                                  FeasiblePath::IoMode ioMode) {
    std::string direction = boost::to_lower_copy(stringify::Rose::BinaryAnalysis::FeasiblePath::IoMode(ioMode, ""));
    if (i.least() < var.stackOffset()) {
        size_t distance = var.stackOffset() - i.least();
        out <<"  " <<direction <<" starts " <<Rose::StringUtility::plural(distance, "bytes") <<" before the variable\n";
    }

    // We have to be careful because the var.maxSizeBytes() is unsigned and can be very large, but the stack offsets are
    // signed and have only half the possible magnitude. We'll do the arithmetic in software with signed 65-bit types.
    // The following code is equivalent to (modulo the types and hexadecimal output):
    //    if (i.greatest() > var.stackOffset() + var.maxSizeBytes()) {
    //        size_t distance = i.greatest() - (var.stackOffset() + var.maxSizeBytes()) + 1;
    //        out <<" " <<direction <<" ends " <<Rose::StringUtility::plural(distance, "bytes") <<" after the variable\n";
    //    }
    typedef Sawyer::Container::BitVector BV;
    BV greatestAccess(65);
    greatestAccess.fromInteger((uint64_t)i.greatest());
    greatestAccess.signExtend(BV::BitRange::hull(0, 63), BV::BitRange::hull(0, 64));
    BV maxSizeBytes(65);
    maxSizeBytes.fromInteger(var.maxSizeBytes());
    BV endOffset(65);
    endOffset.fromInteger((uint64_t)var.stackOffset());
    endOffset.signExtend(BV::BitRange::hull(0, 63), BV::BitRange::hull(0, 64));
    endOffset.add(maxSizeBytes);
    if (greatestAccess.compareSigned(endOffset) > 0) {
        // Access ends after the end of the variable.
        BV distance(greatestAccess);
        distance.subtract(endOffset);
        distance.increment();
        BV maxInt(65);
        maxInt.set(BV::hull(0, 63));
        if (distance.compareSigned(maxInt) <= 0) {
            uint64_t d = distance.toInteger();
            out <<" " <<direction <<" ends " <<Rose::StringUtility::plural(d, "bytes") <<" after the variable\n";
        } else {
            out <<" " <<direction <<" ends 0x" <<distance.toHex() <<" bytes after the variable\n";
        }
    }
}

void
PlainTextFormatter::localVarsFound(std::ostream &out, const Variables::StackVariables &vars,
                                   const P2::Partitioner::ConstPtr &partitioner) {
    out <<"  these local variables were found:\n";
    Variables::print(vars, partitioner, out, "    ");
}

void
PlainTextFormatter::pathIntro(std::ostream &out) {
    out <<"  the weakness occurs on the following execution path\n";
}

void
PlainTextFormatter::pathLength(std::ostream &out, size_t nVerts, size_t nSteps) {
    out <<"  path has " <<Rose::StringUtility::plural(nVerts, "vertices")
        <<" containing " <<Rose::StringUtility::plural(nSteps, "steps") <<"\n";
}

void
PlainTextFormatter::startFunction(std::ostream &out, const std::string &name) {
    out <<"  starting in function \"" <<Rose::StringUtility::cEscape(name) <<"\"\n";
}

void
PlainTextFormatter::endFunction(std::ostream &out, const std::string &name) {
    out <<"  ending in function \"" <<Rose::StringUtility::cEscape(name) <<"\"\n";
}

void
PlainTextFormatter::bbVertex(std::ostream &out, size_t id, const P2::BasicBlock::Ptr&, const std::string &funcName) {
    out <<"    vertex #" <<id <<" in " <<funcName <<"\n";
}

void
PlainTextFormatter::bbSrcLoc(std::ostream &out, const Rose::SourceLocation &loc) {
    out <<"    at " <<loc <<"\n";
}

void
PlainTextFormatter::insnListIntro(std::ostream&) {}

void
PlainTextFormatter::insnStep(std::ostream &out, size_t idx, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(insn);
    out <<"      #" <<(boost::format("%-6d") % idx) <<" " <<partitioner->unparse(insn) <<"\n";
}

void
PlainTextFormatter::semanticFailure(std::ostream &out) {
    out <<"            semantic error caused by previous instruction\n"
        <<"            previous insn ignored; semantic state may be invalid from this point on\n";
}

void
PlainTextFormatter::indetVertex(std::ostream &out, size_t idx) {
    out <<"    vertex #" <<idx <<" at indeterminate location\n";
}

void
PlainTextFormatter::indetStep(std::ostream &out, size_t idx) {
    out <<"      #" <<(boost::format("%-6d") % idx) <<" indeterminate\n";
}

void
PlainTextFormatter::summaryVertex(std::ostream &out, size_t idx, Address va) {
    out <<"    vertex #" <<idx <<" at " <<Rose::StringUtility::addrToString(va) <<"\n";
}

void
PlainTextFormatter::summaryStep(std::ostream &out, size_t idx, const std::string &name) {
    out <<"      #" <<(boost::format("%-6d") % idx) <<" summary";
    if (!name.empty())
        out <<" " <<name;
    out <<"\n";
}

void
PlainTextFormatter::edge(std::ostream &out, const std::string &name) {
    out <<"    edge " <<name <<"\n";
}

void
PlainTextFormatter::state(std::ostream &out, size_t vertexIdx, const std::string &title,
                          const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::State::Ptr &state,
                          const Rose::BinaryAnalysis::RegisterDictionary::Ptr &regdict) {
    out <<"      " <<title <<"\n";
    if (state) {
        BS::Formatter fmt;
        fmt.registerDictionary(regdict);
        fmt.set_line_prefix("        ");
        out <<(*state + fmt);
    } else {
        out <<"      no virtual state calculated at the end of vertex #" <<vertexIdx <<"\n";
    }
}

void
PlainTextFormatter::solverEvidence(std::ostream &out, const Rose::BinaryAnalysis::SmtSolverPtr &solver) {
    out <<"    evidence from SMT solver:\n";
    if (solver) {
        std::vector<std::string> names = solver->evidenceNames();
        if (names.empty()) {
            out <<"      none (trivial solution?)\n";
        } else {
            for (const std::string &name: names) {
                if (SymbolicExpression::Ptr value = solver->evidenceForName(name)) {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// YAML formatter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputFormatter::Ptr
YamlFormatter::instance() {
    return Ptr(new YamlFormatter);
}

std::string
YamlFormatter::formatTag(const std::string &tag) {
    return (boost::format("%-32s") % tag).str();
}

void
YamlFormatter::title(std::ostream &out, const std::string &title) {
    writeln(out, "- title:", title);
}

void
YamlFormatter::pathNumber(std::ostream &out, size_t n) {
    writeln(out, "  sequence:", n);
}

void
YamlFormatter::pathHash(std::ostream &out, const std::string &hash) {
    writeln(out, "  hash:", hash);
}

void
YamlFormatter::ioMode(std::ostream &out, FeasiblePath::IoMode m, const std::string &what) {
    std::string s = boost::to_lower_copy(stringify::Rose::BinaryAnalysis::FeasiblePath::IoMode(m, ""));
    out <<formatTag("  description:") <<" " <<s;
    if (!what.empty())
        out <<" " <<what;
}

void
YamlFormatter::mayMust(std::ostream &out, FeasiblePath::MayOrMust mayMust, const std::string &what) {
    std::string s = FeasiblePath::MUST == mayMust ? "must" : "may";
    out <<" " <<s;
    if (!what.empty())
        out <<" " <<what;
    out <<"\n";
}

void
YamlFormatter::objectAddress(std::ostream &out, const SymbolicExpression::Ptr &address) {
    writeln(out, "  address:", *address);
}

void
YamlFormatter::finalInsn(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner);
    writeln(out, "  instruction:", partitioner->unparse(insn));
}

void
YamlFormatter::variable(std::ostream &out, const Variables::StackVariable &var) {
    writeln(out, "  variable:", var);
}

void
YamlFormatter::variable(std::ostream &out, const Variables::GlobalVariable &var) {
    writeln(out, "  variable:", var);
}

void
YamlFormatter::frameOffset(std::ostream &out, const Variables::OffsetInterval &i) {
    if (i.size() == 1) {
        writeln(out, "  frame-location:", Variables::offsetStr(i.least()));
    } else {
        writeln(out, "  frame-location:",
                Variables::offsetStr(i.least()) + " through " + Variables::offsetStr(i.greatest()));
    }
}

void
YamlFormatter::frameRelative(std::ostream &out, const Variables::StackVariable &var, const Variables::OffsetInterval &i,
                                  FeasiblePath::IoMode ioMode) {
    std::string direction = boost::to_lower_copy(stringify::Rose::BinaryAnalysis::FeasiblePath::IoMode(ioMode, ""));
    if (i.least() < var.stackOffset()) {
        size_t distance = var.stackOffset() - i.least();
        writeln(out, "  before-variable:", Rose::StringUtility::plural(distance, "bytes"));
    }

    // We have to be careful because the var.maxSizeBytes() is unsigned and can be very large, but the stack offsets are
    // signed and have only half the possible magnitude. We'll do the arithmetic in software with signed 65-bit types.
    // The following code is equivalent to (modulo the types and hexadecimal output):
    //    if (i.greatest() > var.stackOffset() + var.maxSizeBytes()) {
    //        size_t distance = i.greatest() - (var.stackOffset() + var.maxSizeBytes()) + 1;
    //        out <<" " <<direction <<" ends " <<Rose::StringUtility::plural(distance, "bytes") <<" after the variable\n";
    //    }
    typedef Sawyer::Container::BitVector BV;
    BV greatestAccess(65);
    greatestAccess.fromInteger((uint64_t)i.greatest());
    greatestAccess.signExtend(BV::BitRange::hull(0, 63), BV::BitRange::hull(0, 64));
    BV maxSizeBytes(65);
    maxSizeBytes.fromInteger(var.maxSizeBytes());
    BV endOffset(65);
    endOffset.fromInteger((uint64_t)var.stackOffset());
    endOffset.signExtend(BV::BitRange::hull(0, 63), BV::BitRange::hull(0, 64));
    endOffset.add(maxSizeBytes);
    if (greatestAccess.compareSigned(endOffset) > 0) {
        // Access ends after the end of the variable.
        BV distance(greatestAccess);
        distance.subtract(endOffset);
        distance.increment();
        BV maxInt(65);
        maxInt.set(BV::hull(0, 63));
        if (distance.compareSigned(maxInt) <= 0) {
            uint64_t d = distance.toInteger();
            writeln(out, "  after-variable:", Rose::StringUtility::plural(d, "bytes"));
        } else {
            writeln(out, "  after-variable:", "0x" + distance.toHex() + " bytes");
        }
    }
}

void
YamlFormatter::localVarsFound(std::ostream &out, const Variables::StackVariables &vars,
                              const P2::Partitioner::ConstPtr &partitioner) {
    writeln(out, "  local-variables:");
    Variables::print(vars, partitioner, out, "    - ");
}

void
YamlFormatter::pathLength(std::ostream &out, size_t nVerts, size_t nSteps) {
    std::string s = Rose::StringUtility::plural(nVerts, "vertices") +
                    " containing " + Rose::StringUtility::plural(nSteps, "steps");
    writeln(out, "  path-length:", s);
}

void
YamlFormatter::startFunction(std::ostream &out, const std::string &name) {
    writeln(out, "  start-function:", Rose::StringUtility::cEscape(name));
}

void
YamlFormatter::endFunction(std::ostream &out, const std::string &name) {
    writeln(out, "  end-function:", Rose::StringUtility::cEscape(name));
}

void
YamlFormatter::pathIntro(std::ostream &out) {
    writeln(out, "  vertices:");
}

void
YamlFormatter::bbVertex(std::ostream &out, size_t idx, const P2::BasicBlock::Ptr &bb, const std::string &funcName) {
    writeln(out, "    - vertex:", idx);
    writeln(out, "      type:", "basic-block");
    writeln(out, "      address:", Rose::StringUtility::addrToString(bb->address()));
    if (!funcName.empty())
        writeln(out, "      contained-in", funcName);
}

void
YamlFormatter::bbSrcLoc(std::ostream &out, const Rose::SourceLocation &loc) {
    writeln(out, "      src-location:", loc);
}

void
YamlFormatter::insnListIntro(std::ostream &out) {
    writeln(out, "      steps:");
}

void
YamlFormatter::insnStep(std::ostream &out, size_t idx, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(insn);
    writeln(out, "        - step:", idx);
    writeln(out, "          address:", Rose::StringUtility::addrToString(insn->get_address()));
    writeln(out, "          instruction:", partitioner->unparsePlain(insn));
}

void
YamlFormatter::semanticFailure(std::ostream &out) {
    writeln(out, "          error:", "semantic-failure");
}

void
YamlFormatter::indetVertex(std::ostream &out, size_t idx) {
    writeln(out, "    - vertex:", idx);
    writeln(out, "      type:", "indeterminate");
    writeln(out, "      steps:");
}

void
YamlFormatter::indetStep(std::ostream &out, size_t idx) {
    writeln(out, "        - step:", idx);
    writeln(out, "          address:", "indeterminate");
}

void
YamlFormatter::summaryVertex(std::ostream &out, size_t idx, Address va) {
    writeln(out, "    - vertex:", idx);
    writeln(out, "      type:", "summary");
    writeln(out, "      address:", Rose::StringUtility::addrToString(va));
    writeln(out, "      steps:");
}

void
YamlFormatter::summaryStep(std::ostream &out, size_t idx, const std::string &name) {
    writeln(out, "        - step:", idx);
    writeln(out, "          entity:", name);
}

void
YamlFormatter::edge(std::ostream &out, const std::string &name) {
    writeln(out, "      outgoing-edge:", name);
}

void
YamlFormatter::state(std::ostream &out, size_t /*vertexIdx*/, const std::string &title,
                     const Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::State::Ptr &state,
                     const Rose::BinaryAnalysis::RegisterDictionary::Ptr &regdict) {
    if (state) {
        writeln(out, "      semantics:", title);
        writeln(out, "      state:");
        BS::Formatter fmt;
        fmt.registerDictionary(regdict);
        fmt.set_line_prefix("        - ");
        out <<(*state + fmt);
    }
}

void
YamlFormatter::solverEvidence(std::ostream &out, const Rose::BinaryAnalysis::SmtSolverPtr &solver) {
    if (solver) {
        std::vector<std::string> names = solver->evidenceNames();
        if (names.empty()) {
            writeln(out, "  evidence:", "none (trivial solution?)");
        } else {
            writeln(out, "  evidence:");
            for (const std::string &name: names) {
                writeln(out, "    - name:", name);
                if (SymbolicExpression::Ptr value = solver->evidenceForName(name)) {
                    writeln(out, "      value:", *value);
                } else {
                    writeln(out, "      value:", "unknown");
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Support functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    return Sawyer::CommandLine::Switch("state-format")
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
    typedef std::vector<Sawyer::Optional<Address> > AddressPerName;
    AddressPerName vas;
    for (const std::string &name: names) {
        errno = 0;
        const char *s = name.c_str();
        char *rest = NULL;
        Address va = rose_strtoull(s, &rest, 0);
        if (0 == errno && *rest == '\0') {
            vas.push_back(va);
        } else {
            vas.push_back(Sawyer::Nothing());
        }
    }

    // Get the list of matching functions
    std::vector<bool> hits(names.size(), false);;
    for (const P2::Function::Ptr &function: functions) {
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
        for (const std::string &name: unmatched)
            errors <<"name or address \"" <<Rose::StringUtility::cEscape(name) <<"\" didn't match any functions\n";
    }
    return retval;
}

std::vector<P2::Function::Ptr>
selectFunctionsContainingInstruction(const P2::Partitioner::ConstPtr &partitioner, const std::set<Address> &insnVas) {
    ASSERT_not_null(partitioner);
    std::vector<P2::Function::Ptr> retval;

    for (Address insnVa: insnVas) {
        std::vector<P2::Function::Ptr> found = partitioner->functionsOverlapping(insnVa);
        for (const P2::Function::Ptr &f: found)
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
            auto arch = Architecture::findByName("intel-pentium4").orThrow();
            auto partitioner = P2::Partitioner::instance(arch);
            output->savePartitioner(partitioner);
        }

        {
            SerialInput::Ptr input = SerialInput::instance();
            input->format(SerialIo::BINARY);
            input->open(tempRbaFile.name());
            P2::Partitioner::Ptr partitioner = input->loadPartitioner();
        }

        return true; // failure would have thrown an exception by now
    }
};

void
registerSelfTests() {
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    Rose::CommandLine::insertSelfTest<CheckRbaIo>();
#endif
}

std::pair<std::string, std::string>
pathEndpointFunctionNames(const FeasiblePath &fpAnalysis, const P2::CfgPath &path, size_t lastVertexIdx) {
    // Find the starting function and the function in which the flaw occurs
    P2::Partitioner::ConstPtr partitioner = fpAnalysis.partitioner();
    std::string firstName, lastName;
    P2::CfgPath::Vertices vertices = path.vertices();
    if (!vertices.empty()) {
        if (vertices.front()->value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertices.front()->value().bblock();
            P2::Function::Ptr f = partitioner->functionsOwningBasicBlock(bb->address()).front();
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
            P2::Function::Ptr f = partitioner->functionsOwningBasicBlock(bb->address()).front();
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
          const BS::RiscOperators::Ptr &cpu, SgAsmInstruction *lastInsn, ShowStates::Flag showStates,
          const OutputFormatter::Ptr &formatter) {
    ASSERT_not_null(formatter);
    P2::Partitioner::ConstPtr partitioner = fpAnalysis.partitioner();

    // We need to prevent the output of two or more paths from being interleaved
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    // Figure out how many instructions to list in the path. We want to show the path up to and including the last occurrence
    // of lastInsn.
    std::pair<size_t, size_t> lastIndices = path.lastInsnIndex(lastInsn);
    size_t lastVertexIdx = lastIndices.first;
    size_t lastInsnIdx = lastIndices.second;
    formatter->finalInsn(out, partitioner, lastInsn);

    // Find the starting function and the function in which the flaw occurs
    std::pair<std::string, std::string> endpointNames = pathEndpointFunctionNames(fpAnalysis, path, lastVertexIdx);
    if (!endpointNames.first.empty())
        formatter->startFunction(out, endpointNames.first);
    if (!endpointNames.second.empty())
        formatter->endFunction(out, endpointNames.second);

    // List the path
    formatter->pathLength(out, lastVertexIdx+1, lastInsnIdx+1);
    formatter->pathIntro(out);
    Rose::BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner->unparser();
    P2::CfgPath::Vertices vertices = path.vertices();
    for (size_t vertexIdx=0, insnIdx=0; vertexIdx <= lastVertexIdx; ++vertexIdx) {
#if 1 // DEBUGGING [Robb Matzke 2020-02-27]
        if (vertexIdx > 0)
            formatter->edge(out, partitioner->edgeName(path.edges()[vertexIdx-1]));
#endif

        P2::ControlFlowGraph::ConstVertexIterator vertex = vertices[vertexIdx];
        if (vertex->value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertex->value().bblock();
            P2::Function::Ptr function = partitioner->functionsOwningBasicBlock(bb->address()).front();
            formatter->bbVertex(out, vertexIdx, bb, function->printableName());
            if (bb->sourceLocation())
                formatter->bbSrcLoc(out, bb->sourceLocation());
            formatter->insnListIntro(out);
            for (SgAsmInstruction *insn: bb->instructions()) {
                formatter->insnStep(out, insnIdx++, partitioner, insn);
                if (insn->semanticFailure())
                    formatter->semanticFailure(out);
                if (insnIdx > lastInsnIdx)
                    break;
            }
        } else if (vertex->value().type() == P2::V_INDETERMINATE) {
            formatter->indetVertex(out, vertexIdx);
            formatter->indetStep(out, vertexIdx++);
        } else if (vertex->value().type() == P2::V_USER_DEFINED) {
            const FeasiblePath::FunctionSummary &summary = fpAnalysis.functionSummary(vertex->value().address());
            formatter->summaryVertex(out, vertexIdx, summary.address);
            formatter->summaryStep(out, insnIdx++, summary.name);
        } else {
            ASSERT_not_reachable("unknown path vertex type: " + boost::lexical_cast<std::string>(vertex->value().type()));
        }

        // Show virtual machine state after each vertex
        if (ShowStates::YES == showStates) {
            RegisterDictionary::Ptr regdict = fpAnalysis.partitioner()->instructionProvider().registerDictionary();
            if (vertexIdx == lastVertexIdx) {
                formatter->state(out, vertexIdx, "state at detected operation:", cpu->currentState(), regdict);
            } else if (BS::State::Ptr state = fpAnalysis.pathPostState(path, vertexIdx)) {
                formatter->state(out, vertexIdx, "state after vertex #" + boost::lexical_cast<std::string>(vertexIdx),
                                 state, regdict);
            }
        }
    }

    // Show evidence from solver
    formatter->solverEvidence(out, solver);
}

P2::ControlFlowGraph::ConstVertexIterator
vertexForInstruction(const P2::Partitioner::ConstPtr &partitioner, const std::string &nameOrVa) {
    ASSERT_not_null(partitioner);
    const char *s = nameOrVa.c_str();
    char *rest;
    errno = 0;
    Address va = rose_strtoull(s, &rest, 0);
    if (*rest || errno!=0) {
        size_t nFound = 0;
        for (const P2::Function::Ptr &function: partitioner->functions()) {
            if (function->name() == nameOrVa) {
                va = function->address();
                ++nFound;
            }
        }
        if (0==nFound)
            return partitioner->cfg().vertices().end();
        if (nFound > 1)
            throw std::runtime_error("vertex \"" + Rose::StringUtility::cEscape(nameOrVa) + "\" is ambiguous");
    }
    return partitioner->instructionVertex(va);
}

P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner::ConstPtr &partitioner,
                    const P2::ControlFlowGraph::ConstVertexIterator &source,
                    const P2::ControlFlowGraph::ConstVertexIterator &target) {
    ASSERT_not_null(partitioner);
    if (source == partitioner->cfg().vertices().end() || target == partitioner->cfg().vertices().end())
        return partitioner->cfg().edges().end();        // sourceVa or targetVa is not an instruction starting address
    ASSERT_require(source->value().type() == P2::V_BASIC_BLOCK);
    ASSERT_require(target->value().type() == P2::V_BASIC_BLOCK);
    for (P2::ControlFlowGraph::ConstEdgeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target() == target)
            return edge;
    }
    return partitioner->cfg().edges().end();
}

P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner::ConstPtr &partitioner, const std::string &sourceNameOrVa,
                    const std::string &targetNameOrVa) {
    return edgeForInstructions(partitioner,
                               vertexForInstruction(partitioner, sourceNameOrVa),
                               vertexForInstruction(partitioner, targetNameOrVa));
}

void
assignCallingConventions(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    const CallingConvention::Dictionary &ccDict = partitioner->instructionProvider().callingConventions();
    CallingConvention::Definition::Ptr defaultCc;
    if (!ccDict.empty())
        defaultCc = ccDict[0];
    partitioner->allFunctionCallingConventionDefinition(defaultCc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PathSelector::Predicate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PathSelector::Predicate::wasRejected(bool /*disposition*/, const Rose::BinaryAnalysis::FeasiblePath&,
                                     const Rose::BinaryAnalysis::Partitioner2::CfgPath&,
                                     SgAsmInstruction */*offendingInstruction*/) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Path selector for pruning path outputs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PathSelector::RejectEndNames::shouldReject(const Rose::BinaryAnalysis::FeasiblePath &fpAnalysis,
                                           const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                                           SgAsmInstruction *offendingInstruction) {
    std::pair<size_t, size_t> lastIndices = path.lastInsnIndex(offendingInstruction);
    size_t lastVertexIdx = lastIndices.first;
    std::pair<std::string, std::string> endpointNames = pathEndpointFunctionNames(fpAnalysis, path, lastVertexIdx);
    // Interesting if starts with no name and ends with a name, so reject all others.
    return !endpointNames.first.empty() || endpointNames.second.empty();
}

bool
PathSelector::RejectDuplicateEndpoints::shouldReject(const Rose::BinaryAnalysis::FeasiblePath&,
                                                     const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                                                     SgAsmInstruction*) {
    Address va = path.backVertex()->value().optionalAddress().orElse((Address)(-1));
    size_t seenLength = seen_.getOrElse(va, Rose::UNLIMITED);
    size_t curLength = path.nVertices();
    return curLength >= seenLength;
}

void
PathSelector::RejectDuplicateEndpoints::wasRejected(bool rejected, const Rose::BinaryAnalysis::FeasiblePath&,
                                                     const Rose::BinaryAnalysis::Partitioner2::CfgPath &path,
                                                     SgAsmInstruction*) {
    if (!rejected) {
        Address va = path.backVertex()->value().optionalAddress().orElse((Address)(-1));
        seen_.insert(va, showShorterPaths_ ? path.nVertices() : 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PathSelector
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PathSelector::resetStats() {
    nSelected_ = nRejected_ = 0;
    for (auto predicate: predicates) {
        predicate->nCalls = predicate->nRejects = 0;
    }
}

size_t
PathSelector::operator()(const FeasiblePath &fpAnalysis, const P2::CfgPath &path, SgAsmInstruction *offendingInstruction) {
    size_t pathId = 0;
    shouldReject(fpAnalysis, path, offendingInstruction, pathId /*out*/);
    return pathId;
}

std::shared_ptr<PathSelector::Predicate>
PathSelector::shouldReject(const FeasiblePath &fpAnalysis, const P2::CfgPath &path, SgAsmInstruction *offendingInstruction,
                           size_t &pathId /*out*/) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    pathId = 0;

    // Run the predicates until one says "reject!"
    std::shared_ptr<Predicate> rejector;                // he who says "reject"
    for (auto predicate: predicates) {
        ASSERT_not_null(predicate);
        ++predicate->nCalls;
        if (predicate->shouldReject(fpAnalysis, path, offendingInstruction)) {
            ++predicate->nRejects;                      // nRejects counter is for the specific predicate that said "reject"
            rejector = predicate;
            break;
        }
    }

    // Tell all the predicates what happened.
    bool wasRejected = rejector != nullptr;
    for (auto predicate: predicates)
        predicate->wasRejected(wasRejected, fpAnalysis, path, offendingInstruction);

    if (wasRejected) {
        ++nRejected_;
    } else {
        pathId = ++nSelected_;
    }

    return rejector;
}

std::shared_ptr<PathSelector::Predicate>
PathSelector::findPredicate(const std::string &name) const {
    for (auto predicate: predicates) {
        if (predicate->name == name)
            return predicate;
    }
    return {};
}

size_t
PathSelector::nRejected() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return nRejected_;
}

size_t
PathSelector::nSelected() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return nSelected_;
}

void
PathSelector::maybeTerminate() const {
    if (auto predicate = findPredicate<RejectTooMany>()) {
        if (nSelected_ >= predicate->limit()) {
            std::cerr <<"path limit reached\n";
            _exit(0);
        }
    }
}

void
PathSelector::printStatistics(std::ostream &out, const std::string &prefix) const {
    out <<prefix <<"  total paths reported:                      " <<nSelected() <<"\n";
    out <<prefix <<"  total reports suppressed:                  " <<nRejected() <<"\n";
    for (auto predicate: predicates)
        out <<(boost::format("%s    because %-32s %d\n") %prefix %predicate->description %predicate->nRejects);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction histograms
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus >= 201402L

InsnHistogram
computeInsnHistogram(const InstructionProvider &insns, const MemoryMap::Ptr &map) {
    InsnHistogram histogram;
    Address va = 0;
    while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        const Address aligned = alignUp(va, insns.instructionAlignment());
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
#endif
