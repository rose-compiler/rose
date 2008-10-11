#include "btorProblem.h"
#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <boost/lexical_cast.hpp>

using namespace std;

BtorType BtorComputation::getType() const { // Also checks operand and immediate counts
#define CHECK_COUNTS(opCount, immCount) do {assert (operands.size() == (opCount)); assert (immediates.size() == (immCount));} while (0)
#define BITVECTOR do {for (size_t i = 0; i < operands.size(); ++i) assert (operands[i]->type.kind == btor_type_bitvector);} while (0)
#define ARRAY do {for (size_t i = 0; i < operands.size(); ++i) assert (operands[i]->type.kind == btor_type_array);} while (0)
#define OPS_HAVE_SAME_SIZE do {for (size_t i = 1; i < operands.size(); ++i) assert (operands[i]->type == operands[0]->type);} while (0)
  switch (op) {
    case btor_op_not:
    case btor_op_neg:
      CHECK_COUNTS(1, 0); BITVECTOR; return operands[0]->type;
    case btor_op_redand:
    case btor_op_redor:
    case btor_op_redxor:
      CHECK_COUNTS(1, 0); BITVECTOR; return BtorType::bitvector(1);
    case btor_op_inc:
    case btor_op_dec:
      CHECK_COUNTS(1, 0); BITVECTOR; return operands[0]->type;
    case btor_op_and:
    case btor_op_or:
    case btor_op_xor:
    case btor_op_nand:
    case btor_op_nor:
    case btor_op_xnor:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0]->type;
    case btor_op_implies:
    case btor_op_iff:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; assert (operands[0]->type.bitWidth == 1); return BtorType::bitvector(1);
    case btor_op_add:
    case btor_op_sub:
    case btor_op_mul:
    case btor_op_urem:
    case btor_op_srem:
    case btor_op_udiv:
    case btor_op_sdiv:
    case btor_op_smod:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0]->type;
    case btor_op_eq:
    case btor_op_ne:
    case btor_op_ult:
    case btor_op_slt:
    case btor_op_ulte:
    case btor_op_slte:
    case btor_op_ugt:
    case btor_op_sgt:
    case btor_op_ugte:
    case btor_op_sgte:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return BtorType::bitvector(1);
    case btor_op_sll:
    case btor_op_srl:
    case btor_op_sra:
    case btor_op_ror:
    case btor_op_rol:
      CHECK_COUNTS(2, 0); BITVECTOR; assert (operands[0]->type.bitWidth == (1ULL << operands[1]->type.bitWidth)); return operands[0]->type;
    case btor_op_uaddo:
    case btor_op_saddo:
    case btor_op_usubo:
    case btor_op_ssubo:
    case btor_op_umulo:
    case btor_op_smulo:
    case btor_op_sdivo:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return BtorType::bitvector(1);
    case btor_op_concat:
      CHECK_COUNTS(2, 0); BITVECTOR; return BtorType::bitvector(operands[0]->type.bitWidth + operands[1]->type.bitWidth);
    case btor_op_cond:
      CHECK_COUNTS(3, 0); BITVECTOR; assert (operands[0]->type.bitWidth == 1); assert (operands[1]->type == operands[2]->type); return operands[1]->type;
    case btor_op_slice:
      CHECK_COUNTS(1, 2); BITVECTOR; assert (operands[0]->type.bitWidth > immediates[1]); assert (immediates[0] >= immediates[1]); return BtorType::bitvector(immediates[0] - immediates[1] + 1);
    case btor_op_read:
      CHECK_COUNTS(2, 0); assert (operands[0]->type.kind == btor_type_array); assert (operands[1]->type.kind == btor_type_bitvector); assert (operands[0]->type.arraySize == operands[1]->type.bitWidth); return BtorType::bitvector(operands[0]->type.bitWidth);
    case btor_op_write:
      CHECK_COUNTS(3, 0); assert (operands[0]->type.kind == btor_type_array); assert (operands[1]->type.kind == btor_type_bitvector); assert (operands[2]->type.kind == btor_type_bitvector); assert (operands[0]->type.arraySize == operands[1]->type.bitWidth); assert (operands[0]->type.bitWidth == operands[2]->type.bitWidth); return operands[0]->type;
    case btor_op_acond:
      CHECK_COUNTS(3, 0); assert (operands[0]->type.kind == btor_type_bitvector); assert (operands[0]->type.bitWidth == 1); assert (operands[1]->type.kind == btor_type_array); assert (operands[1]->type == operands[2]->type); return operands[1]->type;
    case btor_op_array_eq:
      CHECK_COUNTS(2, 0); assert (operands[0]->type.kind == btor_type_array); assert (operands[0]->type == operands[1]->type); return BtorType::bitvector(1);
    case btor_op_var:
      CHECK_COUNTS(0, 0); return BtorType::bitvector(0); // Will be filled in
    case btor_op_array:
      CHECK_COUNTS(0, 0); return BtorType::array(0, 0); // Will be filled in
    case btor_op_const:
      CHECK_COUNTS(0, 1); return BtorType::bitvector(0); // Will be filled in
    case btor_op_one:
      CHECK_COUNTS(0, 0); return BtorType::bitvector(0); // Will be filled in
    case btor_op_ones:
      CHECK_COUNTS(0, 0); return BtorType::bitvector(0); // Will be filled in
    case btor_op_zero:
      CHECK_COUNTS(0, 0); return BtorType::bitvector(0); // Will be filled in
    case btor_op_next:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0]->type;
    case btor_op_anext:
      CHECK_COUNTS(2, 0); ARRAY; OPS_HAVE_SAME_SIZE; return operands[0]->type;
    case btor_op_root:
      CHECK_COUNTS(1, 0); return operands[0]->type;
    default: assert (!"Bad operator");
  }
#undef CHECK_COUNTS
#undef BITVECTOR
#undef ARRAY
#undef OPS_HAVE_SAME_SIZE
}

static string btorOpToString(BtorOperator op) {
  const char* names[] = {"not", "neg", "redand", "redor", "redxor", "inc", "dec", "and", "or", "xor", "nand", "nor", "xnor", "implies", "iff", "add", "sub", "mul", "urem", "srem", "udiv", "sdiv", "smod", "eq", "ne", "ult", "slt", "ulte", "slte", "ugt", "sgt", "ugte", "sgte", "sll", "srl", "sra", "ror", "rol", "uaddo", "saddo", "usubo", "ssubo", "umulo", "smulo", "sdivo", "concat", "cond", "slice", "read", "write", "acond", "eq", "var", "array", "constd", "one", "ones", "zero", "next", "anext", "root"};
  assert ((size_t)op < sizeof(names) / sizeof(*names));
  return names[(size_t)op];
}

static string btorUnparseHelper(string& nameIdea, BtorComputationPtr comp, map<BtorComputationPtr, string>& done, uint& counter) { // nameIdea also returns actual name when nameIdea starts off as ""
  if (done.find(comp) != done.end()) {nameIdea = done[comp]; return "";}
  string result;
  vector<string> opNames;
  for (size_t i = 0; i < comp->operands.size(); ++i) {
    string opName = "";
    result += btorUnparseHelper(opName, comp->operands[i], done, counter);
    assert (opName != "");
    opNames.push_back(opName);
  }
  string name;
  if (nameIdea == "") {
    name = boost::lexical_cast<string>(++counter);
  } else if (isdigit(nameIdea[0])) {
    name = "_" + nameIdea;
  } else {
    name = nameIdea;
  }
  nameIdea = name; // Pass back to outer computations
  done.insert(make_pair(comp, name));
  string thisLine = name + " " + btorOpToString(comp->op) + " " + boost::lexical_cast<string>(comp->type.bitWidth);
  if (comp->type.kind == btor_type_array) thisLine += " " + boost::lexical_cast<string>(comp->type.arraySize);
  for (size_t i = 0; i < opNames.size(); ++i) {
    thisLine += " " + opNames[i];
  }
  for (size_t i = 0; i < comp->immediates.size(); ++i) {
    thisLine += " " + boost::lexical_cast<string>(comp->immediates[i]);
  }
  if (comp->op == btor_op_var && comp->variableName != "") {
    thisLine += " " + comp->variableName;
  }
  result += thisLine + "\n";
  return result;
}

string BtorProblem::unparse() const {
  // Topological sort, covering every node in computations and everything
  // reachable from it
  map<BtorComputationPtr, string> done;
  uint counter = 0;
  string result = "";
  for (vector<BtorComputationPtr>::const_iterator i = computations.begin(); i != computations.end(); ++i) {
    string name = "";
    result += btorUnparseHelper(name, *i, done, counter);
  }
  return result;
}
