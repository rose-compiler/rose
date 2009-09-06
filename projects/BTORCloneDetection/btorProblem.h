#ifndef ROSE_BTORPROBLEM_H
#define ROSE_BTORPROBLEM_H

// Limitations:
//   No constants larger than uintmax_t
//
// Extensions:
//   Tags can be strings, not just numbers (but we only generate numbers so
//     Boolector will work)
//   array and root can have labels at the end (BTOR only allows that on var)

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include <map>
#include <string>
#include "semanticsModule.h"
#include "integerOps.h"

typedef unsigned int uint;

enum BtorOperator {
  btor_op_not,
  btor_op_neg,
  btor_op_redand,
  btor_op_redor,
  btor_op_redxor,
  btor_op_inc,
  btor_op_dec,
  btor_op_and,
  btor_op_or,
  btor_op_xor,
  btor_op_nand,
  btor_op_nor,
  btor_op_xnor,
  btor_op_implies,
  btor_op_iff,
  btor_op_add,
  btor_op_sub,
  btor_op_mul,
  btor_op_urem,
  btor_op_srem,
  btor_op_udiv,
  btor_op_sdiv,
  btor_op_smod,
  btor_op_eq,
  btor_op_ne,
  btor_op_ult,
  btor_op_slt,
  btor_op_ulte,
  btor_op_slte,
  btor_op_ugt,
  btor_op_sgt,
  btor_op_ugte,
  btor_op_sgte,
  btor_op_sll,
  btor_op_srl,
  btor_op_sra,
  btor_op_ror,
  btor_op_rol,
  btor_op_uaddo,
  btor_op_saddo,
  btor_op_usubo,
  btor_op_ssubo,
  btor_op_umulo,
  btor_op_smulo,
  btor_op_sdivo,
  btor_op_concat,
  btor_op_cond,
  btor_op_slice,
  btor_op_read,
  btor_op_write,
  btor_op_acond,
  btor_op_array_eq,
  btor_op_var,
  btor_op_array,
  btor_op_const, // All bases
  btor_op_one,
  btor_op_ones, // -1
  btor_op_zero,
  btor_op_next,
  btor_op_anext,
  btor_op_root,
  btor_NUM_OPERATORS
};

enum BtorTypeKind {btor_type_bitvector, btor_type_array};

struct BtorType {
  BtorTypeKind kind;
  uint bitWidth;
  uint arraySize; // log2 of number of elements
  friend bool operator==(const BtorType& a, const BtorType& b) {
    if (a.kind != b.kind) return false;
    if (a.bitWidth != b.bitWidth) return false;
    if (a.kind == btor_type_bitvector) return true; // Ignore arraySize
    return a.arraySize == b.arraySize;
  }
  friend bool operator!=(const BtorType& a, const BtorType& b) {
    return !(a == b);
  }
  friend bool operator<(const BtorType& a, const BtorType& b) {
    if (a.kind != b.kind) return a.kind < b.kind;
    if (a.bitWidth != b.bitWidth) return a.bitWidth < b.bitWidth;
    if (a.kind == btor_type_array && a.arraySize != b.arraySize) return a.arraySize < b.arraySize;
    return false;
  }
  static BtorType bitvector(uint bw) {
    BtorType t;
    t.kind = btor_type_bitvector;
    t.bitWidth = bw;
    t.arraySize = 0;
    return t;
  }
  static BtorType array(uint bw, uint sz) {
    BtorType t;
    t.kind = btor_type_array;
    t.bitWidth = bw;
    t.arraySize = sz;
    return t;
  }
};

struct BtorOperatorInfo {
  BtorOperator op;
  const char* name;
  uint numOperands;
  uint numImmediates;
  bool returnsArray;
  bool acceptsFinalString;
};

extern BtorOperatorInfo btorOperators[btor_NUM_OPERATORS];

struct BtorComputation;

struct BtorComputationPtr {
  //Parents of the node in a DDG
  boost::shared_ptr<BtorComputation> p;
  bool inverted;

  BtorComputationPtr(BtorComputation* c): p(boost::shared_ptr<BtorComputation>(c)), inverted(false) {}
  BtorComputationPtr(boost::shared_ptr<BtorComputation> c): p(c), inverted(false) {}
  BtorComputationPtr(): p(boost::shared_ptr<BtorComputation>()), inverted(false) {}
  BtorComputationPtr invert() const {
    BtorComputationPtr x;
    x.p = p;
    x.inverted = !inverted;
    return x;
  }

  BtorType type() const;
  BtorTypeKind kind() const;
  uint bitWidth() const;
  uint arraySize() const;
  bool isConstant() const;
  uintmax_t constantValue() const;
};

struct BtorComputation {
  BtorType type;
  BtorOperator op;
  std::vector<uintmax_t> immediates;
  std::vector<BtorComputationPtr> operands;
  std::string variableName; // Only for var

  private: // These should only be used by the build functions in BtorProblem
  friend class BtorProblem;

  BtorComputation(BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates = std::vector<uintmax_t>()): op(op), immediates(immediates), operands(operands) {
    type = getType();
  }

  BtorComputation(uint size, BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates = std::vector<uintmax_t>()): op(op), immediates(immediates), operands(operands) {
    type = getType();
    if (size != (uint)(-1)) {
      if (type.bitWidth == 0) {
        type.bitWidth = size;
      } else {
        assert (type.bitWidth == size);
      }
    }
  }

  public:
  BtorType getType() const; // Also checks operand and immediate counts
  bool isConstant() const;
  uintmax_t constantValue() const;

  friend bool operator==(const BtorComputation& a, const BtorComputation& b) {
    return a.type == b.type && a.op == b.op && a.immediates == b.immediates && a.operands == b.operands && a.variableName == b.variableName;
  }

  friend bool operator<(const BtorComputation& a, const BtorComputation& b) {
    if (a.type != b.type) return a.type < b.type;
    if (a.op != b.op) return a.op < b.op;
    if (a.immediates != b.immediates) return a.immediates < b.immediates;
    if (a.operands != b.operands) return a.operands < b.operands;
    return a.variableName < b.variableName;
  }
};

inline BtorType BtorComputationPtr::type() const {return p->type;}
inline BtorTypeKind BtorComputationPtr::kind() const {return type().kind;}
inline uint BtorComputationPtr::bitWidth() const {return type().bitWidth;}
inline uint BtorComputationPtr::arraySize() const {return type().arraySize;}
inline bool BtorComputationPtr::isConstant() const {return p->isConstant();}
inline uintmax_t BtorComputationPtr::constantValue() const {
  uintmax_t val = p->constantValue();
  if (inverted) val ^= (IntegerOps::shl1<uintmax_t>(this->bitWidth()) - 1);
  return val;
}

inline bool operator==(const BtorComputationPtr& p1, const BtorComputationPtr& p2) {
  return p1.inverted == p2.inverted && p1.p == p2.p;
}

inline bool operator<(const BtorComputationPtr& p1, const BtorComputationPtr& p2) {
  if (p1.inverted != p2.inverted) return p1.inverted < p2.inverted;
  return p1.p < p2.p;
}

struct BtorProblem {
  std::vector<BtorComputationPtr> computations; // Every computation must be either in here or reachable from here
  std::map<BtorComputation, boost::weak_ptr<BtorComputation> > cseTable;

  BtorProblem(): computations() {}

  std::string unparse() const;
  static BtorProblem parse(const std::string& s);
  static BtorProblem parse(FILE* f);

  BtorComputationPtr buildComp(uint size, BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates, const std::string& name);

  BtorComputationPtr build_var(uint size, const std::string& name = "") {
    return buildComp(size, btor_op_var, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(), name);
  }

  BtorComputationPtr build_array(uint bw, uint sz, const std::string& name = "") {
    BtorComputationPtr c = buildComp(bw, btor_op_array, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(), name);
    c.p->type.arraySize = sz;
    return c;
  }

  BtorComputationPtr build_constant(uintmax_t size, uintmax_t value) {
    BtorComputationPtr c = buildComp(size, btor_op_const, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(1, value), "");
    assert ((1ULL << size) == 0 || (1ULL << size) > value);
    return c;
  }

  BtorComputationPtr nameValue(BtorComputationPtr c, const std::string& newName); // Can only be used in combinational circuits, not sequential ones

#define BUILDER1(op) BtorComputationPtr build_##op(BtorComputationPtr arg0) {return buildComp((uint)(-1), btor_##op, std::vector<BtorComputationPtr>(1, arg0), std::vector<uintmax_t>(), "");}
#define BUILDER2(op) BtorComputationPtr build_##op(BtorComputationPtr arg0, BtorComputationPtr arg1) {std::vector<BtorComputationPtr> args; args.push_back(arg0); args.push_back(arg1); return buildComp((uint)(-1), btor_##op, args, std::vector<uintmax_t>(), "");}
#define BUILDER3(op) BtorComputationPtr build_##op(BtorComputationPtr arg0, BtorComputationPtr arg1, BtorComputationPtr arg2) {std::vector<BtorComputationPtr> args; args.push_back(arg0); args.push_back(arg1); args.push_back(arg2); return buildComp((uint)(-1), btor_##op, args, std::vector<uintmax_t>(), "");}
#define BUILDERSIZE(op) BtorComputationPtr build_##op(uintmax_t size) {return buildComp(size, btor_##op, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(), "");}
  BUILDER1(op_not)
  BUILDER1(op_neg)
  BUILDER1(op_redand)
  BUILDER1(op_redor)
  BUILDER1(op_redxor)
  BUILDER1(op_inc)
  BUILDER1(op_dec)
  BUILDER2(op_and)
  BUILDER2(op_or)
  BUILDER2(op_xor)
  BUILDER2(op_nand)
  BUILDER2(op_nor)
  BUILDER2(op_xnor)
  BUILDER2(op_implies)
  BUILDER2(op_iff)
  BUILDER2(op_add)
  BUILDER2(op_sub)
  BUILDER2(op_mul)
  BUILDER2(op_urem)
  BUILDER2(op_srem)
  BUILDER2(op_udiv)
  BUILDER2(op_sdiv)
  BUILDER2(op_smod)
  BUILDER2(op_eq)
  BUILDER2(op_ne)
  BUILDER2(op_ult)
  BUILDER2(op_slt)
  BUILDER2(op_ulte)
  BUILDER2(op_slte)
  BUILDER2(op_ugt)
  BUILDER2(op_sgt)
  BUILDER2(op_ugte)
  BUILDER2(op_sgte)
  BUILDER2(op_sll)
  BUILDER2(op_srl)
  BUILDER2(op_sra)
  BUILDER2(op_ror)
  BUILDER2(op_rol)
  BUILDER2(op_uaddo)
  BUILDER2(op_saddo)
  BUILDER2(op_usubo)
  BUILDER2(op_ssubo)
  BUILDER2(op_umulo)
  BUILDER2(op_smulo)
  BUILDER2(op_sdivo)
  BUILDER2(op_concat)
  BUILDER3(op_cond)
  BtorComputationPtr build_op_slice(BtorComputationPtr arg, uintmax_t to, uintmax_t from) {
    std::vector<uintmax_t> imms(2);
    imms[0] = to;
    imms[1] = from;
    return buildComp(to - from + 1, btor_op_slice, std::vector<BtorComputationPtr>(1, arg), imms, "");
  }
  BUILDER2(op_read)
  BUILDER3(op_write)
  BUILDER3(op_acond)
  BUILDER2(op_array_eq)
  BUILDERSIZE(op_one)
  BUILDERSIZE(op_ones)
  BUILDERSIZE(op_zero)
  BUILDER2(op_next)
  BUILDER2(op_anext)
  BUILDER1(op_root)
#undef BUILDER1
#undef BUILDER2
#undef BUILDER3
#undef BUILDERSIZE
};

#endif // ROSE_BTORPROBLEM_H
