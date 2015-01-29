/*
 * PolyoptModule.cpp: This file is part of the PolyOpt project.
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
 * @file: PolyoptModule.cpp
 * @author: Vincent Cave <vc8@rice.edu>
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

#include <polyopt/PolyoptModule.hpp>
#include <polyopt/PolyOpt.hpp>

using namespace std;

void PolyoptModule::setVerbose(bool v) {
	polyoptions.setQuiet(!v);
}

int PolyoptModule::visit(SgProject * project) {
  return PolyOptOptimizeProject(project, polyoptions);
}

void PolyoptModule::handleModuleOptions(Rose_STL_Container<string> &argStrings) {

	  Rose_STL_Container<std::string> argStringsCopy = argStrings;

	  // 1- Read PoCC options.
	  // Create argv, argc from string argument list, by removing rose/edg
	  // options.
	  SgFile::stripRoseCommandLineOptions (argStringsCopy);
	  SgFile::stripEdgCommandLineOptions (argStringsCopy);
	  int newargc = argStringsCopy.size ();
	  char* newargv[newargc];
	  int i = 0;
	  SgStringList::iterator argIter;
	  for (argIter = argStringsCopy.begin (); argIter != argStringsCopy.end (); ++argIter)
	    newargv[i++] = strdup ((*argIter).c_str ());

	  // 2- Parse PoCC options.
	  PolyRoseOptions polyoptions_ (newargc, newargv);

	  polyoptions = polyoptions_;

	  // 3- Remove PoCC options from arg list.
	  cleanCommandLine(argStrings);
}

void PolyoptModule::cleanCommandLine(Rose_STL_Container<string> &args) {
	CommandlineProcessing::removeArgs (args, "--polyopt");
}
