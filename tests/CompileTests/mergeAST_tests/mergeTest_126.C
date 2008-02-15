// This generates the errors:
// Node is not in parent's child list, node: 0x9974f98 = SgFunctionDefinition = operator== parent: 0xb7f271d8 = SgFunctionDeclaration
// Node is not in parent's child list, node: 0x9975020 = SgFunctionDefinition = operator!= parent: 0xb7f27340 = SgFunctionDeclaration
// Node is not in parent's child list, node: 0x99750a8 = SgFunctionDefinition = operator< parent: 0xb7f274a8 = SgFunctionDeclaration

#include <string>

class CFGEdge
   {
     public:
          std::string id() const;

          bool operator==(const CFGEdge& o) const {return id() == o.id();}
          bool operator!=(const CFGEdge& o) const {return id() != o.id();}
          bool operator<(const CFGEdge& o) const {return id() < o.id();}
   };
