#include <sawyer/CommandLine.h>
#include <iostream>

struct Disassembler {};
struct DisassemblerArm: Disassembler {};
struct DisassemblerPowerpc: Disassembler {};
struct DisassemblerMips: Disassembler {};
struct DisassemblerI386: Disassembler {};
struct DisassemblerAmd64: Disassembler {};
struct DisassemblerM68k: Disassembler {};
struct DisassemblerColdFire: Disassembler {};

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;
    unsigned long startVa;
    unsigned long alignment;
    Settings(): startVa(0), alignment(1) {}
};

static Disassembler *
getDisassembler(const std::string &name)
{
    if (0==name.compare("list")) {
        std::cout <<"The following ISAs are supported:\n"
                  <<"  amd64\n"
                  <<"  arm\n"
                  <<"  coldfire\n"
                  <<"  i386\n"
                  <<"  m68040\n"
                  <<"  mips\n"
                  <<"  ppc\n";
        exit(0);
    } else if (0==name.compare("arm")) {
        return new DisassemblerArm();
    } else if (0==name.compare("ppc")) {
        return new DisassemblerPowerpc();
    } else if (0==name.compare("mips")) {
        return new DisassemblerMips();
    } else if (0==name.compare("i386")) {
        return new DisassemblerI386();
    } else if (0==name.compare("amd64")) {
        return new DisassemblerAmd64();
    } else if (0==name.compare("m68040")) {
        return new DisassemblerM68k();
    } else if (0==name.compare("coldfire")) {
        return new DisassemblerColdFire();
    } else {
        throw std::runtime_error("invalid ISA name \""+name+"\"; use --isa=list");
    }
}

//! [parseCommandLine decl]
Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    //! [parseCommandLine decl]

    //! [parseCommandLine standard]
    SwitchGroup standard;
    standard.doc("The following switches are recognized by all tools in this package.");
    //! [parseCommandLine standard]

    //! [parseCommandLine helpversion]
    standard.insert(Switch("help", 'h')
                    .shortName('?')
                    .action(showHelpAndExit(0))
                    .doc("Show this documentation."));

    standard.insert(Switch("version", 'V')
                    .action(showVersionAndExit("1.2.3", 0))
                    .doc("Show version number."));
    //! [parseCommandLine helpversion]
    

    //! [parseCommandLine isa]
    SwitchGroup tool;
    tool.doc("The following switches are specific to this command.");

    tool.insert(Switch("isa")
                .argument("architecture", anyParser(settings.isaName))
                .doc("Instruction set architecture. Specify \"list\" to see "
                     "a list of possible ISAs."));
    //! [parseCommandLine isa]

    //! [parseCommandLine at]
    tool.insert(Switch("at")
                .argument("virtual-address", nonNegativeIntegerParser(settings.startVa))
                .doc("The first byte of the file is mapped at the specified "
                     "@v{virtual-address}, which defaults to " +
                     boost::lexical_cast<std::string>(settings.startVa) + "."));

    tool.insert(Switch("alignment")
                .argument("align", nonNegativeIntegerParser(settings.alignment))
                .doc("Alignment for instructions.  The default is 1 (no alignment).  Values "
                     "larger than one will cause each candidate address to be rounded up to "
                     "the next multiple of @v{align}.  If this rounding up causes addresses "
                     "after a valid instruction to be skipped then a warning is printed. "
                     "No warning is printed if the alignment skips addresses after a "
                     "disassembly failure."));
    //! [parseCommandLine at]

    //! [parseCommandLine parser]
    Parser parser;
    parser
        .purpose("disassembles files one address at a time")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program is a very simple disassembler that tries to disassemble an "
             "instruction at each address of the specimen file.");
    //! [parseCommandLine parser]

    //! [parseCommandLine parse]
    return parser.with(standard).with(tool).parse(argc, argv).apply();
}
//! [parseCommandLine parse]

int main(int argc, char *argv[])
{
    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();

    // Obtain a disassembler (do this before opening the specimen so "--isa=list" has a chance to run)
    Disassembler *disassembler = getDisassembler(settings.isaName);
    ASSERT_not_null(disassembler);

    // Open the file that needs to be disassembled
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];

    return 0;
}
