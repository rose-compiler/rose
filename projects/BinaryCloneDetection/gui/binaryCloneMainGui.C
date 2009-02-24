/*
 *
 */

#include "rose.h"
#include <vector>
#include <qrose.h>
#include "binaryCloneGui.h"

using namespace qrs;

int main( int argc, char **argv )
{
  QROSE::init(argc,argv);
  BinaryCloneGui binaryCloneGui;

  return QROSE::exec();
}
