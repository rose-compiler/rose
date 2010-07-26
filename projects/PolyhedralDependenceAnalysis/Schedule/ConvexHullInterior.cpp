
#include "ConvexHullInterior.hpp"

#include "ppl.hh"

#include <iostream>
#include <cstdlib>

namespace PolyhedralToolsPPL {

/*
class PointsPool {
	protected:
		int p_dim;
		
		int p_current_pool;
		int p_size;
		int p_top_elem;
		int ** p_pool;
		
		int p_size_free;
		int p_nbr_free;
		int ** p_list_free;

	public:
		PointsPool(int dim, int init_size, int free_size = 256) :
			p_dim(dim),
			p_current_pool(0),
			p_size(init_size), p_top_elem(0), p_pool((int**)malloc(sizeof(int*))),
			p_size_free(free_size), p_nbr_free(0), p_list_free((int**)malloc(p_size_free * sizeof(int *)))
		{
			p_pool[p_current_pool] = (int*)malloc(p_size * p_dim * sizeof(int));
		}
		
		~PointsPool() {
			delete [] p_pool;
			delete p_list_free;
		}
		
		int * allocate() {
			int * res;
//			std::cerr << "+" << std::endl;
			if (p_nbr_free > 0) {
//				std::cerr << "\t-" << std::endl;
				p_nbr_free--;
				res = p_list_free[p_nbr_free];
			}
			else if (p_top_elem < p_size) {
//				std::cerr << "\t+" << std::endl;
				p_top_elem++;
				res = p_pool[p_current_pool] + p_top_elem;
			}
			else {
//				std::cerr << "\t++" << std::endl;
				p_current_pool++;
				p_pool = (int**)realloc(p_pool, (p_current_pool+1) * sizeof(int*));
				p_pool[p_current_pool] = (int*)malloc(p_size * p_dim * sizeof(int));
				p_top_elem = 0;
				res = p_pool[p_current_pool];
			}
//			std::cerr << "*" << std::endl;
			return res;
		}
		
		void free(int * ptr) {
//			std::cerr << "-" << std::endl;
			if (p_nbr_free < p_size_free) {
//				std::cerr << "\t-" << std::endl;
				p_list_free[p_nbr_free++] = ptr;
			}
			else if (ptr == p_pool[p_current_pool] + p_top_elem && p_top_elem > 0) {
//				std::cerr << "\t+" << std::endl;
				p_top_elem--;
			}
			else {
//				std::cerr << "\t--" << std::endl;
				p_size_free = 2 * p_size_free;
				p_list_free = (int**)realloc(p_list_free, p_size_free * sizeof(int *));
				p_list_free[p_nbr_free++] = ptr;
			}
//			std::cerr << "*" << std::endl;
		}
};
*/	

/* PGCD related */

int pgcd(int a, int b) { return a==b ? a : (a>b ? pgcd(a-b, b) : pgcd(a, b-a) ); }

int pgcd_set(int * tab, int a, int b) {
	int res;
//	if (a < 0 || a >= tab->size() || b < 0 || b >= tab->size() || a > b)
//		abort();
	if (a == b) {
		res = tab[a]>0 ? tab[a] : -tab[a];
	}
	else if (a + 1 == b) {
		int _a = tab[a]>0 ? tab[a] : -tab[a];
		int _b = tab[b]>0 ? tab[b] : -tab[b];
		if (_a == 0) return _b;
		if (_b == 0) return _a;
		if (_a == _b) return _a;
		else return pgcd(_a, _b);
	}
	else {
		int _a = pgcd_set(tab, a, a + (b-a)/2);
		int _b = pgcd_set(tab, a + (b-a)/2 + 1, b);
		if (_a == 0) return _b;
		if (_b == 0) return _a;
		if (_a == _b) return _a;
		else return pgcd(_a, _b);
	}
	return res;
}

/* Vector construction */

void vectorFromPoints(int * tab, const int * p1, const int * p2, int dimension) {
	/*if (p1->size() != p2->size() || p1->size() != tab->size()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}*/
	
	for (int i = 0; i < dimension; i++) {
		tab[i] = p2[i] - p1[i];
	}
}

void pointFromPoint(int * tab, const Parma_Polyhedra_Library::Generator * p1, int dimension) {
	/*if (p1->space_dimension() != tab->size() || !p1->is_point()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}*/
	
	for (int i = 0; i < dimension; i++) {
		Parma_Polyhedra_Library::Variable var = Parma_Polyhedra_Library::Variable(i);
		tab[i] = p1->coefficient(var).get_si();
	}
}

int * create(const int * point, const int * vector, int coef, int dimension) {
	/*if (point->size() != vector->size()) {
		std::cerr << "Error in vectorFromPoints !" << std::endl;
		abort();	
	}*/
	int * res = new int[dimension];
	for (int i = 0; i < dimension; i++) {
		res[i] = point[i] + coef * vector[i];
	}
	return res;
}

/* Set of Points manipulation */

int comparePoints(const int * e1, const int * e2, int dimension) {
	int i;
	for (i = 0; i < dimension; i++)
		if (e1[i] != e2[i])
			break;
	return dimension - i;	
}

void addUniq(std::vector<int * > * set, int * element, int dimension) {
	std::vector<int * >::iterator it;
	
	for (it = set->begin(); it != set->end(); it++) {
		if (comparePoints(*it, element, dimension) == 0)
			break;
	}
	if (it == set->end())
		set->push_back(element);
	else
		delete [] element;
		
}

void addUniq(std::vector<int * > * set, std::vector<int * > * elements, int dimension) {
	std::vector<int * >::iterator it;
	
	for (it = elements->begin(); it != elements->end(); it++)
		addUniq(set, *it, dimension);
		
//	delete elements;
		
	elements->clear();
}

/* Convex-Hull Interior computation */

bool isIn(int * point, Parma_Polyhedra_Library::Polyhedron * polyhedron, int dimension) {
	Parma_Polyhedra_Library::Linear_Expression exp;
	for (int i = 0; i < dimension; i++)
		exp += point[i] * Parma_Polyhedra_Library::Variable(i);
	
	return polyhedron->relation_with(Parma_Polyhedra_Library::Generator::point(exp)) ==  Parma_Polyhedra_Library::Poly_Gen_Relation::subsumes();
}

std::vector<int * > * convexHullInterior(std::vector<int * > * convex_hull, Parma_Polyhedra_Library::Polyhedron * polyhedron, int dimension) {
	std::vector<int *> * res_set = new std::vector<int * >();
	std::vector<int *> * gen_set = NULL;
	std::vector<int *> * tmp_set = NULL;
	std::vector<int *> * xch_ptr = NULL;
	std::vector<int *>::iterator it1, it2;
	int * vector = NULL;
	int * point = NULL;
	int pgcd;
	
//	std::cerr << "\t*" << std::endl;
	
	if (convex_hull->size() == 0) {
		std::cerr << "Error in convexHullInterior: convex_hull->size() == 0 !" << std::endl;
		abort();
	}
	
//	std::cerr << "\t**" << std::endl;
	
	vector = new int[dimension];
	
	tmp_set = convex_hull;
	
//	std::cerr << "\t***" << std::endl;


	gen_set = new std::vector<int * >();

	
	int cnt = 0;
	while (tmp_set->size() > 1) {
	
		std::cerr << cnt++ << " - nbr points:" << tmp_set->size() << std::endl;
	
//		std::cerr << "\t\t*" << std::endl;
		
		for (it1 = tmp_set->begin(); it1 != tmp_set->end()-1; it1++) {
	
//			std::cerr << "\t\t\t*" << std::endl;
	
			for (it2 = it1+1; it2 != tmp_set->end(); it2++) {
	
//				std::cerr << "\t\t\t\t*" << std::endl;
	
				vectorFromPoints(vector, *it1, *it2, dimension);
	
//				std::cerr << "\t\t\t\t**" << std::endl;
				
				pgcd = pgcd_set(vector, 0, dimension - 2);
				if (pgcd > 1) {
					for (int i = 0; i < dimension - 1; i++)
						vector[i] = vector[i]/pgcd;
					for (int i = 1; i < pgcd; i++) {
						int * tmp = create(*it1, vector, i, dimension);
						addUniq(gen_set, tmp, dimension);
					}
				}
	
//				std::cerr << "\t\t\t\t***" << std::endl;
				
			}
	
//			std::cerr << "\t\t\t**" << std::endl;
			
		}
		addUniq(res_set, tmp_set, dimension);

		xch_ptr = tmp_set;
		tmp_set = gen_set;
		gen_set = xch_ptr;

		
//		std::cerr << "\t\t**" << std::endl;
	
	}
	if (gen_set) addUniq(res_set, gen_set, dimension);
	else res_set = tmp_set;
	
//	std::cerr << "\t****" << std::endl;
	
	return res_set;
}

std::vector<int * > * allPointsInPolyhedron(Parma_Polyhedra_Library::Polyhedron * polyhedron) {
	std::vector<int * > * res;
	std::vector<int * > * set_of_points = new std::vector<int * > ();
	int * point;
	int dimension = polyhedron->space_dimension();
	
//	std::cerr << "*" << std::endl;
	
	if (false && !polyhedron->is_bounded()) {
		std::cerr << "Error in allPointInPolyhedron: polyhedron->is_bounded() == false !" << std::endl;
		abort();
	}
	
//	std::cerr << "**" << std::endl;
	
	if (false && !polyhedron->contains_integer_point()) {
		std::cerr << "Error in allPointInPolyhedron: polyhedron->contains_integer_point() == false !" << std::endl;
		abort();
	}
	
//	std::cerr << "***" << std::endl;
		
//	std::cerr << "Generators..." << std::endl;
//	Parma_Polyhedra_Library::Generator_System generators = polyhedron->minimized_generators();
	Parma_Polyhedra_Library::Generator_System generators = polyhedron->generators();
	Parma_Polyhedra_Library::Generator_System::const_iterator it_generator;
//	std::cerr << "Points..." << std::endl;
	for (it_generator = generators.begin(); it_generator != generators.end(); it_generator++) {
		point = new int[dimension];
		pointFromPoint(point, &(*it_generator), dimension);
		if (it_generator->divisor() != 1) {
			// TODO
			it_generator->ascii_dump();
			
		}
		else set_of_points->push_back(point);
	}
	
//	std::cerr << "****" << std::endl;
	
//	std::cerr << "Convex-Hull Interior..." << std::endl;
	res = convexHullInterior(set_of_points, polyhedron, dimension);
	
//	if (res != set_of_points) delete set_of_points;
	
//	std::cerr << "*****" << std::endl;
	
//	printPoints(std::cout, res);
	
	return res;
}

/* Printer */
/*
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
*/
}
