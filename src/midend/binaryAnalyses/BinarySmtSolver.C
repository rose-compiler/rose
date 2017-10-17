#include "sage3basic.h"

#include "rose_getline.h"
#include "BinarySmtSolver.h"
#include "YicesSolver.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <fcntl.h> /*for O_RDWR, etc.*/
#include <Sawyer/FileSystem.h>
#include <Sawyer/Stopwatch.h>

namespace Rose {
namespace BinaryAnalysis {


std::ostream&
operator<<(std::ostream &o, const SmtSolver::Exception &e)
{
    return o <<"SMT solver: " <<e.mesg;
}

SmtSolver::Stats SmtSolver::class_stats;
boost::mutex SmtSolver::class_stats_mutex;

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
SmtSolver::get_class_stats() 
{
    boost::lock_guard<boost::mutex> lock(class_stats_mutex);
    return class_stats;
}

// class method
void
SmtSolver::reset_class_stats()
{
    boost::lock_guard<boost::mutex> lock(class_stats_mutex);
    class_stats = Stats();
}

SymbolicExpr::Ptr
SmtSolver::evidence_for_address(uint64_t addr)
{
    return evidence_for_name(StringUtility::addrToString(addr));
}

SmtSolver::Satisfiable
SmtSolver::trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs_)
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

    clear_evidence();

    Satisfiable retval = trivially_satisfiable(exprs);
    if (retval!=SAT_UNKNOWN)
        return retval;

    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(class_stats_mutex);
        ++class_stats.ncalls;
    }
    output_text = "";

    /* Generate the input file for the solver. */
    Sawyer::FileSystem::TemporaryFile tmpfile;
    Definitions defns;
    generate_file(tmpfile.stream(), exprs, &defns);
    tmpfile.stream().close();
    struct stat sb;
    int status __attribute__((unused)) = stat(tmpfile.name().string().c_str(), &sb);
    ASSERT_require(status>=0);
    stats.input_size += sb.st_size;
    {
        boost::lock_guard<boost::mutex> lock(class_stats_mutex);
        class_stats.input_size += sb.st_size;
    }

    /* Show solver input */
    if (debug) {
        fprintf(debug, "SMT Solver input in %s:\n", tmpfile.name().string().c_str());
        size_t n=0;
        std::ifstream f(tmpfile.name().string().c_str());
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            fprintf(debug, "    %5zu: %s\n", ++n, line.c_str());
        }
    }

    /* Run the solver and read its output. The first line should be the word "sat" or "unsat" */
    {
        Sawyer::Stopwatch stopwatch;
        std::string cmd = get_command(tmpfile.name().string());
        FILE *output = popen(cmd.c_str(), "r");
        ASSERT_not_null(output);
        char *line = NULL;
        size_t line_alloc = 0;
        ssize_t nread;
        while ((nread=rose_getline(&line, &line_alloc, output))>0) {
            stats.output_size += nread;
            {
                boost::lock_guard<boost::mutex> lock(class_stats_mutex);
                class_stats.output_size += nread;
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
                output_text += std::string(line);
            }
        }
        if (line) free(line);
        int status = pclose(output);
        stopwatch.stop();
        if (debug) {
            fprintf(debug, "Running SMT solver=\"%s\"; exit status=%d\n", cmd.c_str(), status);
            fprintf(debug, "SMT Solver ran for %g seconds\n", stopwatch.report());
            fprintf(debug, "SMT Solver reported: %s\n", (SAT_YES==retval ? "sat" : SAT_NO==retval ? "unsat" : "unknown"));
            fprintf(debug, "SMT Solver output:\n%s", StringUtility::prefixLines(output_text, "     ").c_str());
        }
    }

    if (SAT_YES==retval)
        parse_evidence();
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

} // namespace
} // namespace
