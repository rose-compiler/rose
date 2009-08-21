#include <rose.h>

extern "C" {

SgProject *_haskell_frontend(int argc, char **argv)
{
  return frontend(argc, argv);
}

int _haskell_backend(SgProject *project)
{
  return backend(project);
}

}
