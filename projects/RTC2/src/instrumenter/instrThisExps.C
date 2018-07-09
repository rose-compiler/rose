#include "instr.h"

namespace Instr {

  void instr(SgThisExp* te) {
    printf("inst this exp\n");

    // create an entry for "this"...
    // create_entry(&this, this, sizeof(*this), Util::getDummyLock())
    // FIXME: Can't use create_entry since it returns void. And in the fn call,
    // we actually deref "this", which itself could fail. Instead, lets do a dummy
    // entry on this for now.
    SgType* exp_type = te->get_type();
#ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
#endif

#if 0
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(te)));
    SgExpression* ptr = SI::copyExpression(te);
    SgExpression* size = SB::buildSizeOfOp(SB::buildPointerDerefExp(SI::copyExpression(te)));
    SgExpression* lock = Util::getDummyLock();
    SgExprListExp* params = SB::buildExprListExp(addr, ptr, size, lock);

    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
        GEFD(SI::getEnclosingStatement(te)),
        true);

    SgExpression* ovl = buildMultArgOverloadFn("create_entry", params,
        retType,
        Util::getScopeForExp(te),
        GEFD(SI::getEnclosingStatement(te)));

    replaceWrapper(te, ovl);
#endif

    // Create a dummy entry prior to this use...
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(te)));
    SgExpression* ovl = buildMultArgOverloadFn("create_dummy_entry", SB::buildExprListExp(addr),
        SgTypeVoid::createType(),
        Util::getScopeForExp(te),
        GEFD(SI::getEnclosingStatement(te)));
    SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(te)), SB::buildExprStatement(ovl));

    // Now, simply create struct(this, &this)...
    SgExpression* ptr = SI::copyExpression(te);
    SgExpression* addr_copy = SI::copyExpression(addr);
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
        GEFD(SI::getEnclosingStatement(te)),
        true);

    ovl = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ptr, addr_copy),
        retType,
        Util::getScopeForExp(te),
        GEFD(SI::getEnclosingStatement(te)));

    replaceWrapper(te, ovl);
  }

}
