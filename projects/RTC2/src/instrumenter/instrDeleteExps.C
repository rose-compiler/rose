#include "instr.h"

namespace Instr {

  void instr(SgDeleteExp* del) {

    // original: delete(ptr)
    // final: delete(ptr), remove_entry(&ptr)
    // FIXME: May want to check in remove_entry if the other elements
    // within the class have been deallocated

    SgExpression* var = del->get_variable();
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var)));

    SgExpression* ovl = buildMultArgOverloadFn("remove_entry", SB::buildExprListExp(addr),
        SgTypeVoid::createType(),
        Util::getScopeForExp(del),
        GEFD(SI::getEnclosingStatement(del)));

    SgExpression* del_copy = SI::copyExpression(del);

    // replace delete(ptr) with delete(ptr), remove_entry(&ptr)
    replaceWrapper(del, SB::buildCommaOpExp(del_copy, ovl));
  }
}
