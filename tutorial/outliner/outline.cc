//! outline.cc: Demonstrates the pragma-interface of the Outliner.
#include <rose.h>
#include <iostream>

#include <Outliner.hh>
#include <vector>
#include <string>

using namespace std;

int
main (int argc, char* argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  //! Accepting command line options to the outliner
  vector<string> argvList(argv,argv+argc);
  Outliner::commandLineProcessing(argvList);

  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  cerr << "[Outlining...]" << endl;
  size_t count = Outliner::outlineAll (proj);

  cerr << "  [Processed " << count << " outline directives.]" << endl;
  return backend (proj);
}
