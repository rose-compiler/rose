#include "parallel_compass.h"

#include <mpi.h>
#include <limits>

class CountingOutputObject: public Compass::OutputObject
{
public:
    CountingOutputObject(std::string name = ""): outputs(0), name(name)
    {
    }

    virtual void addOutput(Compass::OutputViolationBase *)
    {
        outputs++;
    }

    void reset()
    {
        outputs = 0;
    }

    unsigned int outputs;
    std::string name;
};

// What macro? There is no macro here. This is a code generator!
#define generate_checker(CheckerName) \
    try { \
        CompassAnalyses::CheckerName::Traversal *traversal; \
        CountingOutputObject *output = new CountingOutputObject(/*#CheckerName*/); \
        traversal = new CompassAnalyses::CheckerName::Traversal(params, output); \
        traversals.push_back(traversal); \
        bases.push_back(traversal); \
        outputs.push_back(output); \
    } catch (const Compass::ParameterNotFoundException &e) { \
        std::cerr << e.what() << std::endl; \
    }

void compassCheckers(
        std::vector<AstSimpleProcessing *> &traversals,
        std::vector<Compass::TraversalBase *> &bases,
        std::vector<CountingOutputObject *> &outputs)
{
    try {
        Compass::Parameters params("compass_parameters");

        #include "generate_checkers.C"

    } catch (const Compass::ParseError &e) {
        std::cerr << e.what() << std::endl;
    }
}

double timeDifference(struct timespec end, struct timespec begin)
{
    return (end.tv_sec + end.tv_nsec / 1.0e9)
        - (begin.tv_sec + begin.tv_nsec / 1.0e9);
}

inline void gettime(struct timespec &t)
{
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
}

void output_results(std::vector<CountingOutputObject *> &outputs)
{
    std::vector<CountingOutputObject *>::iterator o_itr;
    std::cout << "results: ";
    for (o_itr = outputs.begin(); o_itr != outputs.end(); ++o_itr)
    {
        std::cout << ((*o_itr)->name != "" ? (*o_itr)->name+ ": " : "")
            << (*o_itr)->outputs << " ";
        (*o_itr)->reset();
    }
}

class NodeCounter: public AstSimpleProcessing
{
public:
    size_t totalNodes;
    size_t *fileNodes;

    NodeCounter(int numberOfFiles)
        : totalNodes(0), fileNodes(new size_t[numberOfFiles+1]), fileptr(fileNodes)
    {
        *fileptr = 0;
    }

protected:
    virtual void visit(SgNode *node)
    {
        totalNodes++;
        ++*fileptr;

        // must be run postorder!
        if (isSgFile(node))
        {
            fileptr++;
            *fileptr = fileptr[-1];
        }
    };

    size_t *fileptr;
};

std::pair<int, int> computeFileIndices(SgProject *project, int my_rank, int processes)
{
    NodeCounter nc(project->numberOfFiles());
    nc.traverse(project, postorder);
    ROSE_ASSERT(nc.fileNodes[project->numberOfFiles() - 1] == nc.totalNodes - 1);

#if 0
    if (my_rank == 0)
    {
        std::cout << nc.totalNodes << " nodes total" << std::endl;
    }
#endif

    int lo, hi = 0;
    int my_lo, my_hi;
    for (int rank = 0; rank < processes; rank++)
    {
        const size_t my_nodes_high = (nc.totalNodes / processes + 1) * (rank + 1);
        // set lower limit
        lo = hi;
        // find upper limit
        for (hi = lo + 1; hi < project->numberOfFiles(); hi++)
        {
            if (nc.fileNodes[hi] > my_nodes_high)
                break;
        }

        // make sure all files have been assigned to some process
        if (rank == processes - 1)
            ROSE_ASSERT(hi == project->numberOfFiles());

        if (rank == my_rank)
        {
            my_lo = lo;
            my_hi = hi;
#if 0
            std::cout << "process " << rank << ": files [" << lo << "," << hi
                << "[ for a total of "
                << (lo != 0 ? nc.fileNodes[hi-1] - nc.fileNodes[lo-1] : nc.fileNodes[hi-1])
                << " nodes" << std::endl;
#endif
            break;
        }
    }

    return std::make_pair(my_lo, my_hi);
}

#define USE_BINARY_AST_IO 0

int main(int argc, char **argv)
{
    struct timespec begin, end;

    /* setup MPI */
    int my_rank, processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    /* read the AST, either from a binary file or from sources */
#if USE_BINARY_AST_IO
    if (argc != 2)
    {
        std::cerr << "must be called with exactly one binary AST file argument"
            << std::endl;
    }
    ROSE_ASSERT(argc == 2);
    gettime(begin);
    SgProject *root = AST_FILE_IO::readASTFromFile(argv[1]);
    gettime(end);
    if (my_rank == 0)
    {
#if 0
        std::cout << "reading binary AST took about "
            << timeDifference(end, begin) << " seconds"
            << std::endl;
#endif
    }
#else
    gettime(begin);
    SgProject *root = frontend(argc, argv);
    gettime(end);
    if (my_rank == 0)
    {
        std::cout << "frontend ran for about "
            << timeDifference(end, begin) << " seconds"
            << std::endl;
    }
#endif

    /* setup checkers */
    std::vector<AstSimpleProcessing *> traversals;
    std::vector<AstSimpleProcessing *>::iterator t_itr;
    std::vector<Compass::TraversalBase *> bases;
    std::vector<Compass::TraversalBase *>::iterator b_itr;
    std::vector<CountingOutputObject *> outputs;
    std::vector<CountingOutputObject *>::iterator o_itr;

    compassCheckers(traversals, bases, outputs);

    ROSE_ASSERT(traversals.size() == bases.size() && bases.size() == outputs.size());
#if 1
    if (my_rank == 0)
    {
        std::cout << std::endl << "got " << bases.size() << " checkers:";
        for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
            std::cout << ' ' << (*b_itr)->getName();
        std::cout << std::endl;
    }
#endif


    /* figure out which files to process on this process */
    std::pair<int, int> bounds = computeFileIndices(root, my_rank, processes);


    /* traverse the files */
 #define CALL_RUN_FOR_CHECKERS 1
// #define RUN_COMBINED_CHECKERS 1
    gettime(begin);
    for (int i = bounds.first; i < bounds.second; i++)
    {
      std::cout << "bounds [ " << bounds.first << "," << bounds.second << "[" << std::endl;
#if CALL_RUN_FOR_CHECKERS
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	std::cout << "running checker : " << (*b_itr)->getName() << " \t on " << (root->get_file(i).getFileName()) << std::endl; 
            (*b_itr)->run(&root->get_file(i));
      }
#elif RUN_COMBINED_CHECKERS
        AstCombinedSimpleProcessing combined(traversals);
        combined.traverse(&root->get_file(i), preorder);
#else
        AstSharedMemoryParallelSimpleProcessing parallel(traversals);
        parallel.traverseInParallel(&root->get_file(i), preorder);
#endif
        // output_results(outputs);
    }
    gettime(end);

    /* communicate results */
    unsigned int *my_output_values = new unsigned int[outputs.size()];
    for (size_t i = 0; i < outputs.size(); i++)
        my_output_values[i] = outputs[i]->outputs;
    unsigned int *output_values = new unsigned int[outputs.size()];
    MPI_Reduce(my_output_values, output_values, outputs.size(), MPI_UNSIGNED,
            MPI_SUM, 0, MPI_COMM_WORLD);

    /* communicate times */
    double *times = new double[processes];
    double my_time = timeDifference(end, begin);
    MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
            MPI_COMM_WORLD);

    /* print everything */
    if (my_rank == 0)
    {
#if 1
        std::cout << "results:";
        for (size_t i = 0; i < outputs.size(); i++)
            std::cout << " " << output_values[i];
        std::cout << std::endl;
#endif
        // std::cout << "times:";
        double total_time = 0.0;
        double min_time = std::numeric_limits<double>::max(), max_time = 0.0;
        for (size_t i = 0; i < (size_t) processes; i++)
        {
            // std::cout << " " << times[i];
            total_time += times[i];
            if (min_time > times[i])
                min_time = times[i];
            if (max_time < times[i])
                max_time = times[i];
        }
        // std::cout << std::endl;
        std::cout << "total time: " << total_time
            << ", fastest process: " << min_time
            << ", slowest process: " << max_time
            << std::endl;
        std::cout << std::endl;
    }

    /* all done */
    MPI_Finalize();
    delete[] output_values;
    delete[] times;
}
