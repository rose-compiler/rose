#ifndef ROSE_SATPROBLEM_H
#define ROSE_SATPROBLEM_H

#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include <ext/hash_map>
#include <ext/hash_set>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <stdio.h>
#include <cassert>
#include <math.h>

typedef int Var;
typedef int Lit; // Using Dimacs conventions, but with 0 for false and 0x80000000 for true
template <size_t Len>
struct LitListType: public boost::array<Lit, Len> {};
#define LitList(N) LitListType<(N)> // Stored LSB first, counted in bits
typedef std::vector<Lit> Clause;

// DQ (3/8/2009): Intel Pin defines "TRUE" and "FALSE" as macros so we have
// to be careful about using these names (we can either undefine them or 
// added the suffix "_value" below).  Undefining the macros might be a 
// problem if the order of the header files was reversed (should cause a syntax error).
#ifdef TRUE
  #warning "TRUE is defined as a macro, undefining it to support use as a constant variable."
  #undef TRUE
#endif
#ifdef FALSE
  #warning "FALSE is defined as a macro, undefining it to support use as a constant variable."
  #undef FALSE
#endif
// Len and similar template parameters are always in bits, not bytes
// const Lit TRUE_value = 0x80000000, FALSE_value = 0;
const Lit TRUE = 0x80000000, FALSE = 0;

typedef std::pair<std::string, std::vector<Lit> > InterfaceVariable;
typedef std::vector<InterfaceVariable> InterfaceVariableList;

namespace __gnu_cxx {
  template <size_t N>
  struct hash<LitList(N)> {
    size_t operator()(const LitList(N)& a) const {
      size_t result = 0;
      for (size_t i = 0; i < N; ++i) {
        result *= 3;
        result += (size_t)a[i];
      }
      return result;
    }
  };

  template <>
  struct hash<Clause> {
    size_t operator()(const Clause& a) const {
      size_t result = 0;
      for (size_t i = 0; i < a.size(); ++i) {
        result *= 3;
        result += (size_t)a[i];
      }
      return result;
    }
  };
}

inline bool isConstantBool(Lit lit) {return lit == -lit;}
inline int invert(Lit lit) {if (isConstantBool(lit)) return lit ^ 0x80000000; else return -lit;}

template <size_t Len>
inline LitList(Len) invertWord(const LitList(Len)& a) {
  LitList(Len) result;
  for (size_t i = 0; i < Len; ++i) result[i] = invert(a[i]);
  return result;
}
inline Lit invertMaybe(Lit a, bool inv) {
  return inv ? invert(a) : a;
}

inline LitList(1) single(Lit a) {
  LitList(1) result;
  result[0] = a;
  return result;
}

template <size_t From, size_t To, size_t InputSize>
LitList(To - From) extract(const LitList(InputSize)& in) {
  LitList(To - From) result;
  for (size_t i = 0; i < To - From; ++i) {
    result[i] = in[i + From];
  }
  return result;
}

template <size_t Len1, size_t Len2>
LitList(Len1 + Len2) concat(const LitList(Len1)& a, const LitList(Len2)& b) {
  LitList(Len1 + Len2) result;
  for (size_t i = 0; i < Len1; ++i) result[i] = a[i];
  for (size_t i = 0; i < Len2; ++i) result[i + Len1] = b[i];
  return result;
}

template <size_t NumBits>
LitList(NumBits) number(unsigned long long n) {
  LitList(NumBits) result;
  for (size_t i = 0; i < NumBits; ++i) {
    result[i] = (n % 2 == 1) ? TRUE : FALSE;
    n >>= 1;
  }
  return result;
}

template <size_t NumBits>
unsigned long long fromNumber(const LitList(NumBits)& n) {
  unsigned long long val = 0;
  for (size_t i = 0; i < NumBits; ++i) {
    assert (n[i] == TRUE || n[i] == FALSE);
    if (n[i] == TRUE) val |= (1ULL << i);
  }
  return val;
}

inline bool absLess(Lit a, Lit b) {
  if (abs(a) < abs(b)) return true;
  if (abs(a) > abs(b)) return false;
  return a < b;
}

inline bool clauseLess(const Clause& a, const Clause& b) {
  for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
    if (abs(a[i]) < abs(b[i])) return true;
    if (abs(a[i]) > abs(b[i])) return false;
  }
  return a < b;
}

template <size_t Len>
inline std::vector<Lit> toVector(const LitList(Len)& lits) {
  return std::vector<Lit>(lits.begin(), lits.end());
}

inline std::vector<Lit> toVector(Lit l) {
  return std::vector<Lit>(1, l);
}

template <size_t Len>
inline LitList(Len) fromVector(const std::vector<Lit>& lits) {
  assert (Len == lits.size());
  LitList(Len) result;
  std::copy(lits.begin(), lits.end(), result.begin());
  return result;
}

struct UnsatisfiableException {};

struct SatProblem {
  size_t numVariables;
  std::vector<Clause> clauses;
  __gnu_cxx::hash_map<std::vector<Lit>, Lit> andGateCSE;
  __gnu_cxx::hash_map<LitList(3), Lit> muxCSE; // Fields are sel, ifTrue, ifFalse
  __gnu_cxx::hash_map<LitList(2), Lit> condEquivCSE; // Fields are the two lits being identified
  InterfaceVariableList interfaceVariables;

  public:
  SatProblem(): numVariables(0), clauses() {}

  Var newVar() {return ++numVariables;}

  template <size_t Count>
  LitList(Count) newVars() {LitList(Count) vl; for (size_t i = 0; i < Count; ++i) vl[i] = newVar(); return vl;}

  void addInterface(const std::string& name, const std::vector<Lit>& values);
  void addClause(const Clause& cl);

  template <size_t Len>
  void addClause(const LitList(Len)& cl) {
    addClause(toVector(cl));
  }

  void parse(FILE* f);
  void unparse(FILE* outfile);

  void identify(Lit a, Lit b);
  void condEquivalence(Lit sel, Lit a, Lit b);

  template <size_t Len>
  void condEquivalenceWords(Lit sel, const LitList(Len)& a, const LitList(Len)& b) {
    for (size_t i = 0; i < Len; ++i) {
      condEquivalence(sel, a[i], b[i]);
    }
  }

  Lit mux(Lit sel, Lit ifTrue, Lit ifFalse);

  Lit andAcross(const std::vector<Lit>& a);

  template <size_t Len>
  Lit andAcross(const LitList(Len)& a) {
    return andAcross(toVector(a));
  }

  Lit andGate(Lit a, Lit b) {
    LitList(2) inputs;
    inputs[0] = a;
    inputs[1] = b;
    return andAcross(inputs);
  }

  Lit nandGate(Lit a, Lit b) {
    return invert(andGate(a, b));
  }

  Lit orGate(Lit a, Lit b) {
    return invert(andGate(invert(a), invert(b)));
  }

  template <size_t Len>
  Lit orAcross(const LitList(Len)& a) {
    LitList(Len) aInv = invertWord(a);
    return invert(andAcross(aInv));
  }

  Lit norGate(Lit a, Lit b) {
    return invert(orGate(a, b));
  }

  template <size_t Len>
  Lit norAcross(const LitList(Len)& a) {
    return invert(orAcross(a));
  }

  Lit xorGate(Lit a, Lit b) {
    return mux(a, invert(b), b);
  }

  template <size_t Len>
  Lit xorAcross(const LitList(Len)& a) {
    Lit result = (Len == 0 ? FALSE : a[0]);
    for (size_t i = 1; i < Len; ++i) result = xorGate(result, a[i]);
    return result;
  }

  template <size_t Len>
  LitList(Len) invertWord(const LitList(Len)& a) {
    LitList(Len) result;
    for (size_t i = 0; i < Len; ++i) result[i] = invert(a[i]);
    return result;
  }

  template <size_t Len>
  LitList(Len) andWords(const LitList(Len)& a, const LitList(Len)& b) {
    LitList(Len) output;
    for (size_t i = 0; i < Len; ++i) {
      output[i] = andGate(a[i], b[i]);
    }
    return output;
  }

  template <size_t Len>
  LitList(Len) orWords(const LitList(Len)& a, const LitList(Len)& b) {
    LitList(Len) output;
    for (size_t i = 0; i < Len; ++i) {
      output[i] = orGate(a[i], b[i]);
    }
    return output;
  }

  template <size_t Len>
  LitList(Len) xorWords(const LitList(Len)& a, const LitList(Len)& b) {
    LitList(Len) output;
    for (size_t i = 0; i < Len; ++i) {
      output[i] = xorGate(a[i], b[i]);
    }
    return output;
  }

  template <size_t NBits>
  Lit equal(const LitList(NBits)& a, const LitList(NBits)& b) {
    return norAcross(xorWords(a, b));
  }

  template <size_t NBits>
  LitList(NBits) ite(Lit cond, const LitList(NBits)& ifTrue, const LitList(NBits)& ifFalse) {
    LitList(NBits) output;
    for (size_t i = 0; i < NBits; ++i) {
      output[i] = mux(cond, ifTrue[i], ifFalse[i]);
    }
    return output;
  }

  template <size_t NBits>
  LitList(NBits) adder(const LitList(NBits)& a, const LitList(NBits)& b, Lit carryIn = FALSE, LitList(NBits)* carriesOutOpt = NULL) {
    LitList(NBits) result;
    for (size_t i = 0; i < NBits; ++i) {
      Lit sumOut = xorGate(xorGate(a[i], b[i]), carryIn);
      Lit carryOut = orGate(andGate(a[i], orGate(b[i], carryIn)), andGate(b[i], carryIn));
      result[i] = sumOut;
      if (carriesOutOpt != NULL) {
        (*carriesOutOpt)[i] = carryOut;
      }
      carryIn = carryOut;
    }
    return result;
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) signedMultiplier(const LitList(Len1)& a, const LitList(Len2)& b) {
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) unsignedMultiplier(const LitList(Len1)& a, const LitList(Len2)& b) {
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) signedDivider(const LitList(Len1)& dividend, const LitList(Len2)& divisor) {
    // Returns Len1 bits quotient, Len2 bits remainder concatenated
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len1, size_t Len2>
  LitList(Len1 + Len2) unsignedDivider(const LitList(Len1)& dividend, const LitList(Len2)& divisor) {
    // Returns Len1 bits quotient, Len2 bits remainder concatenated
    return newVars<Len1 + Len2>(); // FIXME
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) rightShifter(const LitList(Len)& in, const LitList(SCLen)& shiftCount) {
    // Logarithmic shifter
    LitList(Len) result = in;
    for (size_t scBit = SCLen, pow2scBit = 1 << (SCLen - 1); scBit > 0; --scBit, pow2scBit >>= 1) {
      LitList(Len) resultCopy = result;
      for (size_t i = 0; i < Len; ++i) {
        resultCopy[i] = mux(shiftCount[scBit - 1], (i >= pow2scBit ? result[i - pow2scBit] : FALSE), result[i]);
      }
      result = resultCopy;
    }
    return result;
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) leftShifter(const LitList(Len)& in, const LitList(SCLen)& shiftCount) {
    // Logarithmic shifter
    LitList(Len) result = in;
    for (size_t scBit = SCLen, pow2scBit = 1 << (SCLen - 1); scBit > 0; --scBit, pow2scBit >>= 1) {
      LitList(Len) resultCopy = result;
      for (size_t i = 0; i < Len; ++i) {
        resultCopy[i] = mux(shiftCount[scBit - 1], (i + pow2scBit >= Len ? FALSE : result[i + pow2scBit]), result[i]);
      }
      result = resultCopy;
    }
    return result;
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) arithmeticLeftShifter(const LitList(Len)& in, const LitList(SCLen)& shiftCount) {
    // Logarithmic shifter
    LitList(Len) result = in;
    for (size_t scBit = SCLen, pow2scBit = 1 << (SCLen - 1); scBit > 0; --scBit, pow2scBit >>= 1) {
      LitList(Len) resultCopy = result;
      for (size_t i = 0; i < Len; ++i) {
        resultCopy[i] = mux(shiftCount[scBit - 1], (i + pow2scBit >= Len ? result[Len - 1] : result[i + pow2scBit]), result[i]);
      }
      result = resultCopy;
    }
    return result;
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) rightRotater(const LitList(Len)& in, const LitList(SCLen)& shiftCount) {
    // Logarithmic shifter
    LitList(Len) result = in;
    for (size_t scBit = SCLen, pow2scBit = 1 << (SCLen - 1); scBit > 0; --scBit, pow2scBit >>= 1) {
      LitList(Len) resultCopy = result;
      for (size_t i = 0; i < Len; ++i) {
        resultCopy[i] = mux(shiftCount[scBit - 1], result[(i - pow2scBit + Len) % Len], result[i]);
      }
      result = resultCopy;
    }
    return result;
  }

  template <size_t Len, size_t SCLen>
  LitList(Len) leftRotater(const LitList(Len)& in, const LitList(SCLen)& shiftCount) {
    // Logarithmic shifter
    LitList(Len) result = in;
    for (size_t scBit = SCLen, pow2scBit = 1 << (SCLen - 1); scBit > 0; --scBit, pow2scBit >>= 1) {
      LitList(Len) resultCopy = result;
      for (size_t i = 0; i < Len; ++i) {
        resultCopy[i] = mux(shiftCount[scBit - 1], result[(i + pow2scBit) % Len], result[i]);
      }
      result = resultCopy;
    }
    return result;
  }

  template <size_t Len>
  LitList(Len) bitReverse(const LitList(Len)& in) {
    LitList(Len) result;
    for (size_t i = Len; i > 0; --i) {
      result[i - 1] = in[Len - i];
    }
    return result;
  }

  template <size_t Len>
  LitList(Len) leastSignificantSetBit(const LitList(Len)& in) {
    LitList(Len) result = number<Len>(0);
    for (size_t i = Len; i > 0; --i) {
      result = ite(in[i - 1], number<Len>(i - 1), result);
    }
    return result;
  }

  template <size_t Len>
  LitList(Len) mostSignificantSetBit(const LitList(Len)& in) {
    LitList(Len) result = number<Len>(0);
    for (size_t i = 0; i < Len; --i) {
      result = ite(in[i], number<Len>(i), result);
    }
    return result;
  }

};

inline Lit biasLiteral(Lit a, size_t bias) {
  if (a == -a) {
    return a;
  } else if (a > 0) {
    return a + bias;
  } else {
    return a - bias;
  }
}

inline std::vector<Lit> biasLiterals(std::vector<Lit> v, size_t bias) {
  for (size_t i = 0; i < v.size(); ++i) {
    v[i] = biasLiteral(v[i], bias);
  }
  return v;
}

#endif // ROSE_SATPROBLEM_H
