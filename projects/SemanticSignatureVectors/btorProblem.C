#include "rose.h"
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h"
#include "integerOps.h"
#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <boost/lexical_cast.hpp>
#include <stdint.h>
#include <inttypes.h>

using namespace std;
using namespace IntegerOps;

BtorOperatorInfo btorOperators[btor_NUM_OPERATORS] = {
  // Fields are enum value, string, num operands, num immediates, returns array, final string
  {btor_op_not, "not", 1, 0, false, false},
  {btor_op_neg, "neg", 1, 0, false, false},
  {btor_op_redand, "redand", 1, 0, false, false},
  {btor_op_redor, "redor", 1, 0, false, false},
  {btor_op_redxor, "redxor", 1, 0, false, false},
  {btor_op_inc, "inc", 1, 0, false, false},
  {btor_op_dec, "dec", 1, 0, false, false},
  {btor_op_and, "and", 2, 0, false, false},
  {btor_op_or, "or", 2, 0, false, false},
  {btor_op_xor, "xor", 2, 0, false, false},
  {btor_op_nand, "nand", 2, 0, false, false},
  {btor_op_nor, "nor", 2, 0, false, false},
  {btor_op_xnor, "xnor", 2, 0, false, false},
  {btor_op_implies, "implies", 2, 0, false, false},
  {btor_op_iff, "iff", 2, 0, false, false},
  {btor_op_add, "add", 2, 0, false, false},
  {btor_op_sub, "sub", 2, 0, false, false},
  {btor_op_mul, "mul", 2, 0, false, false},
  {btor_op_urem, "urem", 2, 0, false, false},
  {btor_op_srem, "srem", 2, 0, false, false},
  {btor_op_udiv, "udiv", 2, 0, false, false},
  {btor_op_sdiv, "sdiv", 2, 0, false, false},
  {btor_op_smod, "smod", 2, 0, false, false},
  {btor_op_eq, "eq", 2, 0, false, false},
  {btor_op_ne, "ne", 2, 0, false, false},
  {btor_op_ult, "ult", 2, 0, false, false},
  {btor_op_slt, "slt", 2, 0, false, false},
  {btor_op_ulte, "ulte", 2, 0, false, false},
  {btor_op_slte, "slte", 2, 0, false, false},
  {btor_op_ugt, "ugt", 2, 0, false, false},
  {btor_op_sgt, "sgt", 2, 0, false, false},
  {btor_op_ugte, "ugte", 2, 0, false, false},
  {btor_op_sgte, "sgte", 2, 0, false, false},
  {btor_op_sll, "sll", 2, 0, false, false},
  {btor_op_srl, "srl", 2, 0, false, false},
  {btor_op_sra, "sra", 2, 0, false, false},
  {btor_op_ror, "ror", 2, 0, false, false},
  {btor_op_rol, "rol", 2, 0, false, false},
  {btor_op_uaddo, "uaddo", 2, 0, false, false},
  {btor_op_saddo, "saddo", 2, 0, false, false},
  {btor_op_usubo, "usubo", 2, 0, false, false},
  {btor_op_ssubo, "ssubo", 2, 0, false, false},
  {btor_op_umulo, "umulo", 2, 0, false, false},
  {btor_op_smulo, "smulo", 2, 0, false, false},
  {btor_op_sdivo, "sdivo", 2, 0, false, false},
  {btor_op_concat, "concat", 2, 0, false, false},
  {btor_op_cond, "cond", 3, 0, false, false},
  {btor_op_slice, "slice", 1, 2, false, false},
  {btor_op_read, "read", 2, 0, false, false},
  {btor_op_write, "write", 3, 0, true, false},
  {btor_op_acond, "acond", 3, 0, true, false},
  {btor_op_array_eq, "eq", 2, 0, false, false},
  {btor_op_var, "var", 0, 0, false, true},
  {btor_op_array, "array", 0, 0, true, true}, // String is not currently allowed
  {btor_op_const, "constd", 0, 0, false, true}, // Special case for parser
  {btor_op_one, "one", 0, 0, false, false},
  {btor_op_ones, "ones", 0, 0, false, false},
  {btor_op_zero, "zero", 0, 0, false, false},
  {btor_op_next, "next", 2, 0, false, false},
  {btor_op_anext, "anext", 2, 0, true, false},
  {btor_op_root, "root", 1, 0, false, true} // String is not currently allowed
};

BtorType BtorComputation::getType() const { // Also checks operand and immediate counts
#define CHECK_COUNTS(opCount, immCount) do {assert (operands.size() == (opCount)); assert (immediates.size() == (immCount));} while (0)
#define BITVECTOR do {for (size_t i = 0; i < operands.size(); ++i) assert (operands[i].kind() == btor_type_bitvector);} while (0)
#define ARRAY do {for (size_t i = 0; i < operands.size(); ++i) assert (operands[i].kind() == btor_type_array);} while (0)
#define OPS_HAVE_SAME_SIZE do {for (size_t i = 1; i < operands.size(); ++i) assert (operands[i].type() == operands[0].type());} while (0)
  switch (op) {
    case btor_op_not:
    case btor_op_neg:
      CHECK_COUNTS(1, 0); BITVECTOR; return operands[0].type();
    case btor_op_redand:
    case btor_op_redor:
    case btor_op_redxor:
      CHECK_COUNTS(1, 0); BITVECTOR; return BtorType::bitvector(1);
    case btor_op_inc:
    case btor_op_dec:
      CHECK_COUNTS(1, 0); BITVECTOR; return operands[0].type();
    case btor_op_and:
    case btor_op_or:
    case btor_op_xor:
    case btor_op_nand:
    case btor_op_nor:
    case btor_op_xnor:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0].type();
    case btor_op_implies:
    case btor_op_iff:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; assert (operands[0].type().bitWidth == 1); return BtorType::bitvector(1);
    case btor_op_add:
    case btor_op_sub:
    case btor_op_mul:
    case btor_op_urem:
    case btor_op_srem:
    case btor_op_udiv:
    case btor_op_sdiv:
    case btor_op_smod:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0].type();
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
      CHECK_COUNTS(2, 0); BITVECTOR; assert (operands[0].type().bitWidth == (1ULL << operands[1].type().bitWidth)); return operands[0].type();
    case btor_op_uaddo:
    case btor_op_saddo:
    case btor_op_usubo:
    case btor_op_ssubo:
    case btor_op_umulo:
    case btor_op_smulo:
    case btor_op_sdivo:
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return BtorType::bitvector(1);
    case btor_op_concat:
      CHECK_COUNTS(2, 0); BITVECTOR; return BtorType::bitvector(operands[0].type().bitWidth + operands[1].type().bitWidth);
    case btor_op_cond:
      CHECK_COUNTS(3, 0); BITVECTOR; assert (operands[0].type().bitWidth == 1); assert (operands[1].type() == operands[2].type()); return operands[1].type();
    case btor_op_slice:
      CHECK_COUNTS(1, 2); BITVECTOR; assert (operands[0].type().bitWidth > immediates[1]); assert (immediates[0] >= immediates[1]); return BtorType::bitvector(immediates[0] - immediates[1] + 1);
    case btor_op_read:
      CHECK_COUNTS(2, 0); assert (operands[0].type().kind == btor_type_array); assert (operands[1].type().kind == btor_type_bitvector); assert (operands[0].type().arraySize == operands[1].type().bitWidth); return BtorType::bitvector(operands[0].type().bitWidth);
    case btor_op_write:
      CHECK_COUNTS(3, 0); assert (operands[0].type().kind == btor_type_array); assert (operands[1].type().kind == btor_type_bitvector); assert (operands[2].type().kind == btor_type_bitvector); assert (operands[0].type().arraySize == operands[1].type().bitWidth); assert (operands[0].type().bitWidth == operands[2].type().bitWidth); return operands[0].type();
    case btor_op_acond:
      CHECK_COUNTS(3, 0); assert (operands[0].type().kind == btor_type_bitvector); assert (operands[0].type().bitWidth == 1); assert (operands[1].type().kind == btor_type_array); assert (operands[1].type() == operands[2].type()); return operands[1].type();
    case btor_op_array_eq:
      CHECK_COUNTS(2, 0); assert (operands[0].type().kind == btor_type_array); assert (operands[0].type() == operands[1].type()); return BtorType::bitvector(1);
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
      CHECK_COUNTS(2, 0); BITVECTOR; OPS_HAVE_SAME_SIZE; return operands[0].type();
    case btor_op_anext:
      CHECK_COUNTS(2, 0); ARRAY; OPS_HAVE_SAME_SIZE; return operands[0].type();
    case btor_op_root:
      CHECK_COUNTS(1, 0); return operands[0].type();
    default: assert (!"Bad operator");
  }
#undef CHECK_COUNTS
#undef BITVECTOR
#undef ARRAY
#undef OPS_HAVE_SAME_SIZE
}

static string btorOpToString(BtorOperator op);

bool BtorComputation::isConstant() const {
  //FIXME should not return false for sra, srl
  if ( op == btor_op_srl ||  op == btor_op_sra || op == btor_op_array || op == btor_op_var || op == btor_op_root) return false;
  for (size_t i = 0; i < operands.size(); ++i) {
    if (!operands[i].isConstant()) return false;
  }
  return true;
}

uintmax_t BtorComputation::constantValue() const {
  assert (isConstant());
  switch (op) {
    case btor_op_const: return immediates[0];
    case btor_op_zero: return 0;
    case btor_op_one: return 1;
    case btor_op_ones: return genMask<uintmax_t>(type.bitWidth);
    case btor_op_eq: return (((operands[0].constantValue() ^ operands[1].constantValue()) & (genMask<uintmax_t>(operands[0].bitWidth()))) == 0);
    case btor_op_and: return operands[0].constantValue() & operands[1].constantValue();
    case btor_op_or: return operands[0].constantValue() | operands[1].constantValue();
    case btor_op_xor: return operands[0].constantValue() ^ operands[1].constantValue();
    case btor_op_not: return operands[0].constantValue() ^ (genMask<uintmax_t>(type.bitWidth));
    case btor_op_nand: return (operands[0].constantValue() & operands[1].constantValue()) ^ genMask<uintmax_t>(type.bitWidth);
    case btor_op_nor: return (operands[0].constantValue() | operands[1].constantValue()) ^ genMask<uintmax_t>(type.bitWidth);
    case btor_op_xnor: return (operands[0].constantValue() ^ operands[1].constantValue()) ^ genMask<uintmax_t>(type.bitWidth);
    case btor_op_redand: return operands[0].constantValue() == genMask<uintmax_t>(type.bitWidth);
    case btor_op_redor: return operands[0].constantValue() != 0;
    case btor_op_add: return (operands[0].constantValue() + operands[1].constantValue()) & genMask<uintmax_t>(type.bitWidth);
    case btor_op_neg: return (-operands[0].constantValue()) & genMask<uintmax_t>(type.bitWidth);
    case btor_op_cond: return operands[0].constantValue() ? operands[1].constantValue() : operands[2].constantValue();
    case btor_op_slice: {
      uintmax_t c = operands[0].constantValue();
      c >>= immediates[1];
      c &= genMask<uintmax_t>(type.bitWidth);
      return c;
    }
    case btor_op_concat: return (operands[0].constantValue() << operands[1].bitWidth()) | operands[1].constantValue();
    default: fprintf(stderr, "Bad op %s for constant\n", btorOpToString(op).c_str()); abort();
  }
}

static string btorOpToString(BtorOperator op) {
  assert ((size_t)op < sizeof(btorOperators) / sizeof(*btorOperators));
  assert (btorOperators[(size_t)op].op == op);
  return btorOperators[(size_t)op].name;
}

static BtorOperator btorStringToOp(const string& s) {
  for (size_t i = 0; i < sizeof(btorOperators) / sizeof(*btorOperators); ++i) {
    if (s == btorOperators[i].name) {
      return btorOperators[i].op;
    }
  }
  assert (!"Bad operator name");
}

static string btorUnparseHelper(string& nameIdea, BtorComputationPtr comp, map<BtorComputation*, string>& done, uint& counter) { // nameIdea also returns actual name when nameIdea starts off as ""
  if (done.find(comp.p.get()) != done.end()) {nameIdea = (comp.inverted ? "-" : "") + done[comp.p.get()]; return "";}
  if (comp.inverted) {
    string uninvertedName;
    string result = btorUnparseHelper(uninvertedName, comp.invert(), done, counter);
    nameIdea = "-" + uninvertedName;
    return result;
  }
  assert (comp.inverted == false);
  string result;
  vector<string> opNames;
  for (size_t i = 0; i < comp.p->operands.size(); ++i) {
    string opName = "";
    result += btorUnparseHelper(opName, comp.p->operands[i], done, counter);
    assert (opName != "");
    opNames.push_back(opName);
  }
  string name;
  if (nameIdea == "") {
    name = boost::lexical_cast<string>(++counter);
  } else {
    name = nameIdea;
  }
  nameIdea = name; // Pass back to outer computations
  done.insert(make_pair(comp.p.get(), name));
  string thisLine = name + " " + btorOpToString(comp.p->op) + " " + boost::lexical_cast<string>(comp.type().bitWidth);
  if (comp.type().kind == btor_type_array) thisLine += " " + boost::lexical_cast<string>(comp.type().arraySize);
  for (size_t i = 0; i < opNames.size(); ++i) {
    thisLine += " " + opNames[i];
  }
  for (size_t i = 0; i < comp.p->immediates.size(); ++i) {
    thisLine += " " + boost::lexical_cast<string>(comp.p->immediates[i]);
  }
  if (comp.p->op == btor_op_var && comp.p->variableName != "") {
    thisLine += " " + comp.p->variableName;
  }
  result += thisLine + "\n";
  return result;
}

string BtorProblem::unparse() const {
  // Topological sort, covering every node in computations and everything
  // reachable from it
  map<BtorComputation*, string> done;
  uint counter = 0;
  string result = "";
  for (vector<BtorComputationPtr>::const_iterator i = computations.begin(); i != computations.end(); ++i) {
    string name = "";
    result += btorUnparseHelper(name, *i, done, counter);
  }
  return result;
}

static vector<string> parseIntoWords(const string& s) {
  vector<string> result;
  size_t i = 0;
  while (true) {
    size_t newI = s.find(' ', i);
    string token = (newI == string::npos ? s.substr(i) : s.substr(i, newI - i));
    if (!token.empty()) result.push_back(token);
    if (newI == string::npos) break;
    i = newI + 1;
  }
  return result;
}

BtorProblem BtorProblem::parse(const std::string& s) {
  vector<vector<string> > lines;
  {
    size_t i = 0;
    while (true) {
      size_t newI = s.find('\n', i);
      string line;
      if (newI == string::npos) {
        line = s.substr(i);
      } else {
        line = s.substr(i, newI - i);
      }
      if (!line.empty()) lines.push_back(parseIntoWords(line));
      if (newI == string::npos) break;
      i = newI + 1;
    }
  }
  BtorProblem p;
  map<string, BtorComputationPtr> comps;
  for (size_t i = 0; i < lines.size(); ++i) {
    const vector<string>& line = lines[i];
    assert (line.size() >= 3);
    string ref = line[0];
    if (comps.find(ref) != comps.end()) {assert (!"Name already in use");}
    string op = line[1];
    // There are a few special cases to get the operator kind
    enum {cb_decimal = 10, cb_hex = 16, cb_binary = 2};
    int constantBase = 10;
    if (op == "const") {
      op = "constd";
      constantBase = cb_binary;
    } else if (op == "consth") {
      op = "constd";
      constantBase = cb_hex;
    } else {
      constantBase = cb_decimal;
    }
    BtorOperator realOp = btorStringToOp(op);
    if (realOp == btor_op_eq && line.size() == 6) { // Overloaded operator
      realOp = btor_op_array_eq;
    }
    assert ((size_t)realOp < sizeof(btorOperators) / sizeof(*btorOperators));
    const BtorOperatorInfo& info = btorOperators[(size_t)realOp];
    assert (info.op == realOp);
    assert (line.size() >= 2 + (info.returnsArray ? 2 : 1) + info.numOperands + info.numImmediates);
    assert (line.size() <= 2 + (info.returnsArray ? 2 : 1) + info.numOperands + info.numImmediates + (info.acceptsFinalString ? 1 : 0));
    vector<BtorComputationPtr> operands;
    vector<uintmax_t> imms;
    BtorType expectedReturnType;
    size_t j = 2;
    expectedReturnType.kind = (info.returnsArray ? btor_type_array : btor_type_bitvector);
    expectedReturnType.bitWidth = boost::lexical_cast<uint>(line[j++]);
    if (info.returnsArray) expectedReturnType.arraySize = boost::lexical_cast<uint>(line[j++]);
    for (size_t k = 0; k < info.numOperands; ++k) {
      assert (j < line.size());
      string operandStr = line[j++];
      bool inv = false;
      if (!operandStr.empty() && operandStr[0] == '-') {
        inv = true;
        operandStr = operandStr.substr(1);
      }
      assert (comps.find(operandStr) != comps.end());
      BtorComputationPtr operand = comps[operandStr];
      if (inv) operand = operand.invert();
      operands.push_back(operand);
    }
    for (size_t k = 0; k < info.numImmediates; ++k) {
      assert (j < line.size());
      imms.push_back(boost::lexical_cast<uintmax_t>(line[j++]));
    }
    string varStr = "";
    if (info.acceptsFinalString && j < line.size()) {
      varStr = line[j++];
    }
    if (realOp == btor_op_const) {
      char* endPtr = NULL;
      uintmax_t constant = strtoumax(varStr.c_str(), &endPtr, constantBase);
      assert (endPtr == varStr.c_str() + varStr.size());
      imms.push_back(constant);
    }
    assert (j == line.size());
    BtorComputationPtr result = p.buildComp(expectedReturnType.bitWidth, realOp, operands, imms, varStr);
    if (result.kind() == btor_type_array) {
      if (result.arraySize() == 0) {
        result.p->type.arraySize = expectedReturnType.arraySize;
      } else {
        assert (result.arraySize() == expectedReturnType.arraySize);
      }
    }
    comps[ref] = result;
    if (realOp == btor_op_root || realOp == btor_op_next || realOp == btor_op_anext) {
      // Only these operators should be kept if their results are unused
      p.computations.push_back(result);
    }
  }
  return p;
}

BtorProblem BtorProblem::parse(FILE* f) {
  string s;
  while (!feof(f) && !ferror(f)) {
    char buf[1024];
    size_t count = fread(buf, 1, 1024, f);
    assert (!ferror(f));
    s += string(buf, buf + count);
  }
  assert (!ferror(f));
  return BtorProblem::parse(s);
}

BtorComputationPtr BtorProblem::nameValue(BtorComputationPtr c, const string& newName) {
#if 0
  return c;
#else
  switch (c.kind()) {
    case btor_type_bitvector: {
      BtorComputationPtr var = build_var(c.bitWidth(), newName);
      computations.push_back(build_op_root(build_op_eq(c, var)));
      return c;
    }
    case btor_type_array: {
      BtorComputationPtr var = build_array(c.bitWidth(), c.arraySize(), newName);
      computations.push_back(build_op_root(build_op_array_eq(c, var)));
      return c;
    }
    default: assert (false);
  }
#endif
}

BtorComputationPtr BtorProblem::buildComp(uint size, BtorOperator op, const std::vector<BtorComputationPtr>& operands, const std::vector<uintmax_t>& immediates, const std::string& name) {
  BtorComputation rawComp(size, op, operands, immediates);
  rawComp.variableName = name;
  std::map<BtorComputation, boost::weak_ptr<BtorComputation> >::iterator cseTableIter = cseTable.find(rawComp);
  if (cseTableIter != cseTable.end()) {
    try {
      return BtorComputationPtr(boost::shared_ptr<BtorComputation>(cseTableIter->second));
    } catch (boost::bad_weak_ptr) {
      // Otherwise unreferenced entry in cache, so delete and recreate it
      cseTable.erase(cseTableIter);
      goto notFound;
    }
  } else if ((op == btor_op_cond || op == btor_op_acond) && operands[0].isConstant()) {
    return operands[0].constantValue() ? operands[1] : operands[2];
  } else if (op != btor_op_const && rawComp.isConstant()) {
    return buildComp(rawComp.type.bitWidth, btor_op_const, std::vector<BtorComputationPtr>(), std::vector<uintmax_t>(1, rawComp.constantValue()), name);
  } else { notFound:
    BtorComputationPtr comp(new BtorComputation(rawComp));
    cseTable.insert(std::make_pair(rawComp, comp.p));
    return comp;
  }
}
