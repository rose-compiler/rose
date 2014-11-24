// Runs all tests from a provided list, forking new processes to handle related tasks.
//
// 1. A new process is forked to handle each binary specimen (the "specimen process")
//    This is so that disassembly-related memory is not allocated in the main process.
//    Only one "specimen process" is created at a time.
//
// 2. A "specimen process" forks N "testing processes" to handle testing.
//    N is specified on the command-line; these processes run in parallel
//    The forking happens after disassembly so that the children can all share the same disassembly info
//
// Since this program uses forking to handle parallelism, it should not usually be called in parallel itself.

#include "rose.h"
#include "RunTests.h"
#include "AST_FILE_IO.h"        // only for the clearAllMemoryPools() function [Robb P. Matzke 2013-06-17]
#include "rose_getline.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <cerrno>
#include <csignal>
#include <sys/wait.h>

using namespace CloneDetection;
using namespace CloneDetection::RunTests;
using namespace rose::BinaryAnalysis;

static bool sortedBySpecimen(const WorkItem &a, const WorkItem &b) {
    return a.specimen_id < b.specimen_id;
}

static void
load_sorted_work(MultiWork &work /*out*/)
{
    // Read list of tests from stdin
    Work all_work;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading worklist from stdin...\n";
        all_work = load_work("stdin", stdin);
    } else {
        FILE *f = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==f) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        all_work = load_work(opt.input_file_name, f);
        fclose(f);
    }
    std::cerr <<argv0 <<": " <<all_work.size() <<(1==all_work.size()?" test needs":" tests need") <<" to be run\n";

    // Return one worklist per specimen
    std::sort(all_work.begin(), all_work.end(), sortedBySpecimen);
    BOOST_FOREACH (const WorkItem &item, all_work) {
        if (work.empty() || item.specimen_id!=work.back().back().specimen_id)
            work.push_back(Work());
        work.back().push_back(item);
    }
}

// How many processors does this machine have?  Returns 1 if it can't figure that out.
size_t nProcessors()
{
    if (FILE *f = fopen("/proc/cpuinfo", "r")) {
        char *buf = NULL;
        size_t bufsz = 0, nprocs = 0;
        while (rose_getline(&buf, &bufsz, f) > 0) {
            if (boost::starts_with(std::string(buf), "processor\t:"))
                ++nprocs;
        }
        fclose(f);
        return std::max((size_t)1, nprocs);
    }
    return 1;
}

// Forks, calls f(), then returns zero on success or non-zero on failure.
template<class Functor>
int forkAndWait(Functor f)
{
    pid_t child = fork();
    if (-1==child) {
        perror("fork");
        exit(1);
    } else if (0!=child) {
#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
        std::cerr <<"forkAndWait: started process " <<child <<"\n";
#endif
        int status;
        while (1) {
            if (-1 == waitpid(child, &status, 0)) {
                perror("waitpid");
                exit(1);
            }
            if (WIFEXITED(status)) {
#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
                std::cerr <<"forkAndWait: process " <<child <<" exits with status " <<WEXITSTATUS(status) <<"\n";
#endif
                return WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
                std::cerr <<"forkAndWait: process " <<child <<" dies with signal " <<WTERMSIG(status) <<"\n";
#endif
                return 256 + WTERMSIG(status);
            }
        }
    } else {
        f();
        exit(0);
    }
}

// Wait for a process to exit and return its simplified exit status via "status" argument.  Returns the pid that was
// reaped, or negative on failure.
typedef Map<pid_t, int> Processes;
pid_t waitForOne(const Processes &running, int *returnedStatus /*out*/)
{
    assert(returnedStatus!=NULL);
    while (1) {
        int status;
        pid_t waited = waitpid(WAIT_ANY, &status, 0);
        if (-1==waited)
            return -errno;
        if (!running.exists(waited)) {
            std::cerr <<"warning: waitForAny() reaped an unexpected child\n";
        } else if (WIFEXITED(status)) {
            *returnedStatus = WEXITSTATUS(status);
            return waited;
        } else if (WIFSIGNALED(status)) {
            *returnedStatus = 256 + WTERMSIG(status);
            return waited;
        }
    }
}

// Run all functors in parallel with a parallelism of nProcs. Returns number of jobs that failed.
template<class Functor>
size_t runInParallel(std::vector<Functor> &functors, size_t nProcs) {
    std::vector<int> fstatus(functors.size(), -1);      // exit status from each functor

    nProcs = std::max((size_t)1, nProcs);
    std::cerr <<"runInParallel: using " <<StringUtility::plural(nProcs, "processors") <<"\n";
    Processes running;                                  // processes that are running indexed by PID
    for (size_t i=0; i<functors.size(); ++i) {
        assert(running.size() < nProcs);

        // Start the child
        pid_t child = fork();
        if (-1==child) {
            perror("fork");
            exit(1);
        } else if (0!=child) {
            running[child] = i;
        } else {
            functors[i]();
            exit(0);
        }

#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
        std::cerr <<"runInParallel: started process " <<child <<" (" <<running.size() <<" now running)\n";
#endif

        // Pause if we've reached our parallelism limit
        if (running.size() >= nProcs) {
            int status;
            pid_t waited = waitForOne(running, &status);
            assert(waited>0);
            assert(running.exists(waited));
            fstatus[running[waited]] = status;
            running.erase(waited);
#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
            std::cerr <<"runInParallel: reaped process " <<waited <<" (" <<running.size() <<" now running)\n";
#endif
        }
    }

    // Wait for everything to terminate
    while (!running.empty()) {
#if 1 /*DEBUGGING [Robb P. Matzke 2014-04-07]*/
        std::cerr <<"runInParallel: waiting for " <<running.size() <<" to finish\n";
#endif
        int status;
        pid_t waited = waitForOne(running, &status);
        assert(waited>0);
        assert(running.exists(waited));
        fstatus[running[waited]] = status;
        running.erase(waited);
    }

    // Look at exit status
    size_t retval = 0;
    BOOST_FOREACH (int status, fstatus)
        retval += status ? 1 : 0;
    return retval;
}

// Run some tests for a single specimen
class SomeTests {
    Work work;                                          // tests that need to run for this specimen
    std::string databaseUrl;
    IdFunctionMap functions;
    FunctionIdMap function_ids;
    const InstructionProvidor *insns;
    int64_t cmd_id;
    const AddressIdMap *entry2id;                              // maps function entry address to function ID
    size_t ntests_ran;
public:
    SomeTests(const Work &work, const std::string &databaseUrl, const IdFunctionMap &functions,
              const FunctionIdMap &function_ids, const InstructionProvidor *insns, int64_t cmd_id,
              const AddressIdMap *entry2id)
        : work(work), databaseUrl(databaseUrl), functions(functions), function_ids(function_ids), insns(insns),
          cmd_id(cmd_id), entry2id(entry2id), ntests_ran(0) {}

    void operator()() {
        // Database connections don't survive over fork() according to SqLite and PostgreSQL documentation, so open it again
        SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(databaseUrl)->transaction();

        // Use zero for the number of tests ran so that this child process doesn't try to update the semantic_history table.
        // If two or more processes try to change the same row (which they will if there's a non-zero number of tests) then
        // they will deadlock with each other.
        static const size_t NO_TESTS_RAN = 0;

        NameSet builtin_function_names;
        add_builtin_functions(builtin_function_names/*out*/);

        InputGroup igroup;
        WorkItem prevWorkItem;
        SgAsmInterpretation *prev_interp = NULL;
        MemoryMap ro_map;
        Disassembler::AddressSet whitelist_exports;         // dynamic functions that should be called
        PointerDetectors pointers;
        InsnCoverage insn_coverage;
        DynamicCallGraph dynamic_cg;
        Tracer tracer;
        ConsumedInputs consumed_inputs;
        FuncAnalyses funcinfo;
        OutputGroups ogroups; // do not load from database (that might take a very long time)
        time_t last_checkpoint = time(NULL);
        for (size_t workIdx=0; workIdx<work.size(); ++workIdx) {
            WorkItem &workItem = work[workIdx];

            // Load the input group from the database if necessary.
            if (workItem.igroup_id!=prevWorkItem.igroup_id) {
                if (!igroup.load(tx, workItem.igroup_id)) {
                    std::cerr <<argv0 <<": input group " <<workItem.igroup_id <<" is empty or does not exist\n";
                    exit(1);
                }
            }

            // Find the function to test
            IdFunctionMap::iterator func_found = functions.find(workItem.func_id);
            assert(func_found!=functions.end());
            SgAsmFunction *func = func_found->second;
            if (opt.verbosity>=LACONIC) {
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<argv0 <<": " <<std::string(100, '=') <<"\n";
                std::cerr <<argv0 <<": processing function " <<function_to_str(func, function_ids) <<"\n";
            }
            SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);
            assert(interp!=NULL);

            // Do per-interpretation stuff
            if (interp!=prev_interp) {
                prev_interp = interp;
                assert(interp->get_map()!=NULL);
                ro_map = *interp->get_map();
                ro_map.require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).keep();
                Disassembler::AddressSet whitelist_imports = get_import_addresses(interp, builtin_function_names);
                whitelist_exports.clear(); // imports are addresses of import table slots; exports are functions
                overmap_dynlink_addresses(interp, *insns, opt.params.follow_calls, &ro_map, GOTPLT_VALUE,
                                          whitelist_imports, whitelist_exports/*out*/);
                if (opt.verbosity>=EFFUSIVE) {
                    std::cerr <<argv0 <<": memory map for SgAsmInterpretation:\n";
                    interp->get_map()->dump(std::cerr, argv0+":   ");
                }
            }

            // Run the test
            assert(insns!=NULL);
            assert(entry2id!=NULL);
            std::cerr <<"process " <<getpid() <<" about to run test " <<workIdx <<"/" <<work.size() <<" " <<workItem <<"\n";
            runOneTest(tx, workItem, pointers, func, function_ids, insn_coverage, dynamic_cg, tracer, consumed_inputs,
                       interp, whitelist_exports, cmd_id, igroup, funcinfo, *insns, &ro_map, *entry2id, ogroups);
            ++ntests_ran;

            // Checkpoint
            if (opt.checkpoint>0 && time(NULL)-last_checkpoint > opt.checkpoint) {
                if (!opt.dry_run)
                    tx = checkpoint(tx, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs, NULL, NO_TESTS_RAN,
                                    cmd_id);
                last_checkpoint = time(NULL);
            }

            prevWorkItem = workItem;
        }
        std::cerr <<"process " <<getpid() <<" is done testing; now finishing up...\n";

        if (!tx->is_terminated()) {
            SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_funcpartials"
                                                           " (func_id, ncalls, nretused, ntests, nvoids) values"
                                                           " (?,       ?,      ?,        ?,      ?)");
            for (FuncAnalyses::iterator fi=funcinfo.begin(); fi!=funcinfo.end(); ++fi) {
                stmt->bind(0, fi->first);
                stmt->bind(1, fi->second.ncalls);
                stmt->bind(2, fi->second.nretused);
                stmt->bind(3, fi->second.ntests);
                stmt->bind(4, fi->second.nvoids);
                stmt->execute();
            }
        }

        // Cleanup
        if (!tx->is_terminated() && !opt.dry_run) {
            std::cerr <<"process " <<getpid() <<" is doing the final checkpoint\n";
            checkpoint(tx, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs, NULL, NO_TESTS_RAN, cmd_id);
        }
        tx.reset();

        std::cerr <<"process " <<getpid() <<" finished\n";
    }
};


// Process all work for one specimen
class SpecimenProcessor {
    const Work &work;
    FilesTable &files;
    std::string databaseUrl;
    int64_t cmd_id;
public:
    SpecimenProcessor(const Work &work, FilesTable &files, const std::string databaseUrl, int64_t cmd_id)
        : work(work), files(files), databaseUrl(databaseUrl), cmd_id(cmd_id) {
    }

    void operator()() {
        if (work.empty())
            return;
        int specimen_id = work.front().specimen_id;

        // Database connections don't survive over fork() according to SqLite and PostgreSQL documentation, so open it again
        SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(databaseUrl)->transaction();

        OutputGroups ogroups; // do not load from database (that might take a very long time)

        if (opt.verbosity>=LACONIC) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<argv0 <<": " <<std::string(100, '#') <<"\n";
            std::cerr <<argv0 <<": processing binary specimen \"" <<files.name(specimen_id) <<"\"\n";
        }

        // Parse the specimen
        SgProject *project = files.load_ast(tx, specimen_id);
        if (!project)
            project = open_specimen(tx, files, specimen_id, argv0);
        if (!project) {
            std::cerr <<argv0 <<": problems loading specimen\n";
            exit(1);
        }

        // Get list of specimen functions and initialize the instruction cache
        std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(project);
        IdFunctionMap functions = existing_functions(tx, files, all_functions);
        FunctionIdMap function_ids;
        AddressIdMap entry2id;                              // maps function entry address to function ID
        for (IdFunctionMap::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            function_ids[fi->second] = fi->first;
            entry2id[fi->second->get_entry_va()] = fi->first;
        }
        InstructionProvidor insns = InstructionProvidor(all_functions);

        // Split the work list into chunks, each containing testsPerChunk except the last, which may contain fewer.
        static const size_t testsPerChunk = 25;
        size_t nChunks = (work.size() + testsPerChunk - 1) / testsPerChunk;
        std::vector<SomeTests> jobs;
        for (size_t i=0; i<nChunks; ++i) {
            size_t beginWorkIdx = i * testsPerChunk;
            size_t endWorkIdx = std::min((i+1)*testsPerChunk, work.size());
            Work partWork(work.begin()+beginWorkIdx, work.begin()+endWorkIdx);
            jobs.push_back(SomeTests(partWork, databaseUrl, functions, function_ids, &insns, cmd_id, &entry2id));
        }

        // Run the parts in parallel using the maximum parallelism specified on the command-line.  We must commit our
        // transaction before forking, otherwise the children won't see the rows we've added to various tables.
        tx->commit();
        tx.reset();
        size_t nfailed = runInParallel(jobs, opt.nprocs);
        if (nfailed!=0) {
            std::cerr <<"SpecimenProcessor: " <<StringUtility::plural(nfailed, "jobs") <<" failed\n";
            exit(1);
        }
    }
};

int
main(int argc, char *argv[])
{
    // Parse command-line
    opt.nprocs = nProcessors();
    int argno = parse_commandline(argc, argv);
    if (argno+1!=argc)
        usage(1);
    std::string databaseUrl = argv[argno++];

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(databaseUrl)->transaction();
    int64_t cmd_id = start_command(tx, argc, argv, "running tests");

    // Load worklist
    MultiWork work;
    load_sorted_work(work/*out*/);
    if (work.empty())
        return 0;


    // Load information about files.  The transaction is not saved anywhere.
    FilesTable files(tx);

    // We must commit our transaction before we fork, otherwise the child processes won't be able to see the rows we've
    // inserted. Specifically, the row in the semantic_history table that says who we are.  Destroy the smart pointer so that
    // the connection is even closed.
    tx->commit();
    tx.reset();

    // Process work items for each specimen sequentially
    BOOST_FOREACH (const Work &workForSpecimen, work)
        if (forkAndWait(SpecimenProcessor(workForSpecimen, files, databaseUrl, cmd_id)))
            exit(1);

    // Indicate that this command is finished
    tx = SqlDatabase::Connection::create(databaseUrl)->transaction();
    finish_command(tx, cmd_id, "ran tests");
    tx->commit();

    return 0;
}

