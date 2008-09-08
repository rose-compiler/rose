/*
 *
 */

#include <vector>
#include <qrose.h>

#include "compassGui.h"
#include "compassInterface.h"

using namespace qrs;

int main( int argc, char **argv )
{
  QROSE::init(argc,argv);

  CompassInterface compassInterface;
  compassInterface.init(argc, argv);

  CompassGui compassGui(compassInterface);

  return QROSE::exec();
}
