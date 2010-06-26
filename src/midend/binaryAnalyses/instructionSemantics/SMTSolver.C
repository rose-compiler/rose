#include "rose.h"
#include "SMTSolver.h"

#include <fcntl.h> /*for O_RDWR, etc.*/

std::ostream&
operator<<(std::ostream &o, const SMTSolver::Exception &e)
{
    return o <<"SMT solver: " <<e.mesg;
}

bool
SMTSolver::satisfiable(const InsnSemanticsExpr::TreeNode *tn)
{
    /* Generate the input file for the solver. */
    char config_name[L_tmpnam];
    while (1) {
#ifndef _MSC_VER
		// tps (06/23/2010) : Does not work under Windows
	int fd = open(config_name, O_RDWR|O_EXCL|O_CREAT, 0666);
        if (fd>=0) {
            close(fd);
            break;
        }
#endif
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
            fprintf(debug, "    %5zu: %s", ++n, line.c_str());
        }
    }

    /* Run the solver and look at the first line of output. It should be "sat" or "unsat". */
    std::string cmd = get_command(config_name);
#ifdef _MSC_VER
	// tps (06/23/2010) : popen not understood in Windows
	FILE *output=NULL;
#else
	FILE *output = popen(cmd.c_str(), "r");
#endif
	ROSE_ASSERT(output!=NULL);
    static char *line=NULL;
    static size_t line_alloc=0;
#ifdef _MSC_VER
	// tps (06/23/2010) : getline not understood in Windows
    abort;
#else
    ssize_t nread = getline(&line, &line_alloc, output);
#endif
	ROSE_ASSERT(nread>0);
#ifdef _MSC_VER
	// tps (06/23/2010) : pclose not understood in Windows
	abort;
#else
    int status = pclose(output);
#endif
    /* First line should be the word "sat" or "unsat" */
    bool retval = false;
    if (debug)
        fprintf(debug, "    result: %s\n", line);
    if (!strcmp(line, "sat\n")) {
        retval = true;
    } else if (!strcmp(line, "unsat\n")) {
        retval = false;
    } else {
#ifdef _MSC_VER
	// tps (06/23/2010) : execl not understood in Windows
#else
        std::cout <<"    exit status=" <<status <<" input=" <<line;
        execl("/bin/cat", "cat", "-n", config_name, NULL);
#endif
		abort(); /*probably not reached*/
    }

    unlink(config_name);
    return retval;
}
