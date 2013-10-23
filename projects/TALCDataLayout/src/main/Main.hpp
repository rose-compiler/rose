/*
 * Main.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <iostream>

std::ostream* tracePtr;

#include "Common.hpp"
#include "Parser.hpp"
#include "AOSModule.hpp"
#include "AOSModuleCheck.hpp"
#include "HaOptModule.hpp"

#include "rose.h"

//#define TRACE 1
//static std::ostream* tracePtr = NULL;
static std::ofstream* nullPtr = NULL;

static bool verbose;
Meta *meta;

#endif /* MAIN_HPP_ */
