#include "sage3basic.h"
#include "AsmUnparser_compat.h"

#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sawyer/CommandLine.h>
#include <stringify.h>

using namespace rose::Diagnostics;

namespace rose {
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
    return ("Lists all CFG-attached instructions overlapping the specified @s{select} address interval when a "
            "basic block or placeholder whose address is in the specified @s{bblock} address interval is attached to the CFG. "
            "The listing indicates address gaps and overlaps with \"+@v{n}\" and \"-@v{n}\" notation where @v{n} is the "
            "number of bytes skipped forward or backward." +
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
                         "vertices, and they will be shown in a different style.  The @s{no-neighbors} switch "
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
            "is in the specified @s{bblock} address interval is inserted into the CFG.  The graph will contain vertices whose "
            "starting address fall within the @s{select} interval, and optionally neighbors of those vertices." +
            Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
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
                         "string \"##\".  This emphasizes these two important values in the output.  The @s{no-accent} "
                         "switch causes them to output as \"00\" and \"ff\". The default is to " +
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
            "in the specified @s{bblock} address interval is attached to the CFG." +
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
        while (AddressInterval avail = args.partitioner->memoryMap().atOrAfter(va).singleSegment().available()) {
            if (avail.least() > settings_.what.greatest())
                break;
            const size_t nPrint = std::min(settings_.what.greatest()+1-avail.least(), avail.size());
            const MemoryMap::Node &node = *args.partitioner->memoryMap().find(avail.least());
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
            "when a basic block or placeholder whose address is in the specified @s{bblock} address interval is attached "
            "to the CFG." + Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches());
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
                    if (partitioner.memoryMap().at(va).exists())
                        partitioner.addressName(va, name);

                    // Sometimes weak symbol values are offsets w.r.t. their linked section.
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK) {
                        va = value + section->get_mapped_actual_va();
                        if (partitioner.memoryMap().at(va).exists())
                            partitioner.addressName(va, name);
                    }
                }
            }
        }
    } t1(partitioner, fileHeader);
    t1.traverse(fileHeader, preorder);
}

static int
validStringChar(int ch) {
    return isascii(ch) && (isgraph(ch) || isspace(ch));
}

void
nameStrings(const Partitioner &partitioner) {
    struct T1: AstSimpleProcessing {
        Sawyer::Container::Map<rose_addr_t, std::string> seen;
        const Partitioner &partitioner;
        T1(const Partitioner &partitioner): partitioner(partitioner) {}
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                if (ival->get_comment().empty()) {
                    rose_addr_t va = ival->get_absoluteValue();
                    std::string label;
                    if (!seen.getOptional(va).assignTo(label) && partitioner.instructionsOverlapping(va).empty()) {
                        // Read a NUL-terminated string from memory which is readable but not writable. We need to make sure it
                        // ends with the NUL character even though we're only going to display the first part of it, therefore
                        // we'll read up to a fairly large amount of data looking for the NUL.
                        std::string c_str = partitioner.memoryMap().readString(va, 8192, validStringChar, NULL,
                                                                               MemoryMap::READABLE, MemoryMap::WRITABLE);
                        if (!c_str.empty()) {
                            static const size_t displayLength = 25; // arbitrary
                            static const size_t maxLength = displayLength + 7; // strlen("+N more")
                            size_t truncated = 0;
                            if (c_str.size() > maxLength) {
                                truncated = c_str.size() - displayLength;
                                c_str = c_str.substr(0, displayLength);
                            }
                            label = "\"" + StringUtility::cEscape(c_str) + "\"";
                            if (truncated)
                                label += "+" + StringUtility::numberToString(truncated) + " more";
                            ival->set_comment(label);
                        }
                        seen.insert(va, label);         // even if it's empty
                    } else if (!label.empty()) {
                        ival->set_comment(label);
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

} // namespace
} // namespace
} // namespace
} // namespace
