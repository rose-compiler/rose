/*
 * Author: Gary yuan
 * Date: July 2008
 * File: roseQMGen/src/commandLinePreProcessing.h
 */

#include "commandLinePreProcessing.h"

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/algorithm/string.hpp>

#include "extern.h"
#include "commandLineProcessing.h"

using namespace boost::algorithm;

/*
 * Function: commandLinePreprocessor
 * Arguments:
 *   const std::string &s -- an original command line argument passed by boost
 * Returns:
 *   std::pair< std::string, std::string > -- a pair representing the option
 *     classification in boost and the stripped option itself
 * Purpose:
 *   This is a special function that is run by boost program_options prior
 *   to parsing the command line arguments. Here the user may specify special
 *   behavior for arguments that match certain patterns
 */
std::pair< std::string, std::string >
commandLinePreprocessor( const std::string &s )
{
  if( s.find("-Wl") == 0 || s.find("-Wp") == 0 ) 
    return std::make_pair( "passing_option", s );

  if( s.find("-c") != std::string::npos )
    isCompileOnly = true;

  if( s.find("-") != 0 ) 
  {
    if( s.find(".o") != std::string::npos )
      return std::make_pair( "object", s );
  
    if( s.find(".h") != std::string::npos )
      return std::make_pair( "header", s );
  
    if( s.find(".c") != std::string::npos ||
        s.find(".C") != std::string::npos ||
        s.find(".f") != std::string::npos ||
        s.find(".F") != std::string::npos ) 
    {
      return std::make_pair( "source", s );
    } //if, is source file
  
    if( s.find(".so") != std::string::npos ||
        s.find(".lo") != std::string::npos ||
        s.find(".a") != std::string::npos ||
        s.find(".s") != std::string::npos ||
        s.find(".S") != std::string::npos ||
        s.find(".pp") != std::string::npos ||
        s.find(".gz") != std::string::npos )
    {
      return std::make_pair( "library", s );
    } //if, is positional library
  
    if( s.find(".") == 0 )
      return std::make_pair( "relative_path", s );
  } //if( s.find("-") != 0 ), is not an option

  return std::make_pair( std::string(), std::string() );
} //commandLinePreprocessor( const std::string &s )

/*
 * Function: commandLinePreprocessing
 * Purpose:
 *   This function performs tasks on the command line arguments and data
 *   structures that are necessary before going onto commandLineProcessing.
 *   Currently, this function performs the split of option passing arguments
 *   such as -Wl, -Wp, etc. and fills the data structure 
 *   option_passing_arguments
 */
void commandLinePreprocessing()
{
  std::vector< std::string > &original_options = option_passing_arguments.first;
  std::vector< std::vector< std::string > > &split_options = option_passing_arguments.second;

  for( std::vector< std::string >::iterator itr = original_options.begin();
       itr != original_options.end(); itr++ )
  {
    split_options.push_back( std::vector< std::string >() );
    split( *split_options.rbegin(), *itr, is_any_of(",") );
  } //for, itr

  assert( original_options.size() == split_options.size() );

  return;
} //commandLinePreprocessing()
