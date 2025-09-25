
#include <sage3basic.h>
#include <Rose/Source/Sarif.h>
#include <iostream>

namespace Rose {
namespace Source {
namespace Sarif {

LocationPtr locate(SgLocatedNode* node) {
  Sg_File_Info * spos = node->get_startOfConstruct();
  return Sarif::Location::instance(
      SourceLocation(spos->get_filenameString(), spos->get_line(), spos->get_col())
  );
}

} } }
