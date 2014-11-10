#ifndef COMPUTE_SIGNATURE_VECTOR_H
#define COMPUTE_SIGNATURE_VECTOR_H

#include "PartialSymbolicSemantics.h"
#include "x86InstructionSemantics.h"

enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};

class SignatureVector {
public:
    static const size_t Size = V_SgNumVariants * 4 + 300 + 9 + 3;
    typedef uint16_t ElementType;

private:
    ElementType values[Size];

public:
    SignatureVector() {
        clear();
    }

    void clear() {
        for (size_t i = 0; i < Size; ++i)
            values[i] = 0;
    }

    ElementType operator[](size_t i) const {
        assert(i < Size);
        return values[i];
    }

    ElementType& totalForVariant(size_t var) {
        assert(var < V_SgNumVariants);
        return values[var * 4];
    }

    ElementType& opsForVariant(ExpressionCategory cat, size_t var) {
        assert(var < V_SgNumVariants);
        return values[var * 4 + (int)cat + 1];
    }

    ElementType& specificOp(ExpressionCategory cat, size_t num) {
	static ElementType dummyVariable = 0;
	if (num < 100) {
            return values[V_SgNumVariants * 4 + 100 * (int)cat + num];
	} else {
            return dummyVariable;
        }
    }

    ElementType& operandPair(ExpressionCategory a, ExpressionCategory b) {
        return values[V_SgNumVariants * 4 + 300 + (int)a * 3 + (int)b];
    }

    ElementType& operandTotal(ExpressionCategory a) {
        return values[V_SgNumVariants * 4 + 300 + 9 + (int)a];
    }

    const ElementType* getBase() const {
        return values;
    }
};



void
createVectorsForAllInstructions(SignatureVector& vec, std::vector<SgAsmInstruction*>& insns, std::vector<std::string> sc);

#endif
