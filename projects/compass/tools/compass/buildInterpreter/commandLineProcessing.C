/*
 * Author: Gary Yuan
 * Date: July 2008
 * File: roseQMGen/src/commandLineProcessing.C
 */

#include "defs.h"
#include "extern.h"
#include "stringManipulations.h"
#include "commandLineProcessing.h"

//using namespace boost::algorithm;

/*
 * Function: commandLineProcessing
 * Purpose:
 *   This function calls subroutines to perform string manipulations on
 *   the command line arguments such that they work with regression testing
 *   using QMTest. Such manipulations are:
 *   1. escaping quotations, i.e. on -DSOMETHING="lots of white space"
 *   2. inserting quotations, where needed some may be lost from cmd line
 *   3. setting the relative path to the regression root, i.e. replace 
 *      regression_root with uplevel of the string given by the realpath
 *   4. setting the real relative path, i.e. replacing regression_root with
 *      "./" relative path to the current working directory
 *
 *   These string manipulation sub routines are called via overloaded wrappers
 */
void commandLineProcessing()
{
  escapeQuotations( includes );
  escapeQuotations( libincludes );
  escapeQuotations( defines );
  escapeQuotations( objects );
  escapeQuotations( sources );
  escapeQuotations( unrecognized_arguments );

  insertQuotations( defines );

  setRelativePathToRegressionRoot( includes );
  setRelativePathToRegressionRoot( libincludes );
  setRelativePathToRegressionRoot( defines );
  setRelativePathToRegressionRoot( option_passing_arguments );
  setRelativePathToRegressionRoot( unrecognized_arguments );

  setRealRelativePath( output );
  setRealRelativePath( objects );

  setRealRelativePathToRegressionRoot( libraries );
  setRealRelativePathToRegressionRoot( headers );
  setRealRelativePathToRegressionRoot( sources );
  setRealRelativePathToRegressionRoot( relative_paths );

  return;
} //commandLineProcessing()
