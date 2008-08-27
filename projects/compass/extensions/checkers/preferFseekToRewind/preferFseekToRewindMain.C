//
// Do not modify this file
//

#include "compass.h"
extern const Compass::Checker* const preferFseekToRewindChecker;
extern const Compass::Checker* const myChecker = preferFseekToRewindChecker;

#include "compass.C"
#include "compassTestMain.C"
