// Support for timing
#include <iostream>
#include <time.h>

using namespace std;

#ifndef __APPLE__
// Moderate resolution timer code (from the web)
timespec diff(timespec start, timespec end);
#endif
