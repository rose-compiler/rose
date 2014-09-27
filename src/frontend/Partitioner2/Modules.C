#include "sage3basic.h"
#include "AsmUnparser_compat.h"
#include "rose_strtoull.h"

#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sawyer/CommandLine.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Modules {

bool
AddGhostSuccessors::operator()(bool chain, const Args &args) {
    if (chain) {
        size_t nBits = args.partitioner->instructionProvider().instructionPointerRegister().get_nbits();
        BOOST_FOREACH (rose_addr_t successorVa, args.partitioner->basicBlockGhostSuccessors(args.bblock))
            args.bblock->insertSuccessor(successorVa, nBits);
    }
    return chain;
}

bool
PreventDiscontiguousBlocks::operator()(bool chain, const Args &args) {
    if (chain) {
        bool complete;
        std::vector<rose_addr_t> successors = args.partitioner->basicBlockConcreteSuccessors(args.bblock, &complete);
        if (complete && 1==successors.size() && successors[0]!=args.bblock->fallthroughVa())
            args.results.terminate = TERMINATE_NOW;
    }
    return chain;
}

class AddressIntervalParser: public Sawyer::CommandLine::ValueParser {
protected:
    AddressIntervalParser() {}
    AddressIntervalParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}
public:
    typedef Sawyer::SharedPointer<AddressIntervalParser> Ptr;
    static Ptr instance() {
        return Ptr(new AddressIntervalParser);
    }
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
        return Ptr(new AddressIntervalParser(valueSaver));
    }
private:
    virtual Sawyer::CommandLine::ParsedValue operator()(const char *input, const char **rest,
                                                        const Sawyer::CommandLine::Location &loc) /*override*/ {
        const char *s = input;
        char *r = NULL;
        bool hadRangeError = false, isEmpty = false;
        while (isspace(*s)) ++s;

        // Minimum
        errno = 0;
        rose_addr_t least = rose_strtoull(s, &r, 0);
        if (r==s)
            throw std::runtime_error("unsigned integer expected for interval minimum");
        if (ERANGE==errno)
            hadRangeError = true;
        s = r;

        // Maximum, end, size, or nothing
        rose_addr_t greatest = least;
        while (isspace(*s)) ++s;
        if (','==*s) {                                  // ',' means a max value is specified
            ++s;
            errno = 0;
            greatest = rose_strtoull(s, &r, 0);
            if (r==s)
                throw std::runtime_error("unsigned integer expected for interval maximum");
            if (ERANGE==errno)
                hadRangeError = true;
            s = r;
        } else if ('-'==*s) {                           // '-' means an exclusive end address is specified (think "-" 1)
            ++s;
            errno = 0;
            greatest = rose_strtoull(s, &r, 0) - 1;
            if (r==s)
                throw std::runtime_error("unsigned integer expected for interval end");
            if (ERANGE==errno)
                hadRangeError = true;
            s = r;
        } else if ('+'==*s) {                           // '+' means a size follows (zero is allowed)
            ++s;
            errno = 0;
            rose_addr_t size = rose_strtoull(s, &r, 0);
            if (r==s)
                throw std::runtime_error("unsigned integer expected for interval size");
            if (ERANGE==errno)
                hadRangeError = true;
            if (0==size)
                isEmpty = true;
            greatest = least + size;
        } else if (!*s) {                               // end-of-string means the interval is a singleton
            /*void*/
        }

        // Successful parsing?
        *rest = r;
        std::string parsed(input, *rest-input);
        if (hadRangeError)
            throw std::range_error("overflow when parsing \""+parsed+"\"");
        if (greatest < least)
            throw std::range_error("interval seems backward: \""+parsed+"\"");
        
        AddressInterval val;
        if (!isEmpty)
            val = AddressInterval::hull(least, greatest);
        return Sawyer::CommandLine::ParsedValue(val, loc, parsed, valueSaver());
    }
};

static Sawyer::CommandLine::Parser*
instructionListerParser() {
    using namespace Sawyer::CommandLine;
    static Parser *parser = NULL;
    if (!parser) {
        SwitchGroup switches;
        switches.insert(Switch("bblock")
                        .argument("interval", AddressIntervalParser::instance())
                        .doc("Address or address interval for basic blocks that trigger this debugging aid.  This debugging "
                             "aid is triggered (subject to other constraints) Whenever a basic block whose starting address "
                             "is equal to the specified address or falls within the specified interval is attached to the "
                             "CFG."));
        switches.insert(Switch("select")
                        .argument("interval", AddressIntervalParser::instance())
                        .doc("Address or address interval specifying which instructions should be listed.  Any instruction "
                             "that overlaps the address or interval is listed.  The default is to list all instructions."));
        parser = new Parser;
        parser->with(switches)
            .longPrefix(":").longPrefix(":--")
            .programName("InstructionLister");
    }
    return parser;
}

// class method
std::string
InstructionLister::docString() {
    return ("Lists all CFG-attached instructions overlapping the specified @s{select} address interval when a "
            "basic block or placeholder whose address is in the specified @s{bblock} address interval is attached to the CFG. "
            "The listing indicates address gaps and overlaps with \"+@v{n}\" and \"-@v{n}\" notation where @v{n} is the "
            "number of bytes skipped forward or backward." +
            instructionListerParser()->docForSwitches());
}

// class method
InstructionLister::Ptr
InstructionLister::instance(const std::string &config) {
    std::vector<std::string> args;
    BOOST_FOREACH (const std::string &s, StringUtility::split(':', config))
        args.push_back(":"+s);
    return instance(args);
}

// class method
InstructionLister::Ptr
InstructionLister::instance(const std::vector<std::string> &args) {
    Sawyer::CommandLine::ParserResult cmdline = instructionListerParser()->parse(args);
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("InstructionLister: invalid config: \""+StringUtility::join("", args)+"\"");

    AddressInterval where;
    if (cmdline.have("bblock"))
        where = cmdline.parsed("bblock", 0).as<AddressInterval>();

    AddressInterval what = AddressInterval::whole();
    if (cmdline.have("select"))
        what = cmdline.parsed("select", 0).as<AddressInterval>();

    return instance(where, Trigger::once(), what);
}

bool
InstructionLister::operator()(bool chain, const AttachedBasicBlock &args) {
    using namespace StringUtility;
    if (chain && where_.isContaining(args.startVa) && when_.shouldTrigger() && !what_.isEmpty()) {
        Stream debug(mlog[DEBUG]);
        debug.enable();
        debug <<"InstructionLister triggered: #" <<when_.nCalls() <<" for "
              <<(args.bblock ? "bblock=" : "placeholder=") <<addrToString(args.startVa) <<"\n";
        std::vector<SgAsmInstruction*> insns = args.partitioner->instructionsOverlapping(what_);
        debug <<"  " <<plural(insns.size(), "instructions")
              <<" in [" <<addrToString(what_.least()) <<"," <<addrToString(what_.greatest()) <<"]\n";
        if (!insns.empty()) {
            rose_addr_t va = insns.front()->get_address();
            BOOST_FOREACH (SgAsmInstruction *insn, insns) {
                std::string label;
                if (insn->get_address() > va) {
                    label = "+" + numberToString(insn->get_address()-va);
                } else if (insn->get_address() < va) {
                    label = "-" + numberToString(va-insn->get_address());
                }
                debug <<"    " <<std::setw(6) <<label <<" " <<unparseInstructionWithAddress(insn) <<"\n";
                va = insn->get_address() + insn->get_size();
            }
        }
    }
    return chain;
}

static Sawyer::CommandLine::Parser*
cfgGraphVizDumperParser() {
    using namespace Sawyer::CommandLine;
    static Parser *parser = NULL;
    if (!parser) {
        SwitchGroup switches;
        switches.insert(Switch("bblock")
                        .argument("interval", AddressIntervalParser::instance())
                        .doc("Address or address interval for basic blocks that trigger this debugging aid.  This "
                             "debugging aid is triggered (subject to other constraints) whenever a basic block whose "
                             "starting address is equal to the specified address or falls within the specified interval "
                             "is attached to the CFG."));
        switches.insert(Switch("select")
                        .argument("interval", AddressIntervalParser::instance())
                        .doc("Address or address interval specifying which CFG vertices should be selected to appear in "
                             "the graph.  Any basic block or placeholder whose starting address is within the specified "
                             "interval is included in the output.  The default is to include all vertices."));
        switches.insert(Switch("neighbors")
                        .intrinsicValue("true", booleanParser())
                        .doc("If specified, then the graph will also contain the immediate neighbors of all selected "
                             "vertices, and they will be shown in a different style.  The @s{no-neighbors} switch "
                             "can turn this off.  The default is to show neighbors."));
        switches.insert(Switch("no-neighbors")
                        .key("neighbors")
                        .intrinsicValue("false", booleanParser())
                        .hidden(true));
        switches.insert(Switch("file")
                        .argument("name", anyParser())
                        .doc("Name of file in which the result is stored. This file will be overwritten if it already "
                             "exists.  The first \"%\", if any, will be expanded into a distinct identification string "
                             "consisting of the starting address of the basic block that triggered this debugging aid "
                             "and a sequence number.  The default string is \"cfg-%.dot\"."));
        parser = new Parser;
        parser->with(switches)
            .longPrefix(":").longPrefix(":--")
            .programName("CfgGraphVizDumper");
    }
    return parser;
}

// class method
std::string
CfgGraphVizDumper::docString() {
    return ("Dumps a GraphViz file representing a control flow sub-graph when a basic block or placeholder whose address "
            "is in the specified @s{bblock} address interval is inserted into the CFG.  The graph will contain vertices whose "
            "starting address fall within the @s{select} interval, and optionally neighbors of those vertices." +
            cfgGraphVizDumperParser()->docForSwitches());
}

// class method
CfgGraphVizDumper::Ptr
CfgGraphVizDumper::instance(const std::string &config) {
    std::vector<std::string> args;
    BOOST_FOREACH (const std::string &s, StringUtility::split(':', config))
        args.push_back(":"+s);
    return instance(args);
}

// class method
CfgGraphVizDumper::Ptr
CfgGraphVizDumper::instance(const std::vector<std::string> &args) {
    Sawyer::CommandLine::ParserResult cmdline = cfgGraphVizDumperParser()->parse(args);
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("CfgGraphVizDumper: invalid config: \""+StringUtility::join("", args)+"\"");

    AddressInterval where;
    if (cmdline.have("bblock"))
        where = cmdline.parsed("bblock", 0).as<AddressInterval>();

    AddressInterval what=AddressInterval::whole();
    if (cmdline.have("select"))
        what = cmdline.parsed("select", 0).as<AddressInterval>();

    bool showNeighbors = true;
    if (cmdline.have("neighbors"))
        showNeighbors = cmdline.parsed("neighbors", 0).as<bool>();

    std::string fileName = "cfg-%.dot";
    if (cmdline.have("file"))
        fileName = cmdline.parsed("file", 0).as<std::string>();

    return instance(where, Trigger::once(), what, showNeighbors, fileName);
}

bool
CfgGraphVizDumper::operator()(bool chain, const AttachedBasicBlock &args) {
    using namespace StringUtility;
    if (chain && where_.isContaining(args.startVa) && when_.shouldTrigger() && !what_.isEmpty()) {
        Stream debug(mlog[DEBUG]);
        debug.enable();
        debug <<"CfgGraphVizDumper triggered: #" <<when_.nCalls() <<" for "
              <<(args.bblock ? "bblock=" : "placeholder=") <<addrToString(args.startVa) <<"\n";
        std::string fileName = fileName_;
        if (boost::contains(fileName, "%")) {
            std::string id = numberToString(serialNumber()) + "-" + addrToString(args.startVa).substr(2);
            boost::replace_first(fileName, "%", id);
        }

        std::ofstream out(fileName.c_str());
        if (!out.good()) {
            debug <<"  cannot open file \"" <<cEscape(fileName) <<"\"\n";
            return chain;
        }
        debug <<"  dumping to GraphViz file \"" <<cEscape(fileName) <<"\"\n";
        args.partitioner->cfgGraphViz(out, what_, showNeighbors_);
    }
    return chain;
}
    
AddressIntervalSet
deExecuteZeros(MemoryMap &map /*in,out*/, size_t threshold) {
    AddressIntervalSet changes;
    if (0==threshold)
        return changes;
    rose_addr_t va = map.hull().least();
    AddressInterval zeros;
    uint8_t buf[4096];
    while (AddressInterval accessed = map.atOrAfter(va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf)) {
        size_t nRead = accessed.size();
        size_t firstZero = 0;
        while (firstZero < nRead) {
            while (firstZero<nRead && buf[firstZero]!=0) ++firstZero;
            if (firstZero < nRead) {
                size_t nZeros = 1;
                while (firstZero+nZeros < nRead && buf[firstZero+nZeros]==0) ++nZeros;

                if (zeros.isEmpty()) {
                    zeros = AddressInterval::baseSize(va+firstZero, nZeros);
                } else if (zeros.greatest()+1 == va+firstZero) {
                    zeros = AddressInterval::baseSize(zeros.least(), zeros.size()+nZeros);
                } else {
                    if (zeros.size() >= threshold) {
                        map.within(zeros).changeAccess(0, MemoryMap::EXECUTABLE);
                        changes.insert(zeros);
                    }
                    zeros = AddressInterval::baseSize(va+firstZero, nZeros);
                }

                firstZero += nZeros+1;
            }
        }
        va += nRead;
    }
    if (zeros.size()>=threshold) {
        map.within(zeros).changeAccess(0, MemoryMap::EXECUTABLE);
        changes.insert(zeros);
    }
    return changes;
}

size_t
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader, std::vector<Function::Ptr> &functions) {
    struct T1: AstSimpleProcessing {
        const Partitioner &partitioner;
        SgAsmGenericHeader *fileHeader;
        std::vector<Function::Ptr> &functions;
        size_t nInserted;
        T1(const Partitioner &p, SgAsmGenericHeader *fh, std::vector<Function::Ptr> &functions)
            : partitioner(p), fileHeader(fh), functions(functions), nInserted(0) {}
        void visit(SgNode *node) {
            if (SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node)) {
                if (symbol->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
                    symbol->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
                    symbol->get_value()     != 0) {
                    rose_addr_t value = fileHeader->get_base_va() + symbol->get_value();
                    SgAsmGenericSection *section = symbol->get_bound();

                    // Add a function at the symbol's value. If the symbol is bound to a section and the section is mapped at a
                    // different address than it expected to be mapped, then adjust the symbol's value by the same amount.
                    rose_addr_t va = value;
                    if (section!=NULL && section->is_mapped() &&
                        section->get_mapped_preferred_va() != section->get_mapped_actual_va()) {
                        va += section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                    }
                    if (partitioner.discoverInstruction(va)) {
                        Function::Ptr function = Function::instance(va, symbol->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_SYMBOL);
                        if (insertUnique(functions, function, sortFunctionsByAddress))
                            ++nInserted;
                    }

                    // Sometimes weak symbol values are offsets from a section (this code handles that), but other times
                    // they're the value is used directly (the above code handled that case). */
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK)
                        value += section->get_mapped_actual_va();
                    if (partitioner.discoverInstruction(value)) {
                        Function::Ptr function = Function::instance(value, symbol->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_SYMBOL);
                        if (insertUnique(functions, function, sortFunctionsByAddress))
                            ++nInserted;
                    }
                }
            }
        }
    } t1(partitioner, fileHeader, functions);
    t1.traverse(fileHeader, preorder);
    return t1.nInserted;
}

std::vector<Function::Ptr>
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader) {
    std::vector<Function::Ptr> functions;
    findSymbolFunctions(partitioner, fileHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findSymbolFunctions(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findSymbolFunctions(partitioner, fileHeader, functions);
    }
    return functions;
}

} // namespace
} // namespace
} // namespace
} // namespace
