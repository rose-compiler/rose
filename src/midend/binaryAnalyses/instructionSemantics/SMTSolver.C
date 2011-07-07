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

size_t SMTSolver::total_calls;

bool
SMTSolver::satisfiable(const InsnSemanticsExpr::TreeNode *tn)
{
    bool retval = false;

#ifdef _MSC_VER
    // tps (06/23/2010) : Does not work under Windows
    abort();
#else

    total_calls++;
    output_text.clear();

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
    generate_file(config, tn, &defns);
    config.close();

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

    /* Run the solver and read its output */
    {
        std::string cmd = get_command(config_name);
        FILE *output = popen(cmd.c_str(), "r");
        assert(output!=NULL);
        char *line = NULL;
        size_t line_alloc = 0;
        while (rose_getline(&line, &line_alloc, output)>0)
            output_text.push_back(std::string(line));
        if (line) free(line);
        int status = pclose(output);
        if (debug) {
            fprintf(debug, "Running SMT solver=\"%s\"; exit status=%d\n", cmd.c_str(), status);
            fprintf(debug, "SMT Solver output:\n");
            for (size_t i=0; i<output_text.size(); i++)
                fprintf(debug, "     %5zu: %s", i+1, output_text[i].c_str());
        }
    }

    /* First line should be the word "sat" or "unsat" */
    assert(!output_text.empty());
    if (output_text[0]=="sat\n") {
        retval = true;
    } else if (output_text[0]=="unsat\n") {
        retval = false;
    } else {
        std::cerr <<"SMT solver failed to say \"sat\" or \"unsat\"\n";
        abort(); /*probably not reached*/
    }

    unlink(config_name);
    parse_evidence();
#endif
    return retval;
}
