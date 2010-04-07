// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"

namespace SageBuilderAsm {

  SgAsmByteValueExpression* makeByteValue(uint8_t val) {
    SgAsmByteValueExpression* v = new SgAsmByteValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmWordValueExpression* makeWordValue(uint16_t val) {
    SgAsmWordValueExpression* v = new SgAsmWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmDoubleWordValueExpression* makeDWordValue(uint32_t val) {
    SgAsmDoubleWordValueExpression* v = new SgAsmDoubleWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmQuadWordValueExpression* makeQWordValue(uint64_t val) {
    SgAsmQuadWordValueExpression* v = new SgAsmQuadWordValueExpression(val);
    ROSE_ASSERT (v);
    return v;
  }

  SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmExpression* segment, SgAsmType* t) {
    SgAsmMemoryReferenceExpression* r = new SgAsmMemoryReferenceExpression(addr);
    addr->set_parent(r);
    if (segment) {
      r->set_segment(segment);
      segment->set_parent(r);
    }
    if (t) r->set_type(t);
    return r;
  }

  SgAsmBinaryAdd* makeAdd(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAdd* a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtract* makeSubtract(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtract* a = new SgAsmBinarySubtract(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAddPreupdate* makeAddPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAddPreupdate* a = new SgAsmBinaryAddPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtractPreupdate* makeSubtractPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtractPreupdate* a = new SgAsmBinarySubtractPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAddPostupdate* makeAddPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAddPostupdate* a = new SgAsmBinaryAddPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinarySubtractPostupdate* makeSubtractPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinarySubtractPostupdate* a = new SgAsmBinarySubtractPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryMultiply* makeMul(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryMultiply* a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryLsl* makeLsl(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryLsl* a = new SgAsmBinaryLsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryLsr* makeLsr(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryLsr* a = new SgAsmBinaryLsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryAsr* makeAsr(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryAsr* a = new SgAsmBinaryAsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmBinaryRor* makeRor(SgAsmExpression* lhs, SgAsmExpression* rhs) {
    SgAsmBinaryRor* a = new SgAsmBinaryRor(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
  }

  SgAsmUnaryRrx* makeRrx(SgAsmExpression* lhs) {
    SgAsmUnaryRrx* a = new SgAsmUnaryRrx(lhs);
    lhs->set_parent(a);
    return a;
  }

  SgAsmUnaryArmSpecialRegisterList* makeArmSpecialRegisterList(SgAsmExpression* lhs) {
    SgAsmUnaryArmSpecialRegisterList* a = new SgAsmUnaryArmSpecialRegisterList(lhs);
    lhs->set_parent(a);
    return a;
  }

  SgAsmExprListExp* makeExprListExp() {
    return new SgAsmExprListExp();
  }

}
