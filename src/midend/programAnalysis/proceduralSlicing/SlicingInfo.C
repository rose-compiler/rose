#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#include "SlicingInfo.h"

void SlicingInfo::visit(SgNode * node) {

  if (isSgPragmaDeclaration(node)) { //figure out what to do with this pragma
    SgPragma * pragma = isSgPragmaDeclaration(node)->get_pragma();

    string pragma_name(pragma->get_pragma());

    if (pragma_name == slicefunction) {
      _markFunction = true;
    } else if (pragma_name = slicestatement) {
      _markStatement = true;
    }
  }
}

