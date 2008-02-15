#include "rose.h"

#include "SlicingInfo.h"
#include <string>
using namespace std;

void SlicingInfo::visit(SgNode * node) {

  if (isSgPragmaDeclaration(node)) { //figure out what to do with this pragma
    SgPragma * pragma = isSgPragmaDeclaration(node)->get_pragma();

    string pragma_name(pragma->get_pragma());

    if (pragma_name == _slicefunction) {
      _markFunction = true;
    } else if (pragma_name == _slicestatement) {
      _markStatement = true;
    }

    return;
  }

  if (_markFunction) {
    if (isSgFunctionDeclaration(node)) {
      _func = isSgFunctionDeclaration(node);
      _markFunction = false;
    }
  }

  if (_markStatement) {
    if (isSgStatement(node)) {
      _target = isSgStatement(node);
      _markStatement = false;
    }
  }

  return;
}
