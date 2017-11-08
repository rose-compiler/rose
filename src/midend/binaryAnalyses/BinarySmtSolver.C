#include "sage3basic.h"

#include "rose_getline.h"
#include "BinarySmtSolver.h"
#include "BinaryYicesSolver.h"

#include <boost/format.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <fcntl.h> /*for O_RDWR, etc.*/
#include <Sawyer/FileSystem.h>
#include <Sawyer/Stopwatch.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility SmtSolver::mlog;

// class method
void
SmtSolver::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::SmtSolver");
    }
}

std::ostream&
operator<<(std::ostream &o, const SmtSolver::Exception &e)
{
    return o <<"SMT solver: " <<e.mesg;
}

SmtSolver::Stats SmtSolver::classStats;
boost::mutex SmtSolver::classStatsMutex;

void
SmtSolver::init()
{}

// class method
SmtSolver*
SmtSolver::instance(const std::string &name) {
    if (name == "yices") {
        return new YicesSolver;
    } else {
        ASSERT_not_reachable("invalid SMT solver name: \"" + StringUtility::cEscape(name) + "\"");
    }
}

// class method
SmtSolver::Stats
SmtSolver::classStatistics() 
{
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    return classStats;
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Stats
SmtSolver::get_class_stats() {
    return classStatistics();
}

// class method
void
SmtSolver::resetClassStatistics()
{
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    classStats = Stats();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::reset_class_stats()
{
    resetClassStatistics();
}

SymbolicExpr::Ptr
SmtSolver::evidenceForAddress(uint64_t addr)
{
    return evidenceForName(StringUtility::addrToString(addr));
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_address(uint64_t addr) {
    return evidenceForAddress(addr);
}

SmtSolver::Satisfiable
SmtSolver::triviallySatisfiable(const std::vector<SymbolicExpr::Ptr> &exprs_)
{
    std::vector<SymbolicExpr::Ptr> exprs(exprs_.begin(), exprs_.end());
    for (size_t i=0; i<exprs.size(); ++i) {
        if (exprs[i]->isNumber()) {
            ASSERT_require(1==exprs[i]->nBits());
            if (0==exprs[i]->toInt())
                return SAT_NO;
            std::swap(exprs[i], exprs.back()); // order of exprs is not important
            exprs.resize(exprs.size()-1);
        }
    }
    return exprs.empty() ? SAT_YES : SAT_UNKNOWN;
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Satisfiable
SmtSolver::trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    return triviallySatisfiable(exprs);
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs)
{
    bool got_satunsat_line = false;

#ifdef _MSC_VER
    // tps (06/23/2010) : Does not work under Windows
    abort();
    Satisfiable retval;
    return retval;
#else

    clearEvidence();

    Satisfiable retval = triviallySatisfiable(exprs);
    if (retval!=SAT_UNKNOWN)
        return retval;

    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }
    outputText = "";

    /* Generate the input file for the solver. */
    Sawyer::FileSystem::TemporaryFile tmpfile;
    Definitions defns;
    generateFile(tmpfile.stream(), exprs, &defns);
    tmpfile.stream().close();
    struct stat sb;
    int status __attribute__((unused)) = stat(tmpfile.name().string().c_str(), &sb);
    ASSERT_require(status>=0);
    stats.input_size += sb.st_size;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        classStats.input_size += sb.st_size;
    }

    /* Show solver input */
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"SMT solver input in " <<tmpfile.name() <<"\n";
        size_t n=0;
        std::ifstream f(tmpfile.name().string().c_str());
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            mlog[DEBUG] <<(boost::format("%5zu") % ++n).str() <<": " <<line <<"\n";
        }
    }

    /* Run the solver and read its output. The first line should be the word "sat" or "unsat" */
    {
        Sawyer::Stopwatch stopwatch;
        std::string cmd = getCommand(tmpfile.name().string());
        FILE *output = popen(cmd.c_str(), "r");
        ASSERT_not_null(output);
        char *line = NULL;
        size_t line_alloc = 0;
        ssize_t nread;
        while ((nread=rose_getline(&line, &line_alloc, output))>0) {
            stats.output_size += nread;
            {
                boost::lock_guard<boost::mutex> lock(classStatsMutex);
                classStats.output_size += nread;
            }
            if (!got_satunsat_line) {
                if (0==strncmp(line, "sat", 3) && isspace(line[3])) {
                    retval = SAT_YES;
                    got_satunsat_line = true;
                } else if (0==strncmp(line, "unsat", 5) && isspace(line[5])) {
                    retval = SAT_NO;
                    got_satunsat_line = true;
                } else {
                    std::cerr <<"SMT solver failed to say \"sat\" or \"unsat\"\n";
                    abort();
                }
            } else {
                outputText += std::string(line);
            }
        }
        if (line) free(line);
        int status = pclose(output);
        stopwatch.stop();
        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"Running SMT solver=\"" <<cmd <<"; exit status="<<status <<"\n";
            mlog[DEBUG] <<"SMT Solver ran for " <<stopwatch <<" seconds\n";
            mlog[DEBUG] <<"SMT Solver reported: " <<(SAT_YES==retval ? "sat" : SAT_NO==retval ? "unsat" : "unknown") <<"\n";
            mlog[DEBUG] <<"SMT Solver output:\n" <<StringUtility::prefixLines(outputText, "     ");
        }
    }

    if (SAT_YES==retval)
        parseEvidence();
#endif
    return retval;
}
    

SmtSolver::Satisfiable
SmtSolver::satisfiable(const SymbolicExpr::Ptr &tn)
{
    std::vector<SymbolicExpr::Ptr> exprs;
    exprs.push_back(tn);
    return satisfiable(exprs);
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(std::vector<SymbolicExpr::Ptr> exprs, const SymbolicExpr::Ptr &expr)
{
    if (expr!=NULL)
        exprs.push_back(expr);
    return satisfiable(exprs);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_variable(uint64_t varno) {
    return evidenceForVariable(varno);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_variable(const SymbolicExpr::Ptr &var) {
    return evidenceForVariable(var);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_name(const std::string &s) {
    return evidenceForName(s);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
std::vector<std::string>
SmtSolver::evidence_names() {
    return evidenceNames();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::clear_evidence() {}

// FIXME[Robb Matzke 2017-10-17]: deprecated
const SmtSolver::Stats&
SmtSolver::get_stats() const {
    return statistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::reset_stats() {
    resetStatistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::parse_evidence() {
    parseEvidence();
}

} // namespace
} // namespace
