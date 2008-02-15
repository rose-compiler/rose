#include <stdio.h>
#include <stdlib.h>

#include <string>

/* OpenMaple routines are defined here */
#include "maplec.h"


namespace MapleRoseInterface
   {
  // Global counter to track invocations of Maple
     extern int mapleEquationCounter;

     void startupMaple( int argc, char *argv[] );
     void stopMaple();

     std::string evaluateMapleStatement ( std::string command );
   }
