#include <smtlib.h>
#include <assert.h>

using namespace smtlib::QF_BV;

int main(void)
   {
     bvbaseP one (new bvconst(true));
     bvbaseP notOne = mkbvunop(bvnot, one);
     assert(notOne->bits() == Bits1);
     assert(notOne->isconst());
     assert(notOne->const1() == false);
     one = bvbaseP(new bvconst(uint8_t(1)));
     notOne = mkbvunop(bvnot, one);
     assert(notOne->bits() == Bits8);
     assert(notOne->isconst());
     assert(notOne->const8() == 0xFE);
     one = bvbaseP(new bvconst(uint16_t(1)));
     notOne = mkbvunop(bvnot, one);
     assert(notOne->bits() == Bits16);
     assert(notOne->isconst());
     assert(notOne->const16() == 0xFFFE);
     one = bvbaseP(new bvconst(uint32_t(1)));
     notOne = mkbvunop(bvnot, one);
     assert(notOne->bits() == Bits32);
     assert(notOne->isconst());
     assert(notOne->const32() == 0xFFFFFFFE);
     one = bvbaseP(new bvconst(uint64_t(1)));
     notOne = mkbvunop(bvnot, one);
     assert(notOne->bits() == Bits64);
     assert(notOne->isconst());
     assert(notOne->const64() == 0xFFFFFFFFFFFFFFFEULL);
     return 0;
   }
