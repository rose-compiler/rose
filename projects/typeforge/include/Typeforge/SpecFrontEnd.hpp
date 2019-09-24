#ifndef TYPEFORGE_SPEC_FRONTEND_H
#define TYPEFORGE_SPEC_FRONTEND_H

#include "Typeforge/TFTypeTransformer.hpp"
#include "Typeforge/CommandList.hpp"

namespace Typeforge {

class SpecFrontEnd {
 public:
  // run frontend for typeforge specification file
  // returns true if reading the spec file failed
  // also reports errors
  static void parse(std::vector<std::string> const & filenames, CommandList & commandList);
  static bool parse(std::string const & specFileName, CommandList & commandList);

 private:
  static bool readJSONFile(std::string const & fileName, CommandList & commandList);
  static bool readTFFile(std::string const & fileName, CommandList & commandList);
};

}

#endif
