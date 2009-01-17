/*
 * Author: Gary Yuan
 * Modified by Andreas Saebjoernsen 1/5/09
 * Date: July 2008
 * File: roseQMGen/src/qmgen.C
 */

/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library

    http://www.boost.org/

    Copyright (c) 2001-2007 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/


#define BOOST_WAVE_SERIALIZATION        0             // enable serialization
#define BOOST_WAVE_BINARY_SERIALIZATION 0             // use binary archives
#define BOOST_WAVE_XML_SERIALIZATION    1             // use XML archives

///////////////////////////////////////////////////////////////////////////////
//  include often used files from the stdlib
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>

#include "rose.h"

///////////////////////////////////////////////////////////////////////////////
//  include boost config
#include <boost/config.hpp>     //  global configuration information


///////////////////////////////////////////////////////////////////////////////
//  include required boost libraries
#include <boost/assert.hpp>
#include <boost/pool/pool_alloc.hpp>

///////////////////////////////////////////////////////////////////////////////
// Include additional Boost libraries
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/timer.hpp>
#include <boost/any.hpp>
#include <boost/program_options.hpp>

#include <stdlib.h>
#include <assert.h>

#include "defs.h"
#include "commandLinePreProcessing.h"
#include "commandLineProcessing.h"
#include "stringManipulations.h"
#include "qmgen.h"
#include "rose_config.h"
using namespace boost;
using namespace boost::program_options;

////////////////////////////////////////////////////////////////////////////////
//OH-NO GLOBALS!! AND THEY'RE ALL EXTERN'D!!!
////////////////////////////////////////////////////////////////////////////////
/*
 * This section describes the data structures used throughout this program
 * to store and manipulate information about the environment and command line
 * arguments. Each is described individually below:
 *
 * includes -- include paths, ex. -I/my/include/path
 * defines -- definitions, ex. -DDEBUG=1
 * libincludes -- linking paths, ex. -L/my/lib/search/path
 * objects -- positional files with extension *.o
 * headers -- positional files with extension *.h
 * sources -- positional files with extension *.[Cc]* *.[Ff]*
 * libraries -- positional linked libraries or other files with extension 
 *   *.so, *.lo, *.a, *.s, *.S, *.pp, *.gz
 * relative_paths -- positional arguments beginning with '.' indicating a 
 *   relative path
 * unrecognized_arguments -- all other arguments not recognized by boost
 *   program_options
 *
 * qmtest_arguments -- the primary data structure that contains the command
 *   line argument information for creating a .qmt test file. This data
 *   structure is a map of integer key and std::pair<std::string, std::string*>
 *   data. The integer key is simply the array index into the argv array which
 *   the argument occurs. The data is a pair of type std::string (first) and
 *   std::string* (second). The first string is an argument prefix stripped by
 *   boost program_options; like, "-I", "-L", "-D", "", etc. The second
 *   string* is a pointer refering to the location of a string in another
 *   data structure such as includes, defines, headers, sources, etc. that go
 *   go through command line (pre)processing. The concatenation of the first
 *   and second members of the pair construct the entire command line argument
 *   given to qmtest create
 *
 * option_passing_arguments -- command line arguments that pass options to
 *   other components such as the preprocessor or linker; e.g. -Wl, -Wp,
 *   Please refer to defs.h for a full explanation of the data structure
 *
 * qmtest_name -- the mangled string name of the .qmt test file
 * executable -- the name of the program which this program wraps
 * shell -- the environment shell, BASH
 * output -- the option of the '-o' flag if exists
 * regression_root -- the absolute path value of ROSE_TEST_REGRESSION_ROOT
 * uplevel -- the string representing a relative path up to the root level of
 *   testing at regression_root; usually, ../../ or 2 levels up
 * test_cc -- the C compiler to be used for testing in .qmt files
 * test_cxx -- the C++ compiler to be used for testing in .qmt files
 * host_cc -- the C compiler used when generating .qmt files, usually gcc
 * host_cxx -- the C++ compiler used when generating .qmt files, usually g++
 * pwd -- the present working directory
 * isCompileOny -- boolean == true if '-c' option used on command line
 * cmdnLineMap -- data structure used in boost program_options to hold command
 *   line arguments
 * confMap -- data structure used in boost program_options to hold 
 *   configuration file options
 */

std::vector< std::string > includes;
std::vector< std::string > defines;
std::vector< std::string > libincludes;
std::vector< std::string > objects;
std::vector< std::string > headers;
std::vector< std::string > sources;
std::vector< std::string > libraries;
std::vector< std::string > relative_paths;
std::vector< std::string > unrecognized_arguments;
std::map<int, std::pair<std::string, std::string*> > qmtest_arguments;
pair_vector_options option_passing_arguments;
std::string qmtest_name;
std::string executable;
std::string shell;
std::string output;
std::string regression_root;
std::string uplevel;
std::string test_cc;
std::string test_cxx;
std::string host_cc;
std::string host_cxx;
std::string db_name;
std::string pwd;
std::string dotpwd;
std::string dotdotpwd;
std::string envoptions;
bool isCompileOnly;
boost::program_options::variables_map cmdLineMap;
boost::program_options::variables_map confMap;

////////////////////////////////////////////////////////////////////////////////

/*
 * Function: initialize_configuration_file_options
 * Arguments:
 * Purpose: parse the configuration file options for this program using boost
 *   program_options. The options currently supported are: (see sample.rqmgc)
 *
 *   uplevel, test_cc, test_cxx, host_cc, host_cxx
 *
 * The config file must be located in the path given by regression_root and
 * be name .rqmgc
 */
void initialize_configuration_file_options()
{
//  std::ifstream conf((std::string(PREFIX)+"/rqmgc").c_str());
  std::cout << ROSE_COMPILE_TREE_PATH"/projects/compass/tools/compass/buildInterpreter/rqmgc" << std::endl;
  //std::ifstream conf(RCFILE);
  std::ifstream conf(ROSE_COMPILE_TREE_PATH"/projects/compass/tools/compass/buildInterpreter/rqmgc");

  assert( conf.good() == true );

  try
  {
    options_description config("Configuration");
    config.add_options()
      ("uplevel", value<std::string>(&uplevel)->default_value("./"),
       "level up to regression root")
      ("testcc", value<std::string>(&test_cc)->default_value("gcc"),
       "testing C compiler")
      ("testcxx", value<std::string>(&test_cxx)->default_value("g++"),
       "testing C++ compiler")
      ("hostcc", value<std::string>(&host_cc)->default_value("gcc"),
       "host C compiler")
      ("hostcxx", value<std::string>(&host_cxx)->default_value("g++"),
       "host C++ compiler")
      ("dbname", value<std::string>(&db_name)->default_value("buildInterpreterDefault.db"),
       "output database name")
      ;


    store(parse_config_file(conf,config),confMap);
    notify(confMap);

  } //try
  catch( std::exception &e )
  {
  }

  conf.close();

  return;
} //initialize_configuration_file_options()

/*
 * Function: initialize_command_line_options
 * Arguments:
 *   int argc -- the number of command line arguments taken from 
 *     initialize_options()
 *   char **argv -- the command line argument strings taken from
 *     initialize_options()
 * Purpose:
 *   This function uses boost program_options to parse the command line 
 *   arguments into data structures declared globally, such as defines, 
 *   includes, ..., unrecognized_arguments.
 */
void initialize_command_line_options( int argc, char **argv )
{
  try
  {
    options_description desc( "Allowed options" );
    desc.add_options()
//      ("help", "TODO: HELP MESSAGE" )
//      ("compile,c", "Compile or assemble the source files, but do not link.")
      ("executable", value<std::string>(&executable))
      ("output,o", value<std::string>(&output)->default_value(""),
       " Write output to file.  This is the same as specifying file as the second non-option argument to cpp.")
      ("object", value<std::vector<std::string> >(&objects)->composing(),
       "An object file.")
      ("header", value<std::vector<std::string> >(&headers)->composing(),
       "A header file.")
      ("source", value<std::vector<std::string> >(&sources)->composing(),
       "A source file.")
      ("library", value<std::vector<std::string> >(&libraries)->composing(),
       "A dynamically linked library file.")
      ("include,I", value<std::vector<std::string> >(&includes)->composing(),
       "Add the directory dir to the list of directories to be searched for header files.  Directories named by -I are searched before the standard system include directories.  If the directory dir is a standard system include directory, the option is ignored to ensure that the default search order for system directories and the special treatment of system headers are not defeated." )
      ("link,L", value<std::vector<std::string> >(&libincludes)->composing(),
       "This option specifies where to find the executables, libraries, include files, and data files of the compiler itself." )
      ("passing_option", value<std::vector<std::string> >(&option_passing_arguments.first)->composing(), "")
      ("relative_path", value<std::vector<std::string> >(&relative_paths)->composing(), "")
      ("define,D", value<std::vector<std::string> >(&defines)->composing(),
       "The contents of definition are tokenized and processed as if they appeared during translation phase three in a #define directive." )
    ;

    basic_parsed_options<char> bpo = command_line_parser(argc,argv).options(desc).allow_unregistered().run();

    //The command line preprocessing function commandLinePreprocessor is run
    store(command_line_parser(argc,argv).options(desc).extra_parser(commandLinePreprocessor).allow_unregistered().run(),cmdLineMap);

    unrecognized_arguments = collect_unrecognized( bpo.options, include_positional );

    notify(cmdLineMap);
  } //try
  catch( std::exception &e )
  {
    exit(1);
  } //catch( exception &e )

  return;
} //initialize_command_line_options()

/*
 * Function: initialize_options
 * Arguments:
 *   int argc -- the number of command line arguments taken from main
 *   char **argv -- the command line arguments taken from main
 * Purpose:
 *   This function parses the command line options using boost program_options.
 *   Additionally, the internal options of this program itself are parsed
 *   using the configuration file .rqmgc; please refer to sample.rqmgc for an
 *   example of these options.
 *
 *   The actual command line arguments are then assigned to the global argument
 *   data structure in initialize_qmtest_arguments().
 */
void initialize_options( int argc, char **argv )
{
  char **my_argv = &argv[1];  //skip the zeroth argument that is this program
  int my_argc = argc-1;

  executable.assign(argv[1]);  //the program name actually being wrapped

  //read this program's options through the configuration file
  initialize_configuration_file_options();  

  //initialize the command line arguments using boost program_options
  initialize_command_line_options(my_argc, my_argv);

  commandLinePreprocessing();

  //set all boost processed options in their original indices in argv
  initialize_qmtest_arguments(argc,argv);

  return;
} //initialize_options( int argc, char **argv )

/*
 * Function: init
 * Arguments:
 * Purpose:
 *   This function initializes the variables holding values taken from the
 *   system environment. In particular we record the working SHELL, e.g. bash,
 *   the present working directory, and the value for ROSE_TEST_REGRESSION_ROOT.
 */
void init()
{
  isCompileOnly = false;


  if( !getenv("SHELL") ) 
  {
    std::cerr << "Error: Environment variable $SHELL not defined\n";
    abort();

  }

  shell.assign( getenv("SHELL") );

  //The ROSE_TEST_REGRESSION_ROOT environment variable must be set. This should
  //be done as part of setting up your testing package.

  if( !getenv("ROSE_TEST_REGRESSION_ROOT") ) 
  {
    std::cerr << "Error: Environment variable $ROSE_TEST_REGRESSION_ROOT not defined\n";
    abort();

  }

  regression_root.assign( getenv("ROSE_TEST_REGRESSION_ROOT") );

  pwd.assign( getenv("PWD") );

  dotpwd.assign(pwd); setRealRelativePath( dotpwd );
  dotdotpwd.assign(pwd); setRelativePathToRegressionRoot( dotdotpwd );

  char *qmsh_flags = getenv("QMSHFLAGS");

  if( qmsh_flags != NULL )
    envoptions.assign( qmsh_flags );
  else
    envoptions = ""; 

  return;
} //init()

/*
 * Function: main
 * Arguments:
 *   int argc -- number of command line arguments
 *   char **argv -- array of command line argument strings
 * Purpose:
 *   Program entry point, performs the steps to generate a .qmt test file in
 *   this order:
 *     1. initialize the environment variables
 *     2. initialize the command line arguments
 *     3. perform processing on the command line arguments
 *     4. write the .qmt test file
 *     5. echo to stdout the name of the newly created .qmt test file
 */
int main( int argc, char **argv )
{
  if( argc < 2 )
  {
    std::cerr << argv[0] << " must be called with at least one argument!\n";
    exit(1);
  } //if( argc < 2 ), the executable, e.g. "gcc" should always be an argument

  init();
  initialize_options( argc, argv );

  //qmtest_name = constructMangledTestName();

  commandLineProcessing();

  write_rose_db();




  //Execute the backend using the ROSE frontend magic

  std::vector<char*> argvVec(&argv[0],&argv[0]+argc);

  argvVec.push_back("-rose:skip_rose");
  std::cout << std::endl;

  for(int i =0 ; i < argvVec.size(); i++ )
  {
    std::cout << argvVec[i] << " ";

  }
  std::cout << std::endl;

  SgProject* project = frontend(argvVec.size(), &argvVec[0]);


  return backend(project);
} //main()
