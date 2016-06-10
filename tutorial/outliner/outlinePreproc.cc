// outlinePreproc.cc: Shows the outliner's preprocessor-only phase.
#include <rose.h>
#include <iostream>

#include <Outliner.hh>

using namespace std;

int
main (int argc, char* argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject* proj = frontend (argc, argv);
  ROSE_ASSERT (proj);

#if 1
  cerr << "[Running outliner's preprocessing phase only...]" << endl;
  size_t count = Outliner::preprocessAll (proj);
  cerr << "  [Processed " << count << " outline directives.]" << endl;
#else
  printf ("Skipping outlining due to recent move from std::list to std::vector in ROSE \n");
#endif

  cerr << "[Unparsing...]" << endl;
  return backend (proj);
}
