/*
 * The smtlib library provides programmatic access to (at least) the QF_BV logic in SMT.
 * It performs automatic simplification of expressions before passing to the solver.
 * Thus, if the expression is a constant, there should be no need to invoke the solver
 * at all.
 * At some stage it should also do CSE.
 * It is independent of ROSE so could in theory be used in other applications.
 */
#ifndef _SMTLIB_H
#define _SMTLIB_H

#include <string>
#include <set>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace smtlib {

class noconst {};

class varbase;

typedef boost::shared_ptr<varbase> varbaseP;
typedef boost::shared_ptr<const varbase> const_varbaseP;

class varbase
   {
     public:
     virtual std::string show() const = 0;
     virtual bool lt(const_varbaseP other) const;
     virtual ~varbase();
   };

struct varcmp : std::binary_function<varbaseP, varbaseP, bool>
     {
       bool operator()(varbaseP v1, varbaseP v2)
          {
            return v1->lt(v2);
          }
     };

typedef std::set<varbaseP, varcmp> varset;

class predbase;

typedef boost::shared_ptr<predbase> predbaseP;

class predbase : public boost::enable_shared_from_this<predbase>
     {
       public:
            virtual std::string show() const = 0;
            virtual bool isconst() const;
            virtual bool getconst() const;
            virtual void freevars(varset &vs) const = 0;
            virtual ~predbase();
     };

class predconst : public predbase
     {
       bool p;

       public:
       predconst(bool p) : p(p) {}
       std::string show() const;
       bool isconst() const;
       bool getconst() const;
       void freevars(varset &vs) const;
     };

enum solveresult_kind
   {
     sat,
     unsat,
   };

struct solveresult
   {
     solveresult_kind kind;
     std::string model;
   };

class solveerror
   {
     public:
          std::string err;
          solveerror(std::string err) : err(err) {}
   };

class solverbase
   {
     public:
          virtual solveresult solve(predbaseP p, bool keep = false) = 0;
          virtual ~solverbase();
   };

class solver_yices : solverbase {};
class solver_smtlib : solverbase
     {
       std::string cmd;

       public:
       solver_smtlib(std::string cmd);

       virtual solveresult solve(predbaseP p, bool keep);
     };

namespace QF_BV {

class bvbase;

typedef boost::shared_ptr<bvbase> bvbaseP;
typedef boost::shared_ptr<const bvbase> const_bvbaseP;

enum Bits { Bits1 = 1, Bits8 = 8, Bits16 = 16, Bits32 = 32, Bits64 = 64 };

class bad_bitwidth {};

class bvbase : public boost::enable_shared_from_this<bvbase>
{
  public:
       virtual Bits bits() const = 0;
       virtual bool isconst() const;
       virtual bool const1() const;
       virtual uint8_t const8() const;
       virtual uint16_t const16() const;
       virtual uint32_t const32() const;
       virtual uint64_t const64() const;

       virtual std::string show() const = 0;
       virtual void freevars(varset &vs) const = 0;

       virtual ~bvbase();
};

class bvconst : public bvbase
{

  Bits m_bits;
  union {
       bool u1;
       uint8_t u8;
       uint16_t u16;
       uint32_t u32;
       uint64_t u64;
  };

  public:
  bvconst(bool u1) : m_bits(Bits1), u1(u1) {}
  bvconst(uint8_t u8) : m_bits(Bits8), u8(u8) {}
  bvconst(uint16_t u16) : m_bits(Bits16), u16(u16) {}
  bvconst(uint32_t u32) : m_bits(Bits32), u32(u32) {}
  bvconst(uint64_t u64) : m_bits(Bits64), u64(u64) {}
  bvconst(Bits bits, uint64_t u);

  Bits bits() const;
  bool isconst() const;
  bool const1() const;
  uint8_t const8() const;
  uint16_t const16() const;
  uint32_t const32() const;
  uint64_t const64() const;

  std::string show() const;
  void freevars(varset &vs) const;

};

class bvvar : public varbase
   {
     Bits nbits;
     std::string name;

     public:
     bvvar(Bits nbits, std::string name) : nbits(nbits), name(name) {}
     bool lt(const_varbaseP other) const;
     std::string show() const;

   };

class bvname : public bvbase
   {
     Bits nbits;
     std::string name;

     public:
     bvname(Bits nbits, std::string name) : nbits(nbits), name(name) {}

     Bits bits() const;
     std::string show() const;
     void freevars(varset &vs) const;

   };

enum bvunop_kind
   {
     bvnot,
     bvneg,
   };

enum bvbinop_kind
   {
     bvadd,
     bvmul,
     bvudiv,
     bvurem,
     bvshl,
     bvlshr,
     bvnand,
     bvnor,
     bvxor,
     bvxnor,
     bvsub,
     bvsdiv,
     bvsrem,
     bvsmod,
     bvashr,
   };

enum bvbinpred_kind
   {
     bveq,
     bvne,
     bvult,
     bvule,
     bvugt,
     bvuge,
     bvslt,
     bvsle,
     bvsgt,
     bvsge,
   };

bvbaseP mkbvunop(bvunop_kind kind, bvbaseP op);

class bvunop : public bvbase
{
  bvunop_kind kind;
  bvbaseP op;

  public:
  bvunop(bvunop_kind kind, bvbaseP op) : kind(kind), op(op) {}

  Bits bits() const;
  std::string show() const;
  void freevars(varset &vs) const;
};

bvbaseP mkbvbinop(bvbinop_kind kind, bvbaseP op1, bvbaseP op2);

class bvbinop : public bvbase
{
  bvbinop_kind kind;
  bvbaseP op1, op2;

  public:
  bvbinop(bvbinop_kind kind, bvbaseP op1, bvbaseP op2);

  Bits bits() const;
  std::string show() const;
  void freevars(varset &vs) const;
};

predbaseP mkbvbinpred(bvbinpred_kind kind, bvbaseP op1, bvbaseP op2);

class bvbinpred : public predbase
{
  bvbinpred_kind kind;
  bvbaseP op1, op2;

  public:
  bvbinpred(bvbinpred_kind kind, bvbaseP op1, bvbaseP op2);

  std::string show() const;
  void freevars(varset &vs) const;
};

enum bvextend_kind
   {
     zero_extend,
     sign_extend,
   };

bvbaseP mkbvcast(bvextend_kind kind, Bits targetbits, bvbaseP op);

bvbaseP mkbvextend(bvextend_kind kind, Bits targetbits, bvbaseP op);

class bvextend : public bvbase
{
  bvextend_kind kind;
  Bits targetbits;
  bvbaseP op;

  public:
  bvextend(bvextend_kind kind, Bits targetbits, bvbaseP op);

  Bits bits() const;
  std::string show() const;
  void freevars(varset &vs) const;
};

bvbaseP mkbvextract(Bits targetbits, bvbaseP op);

class bvextract : public bvbase
{
  Bits targetbits;
  bvbaseP op;

  public:
  bvextract(Bits targetbits, bvbaseP op);

  Bits bits() const;
  std::string show() const;
  void freevars(varset &vs) const;
};

predbaseP mkbv2pred(bvbaseP op);

class pred2bv : public bvbase
   {
     friend predbaseP mkbv2pred(bvbaseP op);

     predbaseP op;

     public:
          pred2bv(predbaseP op) : op(op) {}

          Bits bits() const;
          std::string show() const;
          void freevars(varset &vs) const;
   };

bvbaseP mkpred2bv(predbaseP op);

class bv2pred : public predbase
   {
     friend bvbaseP mkpred2bv(predbaseP op);

     bvbaseP op;

     public:
     bv2pred(bvbaseP op) : op(op) {}

     std::string show() const;
     void freevars(varset &vs) const;
   };

template <typename intT, size_t tsize = sizeof(intT)>
struct bvTypeTraits {};

template <>
struct bvTypeTraits<bool, sizeof(bool)>
   {
     static const Bits bits = Bits1;
     typedef bool canon_type;
   };

template <typename intT>
struct bvTypeTraits<intT, 1>
   {
     static const Bits bits = Bits8;
     typedef uint8_t canon_type;
   };

template <typename intT>
struct bvTypeTraits<intT, 2>
   {
     static const Bits bits = Bits16;
     typedef uint16_t canon_type;
   };

template <typename intT>
struct bvTypeTraits<intT, 4>
   {
     static const Bits bits = Bits32;
     typedef uint32_t canon_type;
   };

template <typename intT>
struct bvTypeTraits<intT, 8>
   {
     static const Bits bits = Bits64;
     typedef uint64_t canon_type;
   };

}
}

#endif
