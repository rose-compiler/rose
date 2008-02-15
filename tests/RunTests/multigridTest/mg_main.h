//     ***********  C++ (MG) HEADER CODE  ************

#define BOUNDS_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <A++.h>
#include <assert.h>

// *************************************************************************
//                  PREPROCESSOR  DEFINE  CONSTANTS
// *************************************************************************

// It is often easier to just make Boolean an integer!
// enum Boolean { FALSE , TRUE };

// Error if Array size is 0!
#define MIN_MG_LEVEL 0
// Limit for doublearray is 90 (on PC)!
#define MAX_MG_LEVEL 6

#define MAX_NUMBER_OF_MG_LEVELS     10
#define MAX_NUMBER_OF_GRIDS        100

#include "pde.h"
#include "mg1level.h"
#include "mg.h"

#ifdef GRAPHICS
#include <graphic_window.h>
#endif

// ****************************************************************************
//                             GLOBAL VARIABLES
// ****************************************************************************

#ifndef MAIN_PROGRAM

extern Index all;

#else

Index all;

#endif


