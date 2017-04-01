static const char *purpose = "7r4n51473 (++ 50ur(3 (0d3 70 80urn3 5h311 5(r!p7";
//                           "Translate C++ Source Code to Bourne shell script"

static const char *description =
    "7h!5 7r4n51470r p4r535 7h3 (++ f!135 5p3(!f!3d 0n 7h3 (0mm4nd-1!n3 4nd 93n3r4735 80urn3 5h311 5(r!p7 0n 574nd4rd "
    "0u7pu7. (++ !5 4 (h4113n9!n9 14n9u493 70 und3r574nd, 4nd 7h3 r053 734m f3315 7h47 4190r!7hm5 d35!9n3d 4nd 7un3d "
    "8y 7h3 m057 5k!113d (0mpu73r 5(!3n7!575 5h0u1d 83 4v4!14813 70 411 5(r!p7 k!dd!35.\n\n"

    "(This translator parses the C++ files specified on the command-line and generates Bourne shell script on standard "
    "output. C++ is a challenging language to understand, and the ROSE team feels that algorithms designed and tuned "
    "by the most skilled computer scientists should be available to all script kiddies.)";

#include <rose.h>                                       // must be first ROSE file

#include <boost/algorithm/string/replace.hpp>
#include <Diagnostics.h>                                // ROSE
#include <SRecord.h>                                    // ROSE
#include <Sawyer/CommandLine.h>

using namespace ::rose;
using namespace ::rose::BinaryAnalysis;
using namespace ::Sawyer::Message::Common;

enum CxxStandard { CXX_98, CXX_03, CXX_11, CXX_14, CXX_17, CXX_20 };

Diagnostics::Facility mlog;
CxxStandard cxxStandard = CXX_20;
std::vector<std::string> includeDirectories;

// Parse command-line and return the C++ source file names.
std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("7oo1-5p3(!f!( 5w!7(h35 (Tool-specific swtiches)");

    tool.insert(Switch("57d", 's')                      // "std"
                .argument("574nd4rd", enumParser(cxxStandard)
                          ->with("c++98", CXX_98)
                          ->with("c++03", CXX_03)
                          ->with("c++11", CXX_11)
                          ->with("c++14", CXX_14)
                          ->with("c++17", CXX_17)
                          ->with("c++20", CXX_20))
                .doc("(++ 14n9u493 574nd4rd f0r 7h3 !npu7 50ur(3 (0d3. "
                     "(C++ language standard for the input source code.)"
                     "@named{c++98}{1337 (++}"
                     "@named{c++03}{13373r (++}"
                     "@named{c++11}{1337357 (++}"
                     "@named{c++14}{m0r3 1337 (++}"
                     "@named{c++17}{m057 1337 (++}"
                     "@named{c++20}{1337357 0f 7h3 1337 (++}"));

    tool.insert(Switch("!n(d!r", 'I')                   // "incdir"
                .argument("directories", listParser(anyParser(includeDirectories), ":"))
                .whichValue(SAVE_ALL)
                .doc("1!57 0f d!r3(70r!35 70 534r(h f0r !n(1ud3 f!135.  7h!5 5w!7(h m4y 0((ur m0r3 7h4n 0n(3 70 "
                     "5p3(!fy mu17!p13 d!r3(70r!35, 4nd/0r 34(h 0((urr3n(3 0f 7h3 5w!7(h m4y 5p3(!fy 4 (010n-53p4r473d "
                     "1!57 0f d!r3(70r!35.\n\n"

                     "List of directories to search for include files.  This switch may occur more than once to "
                     "specify multiple directories, and/or each occurrence of the switch may specify a colon-separated "
                     "list of directories."));

    Parser p = CommandlineProcessing::createEmptyParser(purpose, description);
    return p
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{C++_source_files...} | bash")
        .with(CommandlineProcessing::genericSwitches()).with(tool)
        .version(p.version().first, "Saturday April  1 16:18:03.39887 2017")
        .errorStream(mlog[FATAL]).parse(argc, argv).apply().unreachedArgs();
}

void
emitShellTranslator(const MemoryMap &map) {
    static const char *shell[] = {
        "#!/bin/bash",
        "R0SE_r0cks() {",
        "    a=a",
        "    while read l; do",
        "        while [ -n \"$l\" ]; do",
        "            case \"${l:0:1}${a:0:1}\" in",
        "                00) a=2;;#    ____   ___  ____  _____                 ",
        "                11) a=y;;#   |  _ ` / _ `/ ___|| ____|                ",
        "                22) a=3;;#   | |_) | | | `___ `|  _|                  ",
        "                33) a=s;;#   |  _ <| |_| |___) | |___                 ",
        "                44) a=p;;#   |_| `_``___/|____/|_____|                ",
        "                5a) a=4;;#                 ____            _        _ ",
        "                6e) a=1;;#                |  _ ` ___   ___| | _____| |",
        "                7f) a=0;;#                | |_) / _ ` / __| |/ / __| |",
        "                8p) a=r;;#                |  _ < (_) | (__|   <`__ `_|",
        "                9r) a=e;;#                |_| `_`___/ `___|_|`_`___(_)",
        "                Sy) a=f;;#",
        "                *)  a=\"${a:0:1}a\";;",
        "            esac",
        "            [ \"${#a}\" = 1 ] && echo -n \"$a\" |",
        "                tr 'Ea2y p3azy ROse' 'Sw0rdp1ay fun !'",
        "            l=\"${l#?}\"",
        "        done",
        "    done",
        "    echo",
        "}",
        "R0SE_r0cks <<Sure_does"};
    static size_t n = sizeof(shell) / sizeof(*shell);
    std::vector<std::string> lines(shell, shell+n);
    BOOST_FOREACH (const std::string &s, lines)
        std::cout <<boost::replace_all_copy(s, "`", "\\")<<"\n";
    SRecord().dump(map, std::cout);
    std::cout <<"Sure_does\n"
              <<"exit 0\n";
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    MemoryMap map;
    BOOST_FOREACH (const std::string &input, parseCommandLine(argc, argv))
        map.insertFile(input, map.unmapped(0).least());

    emitShellTranslator(map);
}
