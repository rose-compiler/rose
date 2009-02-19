/*
 * Author: Gary Yuan
 * Modified by Andreas Saebjoernsen 1/5/09
 * Date: July 2008
 * File: roseQMGen/src/qmgen.C
 */

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>

#include "qmgen.h"
#include "extern.h"
#include "commandLineProcessing.h"
#include "stringManipulations.h"

//AS: Header to support sqlite3x 
#include "sqlite3x.h"
#include <boost/lexical_cast.hpp>

using namespace boost::algorithm;

int output_parameter_position = 0;  //1 + the position of the output flag '-o'

/*
 * Function: constructMangledTestName
 * Arguments:
 * Returns: std::string, mangled name of .qmt test file name
 * Purpose: constructs the mangled name for the .qmt test file that is created
 *
 * The mangled name is of the form
 *
 * `date +%d%H%M%S%N`_DIR_OUTPUT.qmt
 *
 * Where:
 *   The prefix `date +%d%H%M%S%N` is the creation timestamp of fixed width
 *   that ensures lexigraphic order of all .qmt test files which is critically
 *   important for the execution order of these files during testing
 *
 *   DIR is the present working directory minus the regression_root path of
 *   the .qmt test file creation location
 *
 *   OUTPUT is the output flag option if given, otherwise it is the name of
 *   the executable
 */
std::string constructMangledTestName()
{
  std::string dir( pwd );
  replace_first( dir, regression_root, "" );

  std::string obj;

  if( output != "" ) obj.assign(output);
  else if( !objects.empty() ) obj.assign( *objects.begin() );
  else obj.assign( executable );

  if( output == "" && isCompileOnly && !sources.empty() )
  {
    output.assign( *sources.begin() );
    int e = output.find_last_of(".");
    int s = output.find_last_of("/");
    obj.assign( output.substr(s+1, e-s-1) + ".o" );
    output.assign( pwd + output.substr(s, e-s) + ".o" );
//    std::cout << output << std::endl;
//    std::cout << obj << std::endl;

    int tail = qmtest_arguments.rbegin()->first;

    qmtest_arguments.insert(
      std::make_pair(tail+1, std::make_pair("",new std::string("-o"))) );

    qmtest_arguments.insert(
      std::make_pair(tail+2, std::make_pair("",&output)) );

    output_parameter_position = tail+2;
  } //if( isCompileOnly && !sources.empty() )

  std::string testname;

  do
  {
    FILE *fdate = popen( "date +%d%H%M%S%N", "r" );
    char timestamp[256] = "\0";
    fread( timestamp, sizeof(char), 17, fdate );

    testname.append( timestamp );
    testname.append( "_" );

    testname.append( dir + "_" + obj );

    //replace the following illegal QMTest characters in the test name:
    //'.' -> '_' in the object name
    //'/' -> '_'
    //'.' -> '_dot_' in the path name
    //'+' -> '_plus_'
    replace_all( obj, ".", "_" ); 
    replace_all( testname, "/", "_" );
    replace_all( testname, ".", "_dot_" );
    replace_all( testname, "+", "_plus_" );
  
    //.qmt test file names must be all lower-case
    std::transform(testname.begin(), testname.end(), testname.begin(), tolower);
  
    testname.append( ".qmt" );
  } while( open( testname.c_str(), O_RDONLY ) != -1 && sleep(1) == 0 );
  //do...while, this loop ensures unique generated file names for a single
  //instance of this program. If the file name generated already exists then
  //the program sleeps 1 second and tries again with another time stamp.

  return testname;
} //constructMangledTestName()

/*
 * Function: get_argv_index
 * Arguments:
 *   std::string arg -- the original argument to search for in argv
 *   char **argv -- the original command line argument strings
 * Returns:
 *   integer -- the index where arg is located in argv
 * Purpose:
 *   Performs a serial search for the occurence of arg in argv and returns the
 *   index of the occurence or -1 if not found
 */
int get_argv_index( std::string arg, int argc, char **argv )
{
  int i;
  for( i = 1; i < argc; i++ )
  {
    if( arg == argv[i] && qmtest_arguments.find(i) == qmtest_arguments.end() )
    {
      return i;
    } //if(arg == argv[i] && qmtest_arguments.find(i) == qmtest_arguments.end())
  } //for

  return -1;
} //get_argv_index( std::string arg, int argc, char **argv )

/*
 * Function: initialize_qmtest_arguments_vector
 * Arguments:
 *   int argc -- the number of command line arguments taken from 
 *     initialize_qmtest_arguments 
 *   char **argv -- the command line argument strings taken from
 *     initialize_qmtest_arguments
 *   std::string prefix -- the stripped argument prefix taken away by boost
 *     program_options, ex. -I, -L, -D, "", etc.
 *   std::vector< std::string > &arguments -- the boost parsed command line
 *     arguments to insert into qmtest_arguments
 * Purpose:
 *   This function takes the boost program_options parsed arguments and
 *   inserts them into the primary data structure qmtest_arguments ordered
 *   by their index into the argv array.
 */
void initialize_qmtest_arguments_vector(
  int argc,
  char **argv,   
  std::string prefix,
  std::vector< std::string > &arguments
) {
  for( std::vector<std::string>::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )   {
    int pos = get_argv_index( (prefix+*itr), argc, argv );

    if( pos > 0 )
    {
      qmtest_arguments.insert(
        std::make_pair( pos, std::make_pair(prefix,&(*itr)) ) );
    } //if( pos > 0 ), only if valid index
  } //for

  return;
} //initialize_qmtest_arguments_vector()

/*
 * Function: initialize_qmtest_arguments
 * Arguments:
 *   int argc -- the number of command line arguments taken form main
 *   char **argv -- the command line argument strings taken from main
 * Purpose:
 *   This function fills the qmtest_arguments data structure with the
 *   1. test executable name
 *   2. all the arguments taken from the command line and parsed by boost
 *   3. the output flag argument (if applicable)
 *   4. the pwd include and link search path (if applicable)
 */
void initialize_qmtest_arguments( int argc, char **argv )
{
  //set the program name used in test
  if( executable == host_cc )
  {
    qmtest_arguments.insert(
      std::make_pair(1, std::make_pair("",&test_cc)) );
  } //if( executable == host_cc )
  else if( executable == host_cxx )
  {
    qmtest_arguments.insert(
      std::make_pair(1, std::make_pair("",&test_cxx)) );
  } //else if( executable == host_cxx )
  else
  {
    qmtest_arguments.insert(
      std::make_pair(1, std::make_pair("",&executable)) );
  } //else

  // Set all program options given on command line

  //option_passing_arguments, i.e. -Wl, -Wp, etc.
  {
    std::vector< std::string > 
      &original_options = option_passing_arguments.first;
    std::vector< std::vector< std::string > > 
      &split_options = option_passing_arguments.second;

    assert( original_options.size() == split_options.size() );

    int size = original_options.size();
    for( int i = 0; i < size; i++ )
    {
      int pos = get_argv_index(original_options.at(i), argc, argv);

      if( pos > 0 )
      {
        std::string option_prefix = joinOptions( 
          split_options.at(i).begin(),
          --split_options.at(i).end(), "," );

        qmtest_arguments.insert(
          std::make_pair(pos, std::make_pair(option_prefix, 
          &(*split_options.at(i).rbegin()))) );
      } //
    } //for i
  } //option_passing_arguments, i.e. -Wl, -Wp, etc.

  if( !includes.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "-I", includes);
  if( !libincludes.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "-L", libincludes);
  if( !defines.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "-D", defines);
  if( !objects.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", objects);
  if( !sources.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", sources);
  if( !headers.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", headers);
  if( !libraries.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", libraries);
  if( !relative_paths.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", relative_paths);
  if( !unrecognized_arguments.empty() )
    initialize_qmtest_arguments_vector(argc, argv, "", unrecognized_arguments);

  // Set the output flag option (if applicable)

  if( cmdLineMap.count("output") )
  {
    int pos = get_argv_index("-o", argc, argv);

    if( pos > 0 )
    {
      qmtest_arguments.insert(
        std::make_pair(pos, std::make_pair("",new std::string("-o"))) );
      qmtest_arguments.insert(
        std::make_pair(pos+1, std::make_pair("",&output)) );

      output_parameter_position = pos+1;
    } //if( pos > 0 )
  } //if( cmdLineMap.count("output") ), append the output flag option

  // Set the include and link paths (if applicable)

  /*int tail = qmtest_arguments.rbegin()->first;

  if( 0 && executable == host_cxx || executable == host_cc )
  {
    qmtest_arguments.insert(
      std::make_pair(tail+1, std::make_pair("-I",&dotdotpwd)) );

    qmtest_arguments.insert(
      std::make_pair(tail+2, std::make_pair("-L",&dotdotpwd)) );
  } //if( executable == test_cxx || executable == test_cc ) two additional
    //arguments for compiler executables to include the current directory
    //as an include and link search path */

  return;
} //initialize_qmtest_arguments( int argc, char **argv )

/*
 * Function: write_qmtest_raw
 * Purpose:
 *   This function calls system() to generate the .qmt test file based on the 
 *   constructed command line calling `qmtest create'. An example of such 
 *   an invocation is:
 *
 * qmtest create -o 16111633834635000__home_yuan5_rose_externalprojects_roseqmgen_timescale.qmt -a program=rose_regression_cc -a arguments="['-DG_DISABLE_DEPRECATED','-g','-O2','-Wall','-o','/home/yuan5/ROSE/externalProjects/roseQMGen/timescale','/home/yuan5/ROSE/externalProjects/roseQMGen/timescale.o','-pthread','/home/yuan5/ROSE/externalProjects/roseQMGen/.libs/libpixops.a','-L/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib','/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib/libgobject-2.0.so','/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib/libgmodule-2.0.so','-ldl','/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib/libgthread-2.0.so','-lpthread','-lrt','/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib/libglib-2.0.so','-lm','-Wl,--rpath','-Wl,/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib','-Wl,--rpath','-Wl,/home/yuan5/ren/ROSE_Regression/mozilla/glib-2.12.12-install/lib']" test rose.RoseTest
 *
 */
void write_qmtest_raw()
{
  std::stringstream ss;
  std::vector< std::string > arguments;

  if( output_parameter_position > 0 )
  {
    qmtest_arguments.find( 
      output_parameter_position )->second.second->assign(output);	
  }  //why is this necessary? somehow the output changes don't get reflected

  for( std::map<int,std::pair<std::string, std::string*> >::iterator
       itr = qmtest_arguments.begin();
       itr != qmtest_arguments.end(); itr++ )
  {
    arguments.push_back( itr->second.first + *itr->second.second );
  } //for

  ss << "qmtest create -o " << qmtest_name;

  if( open( "./rne", O_RDONLY ) != -1 )
  {
    ss << " -a expectation=\"XFAIL\"" << " " << envoptions
       << " -a program=" << *arguments.begin() << " -a arguments=\"[";
  } //if( open( "./rne", O_RDONLY ) != -1 )
  else
  {
    ss << " -a program=" << *arguments.begin() << " " << envoptions
       << " -a arguments=\"[";
  } //else

  for( unsigned int i = 1; i < arguments.size()-1; i++ )
    ss << "'" << arguments.at(i) << "',";

  ss << "'" << arguments.at(arguments.size()-1) << "']\""
     << " test rose.RoseTest";
  std::cout << std::endl;

  std::cout << std::endl;
  for( unsigned int i = 0; i < arguments.size()-1; i++ )
    std::cout << "'" << arguments.at(i) << "',";
  std::cout << std::endl;

  std::cout << std::endl;

  std::cerr << ss.str() << std::endl;
//  system( ss.str().c_str() );

  return;
} //write_qmtest_raw()


/*
 * Function: write_rose_db
 * Purpose:
 *   This function creates a database of command line options based on the 
 *   constructed command line. An example of such 
 */
void write_rose_db()
{
  if(db_name.size() == 0)
  {
    std::cerr << "Error: Please set the database name.\n";
    abort();
  }
  std::stringstream ss;
  std::vector< std::string > arguments;

  if( output_parameter_position > 0 )
  {
    qmtest_arguments.find( 
      output_parameter_position )->second.second->assign(output);	
  }  //why is this necessary? somehow the output changes don't get reflected




  sqlite3x::sqlite3_connection con(db_name.c_str());

  //con.executenonquery("create table IF NOT EXISTS clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, index_within_function INTEGER, vectors_row INTEGER, dist INTEGER)")
  try {
    con.executenonquery("create table IF NOT EXISTS build_interpreted( row_number INTEGER PRIMARY KEY, compile_line TEXT)");

  }
  catch(std::exception &ex) {
    std::cerr << "Exception Occurred: " << ex.what() << std::endl;
  }

  try {
    con.executenonquery("create table IF NOT EXISTS build_interpreted_split( row_number INTEGER PRIMARY KEY, build_index INTEGER, first TEXT, second TEXT)");
  }
  catch(std::exception &ex) {
    std::cerr << "Exception Occurred: " << ex.what() << std::endl;
  }

  try {
    con.executenonquery("create table IF NOT EXISTS file_last_modified( row_number INTEGER PRIMARY KEY, filename TEXT, last_modified TEXT)");
  }
  catch(std::exception &ex) {
    std::cerr << "Exception Occurred: " << ex.what() << std::endl;
  }

  std::string db_select_n = "";

  std::string argumentString;

  std::vector<char*> argv;

  for( std::map<int,std::pair<std::string, std::string*> >::iterator
       itr = qmtest_arguments.begin();
       itr != qmtest_arguments.end(); itr++ )
  {
    argumentString+= ( itr->second.first + *itr->second.second +" " );
    argv.push_back(strdup(argumentString.c_str()));
  } //for


  { // Insert the string version of each commandline

    sqlite3x::sqlite3_command cmd(con, "INSERT INTO build_interpreted( compile_line ) VALUES(\""+argumentString+"\")"  );
    cmd.executenonquery();
  }

  { // Insert the argv version of the command line
    int build_index = con.executeint("SELECT row_number from build_interpreted where compile_line=\""+argumentString+"\"");

    std::string insert_split = "INSERT INTO build_interpreted_split(build_index, first,second ) VALUES(?,?,?)";
    for( std::map<int,std::pair<std::string, std::string*> >::iterator
        itr = qmtest_arguments.begin();
        itr != qmtest_arguments.end(); itr++ )
    {

      sqlite3x::sqlite3_command cmd(con, insert_split);
      cmd.bind(1,build_index);
      cmd.bind(2, itr->second.first);
      cmd.bind(3, *itr->second.second);
      cmd.executenonquery();

    }


  }


  return;
} //write_rose_db()

