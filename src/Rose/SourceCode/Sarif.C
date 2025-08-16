
#include <sage3basic.h>
#include <Rose/SourceCode/Sarif.h>
#include <iostream>

namespace Rose {
namespace SourceCode {
namespace Sarif {

LocationPtr locate(SgLocatedNode * node) {
//  std::cerr << "node: " << std::hex << node << " : " << node->class_name() << std::endl;
  Sg_File_Info * spos = node->get_startOfConstruct();
  Sg_File_Info * epos = node->get_endOfConstruct();
//  std::cerr << "  start: " << spos->get_line() << " : " << spos->get_col() << std::endl;
//  std::cerr << "  start: " << spos->get_raw_line() << " : " << spos->get_raw_col() << std::endl;
//  std::cerr << "  end  : " << spos->get_line() << " : " << spos->get_col() << std::endl;
//  std::cerr << "  end  : " << spos->get_raw_line() << " : " << spos->get_raw_col() << std::endl;
  // if (spos->get_line())
  return Sarif::Location::instance(
      SourceLocation(spos->get_filenameString(), spos->get_line(), spos->get_col())
      // SourceLocation(epos->get_filenameString(), epos->get_line(), epos->get_col())
  );
}

} } }
