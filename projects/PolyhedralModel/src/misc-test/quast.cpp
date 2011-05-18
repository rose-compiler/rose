
#include "maths/Quast.hpp"

#include <iostream>

class TEST {
	public:
		std::string p_name;
		size_t p_it;
		size_t p_g;
		Polyhedron p_domain;
		std::vector<LinearExpression> p_scattering;
	
	public:
		TEST(std::string name, size_t it, size_t g, Polyhedron & dom, std::vector<LinearExpression> scat) :
			p_name(name),
			p_it(it),
			p_g(g),
			p_domain(dom),
			p_scattering(scat)
		{}
};

std::ostream & operator << (std::ostream & out, TEST & t) {
	out << t.p_name;
	return out;
}
std::ostream & operator << (std::ostream & out, const TEST & t) {
	out << t.p_name;
	return out;
}

const Polyhedron & getDomain(const TEST * obj) {
	return obj->p_domain;
}

const std::vector<LinearExpression> & getScattering(const TEST * obj) {
	return obj->p_scattering;
}

size_t getDimension(const TEST * obj) {
	return obj->p_it;
}

size_t getExtraDimension(const TEST * obj) {
	return obj->p_g;
}

template <>
bool isBottom<TEST *>(TEST * p) {
	return p == NULL;
}

template <>
TEST * getBottom<TEST *>() {
	return NULL;
}

int main() {
	
	// S1
/*	
		Polyhedron dom_s1(2);
	
			dom_s1.refine_with_constraint(VariableID(0) >= 0);	
			dom_s1.refine_with_constraint(VariableID(0) <= VariableID(1) - 1);
	
		std::vector<LinearExpression> scat_s1;
	
			scat_s1.push_back(LinearExpression(0));
			scat_s1.push_back(VariableID(0));
			scat_s1.push_back(LinearExpression(0));
	
		TEST s1("s1", 1, 1, dom_s1, scat_s1);
*/	
	// S2
	
		Polyhedron dom_s2(3);
	
			dom_s2.refine_with_constraint(VariableID(0) >= 0);	
			dom_s2.refine_with_constraint(VariableID(0) <= VariableID(2) - 1);
			dom_s2.refine_with_constraint(VariableID(1) >= 0);	
			dom_s2.refine_with_constraint(VariableID(1) <= VariableID(2) - 1);
	
		std::vector<LinearExpression> scat_s2;
	
			scat_s2.push_back(LinearExpression(0));
			scat_s2.push_back(VariableID(0));
			scat_s2.push_back(LinearExpression(1));
			scat_s2.push_back(VariableID(1)); // case j : 0 -> n-1
//			scat_s2.push_back(- VariableID(1) + VariableID(2) - 1); //  case j : n-1 -> 0
			scat_s2.push_back(LinearExpression(0));
	
		TEST s2("s2", 2, 1, dom_s2, scat_s2);
		
	// Compute Max 2->1
/*	
		std::vector<LinearExpression> f_s1;
		
			f_s1.push_back(VariableID(0));
		
		std::vector<LinearExpression> f_s2;
		
			f_s2.push_back(VariableID(0));
			
		QUAST<TEST> * d21 = computeMaxLex<TEST>(&s2, f_s2, &s1, f_s1);
*/		
	// Compute Max 2->2
	
		std::vector<LinearExpression> f_s1;
		std::vector<LinearExpression> f_s2;

		QUAST<TEST *> * d22 = computeMaxLex<TEST *>(&s2, f_s2, &s2, f_s2);
	
//	QUAST<TEST> * d2 = maxQuast<TEST>(d21, d22);
	
	std::cout << std::endl;
	//d2->print(std::cout, "");
	d22->print(std::cout, "");
	std::cout << std::endl;
	
	return 0;
}

