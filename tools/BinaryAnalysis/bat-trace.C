#if __cplusplus >= 201103L

static const char *purpose = "trace program execution";
static const char *description =
    "This tool traces the native execution of a program by single-stepping the process under a debugger. The addresses of the "
    "executed instructions are optionally printed or saved in a database. A subsequent run of the same program can compare "
    "the execution with a previously saved trace and report differences.";

#include <rose.h>
#include <BinaryDebugger.h>                             // rose
#include <CommandLine.h>                                // rose
#include <rose_getline.h>                               // rose
#include <rose_strtoull.h>                              // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Partitioner2/Partitioner.h>                   // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <Sawyer/Message.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/Trace.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    bool showingAddresses;                              // show execution addresses
    bool showingInsns;                                  // show instructions instead of just addresses
    bool onlyDistinct;                                  // show only distinct output lines
    bool showingSummary;                                // show the summary
    boost::filesystem::path saveTrace;                  // should we save, and if so, where?
    boost::filesystem::path compareFile;                // compare current trace with this file

    Settings()
        : showingAddresses(false), onlyDistinct(false), showingSummary(true) {}
};

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    //---------- General switches ----------
    SwitchGroup gen = Rose::CommandLine::genericSwitches();

    //---------- Operational switches ----------
    SwitchGroup op("Operational switches");
    op.name("op");

    op.insert(Switch("compare")
              .argument("filename", anyParser(settings.compareFile))
              .doc("Loads a trace from the specified file and compares it to the current program trace being produced. Once "
                   "a divergence is detected, the current program is aborted."));

    //----------  Output switches ----------
    SwitchGroup out("Output switches");
    out.name("out");
    Rose::CommandLine::insertBooleanSwitch(out, "addresses", settings.showingAddresses,
                                           "Show the execution addresses (starting address of each executed instruction) along "
                                           "the trace.  This information is printed after the process finishes executing so as "
                                           "to not interfere with the process's own output. If @s{instructions} is turned on, "
                                           "then addresses are shown regardless of this switch's value.");
    Rose::CommandLine::insertBooleanSwitch(out, "instructions", settings.showingInsns,
                                           "Show disassembled instructions in the trace rather than just the address.");
    Rose::CommandLine::insertBooleanSwitch(out, "unique", settings.onlyDistinct,
                                           "Instead of showing one line of output for each step in the trace, show one line "
                                           "per distinct address within the trace. The lines are output in address order "
                                           "rather then in the order they occur in the trace.");
    Rose::CommandLine::insertBooleanSwitch(out, "summary", settings.showingSummary,
                                           "Show some final statistics about the path.");
    out.insert(Switch("output", 'o')
               .argument("filename", anyParser(settings.saveTrace))
               .doc("Save the trace to the specified file instead of standard output, creating or truncating the file as "
                    "necessary. Each line consistes of a single instruction, either an address or a disassemble instruction "
                    "that starts with an address."));

    //----------  Parsing ----------
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{command} [@v{args...}]");
    parser.with(gen).with(op).with(out);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return args;
}

Sawyer::Container::Trace<rose_addr_t>
loadTrace(const boost::filesystem::path &fileName) {
    Sawyer::Container::Trace<rose_addr_t> retval;
    if (!fileName.empty()) {
        std::ifstream input(fileName.native().c_str());
        if (!input) {
            mlog[FATAL] <<"cannot open file for reading: " <<fileName <<"\n";
            exit(1);
        }

        while (input) {
            std::string line = rose_getline(input);
            const char *s = line.c_str();
            while (s && isspace(*s)) ++s;
            char *rest = NULL;
            rose_addr_t va = rose_strtoull(s, &rest, 0);
            if (rest != s)
                retval.append(va);
        }
    }
    return retval;
}

struct TraceFilter {
    bool isComparing;                                          // whether we're comparing current trace to previous answer
    Sawyer::Container::Trace<rose_addr_t> answer;              // compare new trace to this previous answer
    Sawyer::Container::Trace<rose_addr_t>::ConstIterator iter; // iterates over the answer
    Sawyer::ProgressBar<size_t> nSteps;
    bool hadError;

    explicit TraceFilter(const boost::filesystem::path &fileName)
        : isComparing(!fileName.empty()), answer(loadTrace(fileName)), iter(answer.begin()), nSteps(mlog[MARCH], "tracing"),
          hadError(false) {
        nSteps.suffix(" insns executed");
    }

    Debugger::FilterAction operator()(rose_addr_t va) {
        SAWYER_MESG(mlog[DEBUG]) <<" executing at " <<StringUtility::addrToString(va) <<"\n";
        if (isComparing) {
            if (iter == answer.end()) {
                mlog[ERROR] <<"current trace extends beyond previous answer trace at step " <<nSteps.value() <<"\n";
                hadError = true;
                return Debugger::STOP;
            } else if (va != *iter++) {
                mlog[ERROR] <<"current trace diverges from previous answer trace at step " <<nSteps.value() <<"\n";
                hadError = true;
                return Debugger::STOP;
            }
        }
        ++nSteps;
        return Debugger::FilterAction();
    }

    void finalCheck() {
        if (isComparing) {
            if (iter != answer.end()) {
                mlog[ERROR] <<"current trace has fewer steps than previous answer trace at step " <<nSteps.value() <<"\n";
                hadError = true;
            } else if (!hadError) {
                mlog[INFO] <<"current trace exactly matches previous answer trace\n";
            }
        }
    }
};

void
showAllInstructions(std::ostream &out, const Sawyer::Container::Trace<rose_addr_t> &trace, const P2::Partitioner &partitioner) {
    struct Visitor {
        std::ostream &out;
        Sawyer::Container::Map<rose_addr_t, std::string> seen;
        const P2::Partitioner &partitioner;

        Visitor(std::ostream &out, const P2::Partitioner &partitioner)
            : out(out), partitioner(partitioner) {}
        
        bool operator()(rose_addr_t va) {
            std::string s;
            if (!seen.getOptional(va).assignTo(s)) {
                if (SgAsmInstruction *insn = partitioner.instructionProvider()[va]) {
                    s = partitioner.unparse(insn);
                } else {
                    s = StringUtility::addrToString(va) + ": no instruction";
                }
                seen.insert(va, s);
            }
            out <<s <<"\n";
            return true;
        }
    } visitor(out, partitioner);
    trace.traverse(visitor);
}

void
showDistinctInstructions(std::ostream &out, const Sawyer::Container::Trace<rose_addr_t> &trace,
                         const P2::Partitioner &partitioner) {
    Sawyer::Container::Set<rose_addr_t> vas = trace.labels();
    BOOST_FOREACH (rose_addr_t va, vas.values()) {
        if (SgAsmInstruction *insn = partitioner.instructionProvider()[va]) {
            out <<partitioner.unparse(insn) <<"\n";
        } else {
            out <<StringUtility::addrToString(va) + ": no instruction\n";
        }
    }
}

void
showDistinctAddresses(std::ostream &out, const Sawyer::Container::Trace<rose_addr_t> &trace) {
    Sawyer::Container::Set<rose_addr_t> vas = trace.labels();
    BOOST_FOREACH (rose_addr_t va, vas.values())
        out <<StringUtility::addrToString(va) <<"\n";
}

void
showAllAddresses(std::ostream &out, const Sawyer::Container::Trace<rose_addr_t> &trace) {
    struct Visitor {
        std::ostream &out;

        Visitor(std::ostream &out)
            : out(out) {}

        bool operator()(rose_addr_t va) {
            out <<StringUtility::addrToString(va) <<"\n";
            return true;
        }
    } visitor(out);
    trace.traverse(visitor);
}

void
showSummary(const Sawyer::Container::Trace<rose_addr_t> &trace) {
    std::cout <<"trace contains " <<StringUtility::plural(trace.size(), "steps")
              <<" at " <<StringUtility::plural(trace.nLabels(), "distinct addresses") <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("tracing program execution");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);

    // Where to send the trace results. We create this file up front so that any failure is reported early and so that the file
    // is truncated if the trace itself fails later.
    std::ostream *output = NULL;
    if (!settings.saveTrace.empty()) {
        static std::ofstream file(settings.saveTrace.native().c_str());
        if (!file) {
            mlog[FATAL] <<"cannot save trace in " <<settings.saveTrace <<"\n";
            exit(1);
        }
        output = &file;
    } else {
        output = &std::cout;
    }

    Debugger::Specimen specimen(args);
    specimen.randomizedAddresses(false);
    auto process = Debugger::instance(specimen);

    P2::Partitioner partitioner;
    if (settings.showingInsns) {
        std::string specimen = "proc:noattach:" + boost::lexical_cast<std::string>(process->isAttached());
        P2::Engine engine;
        engine.settings().disassembler.isaName = "i386";// FIXME[Robb Matzke 2019-12-12]
        partitioner = engine.partition(specimen);
    }
    
    TraceFilter filter(settings.compareFile);
    Sawyer::Stopwatch timer;
    mlog[INFO] <<"tracing process...\n";
    auto trace = process->trace(filter);
    mlog[INFO] <<"tracing process; took " <<timer <<" seconds\n";
    mlog[INFO] <<"process " <<process->howTerminated() <<"\n";
    filter.finalCheck();

    if (settings.showingInsns) {
        if (settings.onlyDistinct) {
            showDistinctInstructions(*output, trace, partitioner);
        } else {
            showAllInstructions(*output, trace, partitioner);
        }
    } else if (settings.showingAddresses) {
        if (settings.onlyDistinct) {
            showDistinctAddresses(*output, trace);
        } else {
            showAllAddresses(*output, trace);
        }
    }

    if (settings.showingSummary)
        showSummary(trace);

    return filter.hadError ? 1 : 0;
}

#else

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    std::cerr <<argv[0] <<": this tool is not configured (compiler is too old)\n";
    exit(1);
}

#endif
