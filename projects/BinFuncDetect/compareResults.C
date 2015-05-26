#include <rose.h>

#include <AsmUnparser.h>
#include <cstring>
#include <Diagnostics.h>
#include <FileSystem.h>
#include <iostream>
#include <Partitioner2/Engine.h>
#include <rose_getline.h>
#include <rose_strtoull.h>
#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/IntervalMap.h>
#include <sawyer/Message.h>
#include <set>
#include <string>
#include <vector>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

// Maps addresses to functions
typedef Sawyer::Container::IntervalMap<AddressInterval, rose_addr_t> FunctionByAddress;

struct Settings {
    std::string specimenName;
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser
        .purpose("compares disassembler results")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .with(CommandlineProcessing::genericSwitches())
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{csv1} @v{csv2}")
        .doc("description",
             "Reads two ASCII files of comma-separated values that have one line per record.  Each record contains the "
             "following fields:"
             "@bullet{A virtual address for a contiguous region of memory.}"
             "@bullet{Size in bytes of the contiguous region.}"
             "@bullet{The letter 'c' or 'd' to indicate whether the region is code or data.}"
             "@bullet{The address of the function that owns this region of memory.}"
             "The two files are then compared and certain statistics presented.")
        .doc("bugs",
             "This comparison does not handle situations when multiple functions claim the same address.");

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("specimen")
                .argument("name", anyParser(settings.specimenName))
                .doc("Name of binary specimen to use when printing instructions."));

    return parser.with(tool).parse(argc, argv).apply();
}

// Reads a CSV file containing these fields:
//    0: address of contiguous region
//    1: size of contiguous region
//    2: user type 'C'=>code, 'D'=>data
//    3: function that owns contiguous region
void
readCsvFile(const std::string &fileName, FunctionByAddress &code /*out*/, FunctionByAddress &data /*out*/) {
    struct Resources {
        FILE *file;
        char *linebuf;
        Resources(): file(NULL), linebuf(NULL) {}
        ~Resources() {
            if (file)
                fclose(file);
            if (linebuf)
                free(linebuf);
        }
    } rsrc;
    size_t linesz=0, lineNumber=0;

    ASSERT_require(fileName.size()==strlen(fileName.c_str()));
    if (NULL==(rsrc.file=fopen(fileName.c_str(), "r")))
        throw std::runtime_error("cannot open file \""+fileName+"\"");

    while (rose_getline(&rsrc.linebuf, &linesz, rsrc.file) > 0) {
        ++lineNumber;

        std::vector<std::string> fields = StringUtility::split(',', rsrc.linebuf, 4, true);
        ASSERT_always_require(fields.size()>=4);
        rose_addr_t addr = rose_strtoull(fields[0].c_str(), NULL, 16);
        rose_addr_t size = rose_strtoull(fields[1].c_str(), NULL, 0);
        rose_addr_t func = rose_strtoull(fields[3].c_str(), NULL, 16);

        if (size > 0) {
            AddressInterval interval = AddressInterval::baseSize(addr, size);
            if (fields[2] == "c") {
                if (code.isOverlapping(interval)) {
                    mlog[WARN] <<fileName <<":" <<lineNumber <<": multiple functions claim code in " <<interval <<"\n";
                    code.erase(interval);
                }
                code.insert(interval, func);
            } else {
                ASSERT_require2(fields[2] == "d", "field 2 must be \"c\" (code) or \"d\" (data)");
                if (data.isOverlapping(interval)) {
                    mlog[ERROR] <<fileName <<":" <<lineNumber <<": multiple functions claim data in " <<interval <<"\n";
                    data.erase(interval);
                }
                data.insert(interval, func);
            }
        }
    }
}

static void
show(std::ostream &out, const std::pair<size_t, size_t> &widths,
     const std::string &label, const std::string &value1, const std::string &value2) {
    static const size_t labelWidth = 30;
    out <<std::setw(labelWidth) <<std::left <<label.substr(0, labelWidth) <<" "
        <<std::setw(widths.first) <<std::right <<value1 <<" "
        <<std::setw(widths.second) <<std::right <<value2 <<"\n";
}

template<class T>
static void
show(std::ostream &out, const std::pair<size_t, size_t> &widths,
     const std::string &label, T value1, T value2) {
    std::string s1 = boost::lexical_cast<std::string>(value1);
    std::string s2 = boost::lexical_cast<std::string>(value2);
    show(out, widths, label, s1, s2);
}

static void
showStats(const std::string &label1, const FunctionByAddress &code1, const FunctionByAddress &data1,
          const std::string &label2, const FunctionByAddress &code2, const FunctionByAddress &data2) {

    AddressIntervalSet code1set(code1), code2set(code2);

    std::ostream &out = std::cout;
    std::pair<size_t, size_t> widths(std::max(size_t(20), label1.size()), std::max(size_t(20), label2.size()));
    show(out, widths, "",                   label1,         label2);

    show(out, widths, "bytes disassembled", code1set.size(),   code2set.size());
    show(out, widths, "intervals disassembled", code1set.nIntervals(), code2set.nIntervals());

    AddressIntervalSet bytesInCommon = code1set & code2set;
    show(out, widths, "bytes in common", bytesInCommon.size(), bytesInCommon.size());
    show(out, widths, "bytes in only one", (code1set-code2set).size(), (code2set-code1set).size());

    AddressIntervalSet functions1, functions2;          // could use std::set, but we want set-theoretic operators
    BOOST_FOREACH (const FunctionByAddress::Value &va, code1.values())
        functions1.insert(va);
    BOOST_FOREACH (const FunctionByAddress::Value &va, code2.values())
        functions2.insert(va);
    show(out, widths, "number of functions", functions1.size(), functions2.size());

    AddressIntervalSet functionsInCommon = functions1 & functions2;
    show(out, widths, "functions in common", functionsInCommon.size(), functionsInCommon.size());
    show(out, widths, "functions in only one", (functions1-functions2).size(), (functions2-functions1).size());
}

static void
listInstructions(const InstructionProvider::Ptr &insns, const MemoryMap &map,
                 const FunctionByAddress &code1, FunctionByAddress &code2) {
    std::ostream &out = std::cout;
    static const size_t insnWidth = 110;
    rose_addr_t va1 = code1.hull().least();
    rose_addr_t va2 = code2.hull().least();
    rose_addr_t va = std::min(va1, va2);
    rose_addr_t expectedVa = va;
    AsmUnparser unparser;

    while (va<=code1.hull().greatest() || va<=code2.hull().greatest()) {
        // Address and contents
        if (va != expectedVa)
            out <<"\n";                                 // visual cue that addresses are not sequential here
        std::ostringstream ss;
        size_t size;
        if (!map.at(va).require(MemoryMap::EXECUTABLE).exists()) {
            ss <<StringUtility::addrToString(va) <<": " <<(map.at(va).exists() ? "not executable" : "not mapped");
            size = 1;
        } else if (SgAsmInstruction *insn = (*insns)[va]) {
            unparser.unparse(ss, insn);
            size = insn->get_size();
        } else {
            ss <<StringUtility::addrToString(va) <<": bad instruction";
            size = 1;
        }
        std::vector<std::string> lines = StringUtility::split('\n', ss.str());
        while (lines.size()>0 && lines[lines.size()-1]=="")
            lines.pop_back();
        for (size_t i=0; i<lines.size(); ++i) {
            if (i+1 < lines.size()) {
                out <<lines[i] <<"\n";
            } else {
                out <<std::setw(insnWidth) <<std::left <<lines[i];
            }
        }
        
        // Functions owning
        Sawyer::Optional<rose_addr_t> f1 = code1.getOptional(va);
        Sawyer::Optional<rose_addr_t> f2 = code2.getOptional(va);
        out <<"\t" <<std::setw(10) <<std::left <<(f1 ? StringUtility::addrToString(*f1) : std::string("none"));
        out <<"\t" <<std::setw(10) <<std::left <<(f2 ? StringUtility::addrToString(*f2) : std::string("none"));
        out <<" " <<(f1.isEqual(f2) ? "" : "<---") <<"\n";

        // Advance address pointer
        rose_addr_t next = va + size;
        expectedVa = next;
        FunctionByAddress::ConstIntervalIterator i1 = code1.upperBound(va);
        if (i1!=code1.nodes().end() && i1->least() < next)
            next = i1->least();
        FunctionByAddress::ConstIntervalIterator i2 = code2.upperBound(va);
        if (i2!=code2.nodes().end() && i2->least() < next)
            next = i2->least();
        if (!map.atOrAfter(next).next().assignTo(va))
            break;
    }
}

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command line
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (args.size()!=2)
        throw std::runtime_error("invalid usage; see --help");

    // Load the CSV files
    FunctionByAddress code1, data1, code2, data2;
    readCsvFile(args[0], code1 /*out*/, data1 /*out*/);
    readCsvFile(args[1], code2 /*out*/, data2 /*out*/);

    showStats(FileSystem::Path(args[0]).filename().string(), code1, data1,
              FileSystem::Path(args[1]).filename().string(), code2, data2);
    std::cout <<"\n";

    // Parse the specimen
    if (!settings.specimenName.empty()) {
        P2::Engine engine;
        MemoryMap map = engine.loadSpecimens(settings.specimenName);
        InstructionProvider::Ptr insns = InstructionProvider::instance(engine.obtainDisassembler(), map);
        map.dump(std::cout);
        listInstructions(insns, map, code1, code2);
    }
}

