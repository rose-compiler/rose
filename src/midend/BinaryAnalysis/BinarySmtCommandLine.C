#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinarySmtCommandLine.h>

#include <BinarySmtSolver.h>

namespace Rose {
namespace BinaryAnalysis {

bool
listSmtSolverNames(std::ostream &out) {
    BinaryAnalysis::SmtSolver::Availability solvers = BinaryAnalysis::SmtSolver::availability();
    bool foundSolver = false;
    out <<"solver \"none\" is available\n";
    BOOST_FOREACH (BinaryAnalysis::SmtSolver::Availability::value_type &node, solvers) {
        out <<"solver \"" <<node.first <<"\" is " <<(node.second?"":"not ") <<"available\n";
        if (node.second)
            foundSolver = true;
    }
    return foundSolver;
}

std::string
validateSmtSolverName(const std::string &name) {
    BinaryAnalysis::SmtSolver::Availability solvers = BinaryAnalysis::SmtSolver::availability();
    if (solvers.find(name) != solvers.end())
        return "";
    return "SMT solver \"" + StringUtility::cEscape(name) + "\" is not recognized";
}

std::string
bestSmtSolverName() {
    std::string name;
    if (const BinaryAnalysis::SmtSolverPtr &solver = BinaryAnalysis::SmtSolver::bestAvailable())
        name = solver->name();
    return name;
}

void
checkSmtCommandLineArg(const std::string &arg, const std::string &listSwitch, std::ostream &out) {
    if ("list" == arg) {
        listSmtSolverNames(std::cout);
        std::cout <<"solver \"best\" is an alias for \"" <<bestSmtSolverName() <<"\"\n";
        exit(0);
    } else if ("" == arg || "none" == arg || "best" == arg) {
        // no solver
    } else {
        std::string err = validateSmtSolverName(arg);
        if (!err.empty()) {
            out <<err <<"\n";
            if (!listSwitch.empty())
                out <<"use \"" <<listSwitch <<"\" to get a list of supported solvers.\n";
            exit(1);
        }
    }
}

std::string
smtSolverDocumentationString(const std::string &dfltValue) {
    using namespace StringUtility;

    std::string docstr = "Specifies which connection is used to interface to an SMT solver for analyses that don't "
                         "otherwise specify a solver. The choices are names of solver interfaces, \"none\" "
                         "(or the empty string), \"best\", or \"list\".";

    SmtSolver::Availability solvers = SmtSolver::availability();
    std::vector<std::string> enabled, disabled;
    BOOST_FOREACH (const SmtSolver::Availability::value_type &node, solvers) {
        if (node.second) {
            enabled.push_back("\"" + cEscape(node.first) + "\"");
        } else {
            disabled.push_back("\"" + cEscape(node.first) + "\"");
        }
    }
    if (enabled.empty()) {
        docstr += " ROSE was not configured with any SMT solvers.";
    } else {
        docstr += " The following solvers are available in this configuration: " + joinEnglish(enabled) + ".";
    }
    if (!disabled.empty()) {
        docstr += " These solvers would be available, but were not configured: " + joinEnglish(disabled) + ".";
    }

    docstr += " In general, solvers ending with \"-exe\" translate the ROSE internal representation to text, send the "
              "text to a solver executable program which then parses it to another internal representation, solves, "
              "converts its internal representation to text, which ROSE then reads and parses. These \"-exe\" parsers "
              "are therefore quite slow, but work well for debugging. On the other hand, the \"-lib\" parsers use "
              "a solver library and can avoid two of the four translation steps, but don't produce much debugging "
              "output. To debug solvers, enable the " + SmtSolver::mlog.name() + " diagnostic facility (see @s{log}).";

    docstr += " The default is \"" + dfltValue + "\"";
    if ("best" == dfltValue) {
        if (SmtSolverPtr solver = SmtSolver::bestAvailable()) {
            docstr += ", which currently means \"" + solver->name() + "\".";
        } else {
            docstr += ", which currently mean \"none\".";
        }
    } else {
        docstr += ".";
    }

    return docstr;
}

void
SmtSolverValidator::operator()(const Sawyer::CommandLine::ParserResult &cmdline) {
    ASSERT_require(cmdline.have("smt-solver"));
    std::string arg = cmdline.parsed("smt-solver", 0).as<std::string>();
    if (cmdline.parser().errorStream().get()) {
        checkSmtCommandLineArg(arg, "--smt-solver=list", *cmdline.parser().errorStream().get());
    } else {
        checkSmtCommandLineArg(arg, "--smt-solver=list", std::cerr);
    }
}
    
} // namespace
} // namespace

#endif
