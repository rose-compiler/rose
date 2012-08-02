/*
 * PolyoptModule.hpp: This file is part of the PolyOpt project.
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
 * @file: PolyoptModule.hpp
 * @author: Vincent Cave <vc8@rice.edu>
 */

#ifndef POLYROSE_POLYROSEMODULE_HPP
#define POLYROSE_POLYROSEMODULE_HPP

#include <rose.h>
#include <polyopt/PolyRoseOptions.hpp>
#include <stdio.h>

class PolyoptModule {

private:
	PolyRoseOptions polyoptions;

public:

	PolyoptModule(Rose_STL_Container<std::string> &args) {
		handleModuleOptions(args);
	}

	int visit(SgProject * project);

	void handleModuleOptions(Rose_STL_Container<std::string> &args);

	void cleanCommandLine(Rose_STL_Container<std::string> &args);

	void setVerbose(bool);
};


#endif
