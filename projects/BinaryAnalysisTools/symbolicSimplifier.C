#include <rose.h>
#include <rosePublicConfig.h>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>
#include <BinarySymbolicExprParser.h>

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

bool testSerialization = false;
bool testSmtSolver = false;


static void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("serialize")
                .intrinsicValue(true, testSerialization)
                .doc("If enabled, then test the serialization mechanism by serializing each expression to a string, then "
                     "deserializing the string into a new expression and comparing the two for structural equality. "
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
                     "The default is to " + std::string(testSerialization?"":"not ") + "perform this test."
#else
                     "This switch does nothing since ROSE was not configured with Boost serialization support."
#endif
                     ));
    tool.insert(Switch("no-serialize")
                .key("serialize")
                .intrinsicValue(false, testSerialization)
                .hidden(true));

    Rose::CommandLine::insertBooleanSwitch(tool, "check-satisfiability", testSmtSolver,
                                           "Runs the expression through an SMT solver. If the expression is one bit wide "
                                           "then it's used as-is, otherwise we compare it to a new variable of the same "
                                           "width.");

    parser
        .purpose("test symbolic simplification")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}]")
        .doc("Description",
             "Parses symbolic expressions from standard input and prints the resulting expression trees. These trees "
             "undergo basic simplifications in ROSE before they're printed.")
        .doc("Symbolic expression syntax", SymbolicExprParser().docString())
        .with(Rose::CommandLine::genericSwitches())
        .with(tool);

    if (!parser.parse(argc, argv).apply().unreachedArgs().empty())
        throw std::runtime_error("incorrect usage; see --help");
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    parseCommandLine(argc, argv);
    unsigned lineNumber = 0;

    SmtSolver::Ptr smtSolver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);

    while (1) {

        // Prompt for and read a line containing one expression
        ++lineNumber;
#ifdef ROSE_HAVE_LIBREADLINE
        char *line = readline("> ");
        if (!line)
            break;
        add_history(line);
#else
        char *line = NULL;
        size_t linesz = 0;
        printf("> ");
        ssize_t nread = rose_getline(&line, &linesz, stdin);
        if (nread <= 0)
            break;
        while (nread > 0 && isspace(line[nread-1]))
            line[--nread] = '\0';
#endif

        // Parse the expression
        try {
            SymbolicExprParser symbolicParser(smtSolver);
            SymbolicExpr::Ptr expr = symbolicParser.parse(line);
            std::cout <<"Parsed value = " <<*expr <<"\n\n";

            if (testSmtSolver && smtSolver) {
                SymbolicExpr::Ptr assertion;
                if (expr->nBits() == 1) {
                    assertion = expr;
                } else {
                    assertion = SymbolicExpr::makeEq(expr, SymbolicExpr::makeIntegerVariable(expr->nBits()));
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
                out.register_type<SymbolicExpr::Interior>();
                out.register_type<SymbolicExpr::Leaf>();
                out <<expr;

                std::cout <<"Deserializing\n";
                std::istringstream iss(oss.str());
                boost::archive::text_iarchive in(iss);
                SymbolicExpr::Ptr expr2;
                in.register_type<SymbolicExpr::Interior>();
                in.register_type<SymbolicExpr::Leaf>();
                in >>expr2;

                std::cout <<"Restored value = " <<*expr2 <<"\n\n";
                if (!expr->isEquivalentTo(expr2))
                    std::cerr <<"error: serialization failed structural equivalence test\n";
            }
#endif
        } catch (const SymbolicExprParser::SyntaxError &e) {
            std::cerr <<e <<"\n";
            if (e.lineNumber != 0) {
                std::cerr <<"    input: " <<line <<"\n"
                          <<"    here---" <<std::string(e.columnNumber, '-') <<"^\n\n";
            }
        } catch (const Rose::FailedAssertion &e) {
            std::cerr <<"\n"; // message has already been printed.
        }

        free(line);
    }
}
