#include "sage3basic.h"
#include "AsmUnparser_compat.h"

#include <BinaryDemangler.h>
#include <BinaryString.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <Sawyer/CommandLine.h>
#include <stringify.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Modules {

// Convert "KERNEL32.dll:EncodePointer" to "EncodePointer@KERNEL32.dll" and similar
std::string
canonicalFunctionName(const std::string &name) {
    size_t colon = name.find_first_of(':');
    if (colon != std::string::npos &&
        colon > 4 && name.substr(colon-4, 4)==".dll" &&
        colon+1 < name.size() && name[colon+1]!=':') {
        std::string library = name.substr(0, colon);
        std::string function = name.substr(colon+1);
        return function + "@" + library;
    }
    return name;
}

void
demangleFunctionNames(const Partitioner &p) {
    // The demangler is most efficient if we give it all the names at once.
    std::vector<std::string> mangledNames;
    BOOST_FOREACH (const Function::Ptr &f, p.functions()) {
        if (!f->name().empty() && f->name() == f->demangledName())
            mangledNames.push_back(f->name());
    }

    // Demangle everything that possible to demangle.  An exception probably means that c++filt is not available or doesn't
    // work.
    Demangler demangler;
    try {
        demangler.fillCache(mangledNames);
    } catch (const std::runtime_error &e) {
        mlog[WARN] <<"name demangler failed: " <<e.what() <<"\n";
        return;
    }

    BOOST_FOREACH (const Function::Ptr &f, p.functions()) {
        if (!f->name().empty() && f->name() == f->demangledName()) { // same condition as above
            std::string demangled = demangler.demangle(f->name());
            if (demangled != f->name())
                f->demangledName(demangled);
        }
    }
}

bool
AddGhostSuccessors::operator()(bool chain, const Args &args) {
    if (chain) {
        size_t nBits = args.partitioner.instructionProvider().instructionPointerRegister().nBits();
        BOOST_FOREACH (rose_addr_t successorVa, args.partitioner.basicBlockGhostSuccessors(args.bblock))
            args.bblock->insertSuccessor(successorVa, nBits);
    }
    return chain;
}

bool
PreventDiscontiguousBlocks::operator()(bool chain, const Args &args) {
    if (chain) {
        bool complete;
        std::vector<rose_addr_t> successors = args.partitioner.basicBlockConcreteSuccessors(args.bblock, &complete);
        if (complete && 1==successors.size() && successors[0]!=args.bblock->fallthroughVa())
            args.results.terminate = TERMINATE_NOW;
    }
    return chain;
}

bool
BasicBlockSizeLimiter::operator()(bool chain, const Args &args) {
    if (chain && maxInsns_ > 0 && args.bblock->nInstructions() >= maxInsns_) {
        args.results.terminate = TERMINATE_NOW;
        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"BasicBlockSizeLimiter triggered (max " <<StringUtility::plural(maxInsns_, "instructions") <<"):\n";
            BOOST_FOREACH (SgAsmInstruction *insn, args.bblock->instructions())
                mlog[DEBUG] <<"        " <<unparseInstructionWithAddress(insn) <<"\n";
        }
    }
    return chain;
}

// class method
Sawyer::CommandLine::SwitchGroup
InstructionLister::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;
    switches.insert(Switch("bblock")
                    .argument("interval", addressIntervalParser(settings.where))
                    .doc("Address or address interval for basic blocks that trigger this debugging aid.  This debugging "
                         "aid is triggered (subject to other constraints) Whenever a basic block whose starting address "
                         "is equal to the specified address or falls within the specified interval is attached to the "
                         "CFG."));
    switches.insert(Trigger::switches(settings.when));
    switches.insert(Switch("select")
                    .argument("interval", addressIntervalParser(settings.what))
                    .doc("Address or address interval specifying which instructions should be listed.  Any instruction "
                         "that overlaps the address or interval is listed.  The default is to list all instructions."));
    return switches;
}

// class method
std::string
InstructionLister::docString() {
    Settings settings;
    return ("Lists all CFG-attached instructions overlapping the specified @s{select}{noerror} address interval when a "
            "basic block or placeholder whose address is in the specified @s{bblock}{noerror} address interval is attached "
            "to the CFG. The listing indicates address gaps and overlaps with \"+@v{n}\" and \"-@v{n}\" notation where "
            "@v{n} is the number of bytes skipped forward or backward." +
            Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
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
    Settings settings;
    Sawyer::CommandLine::Parser parser;
    parser.with(switches(settings)).longPrefix(":").longPrefix(":--").programName("InstructionLister");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(args).apply();
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("InstructionLister: invalid config: \""+StringUtility::join("", args)+"\"");
    return instance(settings);
}

bool
InstructionLister::operator()(bool chain, const AttachedBasicBlock &args) {
    using namespace StringUtility;
    if (chain && settings_.where.isContaining(args.startVa) && trigger_.shouldTrigger() && !settings_.what.isEmpty()) {
        Stream debug(mlog[DEBUG]);
        debug.enable();
        debug <<"InstructionLister triggered: #" <<(trigger_.nCalls()-1) <<" for "
              <<(args.bblock ? "bblock=" : "placeholder=") <<addrToString(args.startVa) <<"\n";
        std::vector<SgAsmInstruction*> insns = args.partitioner->instructionsOverlapping(settings_.what);
        debug <<"  " <<plural(insns.size(), "instructions")
              <<" in [" <<addrToString(settings_.what.least()) <<"," <<addrToString(settings_.what.greatest()) <<"]\n";
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

// class method
Sawyer::CommandLine::SwitchGroup
CfgGraphVizDumper::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;
    switches.insert(Switch("bblock")
                    .argument("interval", addressIntervalParser(settings.where))
                    .doc("Address or address interval for basic blocks that trigger this debugging aid.  This "
                         "debugging aid is triggered (subject to other constraints) whenever a basic block whose "
                         "starting address is equal to the specified address or falls within the specified interval "
                         "is attached to the CFG."));
    switches.insert(Trigger::switches(settings.when));
    switches.insert(Switch("select")
                    .argument("interval", addressIntervalParser(settings.what))
                    .doc("Address or address interval specifying which CFG vertices should be selected to appear in "
                         "the graph.  Any basic block or placeholder whose starting address is within the specified "
                         "interval is included in the output.  The default is to include all vertices."));
    switches.insert(Switch("neighbors")
                    .intrinsicValue("true", booleanParser(settings.showNeighbors))
                    .doc("If specified, then the graph will also contain the immediate neighbors of all selected "
                         "vertices, and they will be shown in a different style.  The @s{no-neighbors}{noerror} switch "
                         "can turn this off.  The default is to " +
                         std::string(settings.showNeighbors?"":"not ") + "show neighbors."));
    switches.insert(Switch("no-neighbors")
                    .key("neighbors")
                    .intrinsicValue("false", booleanParser(settings.showNeighbors))
                    .hidden(true));
    switches.insert(Switch("file")
                    .argument("name", anyParser(settings.fileName))
                    .doc("Name of file in which the result is stored. This file will be overwritten if it already "
                         "exists.  The first \"%\", if any, will be expanded into a distinct identification string "
                         "consisting of the starting address of the basic block that triggered this debugging aid "
                         "and a sequence number.  The default string is \"" +
                         StringUtility::cEscape(settings.fileName) + "\"."));
    return switches;
}

// class method
std::string
CfgGraphVizDumper::docString() {
    Settings settings;
    return ("Dumps a GraphViz file representing a control flow sub-graph when a basic block or placeholder whose address "
            "is in the specified @s{bblock}{noerror} address interval is inserted into the CFG.  The graph will contain "
            "vertices whose starting address fall within the @s{select}{noerror} interval, and optionally neighbors of "
            "those vertices." + Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
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
    Settings settings;
    Sawyer::CommandLine::Parser parser;
    parser.with(switches(settings)).longPrefix(":").longPrefix(":--").programName("CfgGraphVizDumper"); 
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(args).apply();
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("CfgGraphVizDumper: invalid config: \""+StringUtility::join("", args)+"\"");
    return instance(settings);
}

bool
CfgGraphVizDumper::operator()(bool chain, const AttachedBasicBlock &args) {
    using namespace StringUtility;
    if (chain && settings_.where.isContaining(args.startVa) && trigger_.shouldTrigger() && !settings_.what.isEmpty()) {
        Stream debug(mlog[DEBUG]);
        debug.enable();
        debug <<"CfgGraphVizDumper triggered: #" <<(trigger_.nCalls()-1) <<" for "
              <<(args.bblock ? "bblock=" : "placeholder=") <<addrToString(args.startVa) <<"\n";
        std::string fileName = settings_.fileName;
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
        args.partitioner->cfgGraphViz(out, settings_.what, settings_.showNeighbors);
    }
    return chain;
}

// class method
Sawyer::CommandLine::SwitchGroup
HexDumper::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;
    switches.insert(Switch("bblock")
                    .argument("interval", addressIntervalParser(settings.where))
                    .doc("Address or address interval for basic blocks that trigger this debugging aid.  This debugging "
                         "aid is triggered (subject to other constraints) Whenever a basic block whose starting address "
                         "is equal to the specified address or falls within the specified interval is attached to the "
                         "CFG."));
    switches.insert(Trigger::switches(settings.when));
    switches.insert(Switch("select")
                    .argument("interval", addressIntervalParser(settings.what))
                    .doc("Address or address interval specifying which bytes should be listed."));

    switches.insert(Switch("accent")
                    .intrinsicValue(true, settings.accentSpecialValues)
                    .doc("Causes zero bytes to be output as a single dot \".\" and 0xff bytes to be output as the "
                         "string \"##\".  This emphasizes these two important values in the output.  The "
                         "@s{no-accent}{noerror} switch causes them to output as \"00\" and \"ff\". The default is to " +
                         std::string(settings.accentSpecialValues?"":"not ") + "accent them."));
    switches.insert(Switch("no-accent")
                    .key("accent")
                    .intrinsicValue(false, settings.accentSpecialValues)
                    .hidden(true));
    return switches;
}

// class method
std::string
HexDumper::docString() {
    Settings settings;
    return ("Lists virtual memory contents in standard hexdump format when a basic block or placeholder whose address is "
            "in the specified @s{bblock}{noerror} address interval is attached to the CFG." +
            Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
}

// class method
HexDumper::Ptr
HexDumper::instance(const std::string &config) {
    std::vector<std::string> args;
    BOOST_FOREACH (const std::string &s, StringUtility::split(':', config))
        args.push_back(":"+s);
    return instance(args);
}

// class method
HexDumper::Ptr
HexDumper::instance(const std::vector<std::string> &args) {
    Settings settings;
    Sawyer::CommandLine::Parser parser;
    parser.with(switches(settings)).longPrefix(":").longPrefix(":--").programName("HexDumper");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(args).apply();
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("HexDumper: invalid config: \""+StringUtility::join("", args)+"\"");
    return instance(settings);
}

bool
HexDumper::operator()(bool chain, const AttachedBasicBlock &args) {
    using namespace StringUtility;
    if (chain && settings_.where.isContaining(args.startVa) && trigger_.shouldTrigger() && !settings_.what.isEmpty()) {
        Stream debug(mlog[DEBUG]);
        debug.enable();
        debug <<"HexDumper triggered: #" <<(trigger_.nCalls()-1) <<" for "
              <<(args.bblock ? "bblock=" : "placeholder=") <<addrToString(args.startVa) <<"\n";
        HexdumpFormat fmt;
        if (settings_.accentSpecialValues) {
            fmt.numeric_fmt_special[0x00] = " .";       // make zeros less obtrusive
            fmt.numeric_fmt_special[0xff] = "##";       // make 0xff more obtrusive
        }
        fmt.prefix = "    ";                            // prefix before each line

        rose_addr_t va = settings_.what.least();
        while (AddressInterval avail = args.partitioner->memoryMap()->atOrAfter(va).singleSegment().available()) {
            if (avail.least() > settings_.what.greatest())
                break;
            const size_t nPrint = std::min(settings_.what.greatest()+1-avail.least(), avail.size());
            const MemoryMap::Node &node = *args.partitioner->memoryMap()->find(avail.least());
            const MemoryMap::Segment &segment = node.value();
            const AddressInterval segmentInterval = node.key();
            size_t offsetWithinSegment = avail.least() - segmentInterval.least();
            size_t offsetWithinBuffer = segment.offset() + offsetWithinSegment;
            debug <<"  Segment \"" <<cEscape(segment.name()) <<"\" + " <<toHex(offsetWithinSegment) <<"\n";
            ASSERT_require(offsetWithinBuffer < segment.buffer()->size());
            ASSERT_require(nPrint <= segment.buffer()->available(offsetWithinBuffer));
            const unsigned char *data = (const unsigned char*)segment.buffer()->data() + offsetWithinBuffer;
            ASSERT_not_null(data);
            debug <<fmt.prefix;
            SgAsmExecutableFileFormat::hexdump(debug, avail.least(), data, nPrint, fmt);
            debug <<"\n";
            if (avail.least() + (nPrint-1) == settings_.what.greatest())
                break;                                  // avoid possible overflow
            va = avail.greatest() + 1;
        }
    }
    return chain;
}

// class method
Sawyer::CommandLine::SwitchGroup
Debugger::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;
    switches.insert(Switch("bblock")
                    .argument("interval", addressIntervalParser(settings.where))
                    .doc("Address or address interval for basic blocks that trigger this debugging aid.  This debugging "
                         "aid is triggered (subject to other constraints) Whenever a basic block whose starting address "
                         "is equal to the specified address or falls within the specified interval is attached to the "
                         "CFG."));
    switches.insert(Trigger::switches(settings.when));
    return switches;
}

// class method
std::string
Debugger::docString() {
    Settings settings;
    return ("Calls Debugger::debug, which is a convenient point to attach a debugger such as GDB.  The method is called "
            "when a basic block or placeholder whose address is in the specified @s{bblock}{noerror} address interval is "
            "attached to the CFG." + Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
}

// class method
Debugger::Ptr
Debugger::instance(const std::string &config) {
    std::vector<std::string> args;
    BOOST_FOREACH (const std::string &s, StringUtility::split(':', config))
        args.push_back(":"+s);
    return instance(args);
}

// class method
Debugger::Ptr
Debugger::instance(const std::vector<std::string> &args) {
    Settings settings;
    Sawyer::CommandLine::Parser parser;
    parser.with(switches(settings)).longPrefix(":").longPrefix(":--").programName("Debugger");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(args).apply();
    if (!cmdline.unreachedArgs().empty())
        throw std::runtime_error("Debugger: invalid config: \"" + StringUtility::join("", args) + "\"");
    return instance(settings);
}

bool
Debugger::operator()(bool chain, const AttachedBasicBlock &args) {
    if (chain && settings_.where.isContaining(args.startVa) && trigger_.shouldTrigger())
        debug(args.startVa, args.bblock);
    return chain;
}

void
Debugger::debug(rose_addr_t va, const BasicBlock::Ptr &bblock) {
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
    debug.enable();
    size_t callNumber = trigger_.nCalls() - 1;
    bool isBblock = bblock != NULL;
    debug <<"Debugger triggered: #" <<callNumber <<" for " <<(isBblock?"bblock=":"placeholder=") <<addrToString(va) <<"\n";
}

bool
MatchThunk::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // Disassemble the next few undiscovered instructions
    static const size_t maxInsns = 2;                   // max length of a thunk
    std::vector<SgAsmInstruction*> insns;
    rose_addr_t va = anchor;
    for (size_t i=0; i<maxInsns; ++i) {
        if (partitioner.instructionExists(va))
            break;                                      // look only for undiscovered instructions
        SgAsmInstruction *insn = partitioner.discoverInstruction(va);
        if (!insn)
            break;
        insns.push_back(insn);
        va += insn->get_size();
    }
    if (insns.empty())
        return false;

    functions_.clear();
    ThunkDetection found = predicates_->isThunk(partitioner, insns);
    if (!found)
        return false;

    // This is a thunk
    Function::Ptr thunk = Function::instance(anchor, SgAsmFunction::FUNC_THUNK);
    if (!found.name.empty())
        thunk->reasonComment("matched " + found.name);
    functions_.push_back(thunk);

    // Do we know the successors?  They would be the function(s) to which the thunk branches.
    BasicBlock::Ptr bb = BasicBlock::instance(anchor, partitioner);
    for (size_t i=0; i<found.nInsns; ++i)
        bb->append(partitioner, insns[i]);
    BOOST_FOREACH (const BasicBlock::Successor &successor, partitioner.basicBlockSuccessors(bb)) {
        if (successor.expr()->is_number()) {
            rose_addr_t targetVa = successor.expr()->get_number();
            if (Function::Ptr thunkTarget = partitioner.functionExists(targetVa)) {
                thunkTarget->insertReasons(SgAsmFunction::FUNC_THUNK_TARGET);
                if (thunkTarget->reasonComment().empty())
                    thunkTarget->reasonComment("target of thunk " + thunk->printableName());
            } else {
                thunkTarget = Function::instance(targetVa, SgAsmFunction::FUNC_THUNK_TARGET);
                thunkTarget->reasonComment("target of thunk " + thunk->printableName());
                insertUnique(functions_, thunkTarget, sortFunctionsByAddress);
            }
        }
    }

    return true;
}

AddressIntervalSet
deExecuteZeros(const MemoryMap::Ptr &map /*in,out*/, size_t threshold, size_t leaveAtFront, size_t leaveAtBack) {
    ASSERT_not_null(map);
    AddressIntervalSet changes;
    if (leaveAtFront + leaveAtBack >= threshold)
        return changes;
    rose_addr_t va = map->hull().least();
    AddressInterval zeros;
    uint8_t buf[4096];
    while (AddressInterval accessed = map->atOrAfter(va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf)) {
        size_t nRead = accessed.size();
        size_t firstZero = 0;
        while (firstZero < nRead) {
            while (firstZero<nRead && buf[firstZero]!=0) ++firstZero;
            if (firstZero < nRead) {
                size_t nZeros = 1;
                while (firstZero+nZeros < nRead && buf[firstZero+nZeros]==0) ++nZeros;

                if (zeros.isEmpty()) {
                    zeros = AddressInterval::baseSize(accessed.least()+firstZero, nZeros);
                } else if (zeros.greatest()+1 == accessed.least()+firstZero) {
                    zeros = AddressInterval::baseSize(zeros.least(), zeros.size()+nZeros);
                } else {
                    if (zeros.size() >= threshold && zeros.size() > leaveAtFront + leaveAtBack) {
                        AddressInterval affected = AddressInterval::hull(zeros.least()+leaveAtFront,
                                                                         zeros.greatest()-leaveAtBack);
                        map->within(affected).changeAccess(0, MemoryMap::EXECUTABLE);
                        changes.insert(affected);
                    }
                    zeros = AddressInterval::baseSize(va+firstZero, nZeros);
                }

                firstZero += nZeros+1;
            }
        }
        if (accessed.greatest() == map->greatest())
            break;                                      // avoid possible overflow in next statement
        va = accessed.greatest() + 1;
    }
    if (zeros.size() >= threshold && zeros.size() > leaveAtFront + leaveAtBack) {
        AddressInterval affected = AddressInterval::hull(zeros.least()+leaveAtFront, zeros.greatest()-leaveAtBack);
        map->within(affected).changeAccess(0, MemoryMap::EXECUTABLE);
        changes.insert(affected);
    }
    return changes;
}

void
labelSymbolAddresses(Partitioner &partitioner, SgAsmGenericHeader *fileHeader) {
    struct T1: AstSimpleProcessing {
        Partitioner &partitioner;
        SgAsmGenericHeader *fileHeader;
        T1(Partitioner &partitioner, SgAsmGenericHeader *fileHeader)
            : partitioner(partitioner), fileHeader(fileHeader) {}
        void visit(SgNode *node) {
            if (SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node)) {
                std::string name = symbol->get_name()->get_string();
                if (!name.empty()) {
                    if (symbol->get_type() != SgAsmGenericSymbol::SYM_NO_TYPE) {
                        std::string typeName = stringifySgAsmGenericSymbolSymbolType(symbol->get_type(), "SYM_");
                        name = "(" + boost::to_lower_copy(typeName) + ")" + name;
                    }

                    rose_addr_t value = fileHeader->get_base_va() + symbol->get_value();
                    SgAsmGenericSection *section = symbol->get_bound();

                    // Assume symbol's value is a virtual address, but make adjustments if its section is relocated
                    rose_addr_t va = value;
                    if (section && section->is_mapped() &&
                        section->get_mapped_preferred_va() != section->get_mapped_actual_va()) {
                        va += section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                    }
                    if (partitioner.memoryMap()->at(va).exists())
                        partitioner.addressName(va, name);

                    // Sometimes weak symbol values are offsets w.r.t. their linked section.
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK) {
                        va = value + section->get_mapped_actual_va();
                        if (partitioner.memoryMap()->at(va).exists())
                            partitioner.addressName(va, name);
                    }
                }
            }
        }
    } t1(partitioner, fileHeader);
    t1.traverse(fileHeader, preorder);
}

void
nameStrings(const Partitioner &partitioner) {
    struct T1: AstSimpleProcessing {
        Sawyer::Container::Map<rose_addr_t, std::string> seen;
        const Partitioner &partitioner;
        Strings::StringFinder stringFinder;

        T1(const Partitioner &partitioner): partitioner(partitioner) {
            stringFinder.settings().minLength = 1;
            stringFinder.settings().maxLength = 65536;
            stringFinder.settings().keepingOnlyLongest = true;
            ByteOrder::Endianness sex = partitioner.instructionProvider().defaultByteOrder();
            if (sex == ByteOrder::ORDER_UNSPECIFIED)
                sex = ByteOrder::ORDER_LSB;
            stringFinder.insertCommonEncoders(sex);
        }
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                if (ival->get_comment().empty()) {
                    rose_addr_t va = ival->get_absoluteValue();
                    std::string label;
                    if (seen.getOptional(va).assignTo(label)) {
                        ival->set_comment(label);
                    } else if (partitioner.instructionsOverlapping(va).empty()) {
                        stringFinder.reset();
                        stringFinder.find(partitioner.memoryMap()->at(va));
                        if (!stringFinder.strings().empty()) {
                            ASSERT_require(stringFinder.strings().front().address() == va);
                            std::string str = stringFinder.strings().front().narrow(); // front is the longest string
                            static const size_t displayLength = 25;     // arbitrary
                            static const size_t maxLength = displayLength + 7; // strlen("+N more")
                            size_t nTruncated = 0;
                            if (str.size() > maxLength) {
                                nTruncated = str.size() - displayLength;
                                str = str.substr(0, displayLength);
                            }
                            label = "\"" + StringUtility::cEscape(str) + "\"";
                            if (nTruncated)
                                label += "+" + StringUtility::numberToString(nTruncated) + " more";
                            ival->set_comment(label);
                        }
                        seen.insert(va, label); // even if label is empty, so we don't spend time re-searching strings
                    }
                }
            }
        }
    } t1(partitioner);
    BOOST_FOREACH (SgAsmInstruction *insn, partitioner.instructionsOverlapping(AddressInterval::whole()))
        t1.traverse(insn, preorder);
}

void
labelSymbolAddresses(Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            labelSymbolAddresses(partitioner, fileHeader);
    }
}

size_t
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader, std::vector<Function::Ptr> &functions) {
    typedef Sawyer::Container::Map<rose_addr_t, std::string> AddrNames;

    // This traversal only finds the addresses for the new functions, it does not modify the AST since that's a dangerous thing
    // to do while we're traversing it.  It shouldn't make any difference though because we're traversing a different part of
    // the AST (the ELF/PE container) than we're modifying (instructions).
    struct T1: AstSimpleProcessing {
        const Partitioner &partitioner;
        SgAsmGenericHeader *fileHeader;
        AddrNames addrNames;

        T1(const Partitioner &p, SgAsmGenericHeader *h)
            : partitioner(p), fileHeader(h) {}

        void visit(SgNode *node) {
            if (SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node)) {
                if (symbol->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
                    symbol->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
                    symbol->get_value()     != 0) {
                    rose_addr_t value = fileHeader->get_base_va() + symbol->get_value();
                    SgAsmGenericSection *section = symbol->get_bound();

                    // Add a function at the symbol's value. If the symbol is bound to a section and the section is mapped at a
                    // different address than it expected to be mapped, then adjust the symbol's value by the same amount. Keep
                    // only the first non-empty name we find for each address.
                    rose_addr_t va = value;
                    if (section!=NULL && section->is_mapped() &&
                        section->get_mapped_preferred_va() != section->get_mapped_actual_va()) {
                        va += section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                    }
                    if (partitioner.discoverInstruction(va)) {
                        std::string &s = addrNames.insertMaybeDefault(va);
                        if (s.empty())
                            s = symbol->get_name()->get_string();
                    }

                    // Sometimes weak symbol values are offsets from a section (this code handles that), but other times
                    // they're the value is used directly (the above code handled that case). */
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK)
                        value += section->get_mapped_actual_va();
                    if (partitioner.discoverInstruction(value)) {
                        std::string &s = addrNames.insertMaybeDefault(value);
                        if (s.empty())
                            s = symbol->get_name()->get_string();
                    }
                }
            }
        }
    } t1(partitioner, fileHeader);

    size_t nInserted = 0;
    t1.traverse(fileHeader, preorder);
    BOOST_FOREACH (const AddrNames::Node &node, t1.addrNames.nodes()) {
        Function::Ptr function = Function::instance(node.key(), node.value(), SgAsmFunction::FUNC_SYMBOL);
        if (insertUnique(functions, function, sortFunctionsByAddress))
            ++nInserted;
    }
                    
    return nInserted;
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

void
nameConstants(const Partitioner &partitioner) {
    struct ConstantNamer: AstSimpleProcessing {
        const Partitioner &partitioner;
        ConstantNamer(const Partitioner &partitioner): partitioner(partitioner) {}
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                if (ival->get_comment().empty())
                    ival->set_comment(partitioner.addressName(ival->get_absoluteValue()));
            }
        }
    } constantRenamer(partitioner);

    BOOST_FOREACH (SgAsmInstruction *insn, partitioner.instructionsOverlapping(AddressInterval::whole()))
        constantRenamer.traverse(insn, preorder);
}

boost::logic::tribool
isStackBasedReturn(const Partitioner &partitioner, const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    BasicBlockSemantics sem = bb->semantics();
    BaseSemantics::StatePtr state = sem.finalState();
    if (!state)
        return boost::logic::indeterminate;

    ASSERT_not_null(sem.dispatcher);
    ASSERT_not_null(sem.operators);
    SAWYER_MESG(debug) <<"  block has semantic information\n";
    const RegisterDescriptor REG_IP = partitioner.instructionProvider().instructionPointerRegister();
    const RegisterDescriptor REG_SP = partitioner.instructionProvider().stackPointerRegister();
    const RegisterDescriptor REG_SS = partitioner.instructionProvider().stackSegmentRegister();

    // Find the pointer to the return address. Since the return instruction (e.g., x86 RET) has been processed semantically
    // already, the return address is beyond the end of the stack.  Here we handle architecture-specific instructions that
    // might pop more than just the return address (e.g., x86 "RET 4").
    BaseSemantics::SValuePtr stackOffset;           // added to stack ptr to get ptr to return address
    if (SgAsmX86Instruction *x86insn = isSgAsmX86Instruction(bb->instructions().back())) {
        if ((x86insn->get_kind() == x86_ret || x86insn->get_kind() == x86_retf) &&
            x86insn->nOperands() == 1 &&
            isSgAsmIntegerValueExpression(x86insn->operand(0))) {
            uint64_t nbytes = isSgAsmIntegerValueExpression(x86insn->operand(0))
                              ->get_absoluteValue();
            nbytes += REG_IP.nBits() / 8;       // size of return address
            stackOffset = sem.operators->negate(sem.operators->number_(REG_IP.nBits(), nbytes));
        }
    }
    if (!stackOffset) {
        // If no special case above, assume return address is the word beyond the top-of-stack and that the stack grows
        // downward.
        stackOffset = sem.operators->negate(sem.operators->number_(REG_IP.nBits(), REG_IP.nBits()/8));
    }
    BaseSemantics::SValuePtr sp = sem.operators->peekRegister(REG_SP);
    BaseSemantics::SValuePtr retAddrPtr = sem.operators->add(sp, stackOffset);

    // Now that we have the ptr to the return address, read it from the stack and compare it with the new instruction
    // pointer. If equal, then the basic block returns to the caller.
    BaseSemantics::SValuePtr retAddr = sem.operators->undefined_(REG_IP.nBits());
    retAddr = sem.operators->peekMemory(REG_SS, retAddrPtr, retAddr);
    BaseSemantics::SValuePtr ip = sem.operators->peekRegister(REG_IP);
    BaseSemantics::SValuePtr isEqual =
        sem.operators->equalToZero(sem.operators->add(retAddr, sem.operators->negate(ip)));
    bool isReturn = isEqual->is_number() ? (isEqual->get_number() != 0) : false;

    if (debug) {
        debug <<"    stackOffset  = " <<*stackOffset <<"\n";
        debug <<"    sp           = " <<*sp <<"\n";
        debug <<"    retAddrPtr   = " <<*retAddrPtr <<"\n";
        debug <<"    retAddr      = " <<*retAddr <<"\n";
        debug <<"    ip           = " <<*ip <<"\n";
        debug <<"    retAddr==ip? = " <<*isEqual <<"\n";
        debug <<"    returning " <<(isReturn ? "true" : "false") <<"\n";
        //debug <<"    state:" <<*state; // produces lots of output!
    }

    return isReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AST-building functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
buildBasicBlockAst(const Partitioner &partitioner, const BasicBlock::Ptr &bb, const Function::Ptr &parentFunction,
                   const AstConstructionSettings &settings) {
    ASSERT_not_null(bb);
    ASSERT_not_null(parentFunction);
    if (bb->isEmpty()) {
        if (settings.allowEmptyBasicBlocks) {
            mlog[WARN] <<"creating an empty basic block AST node at " <<StringUtility::addrToString(bb->address()) <<"\n";
        } else {
            return NULL;
        }
    }

    ControlFlowGraph::ConstVertexIterator bblockVertex = partitioner.findPlaceholder(bb->address());

    std::vector<SgAsmInstruction*> insns = bb->instructions();
    for (size_t i=0; i<insns.size(); ++i) {
        if (settings.copyAllInstructions) {
            SgTreeCopy deep;
            SgAsmInstruction *newInsn = isSgAsmInstruction(insns[i]->copy(deep));
            ASSERT_not_null(newInsn);
            ASSERT_require(newInsn != insns[i]);
            ASSERT_require(newInsn->get_address() == insns[i]->get_address());
            insns[i] = newInsn;
        }
    }

    SgAsmBlock *ast = SageBuilderAsm::buildBasicBlock(insns);
    ast->set_comment(bb->comment());
    unsigned reasons = 0;

    // Is this block an entry point for any function (not just the parent function)?
    if (bblockVertex->value().isEntryBlock())
        reasons |= SgAsmBlock::BLK_ENTRY_POINT;

    // Does this block have any predecessors?
    if (bblockVertex != partitioner.cfg().vertices().end() && bblockVertex->nInEdges()==0)
        reasons |= SgAsmBlock::BLK_CFGHEAD;

    ast->set_reason(reasons);
    ast->set_code_likelihood(1.0);                      // FIXME[Robb P. Matzke 2014-08-07]

    // Stack delta analysis results for this block in the context of the owning function. If blocks are shared they can have
    // different stack deltas (e.g., eax might have different values in function F and G which means the block "sub esp, eax"
    // has a different stack delta in each function.  Since we're creating a SgAsmBlock for a particular function we need to
    // use stack delta results in the context of that function.
    const StackDelta::Analysis &sdAnalysis = parentFunction->stackDeltaAnalysis();
    ast->set_stackDeltaOut(sdAnalysis.toInt(sdAnalysis.basicBlockOutputStackDeltaWrtFunction(bb->address())));

    // Stack delta analysis results for each instruction in the context of the owning function.  Instructions are shared in the
    // AST if their blocks are shared in the partitioner.  In the AST we share instructions instead of blocks because this
    // makes it easier to attach analysis results to blocks without worrying about whether they're shared -- it's less likely
    // that binary analysis results are attached to instructions.  The problem is that an instruction can have multiple stack
    // deltas based on which function was analyzed. We must choose one function's stack deltas and we must make sure they're
    // current. The approach we take is simple: just store the stack deltas each time and let the last one win.
    if (sdAnalysis.hasResults()) {
        BOOST_FOREACH (SgAsmInstruction *insn, insns)
            insn->set_stackDeltaIn(sdAnalysis.toInt(sdAnalysis.instructionInputStackDeltaWrtFunction(insn)));
    }

    // Cache the basic block successors in the AST since we've already computed them.  If the basic block is in the CFG then we
    // can use the CFG's edges to initialize the AST successors since they are canonical. Otherwise we'll use the successors
    // from bb. In any case, we fill in the successor SgAsmIntegerValueExpression objects with only the address and not
    // pointers to SgAsmBlock IR nodes since we don't have all the blocks yet; the SgAsmBlock pointers will be initialized
    // higher up on the AST-building stack.
    if (bblockVertex != partitioner.cfg().vertices().end()) {
        bool isComplete = true;
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, bblockVertex->outEdges()) {
            const ControlFlowGraph::Vertex &target = *edge.target();
            if (target.value().type() == V_INDETERMINATE || target.value().type() == V_NONEXISTING) {
                isComplete = false;
            } else {
                ASSERT_require(target.value().type() == V_BASIC_BLOCK);
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(target.value().address());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            }
        }
        ast->set_successors_complete(isComplete);
    } else {
        bool isComplete = true;
        BOOST_FOREACH (rose_addr_t successorVa, partitioner.basicBlockConcreteSuccessors(bb, &isComplete)) {
            SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(successorVa);
            succ->set_parent(ast);
            ast->get_successors().push_back(succ);
        }
        ast->set_successors_complete(isComplete);
    }
    return ast;
}

SgAsmBlock*
buildDataBlockAst(const Partitioner &partitioner, const DataBlock::Ptr &dblock, const AstConstructionSettings &settings) {
    // Build the static data item
    SgUnsignedCharList rawBytes(dblock->size(), 0);
    size_t nRead = partitioner.memoryMap()->at(dblock->address()).read(rawBytes).size();
    ASSERT_always_require(nRead==dblock->size());
    SgAsmStaticData *datum = SageBuilderAsm::buildStaticData(dblock->address(), rawBytes);

    // Build the data block IR node pointing to the static item
    return SageBuilderAsm::buildDataBlock(datum);
}

SgAsmFunction*
buildFunctionAst(const Partitioner &partitioner, const Function::Ptr &function, const AstConstructionSettings &settings) {
    ASSERT_not_null(function);

    // Build the child basic block IR nodes and remember all the data blocks
    std::vector<DataBlock::Ptr> dblocks = function->dataBlocks();
    std::vector<SgAsmBlock*> children;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(blockVa);
        if (vertex == partitioner.cfg().vertices().end()) {
            mlog[WARN] <<function->printableName() <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" does not exist in the CFG; no AST node created\n";
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (SgAsmBlock *child = buildBasicBlockAst(partitioner, bb, function, settings))
                children.push_back(child);
            BOOST_FOREACH (const DataBlock::Ptr &dblock, bb->dataBlocks())
                insertUnique(dblocks, dblock, sortDataBlocks);
        } else {
            mlog[WARN] <<function->printableName() <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" is undiscovered in the CFG; no AST node created\n";
        }
    }
    if (children.empty()) {
        if (settings.allowFunctionWithNoBasicBlocks) {
            mlog[WARN] <<"creating an empty AST node for " <<function->printableName() <<"\n";
        } else {
            return NULL;
        }
    }

    // Build the child data block IR nodes.  The data blocks attached to the SgAsmFunction node are the union of the data
    // blocks owned by the function and the data blocks owned by each of its basic blocks.
    BOOST_FOREACH (const DataBlock::Ptr &dblock, dblocks) {
        if (SgAsmBlock *child = buildDataBlockAst(partitioner, dblock, settings))
            children.push_back(child);
    }

    // Sort the children in the order usually used by ROSE
    std::sort(children.begin(), children.end(), sortBlocksForAst);

    unsigned reasons = function->reasons();

    // Is the function part of the CFG?
    ControlFlowGraph::ConstVertexIterator entryVertex = partitioner.findPlaceholder(function->address());
    if (entryVertex != partitioner.cfg().vertices().end())
        reasons |= SgAsmFunction::FUNC_GRAPH;
    
    // Is the function discontiguous?
    if (partitioner.aum(function).extent().nIntervals() > 1)
        reasons |= SgAsmFunction::FUNC_DISCONT;

    // Is the function the target of a function call?
    if (entryVertex != partitioner.cfg().vertices().end()) {
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, entryVertex->inEdges()) {
            if (edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER) {
                reasons |= SgAsmFunction::FUNC_CALL_TARGET;
                break;
            }
        }
    }

    // Can this function return to its caller?
    SgAsmFunction::MayReturn mayReturn = SgAsmFunction::RET_UNKNOWN;
    if (entryVertex != partitioner.cfg().vertices().end()) {
        if (BasicBlock::Ptr bb = entryVertex->value().bblock()) {
            bool b = false;
            if (bb->mayReturn().getOptional().assignTo(b))
                mayReturn = b ? SgAsmFunction::RET_SOMETIMES : SgAsmFunction::RET_NEVER;
        }
    }

    // Function's calling convention. The AST holds the name of the best calling convention, or the empty string if no calling
    // convention has been assigned.
    std::string bestCallingConvention;
    if (CallingConvention::Definition::Ptr ccdef = function->callingConventionDefinition())
        bestCallingConvention = ccdef->name();
    
    // Build the AST
    SgAsmFunction *ast = SageBuilderAsm::buildFunction(function->address(), children);
    ast->set_reason(reasons);
    ast->set_name(function->name());
    ast->set_comment(function->comment());
    ast->set_may_return(mayReturn);
    ast->set_stackDelta(function->stackDeltaConcrete());
    ast->set_callingConvention(bestCallingConvention);
    return ast;
}

SgAsmBlock*
buildGlobalBlockAst(const Partitioner &partitioner, const AstConstructionSettings &settings) {
    // Create the children first
    std::vector<SgAsmFunction*> children;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (SgAsmFunction *func = buildFunctionAst(partitioner, function, settings)) {
            children.push_back(func);
        }
    }
    if (children.empty()) {
        if (settings.allowEmptyGlobalBlock) {
            mlog[WARN] <<"building an empty AST binary global block\n";
        } else {
            return NULL;
        }
    }

    // Build the global block
    SgAsmBlock *global = new SgAsmBlock;
    BOOST_FOREACH (SgAsmFunction *function, children) {
        global->get_statementList().push_back(function);
        function->set_parent(global);
    }
    return global;
}

SgAsmBlock*
buildAst(const Partitioner &partitioner, SgAsmInterpretation *interp/*=NULL*/, const AstConstructionSettings &settings) {
    if (SgAsmBlock *global = buildGlobalBlockAst(partitioner, settings)) {
        fixupAstPointers(global, interp);
        fixupAstCallingConventions(partitioner, global);
        if (interp) {
            if (SgAsmBlock *oldGlobalBlock = interp->get_global_block())
                oldGlobalBlock->set_parent(NULL);
            interp->set_global_block(global);
            global->set_parent(interp);
        }
        return global;
    }
    return NULL;
}

void
fixupAstPointers(SgNode *ast, SgAsmInterpretation *interp/*=NULL*/) {
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmNode*> Index;

    // Build various indexes since ASTs have inherently linear search time.  We store just the starting address for all these
    // things because that's all we ever want to point to.
    struct Indexer: AstSimpleProcessing {
        Index insnIndex, bblockIndex, funcIndex;
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                insnIndex.insert(insn->get_address(), insn);
            } else if (SgAsmBlock *block = isSgAsmBlock(node)) {
                if (!block->get_statementList().empty() && isSgAsmInstruction(block->get_statementList().front()))
                    bblockIndex.insert(block->get_address(), block);
            } else if (SgAsmFunction *func = isSgAsmFunction(node)) {
                funcIndex.insert(func->get_entry_va(), func);
            }
        }
    } indexer;
    indexer.traverse(ast, preorder);

    // Build a list of memory-mapped sections in the interpretation.  We'll use this list to select the best-matching section
    // for a particular address via SgAsmGenericFile::best_section_by_va()
    SgAsmGenericSectionPtrList mappedSections;
    Index sectionIndex;
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *header, interp->get_headers()->get_headers()) {
            if (header->is_mapped())
                mappedSections.push_back(header);
            BOOST_FOREACH (SgAsmGenericSection *section, header->get_mapped_sections()) {
                if (!section->get_mapped_xperm())
                    mappedSections.push_back(section);
            }
        }
    }

    struct FixerUpper: AstSimpleProcessing {
        const Index &insnIndex, &bblockIndex, &funcIndex;
        const SgAsmGenericSectionPtrList &mappedSections;
        FixerUpper(const Index &insnIndex, const Index &bblockIndex, const Index &funcIndex,
                   const SgAsmGenericSectionPtrList &mappedSections)
            : insnIndex(insnIndex), bblockIndex(bblockIndex), funcIndex(funcIndex), mappedSections(mappedSections) {}
        void visit(SgNode *node) {
            std::vector<SgAsmIntegerValueExpression*> ivals;
            if (SgAsmBlock *blk = isSgAsmBlock(node)) {
                // SgAsmBlock::p_successors is not traversed due to limitations of ROSETTA, so traverse explicitly.
                ivals = blk->get_successors();
            } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                ivals.push_back(ival);
            }

            BOOST_FOREACH (SgAsmIntegerValueExpression *ival, ivals) {
                if (ival->get_baseNode()==NULL) {
                    rose_addr_t va = ival->get_absoluteValue();
                    SgAsmNode *base = NULL;
                    if (funcIndex.getOptional(va).assignTo(base) || bblockIndex.getOptional(va).assignTo(base) ||
                        insnIndex.getOptional(va).assignTo(base)) {
                        ival->makeRelativeTo(base);
                    } else if (SgAsmGenericSection *section = SgAsmGenericFile::best_section_by_va(mappedSections, va)) {
                        ival->makeRelativeTo(section);
                    }
                }
            }
        }
    } fixerUpper(indexer.insnIndex, indexer.bblockIndex, indexer.funcIndex, mappedSections);
    fixerUpper.traverse(ast, preorder);
}

void
fixupAstCallingConventions(const Partitioner &partitioner, SgNode *ast) {
    // Pass 1: Sort all calling conventions by number of occurrences within the partitioner.
    Sawyer::Container::Map<std::string, size_t> totals; // how many times does each calling convention name match?
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        const CallingConvention::Analysis &ccAnalysis = function->callingConventionAnalysis();
        if (!ccAnalysis.hasResults())
            continue;                                   // don't run analysis if not run already
        CallingConvention::Dictionary ccDefs = partitioner.functionCallingConventionDefinitions(function);
        BOOST_FOREACH (const CallingConvention::Definition::Ptr &ccDef, ccDefs)
            ++totals.insertMaybe(ccDef->name(), 0);
    }

    // Pass 2: For each function in the AST, select the matching definition that's most frequent overall. If there's a tie, use
    // the first one from the function's definition list since this list is presumably sorted by how frequently the convention
    // is used by the whole world.
    BOOST_FOREACH (SgAsmFunction *astFunction, SageInterface::querySubTree<SgAsmFunction>(ast)) {
        if (Function::Ptr function = partitioner.functionExists(astFunction->get_address())) {
            const CallingConvention::Analysis &ccAnalysis = function->callingConventionAnalysis();
            if (!ccAnalysis.hasResults())
                continue;                               // don't run analysis if not run already
            CallingConvention::Dictionary ccDefs = partitioner.functionCallingConventionDefinitions(function);
            CallingConvention::Definition::Ptr ccBest;
            BOOST_FOREACH (const CallingConvention::Definition::Ptr &ccDef, ccDefs) {
                if (NULL==ccBest) {
                    ccBest = ccDef;
                } else if (totals.getOrElse(ccDef->name(), 0) > totals.getOrElse(ccBest->name(), 0)) {
                    ccBest = ccDef;
                }
            }
            if (ccBest) {
                // We cannot delete previously stored calling conventions because there's no clear rule about whether they need
                // to be allocated on the heap, and if so, who owns them or what allocator was used.
                astFunction->set_callingConvention(ccBest->name());
            }
        }
    }
}

std::vector<Function::Ptr>
findNoopFunctions(const Partitioner &partitioner) {
    partitioner.allFunctionIsNoop();
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (function->isNoop().getOptional().orElse(false))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
    return retval;
}

void
nameNoopFunctions(const Partitioner &partitioner) {
    BOOST_FOREACH (const Function::Ptr &function, findNoopFunctions(partitioner)) {
        if (function->name().empty()) {
            std::string newName = "noop_" + StringUtility::addrToString(function->address()).substr(2) + "() -> void";
            function->name(newName);
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
