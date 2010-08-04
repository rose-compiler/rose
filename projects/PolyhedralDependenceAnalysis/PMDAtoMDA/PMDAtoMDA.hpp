
#include "rose.h"

#ifndef __PMDA_TO_MDA_
#define __PMDA_TO_MDA_

namespace MultiDimArrays {

void traverse(SgFunctionDeclaration * stmt);

void traverse(SgStatement * stmt);

typedef std::pair<int, std::vector<SgInitializedName *> * > factors;
typedef std::vector<factors> set_of_factors;

class IteratorAttribute : public AstAttribute {
	protected:
		bool isIterator;
	
	public:
		IteratorAttribute() : AstAttribute(), isIterator(false) {}
		
		void setIterator(bool val) { isIterator = val; }
		bool getIterator() { return isIterator; }
};

class PseudoMultiDimensionalArrayAttribute : public AstAttribute {
	protected:
		class AccessVectorComponentIR;
		class factorIR;
		
		std::vector<factorIR *> p_factors;
		std::vector<AccessVectorComponentIR *> p_components;
		
		static bool sort_factorIR_pntr(factorIR * a, factorIR * b);
		
		void proccessAnalysis();
		
	public:
		PseudoMultiDimensionalArrayAttribute(std::vector<factors> * factors_vect);
		~PseudoMultiDimensionalArrayAttribute();
		
		bool isPseudoMultiDimArray();
		
		int nbrComponent();
		SgExpression * buildComponent(int idx);
		
	friend bool operator == (PseudoMultiDimensionalArrayAttribute::factorIR & v1, PseudoMultiDimensionalArrayAttribute::factorIR & v2);
	friend bool operator < (PseudoMultiDimensionalArrayAttribute::factorIR & v1, PseudoMultiDimensionalArrayAttribute::factorIR & v2);
};

typedef PseudoMultiDimensionalArrayAttribute PMDA_Attribute;

}

#endif /* __PMDA_TO_MDA_ */
