#include "rose.h"
#ifndef _MSC_VER
#include "rose_getline.h" /* Mac OSX v10.6 does not have GNU getline() */
#endif
#include "SMTSolver.h"

#include <fcntl.h> /*for O_RDWR, etc.*/

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

SMTSolver::Satisfiable
SMTSolver::satisfiable(const std::vector<InsnSemanticsExpr::TreeNodePtr> &exprs)
{
    Satisfiable retval = SAT_UNKNOWN;
    bool got_satunsat_line = false;

#ifdef _MSC_VER
    // tps (06/23/2010) : Does not work under Windows
    abort();
#else

    clear_evidence();
    ++stats.ncalls;
    RTS_MUTEX(class_stats_mutex) {
        ++class_stats.ncalls;
    } RTS_MUTEX_END;
    output_text = "";

    /* Generate the input file for the solver. */
    char config_name[L_tmpnam];
    while (1) {
        tmpnam(config_name);
        int fd = open(config_name, O_RDWR|O_EXCL|O_CREAT, 0666);
        if (fd>=0) {
            close(fd);
            break;
        }
    }
    std::ofstream config(config_name);
    Definitions defns;
    generate_file(config, exprs, &defns);
    config.close();
    struct stat sb;
    int status = stat(config_name, &sb);
    assert(status>=0);
    stats.input_size += sb.st_size;
    RTS_MUTEX(class_stats_mutex) {
        class_stats.input_size += sb.st_size;
    } RTS_MUTEX_END;

    /* Show solver input */
    if (debug) {
        fprintf(debug, "SMT Solver input in %s:\n", config_name);
        size_t n=0;
        std::ifstream f(config_name);
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            if (!line.empty())
                fprintf(debug, "    %5zu: %s\n", ++n, line.c_str());
        }
    }

    /* Run the solver and read its output. The first line should be the word "sat" or "unsat" */
    {
        std::string cmd = get_command(config_name);
        FILE *output = popen(cmd.c_str(), "r");
        assert(output!=NULL);
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
        if (debug) {
            fprintf(debug, "Running SMT solver=\"%s\"; exit status=%d\n", cmd.c_str(), status);
            fprintf(debug, "SMT Solver reported: %s\n", (SAT_YES==retval ? "sat" : SAT_NO==retval ? "unsat" : "unknown"));
            fprintf(debug, "SMT Solver output:\n%s", StringUtility::prefixLines(output_text, "     ").c_str());
        }
    }

    unlink(config_name);

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
