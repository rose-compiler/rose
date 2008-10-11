#ifndef ROSE_BTORPROBLEM_H
#define ROSE_BTORPROBLEM_H

// Limitations:
//   No constants larger than uintmax_t
//   No implicit negation (must use not operator)

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>
#include <string>

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
  btor_op_root
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

struct BtorComputation;
typedef boost::shared_ptr<BtorComputation> BtorComputationPtr;

struct BtorComputation {
  BtorType type;
  BtorOperator op;
  std::vector<uintmax_t> immediates;
  std::vector<BtorComputationPtr> operands;
  std::string variableName; // Only for var

  BtorComputation(BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates = std::vector<uintmax_t>()): op(op), immediates(immediates), operands(operands) {
    type = getType();
  }

  BtorComputation(uint size, BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates = std::vector<uintmax_t>()): op(op), immediates(immediates), operands(operands) {
    type = getType();
    if (type.bitWidth == 0) {
      type.bitWidth = size;
    } else {
      assert (type.bitWidth == size);
    }
  }

  BtorComputation(BtorOperator op, BtorComputationPtr op0 = BtorComputationPtr(), BtorComputationPtr op1 = BtorComputationPtr(), BtorComputationPtr op2 = BtorComputationPtr()): op(op), immediates(), operands() {
    if (op0) operands.push_back(op0);
    if (op1) operands.push_back(op1);
    if (op2) operands.push_back(op2);
    type = getType();
  }

  static BtorComputationPtr var(uint size, const std::string& name = "") {
    BtorComputationPtr c(new BtorComputation(btor_op_var, std::vector<BtorComputationPtr>()));
    c->variableName = name;
    c->type.bitWidth = size;
    return c;
  }

  static BtorComputationPtr array(uint bw, uint sz) {
    BtorComputationPtr c(new BtorComputation(btor_op_array, std::vector<BtorComputationPtr>()));
    c->type.bitWidth = bw;
    c->type.arraySize = sz;
    return c;
  }

  static BtorComputationPtr constant(uintmax_t size, uintmax_t value) {
    BtorComputationPtr c(new BtorComputation(btor_op_const, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(1, value)));
    c->type.bitWidth = size;
    assert ((1ULL << size) == 0 || (1ULL << size) > value);
    return c;
  }

  BtorType getType() const; // Also checks operand and immediate counts
};

struct BtorProblem {
  std::vector<BtorComputationPtr> computations; // Every computation must be either in here or reachable from here

  BtorProblem(): computations() {}

  std::string unparse() const;
};

#endif // ROSE_BTORPROBLEM_H
