
#include "maths/PolyhedronExploration.hpp"

#include <iostream>

#define N 4

int main() {
	Polyhedron p(N);
	
	std::vector<int> lower_bounds(N);
	for (size_t i = 0; i < N; i++)
		lower_bounds[i] = -1;
	std::vector<int> upper_bounds(N);
	for (size_t i = 0; i < N; i++)
		upper_bounds[i] = 1;
	
	VariableID a(0);
	VariableID b(1);
	VariableID c(2);
	VariableID d(3);
	
	p.refine_with_constraint(a >= 0);
	p.refine_with_constraint(b - a >= 0);
	p.refine_with_constraint(c - b >= 0);
	//p.refine_with_constraint(d >= 0);
	
	std::vector<int*> * res = exhaustiveExploration(p, lower_bounds, upper_bounds);
	
	std::cout << res->size() << std::endl;
	
	std::vector<int*>::iterator it;
	for (it = res->begin(); it != res->end(); it++) {
		for (size_t i = 0; i < N; i++) {
			std::cout << (*it)[i] << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}
