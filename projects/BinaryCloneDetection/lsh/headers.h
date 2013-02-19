/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */
#include <string>
#include <iostream>
#include <exception>
#include <stdint.h>

#include "sqlite3x.hpp"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/random.hpp>
#include <boost/smart_ptr.hpp>

#include "vectorCompression.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "BasicDefinitions.h"
#include "Random.h"
#include "Geometry.h"
#include "Util.h"
#include "BucketHashing.h"
#include "LocalitySensitiveHashing.h"
#include "SelfTuning.h"
#include "NearNeighbors.h"

#ifdef DEBUG_MEM
#include <malloc.h>
#endif

#ifdef DEBUG_TIMINGS
#include <sys/time.h>
#endif

#include "GlobalVars.h"

extern int callIsDistSqrLen;
extern int callDistance;
extern std::map<void*,int> howManyTimes;

