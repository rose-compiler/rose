
#include "ppl.hh"

#include "CodeGenerator.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <utility>

namespace CodeGenerator {

void print(Parma_Polyhedra_Library::C_Polyhedron * polyhedron, std::ostream & out) {

	Parma_Polyhedra_Library::Generator_System generators = polyhedron->minimized_generators();
	Parma_Polyhedra_Library::Generator_System::const_iterator it_generator;
	int cnt = 1;
	int dim = polyhedron->space_dimension();
	
	out << "|------|---|---|---|---||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << "-----|";
	out << std::endl;
	out << "| #### | l | p | r | c || ";
	for (int i = 0; i < dim; i++) out << std::setw(3) << i << " | ";
	out << " d  |";
	out << std::endl;
	out << "|------|---|---|---|---||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << "-----|";
	out << std::endl;
	for (it_generator = generators.begin(); it_generator != generators.end(); it_generator++) {
		out << "| " << std::setw(4) << cnt++ << " | ";
		out << (*it_generator).is_line() << " | ";
		out << (*it_generator).is_point() << " | ";
		out << (*it_generator).is_ray()  << " | ";
		out << (*it_generator).is_closure_point()  << " || ";
		for (int i = 0; i < dim; i++) out << std::setw(3) << it_generator->coefficient(Parma_Polyhedra_Library::Variable(i)) << " | ";
		out << std::setw(3);
		if ((*it_generator).is_point() || (*it_generator).is_closure_point())
			out << it_generator->divisor();
		else
			out << 'X';
		out << " | ";
		out << std::endl;
	}
	out << "|------|---|---|---|---||";
	for (int i = 0; i < dim; i++) out << "-----|";
	out << "-----|";
	out << std::endl;
}

void printFor(Parma_Polyhedra_Library::C_Polyhedron * polyhedron, int deep, FadaToPPL::PolyhedricContext * ctx, std::ostream & out) {

	out << "for ";
		
	const Parma_Polyhedra_Library::Constraint_System & constraints = polyhedron->minimized_constraints();
	Parma_Polyhedra_Library::Constraint_System::const_iterator constraint_it;
	for (constraint_it = constraints.begin(); constraint_it != constraints.end(); constraint_it++) {
		if (constraint_it->coefficient(Parma_Polyhedra_Library::Variable(deep-1)) != 0) {
			for (int i = 0; i < constraint_it->space_dimension(); i++) {
				int coef = constraint_it->coefficient(Parma_Polyhedra_Library::Variable(i)).get_si();
				if (coef != 0) {
					if (coef > 0)
						out << " + ";
					else {
						out << " - ";
						coef = -coef;
					}
					if (coef > 1)
						out << coef << " * ";
					if (i < deep)
						out << "it_" << i;
					else
						out << ctx->getGlobalName(i-deep);
				}
			}
			int coef = constraint_it->inhomogeneous_term().get_si();
			if (coef != 0) {
				if (coef > 0)
					out << " + ";
				else {
					out << " - ";
					coef = -coef;
				}
				out << coef;
			}
			if (constraint_it->is_equality())
				out << "==";
			else
				out << ">=";
			out << " 0 | ";
		}
	}
		
	out << std::endl;
}

void parrallelForTag(Parma_Polyhedra_Library::C_Polyhedron * polyhedron_, int deep, FadaToPPL::PolyhedricContext * ctx, std::ostream & out) {
	Parma_Polyhedra_Library::C_Polyhedron * polyhedron = new Parma_Polyhedra_Library::C_Polyhedron(*polyhedron_);

	Parma_Polyhedra_Library::Variables_Set set(Parma_Polyhedra_Library::Variable(deep), Parma_Polyhedra_Library::Variable(deep + ctx->getNbrGlobals() - 1));
	polyhedron->remove_space_dimensions(set);

//	print(polyhedron, out);
//	print(polyhedron_, out);

	Parma_Polyhedra_Library::Coefficient max_p;
	Parma_Polyhedra_Library::Coefficient min_p;
	Parma_Polyhedra_Library::Coefficient d;
	bool max;
	bool min;
	Parma_Polyhedra_Library::Linear_Expression exp(Parma_Polyhedra_Library::Variable(polyhedron->space_dimension() - 1));
	if (polyhedron->maximize(exp, max_p, d, max) && 
		polyhedron->minimize(exp, min_p, d, min) &&
		max_p == min_p)
	out << "parrallel ";
	
	delete polyhedron;
}

class ExpandDimMap {
	protected:
		int p_deep;
		int p_nb_globals;
		int p_delta;

	public:
		ExpandDimMap(int deep, int nb_globals, int delta) : p_deep(deep), p_nb_globals(nb_globals), p_delta(delta) {}
		
		bool has_empty_codomain() const { return false; }
		Parma_Polyhedra_Library::dimension_type max_in_codomain() const { return p_deep + p_nb_globals - 1; }
		bool maps(Parma_Polyhedra_Library::dimension_type i, Parma_Polyhedra_Library::dimension_type & j) const {
			if (i < p_deep - 1)
				j = i;
			else if (i < p_deep + p_nb_globals - 1)
				j = i + p_delta;
			else
				j = i - p_nb_globals;
			return true;
		}
};

class ReduceDimMap {
	protected:
		int p_deep;
		int p_nb_globals;
		int p_delta;

	public:
		ReduceDimMap(int deep, int nb_globals, int delta) : p_deep(deep), p_nb_globals(nb_globals), p_delta(delta) {}
		
		bool has_empty_codomain() const { return false; }
		Parma_Polyhedra_Library::dimension_type max_in_codomain() const { return p_deep + p_nb_globals - 1; }
		bool maps(Parma_Polyhedra_Library::dimension_type i, Parma_Polyhedra_Library::dimension_type & j) const {
			if (i <= p_deep)
				j = i;
			else if (i > p_deep + p_delta)
				j = i - p_delta;
			else
				return false;
			return true;
		}
};

bool isEmpty(Parma_Polyhedra_Library::C_Polyhedron * polyhedron_, int deep, FadaToPPL::PolyhedricContext * ctx) {
	Parma_Polyhedra_Library::C_Polyhedron * polyhedron;
	bool copy = false;
	
	if (polyhedron_->space_dimension() > deep + ctx->getNbrGlobals()) {
		polyhedron = new Parma_Polyhedra_Library::C_Polyhedron(*polyhedron_);
		polyhedron->remove_higher_space_dimensions(deep + ctx->getNbrGlobals());
		copy = true;
	}
	else
		polyhedron = polyhedron_;

	bool res = !polyhedron->constrains(Parma_Polyhedra_Library::Variable(deep - 1)) || polyhedron->is_empty();

	if (copy) delete polyhedron;

	return res;
}

class SetSorter {
	protected:
		int p_deep;
		FadaToPPL::PolyhedricContext * p_ctx;
	public:
		SetSorter(int deep, FadaToPPL::PolyhedricContext * ctx): p_deep(deep), p_ctx(ctx) {}

		bool operator () (std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> p1, std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> p2) {
			Parma_Polyhedra_Library::Coefficient max_p1;
			Parma_Polyhedra_Library::Coefficient min_p2;
			Parma_Polyhedra_Library::Coefficient d;
			bool max;
			bool min;
			
			bool sched_order = 
				p1.first->maximize(Parma_Polyhedra_Library::Linear_Expression(Parma_Polyhedra_Library::Variable(p1.first->space_dimension() - 1)), max_p1, d, max) && 
				p2.first->minimize(Parma_Polyhedra_Library::Linear_Expression(Parma_Polyhedra_Library::Variable(p2.first->space_dimension() - 1)), min_p2, d, min) &&
				max_p1 < min_p2;
				
			Parma_Polyhedra_Library::Coefficient min_it_p1;
			Parma_Polyhedra_Library::Coefficient min_it_p2;
			int cnt = 0;
			while (!sched_order && cnt < p_deep) {
				Parma_Polyhedra_Library::Linear_Expression exp(Parma_Polyhedra_Library::Variable(cnt++));
				sched_order = p1.first->minimize(exp, min_it_p1, d, max) && p2.first->minimize(exp, min_it_p2, d, min) && min_it_p1 < min_it_p2;
			}
			
			return sched_order;
		}
};

void computeDisjointPolhedrons(
	const Parma_Polyhedra_Library::C_Polyhedron * p1, const Parma_Polyhedra_Library::C_Polyhedron * p2,
	Parma_Polyhedra_Library::C_Polyhedron ** intersection,
	Parma_Polyhedra_Library::C_Polyhedron ** difference12,
	Parma_Polyhedra_Library::C_Polyhedron ** difference21,
	int deep, FadaToPPL::PolyhedricContext * ctx
) {

	*intersection = new Parma_Polyhedra_Library::C_Polyhedron(*p1);
	(*intersection)->intersection_assign_and_minimize(*p2);
	
	if (isEmpty(*intersection, deep, ctx)) {
		delete *intersection;
		*intersection = NULL;
	}
	
	*difference12 = new Parma_Polyhedra_Library::C_Polyhedron(*p1);
	(*difference12)->poly_difference_assign(*p2);
	if (isEmpty(*difference12, deep, ctx)) {
		delete *difference12;
		*difference12 = NULL;
	}
	
	*difference21 = new Parma_Polyhedra_Library::C_Polyhedron(*p2);
	(*difference21)->poly_difference_assign(*p1);
	if (isEmpty(*difference21, deep, ctx)) {
		delete *difference21;
		*difference21 = NULL;
	}
	
	if (*intersection) {
//		std::cout << "\t\t\t\t\tintersection" << std::endl;
//		print(*intersection, std::cout);
		Parma_Polyhedra_Library::Coefficient max_i, min_i, max_d12, min_d12, max_d21, min_d21, d;
		bool max, min;
		Parma_Polyhedra_Library::Variable var(deep - 1);
		
		bool min_inter = (*intersection)->minimize(var, min_i, d, min);
		bool max_inter = (*intersection)->maximize(var, max_i, d, max);
		
		if (*difference12) {
//			std::cout << "\t\t\t\t\tdifference12" << std::endl;
//			print(*difference12, std::cout);
			bool min_diff = (*difference12)->minimize(var, min_d12, d, min);
			bool max_diff = (*difference12)->maximize(var, max_d12, d, max);
			if (max_inter && min_diff && min_d12 == max_i)
				(*difference12)->add_constraint(var >= min_d12 + 1);
			if (min_inter && max_diff && max_d12 == min_i)
				(*difference12)->add_constraint(var <= max_d12 - 1);
			
			if (isEmpty(*difference12, deep, ctx)) {
				delete *difference12;
				*difference12 = NULL;
			}
		}
		if (*difference21) {
//			std::cout << "\t\t\t\t\tdifference21" << std::endl;
//			print(*difference21, std::cout);
			bool min_diff = (*difference21)->minimize(var, min_d21, d, min);
			bool max_diff = (*difference21)->maximize(var, max_d21, d, max);
			if (max_inter && min_diff && min_d21 == max_i)
				(*difference21)->add_constraint(var >= min_d21 + 1);
			if (min_inter && max_diff && max_d21 == min_i)
				(*difference21)->add_constraint(var <= max_d21 - 1);
			
			if (isEmpty(*difference21, deep, ctx)) {
				delete *difference21;
				*difference21 = NULL;
			}
		}
	}
}

std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> > * computeDisjointSet(
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> > * in_set,
	std::pair<SgStatement *, Parma_Polyhedra_Library::C_Polyhedron *> element,
	int deep,
	FadaToPPL::PolyhedricContext * ctx
) {
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> > * res = new std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> >();
	std::vector<SgStatement *> * tmp_stmt_set;
	
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> >::iterator in_set_it;
	
//	std::cout << "\t." << std::endl;
	
	bool all_set_empty = true; // or 'in_set' empty
	for (in_set_it = in_set->begin(); in_set_it != in_set->end(); in_set_it++) {
		if (isEmpty(in_set_it->first, deep, ctx)) {
			all_set_empty &= true;
			res->push_back(*in_set_it);
		}
		else {
			all_set_empty &= false;
		}
	}
	
//	std::cout << "\t.." << std::endl;
	
	if (all_set_empty) {
		tmp_stmt_set = new std::vector<SgStatement *>();
		tmp_stmt_set->push_back(element.first);
		res->push_back(std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *>(element.second, tmp_stmt_set));
	}		
	else {
		for (in_set_it = in_set->begin(); in_set_it != in_set->end(); in_set_it++) {
			if (!isEmpty(in_set_it->first, deep, ctx)) {
				Parma_Polyhedra_Library::C_Polyhedron * amb;
				Parma_Polyhedra_Library::C_Polyhedron * bma;
				Parma_Polyhedra_Library::C_Polyhedron * aib;
				
				computeDisjointPolhedrons(in_set_it->first, element.second, &aib, &amb, &bma, deep, ctx);
				
				delete in_set_it->first;
				
				if (amb) {
//					std::cout << "amb" << std::endl;
					tmp_stmt_set = new std::vector<SgStatement *>(*in_set_it->second);
					res->push_back(std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *>(amb, tmp_stmt_set));
				}
				
				if (aib) {
//					std::cout << "aib" << std::endl;
					tmp_stmt_set = new std::vector<SgStatement *>(*in_set_it->second);
					tmp_stmt_set->push_back(element.first);
					res->push_back(std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *>(aib, tmp_stmt_set));
				}
				
				if (bma) {
//					std::cout << "bma" << std::endl;
					tmp_stmt_set = new std::vector<SgStatement *>();
					tmp_stmt_set->push_back(element.first);
					res->push_back(std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *>(bma, tmp_stmt_set));
				}
	
//				std::cout << "\t..." << std::endl;
				
				delete in_set_it->second;
					
			}
		}
		
		delete element.second;
	}
	
//	std::cout << "\t...." << std::endl;
	
	return res;
}
	
void recurs(std::vector<SgStatement *> * Tset, Parma_Polyhedra_Library::C_Polyhedron * context_, int deep, PolyhedralSchedule::ProgramSchedule * schedule, FadaToPPL::PolyhedricContext * ctx) {
	std::map<SgStatement *, Parma_Polyhedra_Library::C_Polyhedron *> projections;

	std::vector<SgStatement *>::iterator stmt_it;
	
//	std::cout << "Tset" << std::endl;
//	for (stmt_it = Tset->begin(); stmt_it != Tset->end(); stmt_it++) {
//		std::cout << *stmt_it << std::endl;
//		print(ctx->getDomain(*stmt_it)->getDomain(), std::cout);
//	}
	
	Parma_Polyhedra_Library::C_Polyhedron * context;
	if (context_->space_dimension() > 0) {
		context = new Parma_Polyhedra_Library::C_Polyhedron(*context_);
		context->expand_space_dimension(Parma_Polyhedra_Library::Variable(0), 1);
		context->unconstrain(Parma_Polyhedra_Library::Variable(context->space_dimension() - 1));
		context->map_space_dimensions<ExpandDimMap>(ExpandDimMap(deep, ctx->getNbrGlobals(), 1));
	}
	else {
		context = new Parma_Polyhedra_Library::C_Polyhedron(1);
	}
	
//	std::cout << "." << std::endl;
	
	for (stmt_it = Tset->begin(); stmt_it != Tset->end(); stmt_it++) {
		Parma_Polyhedra_Library::C_Polyhedron * tmp = new Parma_Polyhedra_Library::C_Polyhedron(*ctx->getDomain(*stmt_it)->getDomain());
		int prev_dim = tmp->space_dimension();
	
//		std::cout << "\t." << std::endl;
	
		if (prev_dim < deep + ctx->getNbrGlobals()) {
			tmp->expand_space_dimension(Parma_Polyhedra_Library::Variable(0), deep + ctx->getNbrGlobals() - prev_dim);
			Parma_Polyhedra_Library::Variables_Set set(Parma_Polyhedra_Library::Variable(prev_dim), Parma_Polyhedra_Library::Variable(deep + ctx->getNbrGlobals() - 1));
			tmp->unconstrain(set);
//			for (int i = prev_dim; i < deep + ctx->getNbrGlobals(); i++)
//				tmp->unconstrain(Parma_Polyhedra_Library::Variable(i));
			tmp->map_space_dimensions<ExpandDimMap>(ExpandDimMap(deep, ctx->getNbrGlobals(), deep + ctx->getNbrGlobals() - prev_dim));
		}
		else if (prev_dim > deep + ctx->getNbrGlobals()) {
//			Parma_Polyhedra_Library::Variables_Set set(Parma_Polyhedra_Library::Variable(deep + ctx->getNbrGlobals()), Parma_Polyhedra_Library::Variable(prev_dim-1));
			Parma_Polyhedra_Library::Variables_Set set(Parma_Polyhedra_Library::Variable(deep), Parma_Polyhedra_Library::Variable(prev_dim-1-ctx->getNbrGlobals()));
//			tmp->unconstrain(set);
			tmp->remove_space_dimensions(set);
//			tmp->map_space_dimensions<ReduceDimMap>(ReduceDimMap(deep, ctx->getGlobals()->size(), prev_dim - (deep + ctx->getNbrGlobals())));
		}
	
//		std::cout << "\t.." << std::endl;
		
		tmp->intersection_assign(*context);
		
		projections.insert(std::pair<SgStatement *, Parma_Polyhedra_Library::C_Polyhedron *>(*stmt_it, tmp));
	}
	
//	std::cout << ".." << std::endl;
	
	// disjoint
	
	std::map<SgStatement *, Parma_Polyhedra_Library::C_Polyhedron *>::iterator projection_it;
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> > * disjoint_set = new std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> >();
	
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> > * tmp_disjoint_set;
	std::vector<SgStatement *> * tmp_stmt_set;
	
//	std::cout << "projections:" << std::endl;
//	for (projection_it = projections.begin(); projection_it != projections.end(); projection_it++) {
//		std::cout << projection_it->first << std::endl;
//		print(projection_it->second, std::cout);
//	}
	
	for (projection_it = projections.begin(); projection_it != projections.end(); projection_it++) {
		projection_it->second->expand_space_dimension(Parma_Polyhedra_Library::Variable(0), 1);
		
		Parma_Polyhedra_Library::Variable schedule_var(projection_it->second->space_dimension() - 1);
		projection_it->second->unconstrain(schedule_var);
		
		FadaToPPL::PolyhedricIterationDomain * stmt_domain = ctx->getDomain(projection_it->first);
		PolyhedralSchedule::StatementSchedule * stmt_schedule = schedule->getSchedule(projection_it->first);
		
		Parma_Polyhedra_Library::Linear_Expression schedule_exp;
		for (int i = 0; i < std::min(stmt_domain->getNbrIterator(), deep); i++) {
			schedule_exp += Parma_Polyhedra_Library::Variable(i) * stmt_schedule->getIteratorCoef(i);
		}
		for (int i = 0; i < ctx->getNbrGlobals(); i++) {
			schedule_exp += Parma_Polyhedra_Library::Variable(deep + i) * stmt_schedule->getGlobalCoef(i);
		}
		if (deep > stmt_domain->getNbrIterator())
			schedule_exp += stmt_schedule->getConstant();
		projection_it->second->add_constraint(schedule_var == schedule_exp);
	
		if (isEmpty(projection_it->second, deep, ctx)) {
			tmp_stmt_set = new std::vector<SgStatement *>();
			tmp_stmt_set->push_back(projection_it->first);
			disjoint_set->push_back(std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *>(projection_it->second, tmp_stmt_set));
		}
		else {
			tmp_disjoint_set = computeDisjointSet(disjoint_set, *projection_it, deep, ctx);
			delete disjoint_set;
			disjoint_set = tmp_disjoint_set;
		}
	}
	
//	std::cout << "..." << std::endl;
	
	// sort
	
	std::sort(disjoint_set->begin(), disjoint_set->end(), SetSorter(deep, ctx));
	
//	std::cout << "...." << std::endl;
	
	// recurs
	
	std::vector<std::pair<Parma_Polyhedra_Library::C_Polyhedron *, std::vector<SgStatement *> *> >::iterator set_it;
	
//	std::cout << "disjoint_set:" << std::endl;
//	for (set_it = disjoint_set->begin(); set_it != disjoint_set->end(); set_it++) {
//		print(set_it->first, std::cout);
//	}
	
//	for (set_it = disjoint_set->begin(); set_it != disjoint_set->end(); set_it++)
//		set_it->first->remove_higher_space_dimensions(set_it->first->space_dimension() - 1);
	
	for (set_it = disjoint_set->begin(); set_it != disjoint_set->end(); set_it++) {
	
//		print(set_it->first, std::cout);
		/*for (std::vector<SgStatement *>::iterator it = set_it->second->begin(); it != set_it->second->end(); it++)
			std::cout << *it << " ";
		std::cout << std::endl;*/
			
		if (!isEmpty(set_it->first, deep, ctx)) {
//			print(set_it->first, std::cout);
			for (int i = 1; i < deep; i++) std::cout << "\t";
			
			parrallelForTag(set_it->first, deep, ctx, std::cout);
			set_it->first->remove_higher_space_dimensions(set_it->first->space_dimension() - 1);
			printFor(set_it->first, deep, ctx, std::cout);
			
			recurs(set_it->second, set_it->first, deep+1, schedule, ctx);
		}
		else {
			for (std::vector<SgStatement *>::iterator it = set_it->second->begin(); it != set_it->second->end(); it++) {
				for (int i = 1; i < deep; i++) std::cout << "\t";
				std::cout << *it << std::endl;
			}
		}
//		std::cout << std::endl << std::endl;
	}
	
//	std::cout << "....." << std::endl;
	
	// merge
	
	delete context;
}

	
void generate(PolyhedralSchedule::ProgramSchedule * schedule, FadaToPPL::PolyhedricContext * ctx) {
	Parma_Polyhedra_Library::C_Polyhedron * context = new Parma_Polyhedra_Library::C_Polyhedron(ctx->getNbrGlobals());
	for (int i = 0; i < ctx->getNbrGlobals(); i++)
		context->add_constraint(Parma_Polyhedra_Library::Variable(i) >= 0);
	recurs(ctx->getStatements(), context, 1, schedule, ctx);
}
	
}
