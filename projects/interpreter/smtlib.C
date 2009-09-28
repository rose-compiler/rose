#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string.h>
#include <errno.h>

#include "smtlib.h"

using namespace std;

namespace smtlib {

bool varbase::lt(const_varbaseP var) const
   {
     return typeid(*this).before(typeid(*var));
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

predbase::~predbase() {}

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

predbaseP mkprednot(predbaseP pred)
   {
     if (pred->isconst())
          return predbaseP(new predconst(!pred->getconst()));

     return predbaseP(new prednot(pred));
   }

string prednot::show() const
   {
     return "(not " + pred->show() + ")";
   }

void prednot::freevars(varset &vs) const
   {
     pred->freevars(vs);
   }

string predbinop::show() const
   {
     return "(iff " + op1->show() + " " + op2->show() + ")";
   }

void predbinop::freevars(varset &vs) const
   {
     op1->freevars(vs);
     op2->freevars(vs);
   }

string predvar::show() const
   {
     return ":extrapreds (("+_name+"))";
   }

string predvar::name() const
   {
     return _name;
   }

bool predvar::lt(const_varbaseP other) const
   {
     if (const predvar *pv = dynamic_cast<const predvar *>(other.get()))
        {
          return _name < pv->_name;
        }
     else
        {
          return varbase::lt(other);
        }
   }

string predname::show() const
   {
     return var->name();
   }

void predname::freevars(varset &vs) const
   {
     vs.insert(var);
   }

solveresult solverbase::solve(predbaseP formula, bool keep)
   {
     using namespace smtlib::QF_BV;

     /*
      * The purpose of this visitor is to create an identical version of the input bv
      * in which (common) subexpressions have been replaced with variables of the appropriate
      * type.  It produces a list of assumptions which are necessary to enforce the
      * equality constraints on the subexpression variables.
      */
     struct csevisitor : predvisitor, bvvisitor
     {
       map<const predbase *, predbaseP> _prednames;
       map<const bvbase *, bvbaseP> _bvnames;

       predbaseP _pred; bvbaseP _bv;

       varset fv;
       vector<predbaseP> assumptions;

       /* To consider: a version of seen that checks for common subexpressions in
        * the case where these are stored in different objects. */
       bool seen(bvbaseP bv)
          {
            map<const bvbase *, bvbaseP>::const_iterator i = _bvnames.find(bv.get());
            if (i != _bvnames.end())
               {
                 _bv = i->second;
                 return true;
               }
            return false;
          }

       bool seen(predbaseP pred)
          {
            map<const predbase *, predbaseP>::const_iterator i = _prednames.find(pred.get());
            if (i != _prednames.end())
               {
                 _pred = i->second;
                 return true;
               }
            return false;
          }

       bvbaseP getbv(bvbaseP bv)
          {
            bv->visit(*this);
            return _bv;
          }

       predbaseP getpred(predbaseP pred)
          {
            pred->visit(*this);
            return _pred;
          }

       void bvassumption(bvbaseP key, bvbaseP val)
          {
            stringstream ss;
            ss << "tmpbv_" << key;
            bvvarP namevar (new bvvar(key->bits(), ss.str()));
            bvbaseP name (new bvname(namevar));
            predbaseP nameEqVal (new bvbinpred(bveq, name, val));
            fv.insert(namevar);
            assumptions.push_back(nameEqVal);
            _bv = _bvnames[key.get()] = name;
          }

       void predassumption(predbaseP key, predbaseP val)
          {
            stringstream ss;
            ss << "tmppred_" << key;
            predvarP namevar (new predvar(ss.str()));
            predbaseP name (new predname(namevar));
            predbaseP nameEqVal (new predbinop(prediff, name, val));
            fv.insert(namevar);
            assumptions.push_back(nameEqVal);
            _pred = _prednames[key.get()] = name;
          }

       void visit_bvconst(bvbaseP self)
          {
            _bv = self;
          }

       void visit_bvname(bvbaseP self, bvvarP var)
          {
            fv.insert(var);
            _bv = self;
          }

       void visit_bvunop(bvbaseP self, bvunop_kind kind, bvbaseP op)
          {
            if (seen(self)) return;
            bvassumption(self, mkbvunop(kind, getbv(op)));
          }

       void visit_bvbinop(bvbaseP self, bvbinop_kind kind, bvbaseP op1, bvbaseP op2)
          {
            if (seen(self)) return;
            bvassumption(self, mkbvbinop(kind, getbv(op1), getbv(op2)));
          }

       void visit_bvextend(bvbaseP self, bvextend_kind kind, Bits targetbits, bvbaseP op)
          {
            if (seen(self)) return;
            bvassumption(self, mkbvextend(kind, targetbits, getbv(op)));
          }

       void visit_bvextract(bvbaseP self, Bits targetbits, bvbaseP op)
          {
            if (seen(self)) return;
            bvassumption(self, mkbvextract(targetbits, getbv(op)));
          }

       void visit_bvite(bvbaseP self, predbaseP p, bvbaseP trueBV, bvbaseP falseBV)
          {
            if (seen(self)) return;
            bvassumption(self, mkbvite(getpred(p), getbv(trueBV), getbv(falseBV)));
          }

       void visit_predconst(predbaseP self)
          {
            _pred = self;
          }

       void visit_prednot(predbaseP self, predbaseP op)
          {
            if (seen(self)) return;
            predassumption(self, mkprednot(getpred(op)));
          }

       void visit_bvbinpred(predbaseP self, bvbinpred_kind kind, bvbaseP op1, bvbaseP op2)
          {
            if (seen(self)) return;
            predassumption(self, mkbvbinpred(kind, getbv(op1), getbv(op2)));
          }

       void visit_predbinop(predbaseP self, predbinop_kind kind, predbaseP op1, predbaseP op2)
          {
            if (seen(self)) return;
            predassumption(self, predbaseP(new predbinop(kind, getpred(op1), getpred(op2))));
          }

       void visit_predname(predbaseP self, predvarP var)
          {
            fv.insert(var);
            _pred = self;
          }

     };

     csevisitor cv;
     formula->visit(cv);
     return xsolve(cv.fv, cv.assumptions, cv._pred, keep);
   }

solverbase::~solverbase() {}

solver_smtlib::solver_smtlib(string cmd) : cmd(cmd) {}

solveresult solver_smtlib::xsolve(const varset &fv, const vector<predbaseP> &assumptions, predbaseP formula, bool keep)
   {
        {
          ofstream smtFile("test.smt");
          if (smtFile.fail())
             {
               throw solveerror(string("Unable to create temporary file: ") + strerror(errno));
             }

          smtFile << "(benchmark test" << endl
                  << " :logic QF_BV" << endl // TODO: change QF_BV to something more general
                  << " :status unknown" << endl;
          for (varset::const_iterator i = fv.begin(); i != fv.end(); ++i)
             {
               smtFile << " " << (*i)->show() << endl;
             }
          for (vector<predbaseP>::const_iterator i = assumptions.begin(); i != assumptions.end(); ++i)
             {
               smtFile << " :assumption " << (*i)->show() << endl;
             }
          smtFile << " :formula " << formula->show() << endl
                  << " )" << endl;
        }

     string fullcmd = cmd + " < test.smt";
     FILE *procFile = popen(fullcmd.c_str(), "r");
     if (procFile == NULL)
        {
          throw solveerror(string("Unable to spawn solver process: ") + strerror(errno));
        }
     char satLine[4097];
     solveresult rv;
     bool gotResult = false;
     errno = 0;
     while (fscanf(procFile, "%4096[^\n]\n", satLine) != EOF)
        {
          if (strcmp(satLine, "sat") == 0)
             {
               rv.kind = sat;
               gotResult = true;
             }
          else if (strcmp(satLine, "unsat") == 0)
             {
               rv.kind = unsat;
               gotResult = true;
             }
          else
             {
               rv.model.append(satLine);
               rv.model.append("\n");
             }
        }
     if (errno != 0)
        {
          throw solveerror(string("Unable to read solver output: ") + strerror(errno));
        }

     fclose(procFile);

     if (rv.model.size() > 0)
        {
          rv.model.erase(rv.model.size()-1);
        }

     if (!gotResult)
        {
          throw solveerror("Received bad input from solver: " + rv.model);
        }

     if (!keep)
        {
          if (remove("test.smt") < 0)
             {
               throw solveerror(string("Unable to remove temporary file: ") + strerror(errno));
             }
        }

     return rv;
   }

void predconst::visit(predvisitor &visitor)
   {
     visitor.visit_predconst(shared_from_this());
   }

void prednot::visit(predvisitor &visitor)
   {
     visitor.visit_prednot(shared_from_this(), pred);
   }

void predbinop::visit(predvisitor &visitor)
   {
     visitor.visit_predbinop(shared_from_this(), kind, op1, op2);
   }

void predname::visit(predvisitor &visitor)
   {
     visitor.visit_predname(shared_from_this(), var);
   }

predvisitor::~predvisitor() {}

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
          case Bits8: ss << "bv" << u8+0 << "[8]"; break;
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
          return nbits < v->nbits || (nbits == v->nbits && _name < v->_name);
        }
     else
        {
          return varbase::lt(other);
        }
   }

string bvvar::show() const
   {
     stringstream ss;
     ss << ":extrafuns ((" << _name << " BitVec[" << nbits << "]))";
     return ss.str();
   }

Bits bvname::bits() const
   {
     return var->bits();
   }

string bvname::show() const
   {
     return var->name();
   }

void bvname::freevars(varset &vs) const
   {
     vs.insert(var);
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
               BVBINOP_CASE(bvand, op1->const1() && op2->const1(), &)
               BVBINOP_CASE(bvor, op1->const1() || op2->const1(), |)
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
          case bvand: ss << "bvand"; break;
          case bvor: ss << "bvor"; break;
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
             break;
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

bvite::bvite(predbaseP p, bvbaseP trueBV, bvbaseP falseBV) : p(p), trueBV(trueBV), falseBV(falseBV)
     {
       if (trueBV->bits() != falseBV->bits())
            throw bad_bitwidth();
     }

Bits bvite::bits() const
   {
     return trueBV->bits();
   }

string bvite::show() const
   {
     return "(ite " + p->show() + " " + trueBV->show() + " " + falseBV->show() + ")";
   }

void bvite::freevars(varset &vs) const
   {
     p->freevars(vs);
     trueBV->freevars(vs);
     falseBV->freevars(vs);
   }

bvbaseP mkbvite(predbaseP p, bvbaseP trueBV, bvbaseP falseBV)
   {
     if (p->isconst())
          return p->getconst() ? trueBV : falseBV;

     return bvbaseP(new bvite(p, trueBV, falseBV));
   }

void bvbinpred::visit(predvisitor &visitor)
   {
     visitor.visit_bvbinpred(shared_from_this(), kind, op1, op2);
   }

void bvconst::visit(bvvisitor &visitor)
   {
     visitor.visit_bvconst(shared_from_this());
   }

void bvname::visit(bvvisitor &visitor)
   {
     visitor.visit_bvname(shared_from_this(), var);
   }

void bvunop::visit(bvvisitor &visitor)
   {
     visitor.visit_bvunop(shared_from_this(), kind, op);
   }

void bvbinop::visit(bvvisitor &visitor)
   {
     visitor.visit_bvbinop(shared_from_this(), kind, op1, op2);
   }

void bvextend::visit(bvvisitor &visitor)
   {
     visitor.visit_bvextend(shared_from_this(), kind, targetbits, op);
   }

void bvextract::visit(bvvisitor &visitor)
   {
     visitor.visit_bvextract(shared_from_this(), targetbits, op);
   }

void bvite::visit(bvvisitor &visitor)
   {
     visitor.visit_bvite(shared_from_this(), p, trueBV, falseBV);
   }

bvvisitor::~bvvisitor() {}

}
}
