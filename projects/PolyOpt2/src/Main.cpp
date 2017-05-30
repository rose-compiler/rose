/*
 * Main.cpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: Main.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

/**
 * \file Program entry point.
 *
 * This file contains the program entry-point and other supporting
 * functionality.
 */

/// LNP: FIXME: Find the option in autoconf to do that!
#define POLYROSE_PACKAGE_BUGREPORT PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#define POLYROSE_PACKAGE_STRING PACKAGE_STRING
#undef PACKAGE_STRING
#define POLYROSE_PACKAGE_TARNAME PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#define POLYROSE_PACKAGE_NAME PACKAGE_NAME
#undef PACKAGE_NAME
#define POLYROSE_PACKAGE_VERSION PACKAGE_VERSION
#undef PACKAGE_VERSION
#include <rose.h>

#include <polyopt/PolyOpt.hpp>

/**
 * \brief Program entry point.
 *
 * This is the point of entry for the plutorose binary.
 *
 * \param argc    The number of arguments on the command-line.
 * \param argv    The command-line arguments.
 * \return        The program result code.
 */
int main (int argc, char* argv[])
{
  // 1- Read PoCC options.
  // Create argv, argc from string argument list, by removing rose/edg
  // options.
  SgStringList argStrings =
    CommandlineProcessing::generateArgListFromArgcArgv (argc, argv);
  SgFile::stripRoseCommandLineOptions (argStrings);
  SgFile::stripEdgCommandLineOptions (argStrings);
  int newargc = argStrings.size ();
  char* newargv[newargc];
  int i = 0;
  SgStringList::iterator argIter;
  for (argIter = argStrings.begin (); argIter != argStrings.end (); ++argIter)
    newargv[i++] = strdup ((*argIter).c_str ());

  // 2- Parse PoCC options.
  PolyRoseOptions polyoptions (newargc, newargv);

  // 3- Remove PoCC options from arg list.
  SgStringList args =
    CommandlineProcessing::generateArgListFromArgcArgv (argc, argv);
  CommandlineProcessing::removeArgs (args, "--polyopt-");

  // 4- Invoke the Rose parser.
  SgProject* project = frontend (args);

  // 5- Invoke the PolyRose processor.
  int retval;
  if (polyoptions.getAnnotateOnly())
    {
      if (polyoptions.getAnnotateInnerLoops())
	retval = PolyOptInnerLoopsAnnotateProject (project, polyoptions);
      else
	retval = PolyOptAnnotateProject (project, polyoptions);
    }
  else
    retval = PolyOptOptimizeProject (project, polyoptions);
  if (retval == EXIT_SUCCESS)
    // 6- If the optimization process succeeded, invoke the Rose unparser.
    return backend (project);
  else
    // Otherwise, return an error code to the console.
    return EXIT_FAILURE;
}
