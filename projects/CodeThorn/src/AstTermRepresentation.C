#include "sage3basic.h"
#include "AstTermRepresentation.h"

using namespace std;

AstTermRepresentationAttribute::AstTermRepresentationAttribute(SgNode* node) : _node(node) {
}

string AstTermRepresentationAttribute::toString() {
  return "AstTerm: "+AstTerm::astTermWithNullValuesToString(_node);
}
	
void AstTermRepresentationAttribute::attachAstTermRepresentationAttributes(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgStatement* stmt=dynamic_cast<SgStatement*>(*i)) {
      AstAttribute* ara=new AstTermRepresentationAttribute(stmt);
      stmt->setAttribute("codethorn-term-representation",ara);
    }
  }
}
