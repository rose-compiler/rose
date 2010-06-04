#include "rose.h"
#include "SMTSolver.h"

bool
SMTSolver::satisfiable(const SymbolicExpr::TreeNode *tn)
{
    /* Generate the input file for the solver. */
    std::string config_name = "x.conf";      /*FIXME: should not use a static file name*/
    std::ofstream config(config_name.c_str(), std::ios::trunc);
    Definitions defns;
    generate_file(config, tn, &defns);
    config.close();

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
    if (!strcmp(line, "sat\n"))
        return true;
    if (!strcmp(line, "unsat\n"))
        return false;
    std::cout <<"    exit status=" <<status <<" input=" <<line;
    execl("/bin/cat", "cat", "-n", config_name.c_str(), NULL);
    abort(); /*probably not reached*/
}
