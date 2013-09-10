// Consults the database to get a list of specimen executables and their functions, then generates syntax vectors
// for each of them, populating the "vectors" table.

#include "rose.h"
#include "SqlDatabase.h"
#include "AST_FILE_IO.h"

#include "../semantic/CloneDetectionLib.h"
#include "createCloneDetectionVectorsBinary.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

using namespace boost::program_options;

static std::string argv0;

int
main(int argc, char* argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    std::string database;
    size_t stride = (size_t)(-1);
    size_t windowSize = (size_t)(-1);

    try {
	options_description desc("Allowed options");
	desc.add_options()
            ("help", "produce a help message")
            ("database", value< std::string >()->composing(), "the sqlite database that we are to use")
            ("stride", value< size_t>()->composing(), "stride to use" )
            ("windowSize", value< size_t >()->composing(), "sliding window size" )
            ;

	variables_map vm;
	store(command_line_parser(argc, argv).options(desc)
              .run(), vm);
        notify(vm);

	if (vm.count("help")) {
            std::cout << desc;           
            exit(0);
	}

	if (vm.count("database")!=1) {
            std::cerr << "usage: createVectorsBinary --database <database-name>\n";
            exit(1);
	}

	database = vm["database"].as<std::string >();
        if (vm.count("stride")==1)
            stride = vm["stride"].as<size_t>();
        if (vm.count("windowSize")==1)
            windowSize = vm["windowSize"].as<size_t>();
    } catch (std::exception& e) {
        std::cout << e.what() << "\n";
    }

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(database)->transaction();
    int64_t cmd_id = CloneDetection::start_command(tx, argc, argv, "creating syntax vectors");
    CloneDetection::FilesTable files(tx);

    // Save parameters in the database; or check against existing parameters
    if (0 == tx->statement("select count(*) from run_parameters")->execute_int()) {
        if ((size_t)-1 == stride || (size_t)-1 == windowSize) {
            std::cerr <<argv0 <<": stride and window size must be specified\n";
            exit(1);
        }
        tx->statement("insert into run_parameters (window_size, stride) values (?, ?)")
            ->bind(0, windowSize)->bind(1, stride)->execute();
    } else {
        SqlDatabase::Statement::iterator params = tx->statement("select window_size, stride from run_parameters")->begin();
        size_t oldWindowSize = params.get<size_t>(0);
        size_t oldStride = params.get<size_t>(1);
        if ((size_t)-1==stride)
            stride = oldStride;
        if ((size_t)-1==windowSize)
            windowSize = oldWindowSize;
        if (oldWindowSize != windowSize || oldStride != stride) {
            std::cerr <<argv0 <<": window size (" <<windowSize <<") and stride (" <<stride <<") do not match"
                      <<" existing window size (" <<oldWindowSize <<") and stride (" <<oldStride <<")\n";
            exit (1);
        }
    }

    CloneDetection::Progress progress(tx->statement("select count(*) from semantic_functions")->execute_int());

    // Process each specimen file that's in the database
    size_t nspecimens = 0, nfunctions = 0;
    SqlDatabase::StatementPtr stmt1 = tx->statement("select distinct specimen_id from semantic_functions");
    for (SqlDatabase::Statement::iterator specfiles=stmt1->begin(); specfiles!=stmt1->end(); ++specfiles, ++nspecimens) {
        
        // Obtain the AST for this specimen
        progress.clear();
        int specimen_id = specfiles.get<int>(0);
        std::string specimen_name = files.name(specimen_id);
        SgProject *project = files.load_ast(tx, specimen_id);
        if (!project)
            project = CloneDetection::open_specimen(tx, files, specimen_id, argv0);

        // Get list of functions for this specimen that are in the database
        std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(project);
        CloneDetection::IdFunctionMap functions = CloneDetection::existing_functions(tx, files, all_functions);

        // Create vectors for each of these functions
        std::cerr <<argv0 <<": generating syntax vectors for " <<specimen_name <<"\n";
        for (CloneDetection::IdFunctionMap::iterator fi=functions.begin(); fi!=functions.end(); ++fi, ++nfunctions, ++progress) {
            int func_id = fi->first;
            SgAsmFunction *func = fi->second;
            createVectorsForAllInstructions(func, specimen_name, func->get_name(), func_id, windowSize, stride, tx);
        }

        // Clean up the AST before we process the next file.  It does not currently work to call
        // SageInterface::deleteAST(project) because we get a failed assertion: idx < p_sections.size(). Therefore, we use the
        // method implemented for AST_FILE_IO. This is probably faster anyway. [Robb P. Matzke 2013-08-13]
        AST_FILE_IO::clearAllMemoryPools();
    }

    progress.clear();
    std::ostringstream mesg;
    mesg <<"added vectors for " <<nfunctions <<" function" <<(1==nfunctions?"":"s")
         <<" in " <<nspecimens <<" specimen" <<(1==nspecimens?"":"s");
    std::cerr <<argv0 <<": " <<mesg.str() <<"\n";

    CloneDetection::finish_command(tx, cmd_id, mesg.str());
    tx->commit();
    return 0;
} 
