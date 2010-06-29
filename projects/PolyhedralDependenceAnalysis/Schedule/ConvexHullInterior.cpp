
#include "ConvexHullInterior.hpp"

#include "ppl.hh"

#include <iostream>

namespace PolyhedralToolsPPL {

/* PGCD related */

int pgcd(int a, int b) { return a==b ? a : (a>b ? pgcd(a-b, b) : pgcd(a, b-a) ); }

int pgcd_set(std::vector<int> * tab, int a, int b) {
	int res;
	if (a < 0 || a >= tab->size() || b < 0 || b >= tab->size() || a > b)
		abort();
	if (a == b) {
		res = (*tab)[a]>0 ? (*tab)[a] : -(*tab)[a];
	}
	else if (a + 1 == b) {
		int _a = (*tab)[a]>0 ? (*tab)[a] : -(*tab)[a];
		int _b = (*tab)[b]>0 ? (*tab)[b] : -(*tab)[b];
		if (_a == 0) _a = _b;
		if (_b == 0) _b = _a;
		if (_a == _b) res = _a;
		else res = pgcd(_a, _b);
	}
	else {
		int _a = pgcd_set(tab, a, a + (b-a)/2);
		int _b = pgcd_set(tab, a + (b-a)/2 + 1, b);
		if (_a == 0) _a = _b;
		if (_b == 0) _b = _a;
		if (_a == _b) res = _a;
		else res = pgcd(_a, _b);
	}
	return res;
}

/* Vector construction */

void vectorFromPoints(std::vector<int> * tab, const std::vector<int> * p1, const std::vector<int> * p2) {
	if (p1->size() != p2->size() || p1->size() != tab->size()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}
	
	for (int i = 0; i < tab->size(); i++) {
		(*tab)[i] = (*p2)[i] - (*p1)[i];
	}
}

void pointFromPoint(std::vector<int> * tab, const Parma_Polyhedra_Library::Generator * p1) {
	if (p1->space_dimension() != tab->size() || !p1->is_point()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}
	
	for (int i = 0; i < tab->size(); i++) {
		Parma_Polyhedra_Library::Variable var = Parma_Polyhedra_Library::Variable(i);
		(*tab)[i] = p1->coefficient(var).get_si();
	}
}

std::vector<int> * create(const std::vector<int> * point, const std::vector<int> * vector, int coef) {
	if (point->size() != vector->size()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}
	std::vector<int> * res = new std::vector<int>(point->size());
	for (int i = 0; i < point->size(); i++) {
		(*res)[i] = (*point)[i] + coef * (*vector)[i];
	}
	return res;
}

/* Set of Points manipulation */

int comparePoints(std::vector<int> * e1, std::vector<int> * e2) {
	int i;
	if (e1->size() == e2->size()) {
		for (i = 0; i < e1->size(); i++)
			if ((*e1)[i] != (*e2)[i])
				break;
		return e1->size() - i;
	}
	else {
		std::cerr << "Error in comparePoints !" << std::endl;
		abort();
	}		
}

void addUniq(std::vector<std::vector<int> * > * set, std::vector<int> * element) {
	std::vector<std::vector<int> * >::iterator it;
	
	for (it = set->begin(); it != set->end(); it++) {
		if (comparePoints(*it, element) == 0)
			break;
	}
	if (it == set->end())
		set->push_back(element);
	else
		delete element;
}

void addUniq(std::vector<std::vector<int> * > * set, std::vector<std::vector<int> * > * elements) {
	std::vector<std::vector<int> * >::iterator it;
	
	for (it = elements->begin(); it != elements->end(); it++)
		addUniq(set, *it);
		
	delete elements;
}

/* Convex-Hull Interior computation */

bool isIn(std::vector<int> * point, Parma_Polyhedra_Library::Polyhedron * polyhedron) {
	Parma_Polyhedra_Library::Linear_Expression exp;
	for (int i = 0; i < point->size(); i++)
		exp += point->at(i) * Parma_Polyhedra_Library::Variable(i);
	
	return polyhedron->relation_with(Parma_Polyhedra_Library::Generator::point(exp)) ==  Parma_Polyhedra_Library::Poly_Gen_Relation::subsumes();
}

std::vector<std::vector<int> * > * convexHullInterior(std::vector<std::vector<int> * > * convex_hull, Parma_Polyhedra_Library::Polyhedron * polyhedron) {
	std::vector<std::vector<int> * > * res_set = new std::vector<std::vector<int> * >();
	std::vector<std::vector<int> * > * gen_set;
	std::vector<std::vector<int> * > * tmp_set;
	std::vector<std::vector<int> * >::iterator it1, it2;
	std::vector<int> * vector;
	std::vector<int> * point;
	int dimension;
	int pgcd;
	
	if (convex_hull->size() > 0)
		dimension = (*convex_hull)[0]->size();
	else {
		std::cerr << "Error in convexHullInterior: convex_hull->size() == 0 !" << std::endl;
		abort();
	}
	
	vector = new std::vector<int>(dimension);
	
	tmp_set = convex_hull;
		
	while (tmp_set->size() > 1) {
		gen_set = new std::vector<std::vector<int> * >();
		for (it1 = tmp_set->begin(); it1 != tmp_set->end()-1; it1++) {
			for (it2 = it1+1; it2 != tmp_set->end(); it2++) {
				vectorFromPoints(vector, *it1, *it2);
				
				pgcd = pgcd_set(vector, 0, vector->size() - 1);
				if (pgcd > 1) {
					for (int i = 0; i < vector->size(); i++)
						(*vector)[i] = (*vector)[i]/pgcd;
					for (int i = 1; i < pgcd; i++) {
						std::vector<int> * tmp = create(*it1, vector, i);
						if (isIn(tmp, polyhedron))
							addUniq(gen_set, tmp);
					}
				}
			}
		}
		addUniq(res_set, tmp_set);
		tmp_set = gen_set;
	}
	addUniq(res_set, gen_set);
	
	return res_set;
}

std::vector<std::vector<int> * > * allPointsInPolyhedron(Parma_Polyhedra_Library::Polyhedron * polyhedron) {
	std::vector<std::vector<int> * > * res;
	std::vector<std::vector<int> * > * set_of_points = new std::vector<std::vector<int> * > ();
	std::vector<int> * point;
	
	Parma_Polyhedra_Library::Generator_System generators = polyhedron->minimized_generators();
	Parma_Polyhedra_Library::Generator_System::const_iterator it_generator;
	
	if (!polyhedron->is_bounded()) {
		std::cerr << "Error in allPointInPolyhedron: polyhedron->is_bounded() == false !" << std::endl;
		abort();
	}

	for (it_generator = generators.begin(); it_generator != generators.end(); it_generator++) {
		point = new std::vector<int>(polyhedron->space_dimension());
		pointFromPoint(point, &(*it_generator));
		set_of_points->push_back(point);
	}
	
	res = convexHullInterior(set_of_points, polyhedron);
	
	std::vector<std::vector<int> * >::iterator it = res->begin();
	while (it != res->end())
		if (!isIn(*it, polyhedron))
			it = res->erase(it);
		else
			it++;
	
	return res;
}

/* Printer */

void printPoints(std::ostream & out, std::vector<std::vector<int> * > * set) {
	std::vector<std::vector<int> * >::iterator it;
	int cnt = 1;
	
	int dim = (*set)[0]->size();
	
	out << "|------||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << std::endl;
	out << "| #### || ";
	for (int i = 0; i < dim; i++) out << std::setw(3) << i << " | ";
	out << std::endl;
	out << "|------||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << std::endl;
	
	for (it = set->begin(); it != set->end(); it++) {
		out << "| " << std::setw(4) << cnt++ << " || ";
		for (int i = 0; i < dim; i++) out << std::setw(3) << (**it)[i] << " | ";
		out << std::endl;
	}
	
	out << "|------||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << std::endl;
}

}
