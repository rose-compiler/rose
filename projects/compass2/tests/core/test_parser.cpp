/**
 * \file test_basic.C
 * \author Justin Too <too1@llnl.gov>
 */

#include <iostream>

#include "rose.h"
#include "compass2/compass.h"

int
main(int argc, char** argv)
  {
    SgProject* sageProject = frontend (argc, argv);
    Compass::Parser parser ("compass_parameters.xml", "compass_parameters.xsd");
    return 0;
  }
