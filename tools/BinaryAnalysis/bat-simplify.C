static const char *purpose = "simplify symbolic expressions";
static const char *description =
    "Reads symbolic expressions from standard input, one per line, and processes them through ROSE's simplification "
    "layer, printing the result. Command-line switches cause other tests to also be performed.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/SymbolicExpressionParser.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>
#include <Sawyer/Optional.h>

#include <boost/algorithm/string/trim.hpp>

#ifdef ROSE_HAVE_LIBREADLINE
# include <readline/readline.h>
# include <readline/history.h>
#else
# include <rose_getline.h>
#endif

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
# include <boost/archive/text_oarchive.hpp>
# include <boost/archive/text_iarchive.hpp>
#endif

using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

static bool testSerialization = false;
static bool testSmtSolver = false;
static Sawyer::Message::Facility mlog;

static void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");

    Rose::CommandLine::insertBooleanSwitch(tool, "serialize", testSerialization,
                                           "If enabled, then test the ROSE serialization mechanism by serializing each expression "
                                           "and deserializing to get a second expression. The two expressions are compared to ensure "
                                           "they are identical."
#ifndef ROSE_HAVE_BOOST_SERIALIZATION_LIB
                                           " (Since ROSE was configured without serialization support, this switch is recognized but "
                                           "has no effect.)"
#endif
                                           );

    Rose::CommandLine::insertBooleanSwitch(tool, "check-satisfiability", testSmtSolver,
                                           "Runs the expression through an SMT solver. If the expression is one bit wide "
                                           "then it's used as-is, otherwise we compare it to a new variable of the same "
                                           "width.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}]");
    parser.with(tool);
    parser.with(Rose::CommandLine::genericSwitches());
    if (!parser.parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
}

// Read a line of input and trim white space, or return nothing.
static Sawyer::Optional<std::string>
readInput() {
#ifdef ROSE_HAVE_LIBREADLINE
    char *tmpLine = readline("> ");
    if (!tmpLine)
        return Sawyer::Nothing();
    add_history(tmpLine);
    std::string line = tmpLine;
    free(tmpLine);
    boost::trim(line);
    return line;
#else
    if (isatty(0))
        std::cout <<"> ";
    std::string line = rose_getline(std::cin);
    if (line.empty())
        return Sawyer::Nothing();
    boost::trim(line);
    return line;
#endif
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("simplifying symbolic expressions");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);

    parseCommandLine(argc, argv);
    unsigned lineNumber = 0;

    SmtSolver::Ptr smtSolver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);

    while (auto line = readInput()) {
        ++lineNumber;

        // Parse the expression
        try {
            SymbolicExpressionParser symbolicParser(smtSolver);
            SymbolicExpression::Ptr expr = symbolicParser.parse(*line);
            std::cout <<"Parsed value = " <<*expr <<"\n\n";

            if (testSmtSolver && smtSolver) {
                SymbolicExpression::Ptr assertion;
                if (expr->nBits() == 1) {
                    assertion = expr;
                } else {
                    assertion = SymbolicExpression::makeEq(expr, SymbolicExpression::makeIntegerVariable(expr->nBits()));
                }
                std::cout <<"Checking satisfiability of " <<*assertion <<"\n";
                switch (smtSolver->satisfiable(assertion)) {
                    case SmtSolver::SAT_NO:
                        std::cout <<"not satisfiable\n";
                        break;
                    case SmtSolver::SAT_YES:
                        std::cout <<"satisfiable\n";
                        break;
                    case SmtSolver::SAT_UNKNOWN:
                        std::cout <<"satisfiability is unknown\n";
                        break;
                }
            }

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
            if (testSerialization) {
                std::cout <<"Serializing\n";
                std::ostringstream oss;
                boost::archive::text_oarchive out(oss);
                out.register_type<SymbolicExpression::Interior>();
                out.register_type<SymbolicExpression::Leaf>();
                out <<expr;

                std::cout <<"Deserializing\n";
                std::istringstream iss(oss.str());
                boost::archive::text_iarchive in(iss);
                SymbolicExpression::Ptr expr2;
                in.register_type<SymbolicExpression::Interior>();
                in.register_type<SymbolicExpression::Leaf>();
                in >>expr2;

                std::cout <<"Restored value = " <<*expr2 <<"\n\n";
                if (!expr->isEquivalentTo(expr2))
                    std::cerr <<"error: serialization failed structural equivalence test\n";
            }
#endif
        } catch (const SymbolicExpressionParser::SyntaxError &e) {
            std::cerr <<e <<"\n";
            if (e.lineNumber != 0) {
                std::cerr <<"    input: " <<*line <<"\n"
                          <<"    here---" <<std::string(e.columnNumber, '-') <<"^\n\n";
            }
        }
    }
}
