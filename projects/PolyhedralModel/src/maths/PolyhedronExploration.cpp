/**
 * \file src/maths/PolyhedronExploration.cpp
 * \brief Implementation of polyhedron's points listing
 * \author Tristan Vanderbruggen
 * \version 0.1
 *
 * \todo l.213: throw an exception instead of error printing
 *
 * \todo l.247: throw an exception instead of error printing
 *
 * \todo l.256: throw an exception instead of error printing
 *
 * \note l.64-73: About comparaison of constraints in purpose to sort them. The "lower" constraint will see her variables schedule to be generate first.\n
 * I'm not sure about the following part: idea is if 2 constraints impact the same number of variables, we look for the number of constraints
 * impacted througt those variables. The constraint which impact more constraints has to be remove first.\n
 * To make it easier, I don't count unique constraint hit (2 variables can impact the same constraint...)
 */

#include "maths/PolyhedronExploration.hpp"

struct ConstraintEdge;

struct VariableEdge;

struct VariableEdge {
	size_t id;
	std::vector<ConstraintEdge *> cons;
	
	VariableEdge(size_t id_) :
		id(id_),
		cons()
	{}
	
	void remove(ConstraintEdge * cons_) {
		std::vector<ConstraintEdge *> tmp = cons;
		cons.clear();
		std::vector<ConstraintEdge *>::iterator it_cons;
		for (it_cons = tmp.begin(); it_cons != tmp.end(); it_cons++)
			if (*it_cons != cons_)
				cons.push_back(*it_cons);
	}
};

struct ConstraintEdge {
	Constraint cons;
	std::vector<VariableEdge *> vars;
	
	ConstraintEdge(Constraint cons_) :
		cons(cons_),
		vars()
	{}
	
	void remove(VariableEdge * vars_) {
		std::vector<VariableEdge *> tmp = vars;
		vars.clear();
		std::vector<VariableEdge *>::iterator it_vars;
		for (it_vars = tmp.begin(); it_vars != tmp.end(); it_vars++)
			if (*it_vars != vars_)
				vars.push_back(*it_vars);
	}
};

bool compareConstraintEdges(ConstraintEdge * c1, ConstraintEdge * c2) {
	if (c1->vars.size() != c2->vars.size())
		return c1->vars.size() < c2->vars.size();
	std::vector<VariableEdge *>::iterator it_vars;
	size_t cnt_cons_1 = 0;
	for (it_vars = c1->vars.begin(); it_vars != c1->vars.end(); it_vars++)
		cnt_cons_1 += (*it_vars)->cons.size();
	size_t cnt_cons_2 = 0;
	for (it_vars = c2->vars.begin(); it_vars != c2->vars.end(); it_vars++)
		cnt_cons_2 += (*it_vars)->cons.size();
	return cnt_cons_1 > cnt_cons_2;
}

struct Graph {
	std::vector<ConstraintEdge *> cons;
	std::vector<VariableEdge *> vars;
	
	Graph(const ConstraintSystem & cs, size_t dim) :
		cons(),
		vars()
	{
		for (size_t i = 0; i < dim; i++)
			vars.push_back(new VariableEdge(i));
	
		ConstraintSystem::const_iterator it;
		for (it = cs.begin(); it != cs.end(); it++) {
			ConstraintEdge * cons_ = new ConstraintEdge(*it);
			for (size_t i = 0; i < dim && i < it->space_dimension(); i++) {
				if (it->coefficient(VariableID(i)) != 0) {
					cons_->vars.push_back(vars[i]);
					vars[i]->cons.push_back(cons_);
				}
			}
			cons.push_back(cons_);
		}
	}
	
	~Graph() {
		std::vector<ConstraintEdge *>::iterator it_cons;
		for (it_cons = cons.begin(); it_cons != cons.end(); it_cons++)
			delete *it_cons;
		std::vector<VariableEdge *>::iterator it_vars;
		for (it_vars = vars.begin(); it_vars != vars.end(); it_vars++)
			delete *it_vars;
	}
	
	void removeConstraint(ConstraintEdge * cons_) {
		std::vector<VariableEdge *>::iterator it_vars;
		for (it_vars = vars.begin(); it_vars != vars.end(); it_vars++)
			(*it_vars)->remove(cons_);
		std::vector<ConstraintEdge *> tmp = cons;
		cons.clear();
		std::vector<ConstraintEdge *>::iterator it_cons;
		for (it_cons = tmp.begin(); it_cons != tmp.end(); it_cons++)
			if (*it_cons != cons_)
				cons.push_back(*it_cons);
		delete cons_;
	}
	
	void removeVariable(VariableEdge * vars_) {
		std::vector<ConstraintEdge *>::iterator it_cons;
		for (it_cons = cons.begin(); it_cons != cons.end(); it_cons++)
			(*it_cons)->remove(vars_);
		std::vector<VariableEdge *> tmp = vars;
		vars.clear();
		std::vector<VariableEdge *>::iterator it_vars;
		for (it_vars = tmp.begin(); it_vars != tmp.end(); it_vars++)
			if (*it_vars != vars_)
				vars.push_back(*it_vars);
		delete vars_;
	}
	
	void sortConstraints() {
		std::sort(cons.begin(), cons.end(), compareConstraintEdges);
	}
	
	void print() {
		size_t cnt = 0;
		std::vector<ConstraintEdge *>::const_iterator it_cons;
		std::vector<VariableEdge *>::const_iterator it_vars;
		std::cout << "Constraint" << std::endl;
		for (it_cons = cons.begin(); it_cons != cons.end(); it_cons++) {
			std::cout << "|\t" << (*it_cons) << " : ";
			for (it_vars = (*it_cons)->vars.begin(); it_vars != (*it_cons)->vars.end(); it_vars++)
				std::cout << (*it_vars)->id << "; ";
			std::cout << std::endl;
		}
		std::cout << "Variable" << std::endl;
		for (it_vars = vars.begin(); it_vars != vars.end(); it_vars++) {
			std::cout << "|\t" << (*it_vars)->id << " : ";
			for (it_cons = (*it_vars)->cons.begin(); it_cons != (*it_vars)->cons.end(); it_cons++)
				std::cout << (*it_cons) << ", ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

struct SolutionTree {
	size_t id;
	int val;
	SolutionTree * previous;
	std::vector<SolutionTree *> next;
	
	SolutionTree(size_t id_ = 0, int val_ = 0) :
		id(id_),
		val(val_),
		previous(NULL),
		next()
	{}
	
	~SolutionTree() {
		std::vector<SolutionTree *>::iterator it;
		for (it = next.begin(); it != next.end(); it++)
			delete *it;
	}
	
	void addNext(SolutionTree * next_) {
		next.push_back(next_);
		next_->previous = this;
	}
	
	void remove(SolutionTree * n) {
		std::vector<SolutionTree *>::iterator it;
		std::vector<SolutionTree *> tmp = next;
		next.clear();
		for (it = tmp.begin(); it != tmp.end(); it++)
			if (*it != n)
				next.push_back(*it);
		delete n;
	}
	
	void generate(std::vector<int*> & res, size_t dim) {
		if (next.size() > 0) {
			std::vector<SolutionTree *>::iterator it;
			for (it = next.begin(); it != next.end(); it++)
				(*it)->generate(res, dim);
		}
		else {
			int * res_ = new int[dim];
			bool * check = new bool[dim];
			for (size_t i = 0; i < dim; i++) check[i] = false;
			SolutionTree * tmp = this;
			while (tmp->previous != NULL) {
				res_[tmp->id] = tmp->val;
				check[tmp->id] = true;
				tmp = tmp->previous;
			}
			for (size_t i = 0; i < dim; i++)
				if (!check[i])
					{ std::cerr << "Not all var !" << std::endl; } 
			res.push_back(res_);
		}
	}
	
	void print(std::string indent) {
		std::cout << indent << "( " << id << " , " << val << " )" << std::endl;
		std::vector<SolutionTree *>::iterator it;
		for (it = next.begin(); it != next.end(); it++)
			(*it)->print(indent + "|\t");
	}
};

std::vector<int*> * exhaustiveExploration(Polyhedron & p, const std::vector<int> & lower_bounds_, const std::vector<int> & upper_bounds_) {
	std::vector<int> lower_bounds = lower_bounds_;
	std::vector<int> upper_bounds = upper_bounds_;
	Graph graph(p.minimized_constraints(), p.space_dimension());
	
	std::vector<ConstraintEdge *>::iterator it_cons;
	std::vector<VariableEdge *>::iterator it_vars;
	
	std::vector<size_t> last_vars_list;
	
	std::vector<VariableEdge *> uncons_var;
	for (it_vars = graph.vars.begin();it_vars != graph.vars.end(); it_vars++)
		if ((*it_vars)->cons.size() == 0)
			uncons_var.push_back(*it_vars);
	for (it_vars = uncons_var.begin();it_vars != uncons_var.end(); it_vars++) {
		last_vars_list.push_back((*it_vars)->id);
		graph.removeVariable(*it_vars);
	}
	
	graph.sortConstraints();
	if (graph.cons.front()->vars.size() == 0)
		{ std::cerr << "Constraint without var ref." << std::endl; }
	while (graph.cons.size() > 0) {
		ConstraintEdge * current = graph.cons.front();
		
		if (current->vars.size() > 1) break;
		
		size_t id = current->vars[0]->id;
		int coef = current->cons.coefficient(VariableID(id)).get_si();
		int inho = current->cons.inhomogeneous_term().get_si();
			{ std::cerr << "Will give a fraction" << std::endl; }
		if (current->cons.is_equality()) {
			lower_bounds[id] = upper_bounds[id] = -inho/coef;
		}
		else {
			if (coef > 0)
				lower_bounds[id] = -inho/coef;
			else
				upper_bounds[id] = inho/coef;
		}
		if (current->vars[0]->cons.size() == 1) {
			last_vars_list.push_back(current->vars[0]->id);
			graph.removeVariable(current->vars[0]);
		}
		graph.removeConstraint(current);
	}
	
	std::vector<std::pair<Constraint, size_t> > cons_list;
	std::vector<size_t> vars_list;
	
	while (graph.cons.size() > 0) {
		graph.sortConstraints();
		
		ConstraintEdge * current;
		
		while (graph.cons.size() > 0) {
			current = graph.cons.front();
			if (current->vars.size() == 0) {
				cons_list.push_back(std::pair<Constraint, size_t>(current->cons, vars_list.back()));
				graph.removeConstraint(current);
			}
			else break;
		}
		
		if (graph.cons.size() > 0) {
			while (current->vars.size() > 0) {
				vars_list.push_back(current->vars.front()->id);
				graph.removeVariable(current->vars.front());
			}
		}
	}
	
	SolutionTree * root = new SolutionTree();
	std::vector<size_t>::iterator vars_it = vars_list.begin();
	std::vector<SolutionTree *> * leafs = new std::vector<SolutionTree *>();
	std::vector<SolutionTree *>::iterator it_leaf;
	leafs->push_back(root);
	while (cons_list.size() != 0) {
		std::vector<SolutionTree *> * tmp_leafs = new std::vector<SolutionTree *>();
		for (it_leaf = leafs->begin(); it_leaf != leafs->end(); it_leaf++) {
			for (int val = lower_bounds[*vars_it]; val <= upper_bounds[*vars_it]; val++) {
				SolutionTree * tmp = new SolutionTree(*vars_it, val);
				(*it_leaf)->addNext(tmp);
				tmp_leafs->push_back(tmp);
			}
		}
		
		while (cons_list.size() != 0 && *vars_it == cons_list.front().second) {
			std::vector<SolutionTree *> * out_tmp_leafs = new std::vector<SolutionTree *>();
			std::vector<std::pair<Constraint, size_t> > tmp_cons_list = cons_list;
			cons_list.clear();
			for (it_leaf = tmp_leafs->begin(); it_leaf != tmp_leafs->end(); it_leaf++) {
				bool is_wrong = true;
				SolutionTree * tmp = *it_leaf;
				int res = 0;
				while (tmp != NULL && tmp->previous != NULL) {
					res += tmp_cons_list.front().first.coefficient(VariableID(tmp->id)).get_si() * tmp->val;
					tmp = tmp->previous;
				}
				res += tmp_cons_list.front().first.inhomogeneous_term().get_si();
				if (tmp_cons_list.front().first.is_equality()) {
					is_wrong = res != 0;
				}
				else {
					is_wrong = res < 0;
				}
				if (is_wrong) {
					SolutionTree * node = *it_leaf;
					while (node->previous != NULL) {
						tmp = node->previous;
						tmp->remove(node);
						node = tmp;
						if (tmp->next.size() > 0)
							break;
					}
				}
				else {
					out_tmp_leafs->push_back(*it_leaf);
				}
			}
			cons_list.assign(tmp_cons_list.begin() + 1, tmp_cons_list.end());
			delete tmp_leafs;
			tmp_leafs = out_tmp_leafs;
		}
		delete leafs;
		leafs = tmp_leafs;
		
		vars_it++;
	}
	
	for (vars_it = last_vars_list.begin(); vars_it != last_vars_list.end(); vars_it++) {
		std::vector<SolutionTree *> * tmp_leafs = new std::vector<SolutionTree *>();
		for (it_leaf = leafs->begin(); it_leaf != leafs->end(); it_leaf++) {
			for (int val = lower_bounds[*vars_it]; val <= upper_bounds[*vars_it]; val++) {
				SolutionTree * tmp = new SolutionTree(*vars_it, val);
				(*it_leaf)->addNext(tmp);
				tmp_leafs->push_back(tmp);
			}
		}
		delete leafs;
		leafs = tmp_leafs;
	}
	
	std::vector<int*> * res = new std::vector<int*>();
	
	root->generate(*res, p.space_dimension());
	delete root;
	
	return res;
}

