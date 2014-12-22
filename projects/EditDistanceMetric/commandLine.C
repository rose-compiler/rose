#include <rose.h>
#include "commandLine.h"

Sawyer::CommandLine::SwitchGroup
toolCommandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Switches for this tool");

    // Edit costs
    tool.insert(Switch("insertion-cost")
                .argument("cost", realNumberParser(settings.insertionCost))
                .doc("Non-negative cost for performing an insertion edit. The default is " +
                     boost::lexical_cast<std::string>(settings.insertionCost) + "."));
    tool.insert(Switch("deletion-cost")
                .argument("cost", realNumberParser(settings.deletionCost))
                .doc("Non-negative cost for performing a deletion edit. The default is " +
                     boost::lexical_cast<std::string>(settings.deletionCost) + "."));
    tool.insert(Switch("substitution-cost")
                .argument("cost", realNumberParser(settings.substitutionCost))
                .doc("Non-negative cost for performing a substitution edit. The default is " +
                     boost::lexical_cast<std::string>(settings.substitutionCost) + "."));

    // Use old API?
    tool.insert(Switch("use-old")
                .intrinsicValue(true, settings.useOldImplementation)
                .doc("Calls the old, locally-implemented version of tree edit distance. Note that the old version "
                     "doesn't return any useful value, but rather emits lots of debugging information to standard output "
                     "and to a couple files in the current working directory. Also, the old implementation has hard-coded "
                     "values for edit costs and will ignore the values specified with the various \"cost\" switches. The "
                     "@s{no-use-old} switch suppresses calls to the old implementation. The default is to " +
                     std::string(settings.useOldImplementation?"call":"not call") + " the old implementation."));
    tool.insert(Switch("no-use-old")
                .key("use-old")
                .intrinsicValue(false, settings.useOldImplementation)
                .hidden(true));

    return tool;
}
