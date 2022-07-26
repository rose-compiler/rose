#ifndef ROSE_BinaryAnalysis_Dwarf_Attributes_H
#define ROSE_BinaryAnalysis_Dwarf_Attributes_H

#include <AstAttributeMechanism.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Dwarf {

  /* DWARF attributes use the built-in AstAttribute mechanism for storage in SgAsmDwarfConstruct
   * nodes in the AST. */
  class DwarfAttribute : public AstAttribute {
  public:
    DwarfAttribute(std::string s) : value(s) { }
    virtual std::string toString() { return value; }
    std::string get() { return value; }
  private:
    std::string value;
  };

}
}
}

#endif // ROSE_BinaryAnalysis_Dwarf_Attributes_H
