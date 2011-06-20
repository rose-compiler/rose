/**
 * \file src/maths/Quast.tpp
 * \brief Implementation of Quast (template)
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Type>
QUAST<Type> * computeMaxLex(Type from, const std::vector<LinearExpression> & f_from, Type to, const std::vector<LinearExpression> & f_to) {
	QUAST<Type> * res = makeEmpty<Type>(from);
	
	const std::vector<LinearExpression> & scat_from = getScattering(from);
	const std::vector<LinearExpression> & scat_to   = getScattering(to);
	
	size_t min = scat_from.size() < scat_to.size() ? scat_from.size() : scat_to.size();
		
	for (size_t i = 0; i < min; i++) {
		res = maxQuast<Type>(
			res,
			computeMax<Type>(from, f_from, /*scat_from_,*/ to, f_to/*, scat_to_*/, i)
		);
	}
	
	return res;
}

template <class Type>
QUAST<Type> * computeMax(
	Type from, const std::vector<LinearExpression> & f_from, //const std::vector<LinearExpression> * scat_from,
	Type to  , const std::vector<LinearExpression> & f_to  , //const std::vector<LinearExpression> * scat_to,
	size_t depth
) {
	QUAST<Type> * res;
	
	if (f_from.size() != f_to.size())
		throw Exception::DimensionsError("Can not compute the max if function for both objects have not the same dimension.");
	
	size_t df = getDimension(from);
	size_t dt = getDimension(to);
	size_t gf = getExtraDimension(from);
	size_t gt = getExtraDimension(to);
	
	Polyhedron p(dt + df + gf);
	
	const ConstraintSystem & dom_from = getDomain(from).minimized_constraints();
	const ConstraintSystem & dom_to = getDomain(to).minimized_constraints();
	
//	const ConstraintSystem & dom_from = getDomain(from).constraints();
//	const ConstraintSystem & dom_to = getDomain(to).constraints();

	LinearExpression le(0);

//	std::cout << std::endl << std::endl << depth << std::endl;
//	std::cout << "\tTranslate 'from' domain" << std::endl;
	ConstraintSystem::const_iterator it_dom;
	for (it_dom = dom_from.begin(); it_dom != dom_from.end(); it_dom++) {
		le += it_dom->inhomogeneous_term();
		for (size_t i = 0; i < df && i < it_dom->space_dimension() ; i++)
			le += it_dom->coefficient(VariableID(i)) * VariableID(dt + i);
		for (size_t i = df; i < df + gf && i < it_dom->space_dimension(); i++)
			le += it_dom->coefficient(VariableID(i)) * VariableID(dt + i);
/*
		std::cout << "\t\t";
		for (int j = 0; j < df+dt+gt; j++)
			std::cout << le.coefficient(VariableID(j)) << "\t";
		if (it_dom->is_inequality())
			std::cout << le.inhomogeneous_term() << " >= 0" << std::endl;
		else
			std::cout << le.inhomogeneous_term() << " == 0" << std::endl;
*/
		if (it_dom->is_inequality())
			p.refine_with_constraint(le >= 0);
		else
			p.refine_with_constraint(le == 0);
		le = LinearExpression::zero();
	}
//	std::cout << "\tTranslate 'to' domain" << std::endl;
	for (it_dom = dom_to.begin(); it_dom != dom_to.end(); it_dom++) {
		le += it_dom->inhomogeneous_term();
		for (size_t i = 0; i < dt && i < it_dom->space_dimension(); i++)
			le += it_dom->coefficient(VariableID(i)) * VariableID(i);
		for (size_t i = dt; i < dt + gt && i < it_dom->space_dimension(); i++)
			le += it_dom->coefficient(VariableID(i)) * VariableID(df + i);
/*		
		std::cout << "\t\t";
		for (int j = 0; j < df+dt+gt; j++)
			std::cout << le.coefficient(VariableID(j)) << "\t";
		if (it_dom->is_inequality())
			std::cout << le.inhomogeneous_term() << " >= 0" << std::endl;
		else
			std::cout << le.inhomogeneous_term() << " == 0" << std::endl;
*/		
		if (it_dom->is_inequality())
			p.refine_with_constraint(le >= 0);
		else
			p.refine_with_constraint(le == 0);
		le = LinearExpression::zero();
	}
//	std::cout << "\tTranslate function rel" << std::endl;
	for (size_t i = 0; i < f_from.size(); i++) {
		le += f_to[i].inhomogeneous_term() - f_from[i].inhomogeneous_term();
		for (size_t j = 0; j < dt && j < f_to[i].space_dimension(); j++)
			le += f_to[i].coefficient(VariableID(j)) * VariableID(j);
		for (size_t j = 0; j < df && j < f_from[i].space_dimension(); j++)
			le -= f_from[i].coefficient(VariableID(j)) * VariableID(dt + j);
		for (size_t j = 0; j < gt && j < f_to[i].space_dimension(); j++)
			le += f_to[i].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
		for (size_t j = 0; j < gf && j < f_from[i].space_dimension(); j++)
			le -= f_from[i].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
/*		
		std::cout << "\t\t";
		for (int j = 0; j < df+dt+gt; j++)
			std::cout << le.coefficient(VariableID(j)) << "\t";
		std::cout << le.inhomogeneous_term() << " == 0" << std::endl;
*/			
		p.refine_with_constraint(le == 0);
		le = LinearExpression::zero();
	}
	// Scattering part
	
	const std::vector<LinearExpression> & scat_from = getScattering(from);
	const std::vector<LinearExpression> & scat_to   = getScattering(to);
	
//	std::cout << "\tTranslate Scattering" << std::endl;
	le = LinearExpression::zero();
	for (int i = 0; i < depth; i++) {
		le += scat_to[i].inhomogeneous_term() - scat_from[i].inhomogeneous_term();
		for (size_t j = 0; j < dt && j < scat_to[i].space_dimension(); j++)
			le += scat_to[i].coefficient(VariableID(j)) * VariableID(j);
		for (size_t j = 0; j < df && j < scat_from[i].space_dimension(); j++)
			le -= scat_from[i].coefficient(VariableID(j)) * VariableID(dt + j);
		for (size_t j = 0; j < gt && j < scat_to[i].space_dimension(); j++)
			le += scat_to[i].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
		for (size_t j = 0; j < gf && j < scat_from[i].space_dimension(); j++)
			le -= scat_from[i].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
/*		
		std::cout << "\t\t";
		for (int j = 0; j < df+dt+gt; j++)
			std::cout << le.coefficient(VariableID(j)) << "\t";
		std::cout << le.inhomogeneous_term() << " == 0" << std::endl;
*/			
		p.refine_with_constraint(le == 0);
		le = LinearExpression::zero();
	}
	le = LinearExpression::zero();
	le += scat_to[depth].inhomogeneous_term() - scat_from[depth].inhomogeneous_term();
	for (size_t j = 0; j < dt && j < scat_to[depth].space_dimension(); j++)
		le += scat_to[depth].coefficient(VariableID(j)) * VariableID(j);
	for (size_t j = 0; j < df && j < scat_from[depth].space_dimension(); j++)
		le -= scat_from[depth].coefficient(VariableID(j)) * VariableID(dt + j);
	for (size_t j = 0; j < gt && j < scat_to[depth].space_dimension(); j++)
		le += scat_to[depth].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
	for (size_t j = 0; j < gf && j < scat_from[depth].space_dimension(); j++)
		le -= scat_from[depth].coefficient(VariableID(dt + j)) * VariableID(dt + df + j);
/*		
	std::cout << "\t\t";
	for (int j = 0; j < df+dt+gt; j++)
		std::cout << le.coefficient(VariableID(j)) << "\t";
	std::cout << le.inhomogeneous_term() << " <= -1" << std::endl;
*/			
	p.refine_with_constraint(le <= -1);
	le = LinearExpression::zero();
	
	// Compute Max
	
	if (!p.contains_integer_point())
		return makeEmpty<Type>(from);
	
	std::vector<bool> sign(dt);
	size_t cnt = 0;
	for (int i = 0; i < scat_to.size(); i++) {
		if (!(scat_to[i].all_homogeneous_terms_are_zero())) {
			sign[cnt] = (scat_to[i].coefficient(VariableID(cnt)) > 0);
			cnt++;
		}
	}
	
	std::vector<LinearExpression> * ml = maxlex(p, sign, dt, df, gt);
	
	std::vector<Constraint> * cond = conditions(dom_to, dom_from, *ml, dt, df, gt);
	
	if (!cond)
		res = makeEmpty<Type>(from);
	else if (cond->size() == 0) {
		res = makeLeaf<Type>(from, to, *ml);
	}
	else {
		res = makeBranch<Type>(
			from,
			*cond,
			makeLeaf<Type>(from, to, *ml),
			makeEmpty<Type>(from)
		);
	}
	
	delete cond;
	delete ml;
	
	return res;
}

template <class Type>
std::vector<LinearExpression> * computeRelation(Type obj, std::vector<LinearExpression> & arg, size_t d) {
	std::vector<LinearExpression> * res = new std::vector<LinearExpression>();
	
	size_t dim = getDimension(obj);
	
	if (arg.size() != dim) {
		throw Exception::DimensionsError("To compute a relation the object and the vector of linear expression need to have the same dimension.");
	}
	
	std::vector<LinearExpression> scattering = getScattering(obj);
	
	LinearExpression le(0);
	std::vector<LinearExpression>::iterator it;
	for (it = scattering.begin(); it != scattering.end(); it++) {
		le += it->inhomogeneous_term();
		
		for (int i = 0; i < dim; i++) {
			le += it->coefficient(VariableID(i)) * arg[i];
		}
		for (int i = 0; i < getExtraDimension(obj); i++) {
			le += it->coefficient(VariableID(dim + i)) * VariableID(d + i);
		}
		
		res->push_back(le);
		
		le = LinearExpression::zero();
	}
	
	return res;
}

/*********/
/* QUAST */
/*********/

template <class Type>		
QUAST<Type>::QUAST(Type source) :
	p_source(source),
	p_conditions(),
	p_then(NULL),
	p_else(NULL),
	p_is_leaf(false),
	p_leaf(getBottom<Type>()),
	p_relation()
{}

template <class Type>
QUAST<Type>::QUAST(const QUAST<Type> & quast) :
	p_source(quast.p_source),
	p_conditions(quast.p_conditions),
	p_then(quast.p_then ? new QUAST<Type>(*(quast.p_then)) : NULL),
	p_else(quast.p_else ? new QUAST<Type>(*(quast.p_else)) : NULL),
	p_is_leaf(quast.p_is_leaf),
	p_leaf(quast.p_leaf),
	p_relation(quast.p_relation)
{}

template <class Type>
QUAST<Type>::~QUAST() {
	if (p_then)
		delete p_then;
	if (p_else)
		delete p_else;
}

template <class Type>	
Type QUAST<Type>::Source() const {
	return p_source;
}

template <class Type>		
const std::vector<Constraint> & QUAST<Type>::Condition() const {
	return p_conditions;
}

template <class Type>		
QUAST<Type> * QUAST<Type>::Then() const {
	return p_then;
}

template <class Type>
QUAST<Type> * QUAST<Type>::Else() const {
	return p_else;
}

template <class Type>		
bool QUAST<Type>::isLeaf() const {
	return p_is_leaf;
}

template <class Type>		
Type QUAST<Type>::Leaf() const {
	return p_leaf;
}

template <class Type>
const std::vector<LinearExpression> & QUAST<Type>::Relation() const {
	return p_relation;
}

template <class Type>
bool operator == (const QUAST<Type> & q1, const QUAST<Type> & q2) {
	if (q1.p_source == q2.p_source && q1.p_is_leaf == q2.p_is_leaf) {
		if (q1.p_is_leaf) {
			if (q1.p_leaf == q2.p_leaf) {
				return (q1.p_relation == q2.p_relation);
			}
			else return false;
		}
		else {
			return (q1.p_conditions == q2.p_conditions) && (*(q1.p_then) == *(q2.p_then)) && (*(q1.p_else) == *(q2.p_else));
		}
	}
	else return false;
}

template <class Type>
QUAST<Type> * maxQuast(QUAST<Type> * q1, QUAST<Type> * q2) {
	if (q1->p_source != q2->p_source)
		throw Exception::DimensionsError("Can not compute the maximum of 2 Quast with different sources.");

/*	q1->print(std::cout, "1 > ");
	std::cout << std::endl;
	q2->print(std::cout, "2 > ");
	std::cout << std::endl;
	std::cout << std::endl;*/

	if (q1->isLeaf() && isBottom<Type>(q1->p_leaf)) {
		delete q1;
		return q2;
	}
	if (q2->isLeaf() && isBottom<Type>(q2->p_leaf)) {
		delete q2;
		return q1;
	}
	if (!q1->isLeaf()) {
		q1->p_then = maxQuast<Type>(q1->p_then, new QUAST<Type>(*q2));
		q1->p_else = maxQuast<Type>(q1->p_else, new QUAST<Type>(*q2));
		delete q2;
		return q1;
	}
	if (!q2->isLeaf()) {
		q2->p_then = maxQuast<Type>(new QUAST<Type>(*q1), q2->p_then);
		q2->p_else = maxQuast<Type>(new QUAST<Type>(*q1), q2->p_else);
		delete q1;
		return q2;
	}
	
	// q1->isLeaf() && q2->isLeaf()
	
	LinearExpression le(0);
	
	if (q1->p_leaf == q2->p_leaf) {
		for (int i = 0; i < q1->p_relation.size(); i++) {
			le = q2->p_relation[i] - q1->p_relation[i];
			if ((!le.all_homogeneous_terms_are_zero()) || le.inhomogeneous_term() != 0) break;
		}
		if (le.all_homogeneous_terms_are_zero() && le.inhomogeneous_term() == 0) { // q1 == q2
			delete q2;
			return q1;
		}
	}
	
	size_t dim_source = getDimension(q1->p_source);
	
	std::vector<LinearExpression> * rel_q1 = computeRelation(q1->p_leaf, q1->p_relation, dim_source);
	std::vector<LinearExpression> * rel_q2 = computeRelation(q2->p_leaf, q2->p_relation, dim_source);
	
	std::vector<LinearExpression> condition;
	le = LinearExpression::zero();
	for (int i = 0; (i < rel_q1->size()) && (i < rel_q2->size()); i++) {
		 le = rel_q1->at(i) - rel_q2->at(i);
		 if (le.all_homogeneous_terms_are_zero()) {
		 	if (le.inhomogeneous_term() != 0) break;
		 }
		 else {
		 	condition.push_back(le);
		 	le = LinearExpression::zero();
		 }
	}
	
	delete rel_q1;
	delete rel_q2;
	
	if (condition.size() == 0) {
		if (le.inhomogeneous_term() > 0) {
			delete q2;
			return q1;
		}
		else { // le.inhomogeneous_term() < 0
			delete q1;
			return q2;
		}
	}
	// else
	
	QUAST<Type> * tmp;
	if (le.inhomogeneous_term() < 0)
		tmp = new QUAST<Type>(*q2);
	else
		tmp = new QUAST<Type>(*q1);
	
	QUAST<Type> * res;
	for (int i = condition.size() - 1; i >= 0; i--) {
		res = new QUAST<Type>(q1->p_source);
		res->p_then = new QUAST<Type>(*q2);
		res->p_else = tmp;
		res->p_conditions.push_back(condition[i] + 1 <= 0);
		tmp = res;
		
		
		res = new QUAST<Type>(q1->p_source);
		res->p_then = new QUAST<Type>(*q1);
		res->p_else = tmp;
		res->p_conditions.push_back(condition[i] - 1 >= 0);
		tmp = res;
	}
	
	delete q1;
	delete q2;
	
	res = simplify<Type>(tmp);
	
	return res;
}

template <class Type>
QUAST<Type> * simplify(QUAST<Type> * q, Polyhedron * p) {
	QUAST<Type> * res = q;
	QUAST<Type> * tmp;
	
	if (p == NULL)
		p = new Polyhedron(getDomain(q->p_source));

	while (!res->p_is_leaf) {
		std::vector<Constraint>::iterator it;
		for (it = res->p_conditions.begin(); it != res->p_conditions.end(); it++)
			p->refine_with_constraint(*it);

		if (!p->contains_integer_point()) {
			tmp = new QUAST<Type>(*(res->p_else));
			delete res;
			res = tmp;
		}
		else if (!p->contains(getDomain(res->p_source))) {
			tmp = new QUAST<Type>(*(res->p_then));
			delete res;
			res = tmp;
		}
		else
			break;
	}
	if (!res->p_is_leaf) {
		res->p_then = simplify<Type>(res->p_then, p);
		res->p_else = simplify<Type>(res->p_else, p);
		if (*(res->p_then) == *(res->p_else)) {
			tmp = new QUAST<Type>(*(res->p_then));
			delete res;
			res = tmp;
		}
	}

	return res;
}

template <class Type>
QUAST<Type> * makeEmpty(Type source) {
	QUAST<Type> * res = new QUAST<Type>(source);
	res->p_is_leaf = true;
	res->p_leaf = getBottom<Type>();
	return res;
}

template <class Type>
QUAST<Type> * makeBranch(Type source, std::vector<Constraint> & cs, QUAST<Type> * q1, QUAST<Type> * q2) {
	QUAST<Type> * res = new QUAST<Type>(source);
	res->p_conditions = cs;
	res->p_then = q1;
	res->p_else = q2;
	return res;
}

template <class Type>
QUAST<Type> * makeLeaf(Type source, Type leaf, std::vector<LinearExpression> & iterators) {
	QUAST<Type> * res = new QUAST<Type>(source);
	res->p_is_leaf = true;
	res->p_leaf = leaf;
	res->p_relation = iterators;
	return res;
}

template <class Type>
void QUAST<Type>::print(std::ostream & out, std::string indent) const {
	if (p_is_leaf) {
		if (!isBottom<Type>(p_leaf)) {
			out << indent << p_leaf << std::endl;
			std::vector<LinearExpression>::const_iterator it;
			for (it = p_relation.begin(); it != p_relation.end(); it++) {
				out << indent;
				for (int i = 0; i < it->space_dimension(); i++) {
					out << it->coefficient(VariableID(i)) << "\t";
				}
				out << it->inhomogeneous_term() << std::endl;
			}
		}
		else
			out << indent << "_|_ (empty)" << std::endl;
	}
	else {
		out << indent << "If ";
		std::vector<Constraint>::const_iterator it;
		for (it = p_conditions.begin(); it != p_conditions.end(); it++) {
			out << "( ";
			if (it->is_equality())
				out << "eq\t";
			else
				out << "ineq\t";
			for (int i = 0; i < it->space_dimension(); i++) {
				out << it->coefficient(VariableID(i)) << "\t";
			}
			out << it->inhomogeneous_term() << ")";
			if (it != p_conditions.end()-1)
				out << " & ";
		}
		out << std::endl;
		p_then->print(out, indent + "|\t");
		out << indent << "Else" << std::endl;
		p_else->print(out, indent + "|\t");
	}
}

