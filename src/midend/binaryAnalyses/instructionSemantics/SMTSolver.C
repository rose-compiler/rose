#include "sage3basic.h"

#include "rose_getline.h"
#include "SMTSolver.h"

#include <fcntl.h> /*for O_RDWR, etc.*/
#include <sawyer/Stopwatch.h>

namespace rose {
namespace BinaryAnalysis {


std::ostream&
operator<<(std::ostream &o, const SMTSolver::Exception &e)
{
    return o <<"SMT solver: " <<e.mesg;
}

SMTSolver::Stats SMTSolver::class_stats;
RTS_mutex_t SMTSolver::class_stats_mutex = RTS_MUTEX_INITIALIZER(RTS_LAYER_ROSE_SMT_SOLVERS);

void
SMTSolver::init()
{}

// class method
SMTSolver::Stats
SMTSolver::get_class_stats() 
{
    Stats retval;
    RTS_MUTEX(class_stats_mutex) {
        retval = class_stats;
    } RTS_MUTEX_END;
    return retval;
}

// class method
void
SMTSolver::reset_class_stats()
{
    RTS_MUTEX(class_stats_mutex) {
        class_stats = Stats();
    } RTS_MUTEX_END;
}

InsnSemanticsExpr::TreeNodePtr
SMTSolver::evidence_for_address(uint64_t addr)
{
    return evidence_for_name(StringUtility::addrToString(addr));
}

SMTSolver::Satisfiable
SMTSolver::trivially_satisfiable(const std::vector<InsnSemanticsExpr::TreeNodePtr> &exprs_)
{
    std::vector<InsnSemanticsExpr::TreeNodePtr> exprs(exprs_.begin(), exprs_.end());
    for (size_t i=0; i<exprs.size(); ++i) {
        if (exprs[i]->is_known()) {
            ASSERT_require(1==exprs[i]->get_nbits());
            if (0==exprs[i]->get_value())
                return SAT_NO;
            std::swap(exprs[i], exprs.back()); // order of exprs is not important
            exprs.resize(exprs.size()-1);
        }
    }
    return exprs.empty() ? SAT_YES : SAT_UNKNOWN;
}

SMTSolver::Satisfiable
SMTSolver::satisfiable(const std::vector<InsnSemanticsExpr::TreeNodePtr> &exprs)
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
    RTS_MUTEX(class_stats_mutex) {
        ++class_stats.ncalls;
    } RTS_MUTEX_END;
    output_text = "";

    /* Generate the input file for the solver. */
    struct TempFile {
        std::ofstream file;
        char name[L_tmpnam];
        TempFile() {
            while (1) {
                tmpnam(name);
                int fd = open(name, O_RDWR|O_EXCL|O_CREAT, 0666);
                if (fd>=0) {
                    close(fd);
                    break;
                }
            }
            std::ofstream config(name);
            file.open(name);
        }
        ~TempFile() {
            unlink(name);
        }
    } tmpfile;

    Definitions defns;
    generate_file(tmpfile.file, exprs, &defns);
    tmpfile.file.close();
#if 0 // DEBUGGING [Robb P. Matzke 2015-03-19]
    std::cerr <<"ROBB: saving SMT file as 'x.smt'\n";
    system((std::string("cp ") + tmpfile.name + " x.smt").c_str());
#endif
    struct stat sb;
    int status __attribute__((unused)) = stat(tmpfile.name, &sb);
    ASSERT_require(status>=0);
    stats.input_size += sb.st_size;
    RTS_MUTEX(class_stats_mutex) {
        class_stats.input_size += sb.st_size;
    } RTS_MUTEX_END;

    /* Show solver input */
    if (debug) {
        fprintf(debug, "SMT Solver input in %s:\n", tmpfile.name);
        size_t n=0;
        std::ifstream f(tmpfile.name);
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            fprintf(debug, "    %5zu: %s\n", ++n, line.c_str());
        }
    }

    /* Run the solver and read its output. The first line should be the word "sat" or "unsat" */
    {
        Sawyer::Stopwatch stopwatch;
        std::string cmd = get_command(tmpfile.name);
        FILE *output = popen(cmd.c_str(), "r");
        ASSERT_not_null(output);
        char *line = NULL;
        size_t line_alloc = 0;
        ssize_t nread;
        while ((nread=rose_getline(&line, &line_alloc, output))>0) {
            stats.output_size += nread;
            RTS_MUTEX(class_stats_mutex) {
                class_stats.output_size += nread;
            } RTS_MUTEX_END;
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
    

SMTSolver::Satisfiable
SMTSolver::satisfiable(const InsnSemanticsExpr::TreeNodePtr &tn)
{
    std::vector<InsnSemanticsExpr::TreeNodePtr> exprs;
    exprs.push_back(tn);
    return satisfiable(exprs);
}

SMTSolver::Satisfiable
SMTSolver::satisfiable(std::vector<InsnSemanticsExpr::TreeNodePtr> exprs, const InsnSemanticsExpr::TreeNodePtr &expr)
{
    if (expr!=NULL)
        exprs.push_back(expr);
    return satisfiable(exprs);
}

} // namespace
} // namespace
