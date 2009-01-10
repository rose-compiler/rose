/*
 * Author: Gary Yuan
 * Date: July 2008
 * File: roseQMGen/src/stringManipulations.C
 */

#include <libgen.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/algorithm/string.hpp>

#include "defs.h"
#include "extern.h"
#include "stringManipulations.h"

using namespace boost::algorithm;

/*
 * Function: joinOptions
 * Arguments:
 *   std::vector< std::string >::iterator b -- begin iterator
 *   std::vector< std::string >::iterator e -- end iterator
 *   const char *jc -- join character, ex. ","
 * Returns:
 *   std::string, the concatenation of the strings in vector and jc character
 * Purpose:
 *   This function joins the strings in a vector with the joining character
 *
 * This function is used with the pair_vector_options data-type in defs.h
 */
std::string joinOptions(
  std::vector< std::string >::iterator b,
  std::vector< std::string >::iterator e,
  const char *jc )
{
  std::string r("");
  for( std::vector< std::string >::iterator itr = b; itr != e; itr++ )
  {
    r.append(*itr);
    r.append(jc);
  } //for, itr

  return r;
} //joinOptions()



/*
 * Function: escapeQuotations, wrapper for vector of strings
 */
void escapeQuotations( std::vector< std::string > &arguments )
{
  for( std::vector< std::string >::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    escapeQuotations( *itr );
  } //for
} //escapeQuotations( std::vector< std::string > &arguments )



/*
 * Function: escapeQuotations
 * Arguments:
 *   std::string &argument -- escape the quotations in argument
 * Returns:
 *   std::string &argument -- escaped quotations in argument
 * Purpose:
 *   This function escapes all instances of single and double quotes
 *   in the string given by argument with the backslash character
 */
void escapeQuotations( std::string &argument )
{
  replace_all( argument, "\"", "\\\"" );
  replace_all( argument, "'", "\\'" );

  return;
} //escapeQuotations( std::string &argument )



/*
 * Function: insertQuotations, wrapper for vector of strings
 */
void insertQuotations( std::vector< std::string > &arguments )
{
  for( std::vector< std::string >::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    insertQuotations( *itr );
  } //for
} //insertQuotations( std::vector< std::string > &arguments )



/*
 * Function: insertQuotations
 * Arguments:
 *   std::string &argument -- insert double quotes into argument
 * Returns:
 *   std::string &argument -- inserted double quotes into argument
 * Purpose:
 *   This function inserts double quotes where needed of a command
 *   line argument, usually for defines -D
 */
void insertQuotations( std::string &argument )
{
  if( argument.find("\"") == std::string::npos &&
      argument.find(" ") != std::string::npos &&
      argument.find("=") != std::string::npos )
  {
    replace_first( argument, "=", "=\\\"" );
    argument.assign( argument + "\\\"" );
  } //if, argument does not contain quotations, contains spaces and "="
} //insertQuotations( std::string &argument )



/*
 * Function: setRelativePathToRegressionRoot, wrapper for pair_vector_options
 */
void setRelativePathToRegressionRoot( pair_vector_options & options )
{
  std::vector< std::vector< std::string > > &split_options = options.second;

  std::vector< std::vector<std::string> >::iterator itr = split_options.begin();
  for( ; itr != split_options.end(); itr++ )
  {
    setRelativePathToRegressionRoot( *itr->rbegin() );
  } //for, itr

  return;
} //setRelativePathToRegressionRoot( pair_vector_options & options )



/*
 * Function: setRelativePathToRegressionRoot, wrapper for vector of strings
 */
void setRelativePathToRegressionRoot( std::vector< std::string > &arguments )
{
  for( std::vector< std::string >::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    setRelativePathToRegressionRoot( *itr );
  } //for, itr
} //setRelativePathToRegressionRoot( std::vector< std::string > &arguments )



/*
 * Function: adjustForRealPath
 * Arguments:
 *   std::string &argument -- input absolute or relative path argument
 * Returns:
 *   std::string &argument -- resolved absolute realpath of argument
 * Purpose:
 *   This function resolves the absolute realpath of an argument
 */
void adjustForRealPath( std::string &argument )
{
  char path_max_buffer[4096] = "\0";
  std::string dir( dirname(strdup(argument.c_str())) );
  std::string base( basename(strdup(argument.c_str())) );

  realpath( dir.c_str(), path_max_buffer );
  dir.assign( path_max_buffer );

  assert( dir.find_first_of(".") != 0 );
  replace_first( dir, regression_root, uplevel );

  argument.assign( dir + "/" + base );

  return;
} //adjustForRealPath( std::string &argument )



/*
 * Function: setRelativePathToRegressionRoot
 * Arguments:
 *   std::string &argument -- absolute or relative path argument
 * Returns:
 *   std::string &argument -- resolved path of argument using uplevel
 * Purpose:
 *   This function replaces any instance of regression_root with
 *   uplevel in the resolved real path of an absolute path argument
 */
void setRelativePathToRegressionRoot( std::string &argument )
{
  if( open(argument.c_str(), O_RDONLY) != -1 || argument.find(".") == 0 )
    adjustForRealPath( argument );

  replace_first( argument, regression_root, uplevel );
//  replace_first( argument, "/home/yuan5/", uplevel );

  return;
} //setRelativePathToRegressionRoot( std::string &argument )



/*
 * Function: setRealRelativePath, wrapper for vector of strings 
 */
void setRealRelativePath( std::vector< std::string > &arguments )
{
  for( std::vector< std::string >::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    setRealRelativePath( *itr );
  } //for

  return;
} //setRealRelativePath( std::vector< std::string > &arguments )



/*
 * Function: setRealRelativePath
 * Arguments:
 *   std::string &argument -- absolute or relative path argument
 * Returns:
 *   std::string &argument -- resolved path of argument using "./"
 * Purpose:
 *   This function replaces any instance of regression_root with
 *   "./" in the resolved real path of an absolute path argument.
 */
void setRealRelativePath( std::string &argument )
{
  char path_max_buffer[4096] = "\0";

  realpath( argument.c_str(), path_max_buffer );

  argument.assign( path_max_buffer );

  replace_first( argument, regression_root, "./" );

//  replace_first( argument, "/home/yuan5/ren/roseQMGen", "./" );
} //void setRealRelativePath( std::string &argument )



/*
 * Function: setRealRelativePathToRegressionRoot, wrapper for vector of strings
 */
void setRealRelativePathToRegressionRoot( std::vector<std::string> &arguments )
{
  for( std::vector< std::string >::iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    setRealRelativePathToRegressionRoot( *itr );
  } //for, itr

  return;
} //setRealRelativePathToRegressionRoot( std::vector<std::string> &arguments )



/*
 * Function: setRealRelativePathToRegressionRoot
 * Arguments:
 *   std::string &argument -- absolute or relative path argument
 * Returns:
 *   std::string &argument -- resolved realpath of argument using uplevel
 * Purpose:
 *   This function unconditionally, resolves the realpath of an absolute or
 *   relative path and replaces any instance of regression_root with uplevel.
 */
void setRealRelativePathToRegressionRoot( std::string &argument )
{
  adjustForRealPath( argument );

  replace_first( argument, regression_root, uplevel );
} //setRealRelativePathToRegressionRoot( std::string &argument )
