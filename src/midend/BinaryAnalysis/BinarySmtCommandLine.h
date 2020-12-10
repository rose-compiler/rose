// Command-line processing utilities related to SMT solvers
#ifndef Rose_BinaryAnalysis_SmtCommandLine
#define Rose_BinaryAnalysis_SmtCommandLine
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <iostream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

/** List known SMT solvers and their availability.
 *
 *  The names are listed alphabeticallly to the specified output stream. Returns true if any solvers are available, false if no
 *  solvers are available. */
bool listSmtSolverNames(std::ostream&);

/** Validate SMT solver name.
 *
 *  Returns an empty string if the name is valid, an error message otherwise. */
std::string validateSmtSolverName(const std::string &name);

/** SMT solver corresponding to "best".
 *
 *  Returns the name of the SMT solver that corresponds to the "best" solver. Returns an empty string if no solvers are
 *  available. */
std::string bestSmtSolverName();

/** Process SMT solver name from command-line.
 *
 *  Checks that the specified SMT solver name is valid, empty, "best", or "none". Returns if the check is successful,
 *  or prints an error message and exits if the check is unsuccessful.  If an error occurs, and @p listSwitch is non-empty then
 *  a second error message is shown that says "use xxxx to get a list of supported solvers". */
void checkSmtCommandLineArg(const std::string &arg, const std::string &listSwitch, std::ostream &errorStream = std::cerr);

/** Documentation string for an SMT solver switch. */
std::string smtSolverDocumentationString(const std::string &dfltSolver);

/** Validates SMT solver name from command-line.
 *
 *  This is a Sawyer command-line switch action that validates the "--smt-solver=NAME" command-line switch value and also
 *  handles the case when NAME is "list". When NAME is "list" a list of all known SMT solvers is shows on standard output
 *  along with an indication of whether that solver is available, then <code>exit(0)</code> is called. */
class SmtSolverValidator: public Sawyer::CommandLine::SwitchAction {
protected:
    SmtSolverValidator() {}
public:
    typedef Sawyer::SharedPointer<SmtSolverValidator> Ptr;
    static Ptr instance() { return Ptr(new SmtSolverValidator); }
protected:
    void operator()(const Sawyer::CommandLine::ParserResult&);
};

} // namespace
} // namespace

#endif
#endif
