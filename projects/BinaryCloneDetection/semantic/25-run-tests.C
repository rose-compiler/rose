// Runs specific functions with specific inputs.

#include "sage3basic.h"
#include "RunTests.h"
#include "AST_FILE_IO.h"        // only for the clearAllMemoryPools() function [Robb P. Matzke 2013-06-17]

#include <cerrno>
#include <csignal>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace CloneDetection;
using namespace CloneDetection::RunTests;

int
main(int argc, char *argv[])
{
    int argno = parse_commandline(argc, argv);
    if (argno+1!=argc)
        CloneDetection::RunTests::usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();
    int64_t cmd_id = start_command(tx, argc, argv, "running tests");

    // Read list of tests from stdin
    Work work;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading worklist from stdin...\n";
        work = load_work("stdin", stdin);
    } else {
        FILE *f = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==f) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        work = load_work(opt.input_file_name, f);
        fclose(f);
    }
    std::cerr <<argv0 <<": " <<work.size() <<(1==work.size()?" test needs":" tests need") <<" to be run\n";
    if (work.empty())
        return 0;
    Progress progress(work.size());
    progress.force_output(opt.progress);
    OutputGroups ogroups; // do not load from database (that might take a very long time)
    NameSet builtin_function_names;
    add_builtin_functions(builtin_function_names/*out*/);

    // Set up the interrupt handler
    if (opt.interactive) {
        struct sigaction sa;
        sa.sa_handler = sig_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGINT, &sa, NULL);
    }

    // Process each item on the work list.
    PointerDetectors pointers;
    FilesTable files(tx);
    WorkItem prev_work;
    IdFunctionMap functions;
    FunctionIdMap function_ids;
    Disassembler::AddressSet whitelist_exports;         // dynamic functions that should be called
    InputGroup igroup;
    InstructionProvidor insns;
    SgAsmInterpretation *prev_interp = NULL;
    MemoryMap ro_map;
    AddressIdMap entry2id;                              // maps function entry address to function ID
    Tracer tracer;
    InsnCoverage insn_coverage;
    DynamicCallGraph dynamic_cg;
    ConsumedInputs consumed_inputs;
    time_t last_checkpoint = time(NULL);
    size_t ntests_ran=0;
    FuncAnalyses funcinfo;
    for (size_t work_idx=0; work_idx<work.size(); ++work_idx, ++progress) {
        const WorkItem &workItem = work[work_idx];

        // If we're switching to a new specimen then we need to reinitialize the AST, throwing away the old information and
        // getting new information.  The new information comes from either a stored AST, or by extracting the binaries from
        // the database and reparsing them, or by reparsing existing binaries (in that order of preference).
        if (workItem.specimen_id!=prev_work.specimen_id) {
            if (opt.verbosity>=LACONIC) {
                progress.clear();
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<argv0 <<": " <<std::string(100, '#') <<"\n";
                std::cerr <<argv0 <<": processing binary specimen \"" <<files.name(workItem.specimen_id) <<"\"\n";
            }

            if (prev_work.specimen_id>=0) {
#if 0 // [Robb P. Matzke 2013-06-17]
                // Is this the preferred way to delete a whole AST? It fails an assertion:
                // Cxx_GrammarTreeTraversalSuccessorContainer.C:23573:
                //     virtual SgNode* SgAsmGenericSectionList::get_traversalSuccessorByIndex(size_t):
                //     Assertion `idx < p_sections.size()' failed.
                SageInterface::deleteAST(SageInterface::getProject());
#else
                AST_FILE_IO::clearAllMemoryPools();
#endif
                prev_interp = NULL;
            }

            progress.message("loading AST");
            SgProject *project = files.load_ast(tx, workItem.specimen_id);
            progress.message("");

            if (!project) {
                progress.message("parsing specimen");
                project = open_specimen(tx, files, workItem.specimen_id, argv0);
                progress.message("");
            }
            if (!project) {
                std::cerr <<argv0 <<": problems loading specimen\n";
                exit(1);
            }

            // Get list of functions and initialize the instruction cache
            std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(project);
            functions = existing_functions(tx, files, all_functions);
            function_ids.clear();
            entry2id.clear();
            for (IdFunctionMap::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                function_ids[fi->second] = fi->first;
                entry2id[fi->second->get_entry_va()] = fi->first;
            }
            insns = InstructionProvidor(all_functions);
            progress.message("");
        }

        // Load the input group from the database if necessary.
        if (workItem.igroup_id!=prev_work.igroup_id) {
            if (!igroup.load(tx, workItem.igroup_id)) {
                progress.clear();
                std::cerr <<argv0 <<": input group " <<workItem.igroup_id <<" is empty or does not exist\n";
                exit(1);
            }
        }

        // Find the function to test
        IdFunctionMap::iterator func_found = functions.find(workItem.func_id);
        assert(func_found!=functions.end());
        SgAsmFunction *func = func_found->second;
        if (opt.verbosity>=LACONIC) {
            progress.clear();
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
            overmap_dynlink_addresses(interp, insns, opt.params.follow_calls, &ro_map, GOTPLT_VALUE,
                                      whitelist_imports, whitelist_exports/*out*/);
            if (opt.verbosity>=EFFUSIVE) {
                std::cerr <<argv0 <<": memory map for SgAsmInterpretation:\n";
                interp->get_map()->dump(std::cerr, argv0+":   ");
            }
        }

        // Run the test
        runOneTest(tx, workItem, pointers, func, function_ids, insn_coverage, dynamic_cg, tracer, consumed_inputs,
                   interp, whitelist_exports, cmd_id, igroup, funcinfo, insns, &ro_map, entry2id, ogroups);
        ++ntests_ran;

        // Check for user interrupts
        bool do_checkpoint=false, do_exit=false;
        if (SIGINT==interrupted) {
            progress.clear();
            std::cout <<argv0 <<": interrupted by user.\n";
            if (isatty(1)) {
                FILE *f = fopen("/dev/tty", "r");
                if (f!=NULL) {
                    if (opt.dry_run) {
                        std::cout <<argv0 <<":"
                                  <<" c=continue in dry-run mode;"
                                  <<" d=turn dry-run mode off;"
                                  <<" q=commit and quit;"
                                  <<" a=abort w/out commit\n"
                                  <<argv0 <<": your choice? [C/d/q/a] ";
                    } else {
                        std::cout <<argv0 <<":"
                                  <<" c=commit and conintue;"
                                  <<" q=commit and quit;"
                                  <<" a=abort w/out commit\n"
                                  <<argv0 <<": your choice? [C/q/a] ";
                    }
                    char *line=NULL;
                    size_t line_sz=0;
                    if (rose_getline(&line, &line_sz, f)>0) {
                        do_checkpoint = NULL!=strchr("cCdDqQ\r\n", line[0]);
                        do_exit = NULL==strchr("cC\r\n", line[0]);
                        if (opt.dry_run && strchr("dD", line[0]))
                            opt.dry_run = false;
                    }
                }
            }
            interrupted = -1;
        }

        // Checkpoint
        if (do_checkpoint || (opt.checkpoint>0 && time(NULL)-last_checkpoint > opt.checkpoint)) {
            if (!opt.dry_run)
                tx = checkpoint(tx, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs, &progress, ntests_ran, cmd_id);
            last_checkpoint = time(NULL);
        }
        if (do_exit) {
            tx->rollback();
            break;
        }

        prev_work = workItem;
    }

    // Store results for the analysis that tries to determine whether a function returns a value.
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
    if (!tx->is_terminated() && !opt.dry_run)
        tx = checkpoint(tx, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs, &progress, ntests_ran, cmd_id);
    progress.clear();

    return 0;
}
