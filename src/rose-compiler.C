
#include "sage3basic.h"

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);

#if defined(ROSE_COMPILER_FOR_LANGUAGE)
  std::string language(ROSE_COMPILER_FOR_LANGUAGE);
  bool has_dialect = false;
  for (std::vector<std::string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
    if (arg->find("-std=") == 0) {
      has_dialect = true;
      break;
    }
  }
  if (!has_dialect) {
    args.insert(args.begin()+1, "-std="+language);
  }
#endif

  return backend(frontend(args));
}

