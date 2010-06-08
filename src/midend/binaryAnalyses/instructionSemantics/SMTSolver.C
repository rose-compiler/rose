#include "rose.h"
#include "SMTSolver.h"

#include <fcntl.h> /*for O_RDWR, etc.*/

bool
SMTSolver::satisfiable(const InsnSemanticsExpr::TreeNode *tn)
{
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
            fprintf(debug, "    %5zu: %s", ++n, line.c_str());
        }
    }

    /* Run the solver and look at the first line of output. It should be "sat" or "unsat". */
    std::string cmd = get_command(config_name);
    FILE *output = popen(cmd.c_str(), "r");
    ROSE_ASSERT(output!=NULL);
    static char *line=NULL;
    static size_t line_alloc=0;
    ssize_t nread = getline(&line, &line_alloc, output);
    ROSE_ASSERT(nread>0);
    int status = pclose(output);

    /* First line should be the word "sat" or "unsat" */
    bool retval = false;
    if (debug)
        fprintf(debug, "    result: %s\n", line);
    if (!strcmp(line, "sat\n")) {
        retval = true;
    } else if (!strcmp(line, "unsat\n")) {
        retval = false;
    } else {
        std::cout <<"    exit status=" <<status <<" input=" <<line;
        execl("/bin/cat", "cat", "-n", config_name, NULL);
        abort(); /*probably not reached*/
    }

    unlink(config_name);
    return retval;
}
