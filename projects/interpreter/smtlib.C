#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <errno.h>

#include "smtlib.h"

using namespace std;

namespace smtlib {

bool varbase::lt(const_varbaseP var) const
   {
     return typeid(this).before(typeid(var.get()));
   }

varbase::~varbase() {}

bool predbase::isconst() const
   {
     return false;
   }

bool predbase::getconst() const
   {
     throw noconst();
   }

string predconst::show() const
   {
     return p ? "true" : "false";
   }

bool predconst::isconst() const
   {
     return true;
   }

bool predconst::getconst() const
   {
     return p;
   }

void predconst::freevars(varset &vs) const {}

predbase::~predbase() {}

solverbase::~solverbase() {}

solver_smtlib::solver_smtlib(string cmd) : cmd(cmd) {}

solveresult solver_smtlib::solve(predbaseP p, bool keep)
   {
        {
          ofstream smtFile("test.smt");
          if (smtFile.fail())
             {
               throw solveerror(string("Unable to create temporary file: ") + strerror(errno));
             }
          varset fv;
          p->freevars(fv);

          smtFile << "(benchmark test" << endl
                  << " :logic QF_BV" << endl // TODO: change QF_BV to something more general
                  << " :status unknown" << endl;
          for (varset::iterator i = fv.begin(); i != fv.end(); ++i)
             {
               smtFile << " :extrafuns " << (*i)->show() << endl;
             }
          smtFile << " :formula " << p->show() << endl
                  << " )" << endl;
        }

     string fullcmd = cmd + " < test.smt";
     FILE *procFile = popen(fullcmd.c_str(), "r");
     if (procFile == NULL)
        {
          throw solveerror(string("Unable to spawn solver process: ") + strerror(errno));
        }
     char satResult[8], satModel[4097];
     if (fscanf(procFile, "%7s", satResult) == EOF)
        {
          throw solveerror(string("Unable to read solver result: ") + strerror(errno));
        }

     solveresult rv;
     if (strcmp(satResult, "sat") == 0)
        {
          rv.kind = sat;
        }
     else if (strcmp(satResult, "unsat") == 0)
        {
          rv.kind = unsat;
        }
     else
        {
          throw solveerror(string("Unexpected output from solver: ") + satResult);
        }

     size_t readSize;
     while ((readSize = fread(satModel, 1, sizeof(satModel)-1, procFile)) != 0)
        {
          satModel[readSize] = '\0';
          rv.model.append(satModel);
        }

     fclose(procFile);

     if (!keep)
        {
          if (remove("test.smt") < 0)
             {
               throw solveerror(string("Unable to remove temporary file: ") + strerror(errno));
             }
        }

     return rv;
   }

namespace QF_BV {

bool bvbase::isconst() const
   {
     return false;
   }

bool bvbase::const1() const
   {
     throw noconst();
   }

uint8_t bvbase::const8() const
   {
     throw noconst();
   }

uint16_t bvbase::const16() const
   {
     throw noconst();
   }

uint32_t bvbase::const32() const
   {
     throw noconst();
   }

uint64_t bvbase::const64() const
   {
     throw noconst();
   }

bvbase::~bvbase() {}

bvconst::bvconst(Bits bits, uint64_t u) : m_bits(bits)
   {
     switch (bits)
        {
          case Bits1: u1 = u; break;
          case Bits8: u8 = u; break;
          case Bits16: u16 = u; break;
          case Bits32: u32 = u; break;
          case Bits64: u64 = u; break;
        }
   }

Bits bvconst::bits() const
   {
     return m_bits;
   }

bool bvconst::isconst() const
   {
     return true;
   }

bool bvconst::const1() const
   {
     if (m_bits != Bits1) throw noconst();
     return u1;
   }

uint8_t bvconst::const8() const
   {
     if (m_bits != Bits8) throw noconst();
     return u8;
   }

uint16_t bvconst::const16() const
   {
     if (m_bits != Bits16) throw noconst();
     return u16;
   }

uint32_t bvconst::const32() const
   {
     if (m_bits != Bits32) throw noconst();
     return u32;
   }

uint64_t bvconst::const64() const
   {
     if (m_bits != Bits64) throw noconst();
     return u64;
   }

string bvconst::show() const
   {
     stringstream ss;
     switch (m_bits)
        {
          case Bits1: ss << (u1 ? "bit1" : "bit0"); break;
          case Bits8: ss << "bv" << u8 << "[8]"; break;
          case Bits16: ss << "bv" << u16 << "[16]"; break;
          case Bits32: ss << "bv" << u32 << "[32]"; break;
          case Bits64: ss << "bv" << u64 << "[64]"; break;
        }
     return ss.str();
   }

void bvconst::freevars(varset &vs) const {}

bool bvvar::lt(const_varbaseP other) const
   {
     if (const bvvar *v = dynamic_cast<const bvvar *>(other.get()))
        {
          return nbits < v->nbits || (nbits == v->nbits && name < v->name);
        }
     else
        {
          return varbase::lt(other);
        }
   }

string bvvar::show() const
   {
     stringstream ss;
     ss << "((" << name << " BitVec[" << nbits << "]))";
     return ss.str();
   }

Bits bvname::bits() const
   {
     return nbits;
   }

string bvname::show() const
   {
     return name;
   }

void bvname::freevars(varset &vs) const
   {
     vs.insert(varbaseP(new bvvar(nbits, name)));
   }

bvbaseP mkbvunop(bvunop_kind kind, bvbaseP op)
   {
     if (op->isconst())
        {
          switch (kind)
             {
               case bvnot:
                  switch (op->bits())
                     {
                       case Bits1: return bvbaseP(new bvconst(!op->const1()));
                       case Bits8: return bvbaseP(new bvconst(uint8_t(~op->const8())));
                       case Bits16: return bvbaseP(new bvconst(uint16_t(~op->const16())));
                       case Bits32: return bvbaseP(new bvconst(~op->const32()));
                       case Bits64: return bvbaseP(new bvconst(~op->const64()));
                     }
               case bvneg:
                  switch (op->bits())
                     {
                       case Bits1: return bvbaseP(new bvconst(op->const1()));
                       case Bits8: return bvbaseP(new bvconst(uint8_t(-int8_t(op->const8()))));
                       case Bits16: return bvbaseP(new bvconst(uint16_t(-int16_t(op->const16()))));
                       case Bits32: return bvbaseP(new bvconst(uint32_t(-int32_t(op->const32()))));
                       case Bits64: return bvbaseP(new bvconst(uint64_t(-int64_t(op->const64()))));
                     }
             }
        }
     return bvbaseP(new bvunop(kind, op));
   }

Bits bvunop::bits() const
   {
     return op->bits();
   }

string bvunop::show() const
   {
     stringstream ss;
     ss << "(";
     switch (kind)
        {
          case bvnot: ss << "bvnot"; break;
          case bvneg: ss << "bvneg"; break;
        }
     ss << " " << op->show() << ")";
     return ss.str();
   }

void bvunop::freevars(varset &vs) const
   {
     op->freevars(vs);
   }

bvbaseP mkbvbinop(bvbinop_kind kind, bvbaseP op1, bvbaseP op2)
   {
     if (op1->isconst() && op2->isconst())
        {
          if (op1->bits() != op2->bits()) throw bad_bitwidth();
          switch (kind)
             {
#define BVBINOP_CASE(bvkind, boolexp, intop) \
               case bvkind: \
                  switch (op1->bits()) \
                     { \
                       case Bits1: return bvbaseP(new bvconst(boolexp)); \
                       case Bits8: return bvbaseP(new bvconst(uint8_t(op1->const8() intop op2->const8()))); \
                       case Bits16: return bvbaseP(new bvconst(uint16_t(op1->const16() intop op2->const16()))); \
                       case Bits32: return bvbaseP(new bvconst(op1->const32() intop op2->const32())); \
                       case Bits64: return bvbaseP(new bvconst(op1->const64() intop op2->const64())); \
                     }
#define BVBINOP_SIGNED_CASE(bvkind, boolexp, intop) \
               case bvkind: \
                  switch (op1->bits()) \
                     { \
                       case Bits1: return bvbaseP(new bvconst(boolexp)); \
                       case Bits8: return bvbaseP(new bvconst(uint8_t(int8_t(op1->const8()) intop int8_t(op2->const8())))); \
                       case Bits16: return bvbaseP(new bvconst(uint16_t(int16_t(op1->const16()) intop int16_t(op2->const16())))); \
                       case Bits32: return bvbaseP(new bvconst(uint32_t(int32_t(op1->const32()) intop int32_t(op2->const32())))); \
                       case Bits64: return bvbaseP(new bvconst(uint64_t(int64_t(op1->const64()) intop int64_t(op2->const64())))); \
                     } 
               BVBINOP_CASE(bvadd, op1->const1() != op2->const1(), +)
               BVBINOP_CASE(bvmul, op1->const1() && op2->const1(), *)
               BVBINOP_CASE(bvudiv, op1->const1(), /)
               BVBINOP_CASE(bvurem, false, %)
               BVBINOP_CASE(bvshl, op1->const1() && !op2->const1(), <<)
               BVBINOP_CASE(bvlshr, op1->const1() && !op2->const1(), >>)
               BVBINOP_CASE(bvxor, op1->const1() != op2->const1(), ^)
               BVBINOP_SIGNED_CASE(bvsub, op1->const1() != op2->const1(), -)
               BVBINOP_SIGNED_CASE(bvsdiv, op1->const1(), /)
               BVBINOP_SIGNED_CASE(bvsrem, false, %)
               BVBINOP_SIGNED_CASE(bvashr, op1->const1() && !op2->const1(), >>)
             }
        }
     return bvbaseP(new bvbinop(kind, op1, op2));
   }

bvbinop::bvbinop(bvbinop_kind kind, bvbaseP op1, bvbaseP op2) : kind(kind), op1(op1), op2(op2)
     {
       if (op1->bits() != op2->bits()) throw bad_bitwidth();
     }

Bits bvbinop::bits() const
   {
     return op1->bits();
   }

string bvbinop::show() const
   {
     stringstream ss;
     ss << "(";
     switch (kind)
        {
          case bvadd: ss << "bvadd"; break;
          case bvmul: ss << "bvmul"; break;
          case bvudiv: ss << "bvudiv"; break;
          case bvurem: ss << "bvurem"; break;
          case bvshl: ss << "bvshl"; break;
          case bvlshr: ss << "bvlshr"; break;
          case bvnand: ss << "bvnand"; break;
          case bvnor: ss << "bvnor"; break;
          case bvxor: ss << "bvxor"; break;
          case bvxnor: ss << "bvxnor"; break;
          case bvsub: ss << "bvsub"; break;
          case bvsdiv: ss << "bvsdiv"; break;
          case bvsrem: ss << "bvsrem"; break;
          case bvsmod: ss << "bvsmod"; break;
          case bvashr: ss << "bvashr"; break;
        }
     ss << " " << op1->show() << " " << op2->show() << ")";
     return ss.str();
   }

void bvbinop::freevars(varset &vs) const
   {
     op1->freevars(vs);
     op2->freevars(vs);
   }

predbaseP mkbvbinpred(bvbinpred_kind kind, bvbaseP op1, bvbaseP op2)
   {
     if (op1->isconst() && op2->isconst())
        {
          if (op1->bits() != op2->bits()) throw bad_bitwidth();
          switch (kind)
             {
#define BVBINPRED_CASE(bvkind, intpred) \
               case bvkind: \
                  switch (op1->bits()) \
                     { \
                       case Bits1: return predbaseP(new predconst(op1->const1() intpred op2->const1())); \
                       case Bits8: return predbaseP(new predconst(op1->const8() intpred op2->const8())); \
                       case Bits16: return predbaseP(new predconst(op1->const16() intpred op2->const16())); \
                       case Bits32: return predbaseP(new predconst(op1->const32() intpred op2->const32())); \
                       case Bits64: return predbaseP(new predconst(op1->const64() intpred op2->const64())); \
                     }
#define BVBINPRED_SIGNED_CASE(bvkind, intpred) \
               case bvkind: \
                  switch (op1->bits()) \
                     { \
                       case Bits1: return predbaseP(new predconst(op1->const1() intpred op2->const1())); \
                       case Bits8: return predbaseP(new predconst(int8_t(op1->const8()) intpred int8_t(op2->const8()))); \
                       case Bits16: return predbaseP(new predconst(int16_t(op1->const16()) intpred int16_t(op2->const16()))); \
                       case Bits32: return predbaseP(new predconst(int32_t(op1->const32()) intpred int32_t(op2->const32()))); \
                       case Bits64: return predbaseP(new predconst(int64_t(op1->const64()) intpred int64_t(op2->const64()))); \
                     }
               BVBINPRED_CASE(bveq, ==)
               BVBINPRED_CASE(bvne, !=)
               BVBINPRED_CASE(bvult, <)
               BVBINPRED_CASE(bvule, <=)
               BVBINPRED_CASE(bvugt, >)
               BVBINPRED_CASE(bvuge, >=)
               BVBINPRED_SIGNED_CASE(bvslt, <)
               BVBINPRED_SIGNED_CASE(bvsle, <=)
               BVBINPRED_SIGNED_CASE(bvsgt, >)
               BVBINPRED_SIGNED_CASE(bvsge, >=)
             }
        }
     return predbaseP(new bvbinpred(kind, op1, op2));
   }

bvbinpred::bvbinpred(bvbinpred_kind kind, bvbaseP op1, bvbaseP op2) : kind(kind), op1(op1), op2(op2)
     {
       if (op1->bits() != op2->bits()) throw bad_bitwidth();
     }

string bvbinpred::show() const
   {
     stringstream ss;
     ss << "(";
     switch (kind)
        {
          case bveq: ss << "="; break;
          case bvne: ss << "!="; break;
          case bvult: ss << "bvult"; break;
          case bvule: ss << "bvule"; break;
          case bvugt: ss << "bvugt"; break;
          case bvuge: ss << "bvuge"; break;
          case bvslt: ss << "bvslt"; break;
          case bvsle: ss << "bvsle"; break;
          case bvsgt: ss << "bvsgt"; break;
          case bvsge: ss << "bvsge"; break;
        }
     ss << " " << op1->show() << " " << op2->show() << ")";
     return ss.str();
   }

void bvbinpred::freevars(varset &vs) const
   {
     op1->freevars(vs);
     op2->freevars(vs);
   }

static bvbaseP bvconstcast(bvextend_kind kind, Bits targetbits, bvbaseP op)
   {
     uint64_t result;
     switch (kind)
        {
          case zero_extend:
             switch (op->bits())
                {
                  case Bits1: result = op->const1() ? 1 : 0; break;
                  case Bits8: result = op->const8(); break;
                  case Bits16: result = op->const16(); break;
                  case Bits32: result = op->const32(); break;
                  case Bits64: result = op->const64(); break;
                }
          case sign_extend:
             switch (op->bits())
                {
                  case Bits1: result = op->const1() ? uint64_t(int64_t(-1)) : 0; break;
                  case Bits8: result = uint64_t(int64_t(int8_t(op->const8()))); break;
                  case Bits16: result = uint64_t(int64_t(int16_t(op->const16()))); break;
                  case Bits32: result = uint64_t(int64_t(int32_t(op->const32()))); break;
                  case Bits64: result = uint64_t(int64_t(op->const64())); break;
                }
        }
     switch (targetbits)
        {
          case Bits1: return bvbaseP(new bvconst(bool(result)));
          case Bits8: return bvbaseP(new bvconst(uint8_t(result)));
          case Bits16: return bvbaseP(new bvconst(uint16_t(result)));
          case Bits32: return bvbaseP(new bvconst(uint32_t(result)));
          case Bits64: return bvbaseP(new bvconst(uint64_t(result)));
        }
   }

bvbaseP mkbvcast(bvextend_kind kind, Bits targetbits, bvbaseP op)
   {
     if (targetbits < op->bits())
        {
          return mkbvextract(targetbits, op);
        }
     else if (targetbits == op->bits())
        {
          return op;
        }
     else // targetbits > op->bits()
        {
          return mkbvextend(kind, targetbits, op);
        }
   }

bvbaseP mkbvextend(bvextend_kind kind, Bits targetbits, bvbaseP op)
   {
     if (op->isconst())
        {
          return bvconstcast(kind, targetbits, op);
        }
     return bvbaseP(new bvextend(kind, targetbits, op));
   }

bvbaseP mkbvextract(Bits targetbits, bvbaseP op)
   {
     if (op->isconst())
        {
          return bvconstcast(zero_extend, targetbits, op);
        }
     return bvbaseP(new bvextract(targetbits, op));
   }

bvextend::bvextend(bvextend_kind kind, Bits targetbits, bvbaseP op) : kind(kind), targetbits(targetbits), op(op)
     {
       if (targetbits < op->bits())
          {
            throw bad_bitwidth();
          }
     }

bvextract::bvextract(Bits targetbits, bvbaseP op) : targetbits(targetbits), op(op)
     {
       if (targetbits > op->bits())
          {
            throw bad_bitwidth();
          }
     }

Bits bvextend::bits() const
   {
     return targetbits;
   }

string bvextend::show() const
   {
     stringstream ss;
     ss << "(";
     switch (kind)
        {
          case zero_extend: ss << "zero_extend"; break;
          case sign_extend: ss << "sign_extend"; break;
        }
     ss << "[" << (targetbits - op->bits()) << "]";
     ss << " " << op->show() << ")";
     return ss.str();
   }

void bvextend::freevars(varset &vs) const
   {
     op->freevars(vs);
   }

Bits bvextract::bits() const
   {
     return targetbits;
   }

string bvextract::show() const
   {
     stringstream ss;
     ss << "(extract";
     ss << "[" << (targetbits - 1) << ":0]";
     ss << " " << op->show() << ")";
     return ss.str();
   }

void bvextract::freevars(varset &vs) const
   {
     op->freevars(vs);
   }

Bits pred2bv::bits() const
   {
     return Bits1;
   }

string pred2bv::show() const
   {
     stringstream ss;
     ss << "(ite " << op->show() << " bit1 bit0)";
     return ss.str();
   }

void pred2bv::freevars(varset &vs) const
   {
     op->freevars(vs);
   }

string bv2pred::show() const
   {
     stringstream ss;
     ss << "(!= " << op->show() << " bv0[" << op->bits() << "])";
     return ss.str();
   }

void bv2pred::freevars(varset &vs) const
   {
     op->freevars(vs);
   }

predbaseP mkbv2pred(bvbaseP op)
   {
     if (op->isconst())
        {
          bool p;
          switch (op->bits())
             {
               case Bits1: p = op->const1(); break;
               case Bits8: p = op->const8() != 0; break;
               case Bits16: p = op->const16() != 0; break;
               case Bits32: p = op->const32() != 0; break;
               case Bits64: p = op->const64() != 0; break;
             }
          return predbaseP(new predconst(p));
        }
     if (pred2bv *p2b = dynamic_cast<pred2bv *>(op.get()))
        {
          return p2b->op;
        }
     return predbaseP(new bv2pred(op));
   }

bvbaseP mkpred2bv(predbaseP op)
   {
     if (op->isconst())
        {
          return bvbaseP(new bvconst(op->getconst()));
        }
     return bvbaseP(new pred2bv(op));
   }

}
}
