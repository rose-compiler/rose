//
// Do not modify this file
//

#include "compass.h"
extern const Compass::Checker* const noSecondTermSideEffectsChecker;
extern const Compass::Checker* const myChecker = noSecondTermSideEffectsChecker;

#include "compass.C"
#include "compassTestMain.C"
