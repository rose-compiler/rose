/**
 * \file src/common/ScopTree.tpp
 * \brief Implementation of ScopTree (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

/************/
/* ScopTree */
/************/

template <class Function, class Expression, class VariableLBL>
ScopTree<Function, Expression, VariableLBL>::ScopTree(size_t position, ScopTree<Function, Expression, VariableLBL> * parent) :
	p_position(position),
	p_parent(parent),
	p_childs()
{
	if (parent)
		if (parent->p_childs.size() == position)
			parent->p_childs.push_back(this);
		else if (parent->p_childs.size() > position)
			parent->p_childs[position] = this; //! May be interresting for parrallel AST traversal/construction or ScopTree rewrite/simplify.
}	

template <class Function, class Expression, class VariableLBL>
ScopTree<Function, Expression, VariableLBL> * ScopTree<Function, Expression, VariableLBL>::getParent() const { return p_parent; }

template <class Function, class Expression, class VariableLBL>
const std::vector<ScopTree<Function, Expression, VariableLBL> *> & ScopTree<Function, Expression, VariableLBL>::getChilds() const { return p_childs; }

template <class Function, class Expression, class VariableLBL>
const size_t ScopTree<Function, Expression, VariableLBL>::getPosition() const { return p_position; }

template <class Function, class Expression, class VariableLBL>
void ScopTree<Function, Expression, VariableLBL>::Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop) {
	
	vect->push_back(this);
	
	typename std::vector<ScopTree<Function, Expression, VariableLBL> *>::iterator it;
	for (it = p_childs.begin(); it != p_childs.end(); it++)
		(*it)->Traverse(new std::vector<ScopTree<Function, Expression, VariableLBL> *>(*vect), nbr_surr_loop);

	delete vect;
}

/************/
/* ScopRoot */
/************/

template <class Function, class Expression, class VariableLBL>
ScopRoot<Function, Expression, VariableLBL>::ScopRoot(Function * function) :
	ScopTree<Function, Expression, VariableLBL>(0, NULL),
	p_function(function)
{}

template <class Function, class Expression, class VariableLBL>
void ScopRoot<Function, Expression, VariableLBL>::Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop) {

	if (vect)
		delete vect;
	vect = new std::vector<ScopTree<Function, Expression, VariableLBL> *>();
	
	PolyhedricAnnotation::setPolyhedralProgram<Function, Expression, VariableLBL>(p_function);
	
	ScopTree<Function, Expression, VariableLBL>::Traverse(vect, 0);
}

template <class Function, class Expression, class VariableLBL>
Function * ScopRoot<Function, Expression, VariableLBL>::getFunction() const { return p_function; }

template <class Function, class Expression, class VariableLBL>
bool ScopRoot<Function, Expression, VariableLBL>::isRoot() const { return true; }

template <class Function, class Expression, class VariableLBL>
bool ScopRoot<Function, Expression, VariableLBL>::isLoop() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopRoot<Function, Expression, VariableLBL>::isConditinnal() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopRoot<Function, Expression, VariableLBL>::isStatement() const { return false; }

/************/
/* ScopLoop */
/************/

template <class Function, class Expression, class VariableLBL>
ScopLoop<Function, Expression, VariableLBL>::ScopLoop(
	size_t position,
	ScopTree<Function, Expression, VariableLBL> * parent,
	VariableLBL iterator,
	void * original_loop_ 
) :
	ScopTree<Function, Expression, VariableLBL>(position, parent),
	p_iterator(iterator),
	p_lb(),
	p_ub(),
	original_loop(original_loop_)
{}

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop) {
	if ((*vect)[0]->isRoot())
		PolyhedricAnnotation::getPolyhedralProgram<Function, Expression, VariableLBL>(
			((ScopRoot<Function, Expression, VariableLBL>*)((*vect)[0]))->getFunction()
		).addIterator(p_iterator);
	else
		throw Exception::UnexpectedScopNode("In ScopLoop::Traverse(...)::vect[0] need to be a ScopRoot.");

	ScopTree<Function, Expression, VariableLBL>::Traverse(vect, nbr_surr_loop + 1);
}

template <class Function, class Expression, class VariableLBL>
VariableLBL ScopLoop<Function, Expression, VariableLBL>::getIterator() { return p_iterator; }

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::addLowerBound(std::map<VariableLBL, int> & lb, int div) {
	p_lb.push_back(std::pair<std::map<VariableLBL, int>, int>(lb, div));
}

template <class Function, class Expression, class VariableLBL>
std::vector<std::pair<std::map<VariableLBL, int>, int> > & ScopLoop<Function, Expression, VariableLBL>::getLowerBound() { return p_lb; }

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::addUpperBound(std::map<VariableLBL, int> & ub, int div) {
	p_ub.push_back(std::pair<std::map<VariableLBL, int>, int>(ub, div));
}

template <class Function, class Expression, class VariableLBL>
std::vector<std::pair<std::map<VariableLBL, int>, int> > & ScopLoop<Function, Expression, VariableLBL>::getUpperBound() { return p_ub; }

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::setIncrement(int inc) { p_inc = inc; }

template <class Function, class Expression, class VariableLBL>
int ScopLoop<Function, Expression, VariableLBL>::getIncrement() { return p_inc; }

template <class Function, class Expression, class VariableLBL>
void * ScopLoop<Function, Expression, VariableLBL>::getOriginalLoop() const { return original_loop; }

template <class Function, class Expression, class VariableLBL>
bool ScopLoop<Function, Expression, VariableLBL>::isRoot() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopLoop<Function, Expression, VariableLBL>::isLoop() const { return true; }

template <class Function, class Expression, class VariableLBL>
bool ScopLoop<Function, Expression, VariableLBL>::isConditinnal() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopLoop<Function, Expression, VariableLBL>::isStatement() const { return false; }

/********************/
/* ScopConditionnal */
/********************/

template <class Function, class Expression, class VariableLBL>
ScopConditionnal<Function, Expression, VariableLBL>::ScopConditionnal(size_t position, ScopTree<Function, Expression, VariableLBL> * parent) :
	ScopTree<Function, Expression, VariableLBL>(position, parent),
	p_conditions()
{}

template <class Function, class Expression, class VariableLBL>
void ScopConditionnal<Function, Expression, VariableLBL>::addCondition(const std::vector<std::pair<VariableLBL, int> > & lin_exp, bool equality) {
	p_conditions.push_back(std::pair<std::vector<std::pair<VariableLBL, int> >, bool>(lin_exp, equality));
}
	
template <class Function, class Expression, class VariableLBL>	
size_t ScopConditionnal<Function, Expression, VariableLBL>::getNumberOfCondition() const {
	return p_conditions.size();
}

template <class Function, class Expression, class VariableLBL>
const std::vector<std::pair<VariableLBL, int> > & ScopConditionnal<Function, Expression, VariableLBL>::getConditionLinearExpression(size_t id) const {
	return p_conditions[id].first;
}

template <class Function, class Expression, class VariableLBL>
bool ScopConditionnal<Function, Expression, VariableLBL>::getConditionType(size_t id) const {
	return p_conditions[id].second;
}

template <class Function, class Expression, class VariableLBL>
bool ScopConditionnal<Function, Expression, VariableLBL>::isRoot() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopConditionnal<Function, Expression, VariableLBL>::isLoop() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopConditionnal<Function, Expression, VariableLBL>::isConditinnal() const { return true; }

template <class Function, class Expression, class VariableLBL>
bool ScopConditionnal<Function, Expression, VariableLBL>::isStatement() const { return false; }

/*****************/
/* ScopStatement */
/*****************/

template <class Function, class Expression, class VariableLBL>
ScopStatement<Function, Expression, VariableLBL>::ScopStatement(size_t position, ScopTree<Function, Expression, VariableLBL> * parent, Expression * expression) :
	ScopTree<Function, Expression, VariableLBL>(position, parent),
	p_expression(expression)
{}

template <class Function, class Expression, class VariableLBL>
void ScopStatement<Function, Expression, VariableLBL>::Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop) {
	
	PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> * tmp;
	
	if ((*vect)[0]->isRoot())
		tmp = &PolyhedricAnnotation::getPolyhedralProgram<Function, Expression, VariableLBL>(
			((ScopRoot<Function, Expression, VariableLBL> *)(*vect)[0])->getFunction()
		);
	else
		throw Exception::UnexpectedScopNode("In ScopStatement::Traverse(...)::vect[0] need to be a ScopRoot.");
		
	PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program = *tmp;
		
	polyhedral_program.addExpression(p_expression);
	
	PolyhedricAnnotation::setDomain<Function, Expression, VariableLBL>(polyhedral_program, p_expression, nbr_surr_loop);
	PolyhedricAnnotation::Domain<Function, Expression, VariableLBL> & domain =
		PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(p_expression);
	
	PolyhedricAnnotation::setScattering<Function, Expression, VariableLBL>(polyhedral_program, p_expression, nbr_surr_loop);
	PolyhedricAnnotation::Scattering<Function, Expression, VariableLBL> & scattering =
		PolyhedricAnnotation::getScattering<Function, Expression, VariableLBL>(p_expression);
	
	PolyhedricAnnotation::setDataAccess<Function, Expression, VariableLBL>(polyhedral_program, p_expression, nbr_surr_loop);
	
	typename std::vector<ScopTree<Function, Expression, VariableLBL> *>::iterator it1;
	for (it1 = vect->begin()+1; it1 != vect->end(); it1++) {
		if ((*it1)->isRoot())
			throw Exception::UnexpectedScopNode("In ScopStatement::Traverse(...)::vect[i] (i > 0) can't be a ScopRoot.");
		else if ((*it1)->isLoop()) {
			int inc = ((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIncrement();
			if (inc != -1 && inc != 1)
				throw Exception::OutOfScopRestrainedDefinition("Loop definition use an increment different from 1 or -1.");

			std::vector<std::pair<VariableLBL, int> > * e;

			// domain add bound
			typename std::vector<std::pair<std::map<VariableLBL, int>, int> >::iterator it_bounds;
			std::vector<std::pair<std::map<VariableLBL, int>, int> > & lb = 
                            ((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getLowerBound();
			std::vector<std::pair<std::map<VariableLBL, int>, int> > & ub =
                            ((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getUpperBound();
			for (it_bounds = lb.begin(); it_bounds != lb.end(); it_bounds++) {
				e = new std::vector<std::pair<VariableLBL, int> >(it_bounds->first.begin(), it_bounds->first.end());
				e->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), -1));
				if (it_bounds->second != 1) {
					typename std::vector<std::pair<VariableLBL, int> >::iterator it_coef;
					for (it_coef = e->begin(); it_coef != e->end(); it_coef++)
						it_coef->second *= it_bounds->second;
				}
				domain.addInequation(*e, inc != 1);
				delete e;
			}
			for (it_bounds = ub.begin(); it_bounds != ub.end(); it_bounds++) {
				e = new std::vector<std::pair<VariableLBL, int> >(it_bounds->first.begin(), it_bounds->first.end());
				e->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), -1));
				if (it_bounds->second != 1) {
					typename std::vector<std::pair<VariableLBL, int> >::iterator it_coef;
					for (it_coef = e->begin(); it_coef != e->end(); it_coef++)
						it_coef->second *= it_bounds->second;
				}
				domain.addInequation(*e, inc == 1);
				delete e;
			}

			// scattering add position
			e = new std::vector<std::pair<VariableLBL, int> >();
			e->push_back(std::pair<VariableLBL, int>(constantLBL(), (*it1)->getPosition()));
			scattering.addEquation(*e);
			delete e;
			
			// scattering add iterator
			e = new std::vector<std::pair<VariableLBL, int> >();
			e->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), inc));
			scattering.addEquation(*e);
			delete e;
		}
		else if ((*it1)->isConditinnal()) {
			
			// domain add condition
			for (int i = 0; i < ((ScopConditionnal<Function, Expression, VariableLBL>*)*it1)->getNumberOfCondition(); i++) {
				if (((ScopConditionnal<Function, Expression, VariableLBL>*)*it1)->getConditionType(i)) {
					domain.addInequation(((ScopConditionnal<Function, Expression, VariableLBL>*)*it1)->getConditionLinearExpression(i), true);
					domain.addInequation(((ScopConditionnal<Function, Expression, VariableLBL>*)*it1)->getConditionLinearExpression(i), false);
				}
				else {
					domain.addInequation(((ScopConditionnal<Function, Expression, VariableLBL>*)*it1)->getConditionLinearExpression(i), true);
				}
			}

			// scattering add position
			std::vector<std::pair<VariableLBL, int> > * equation;
			equation = new std::vector<std::pair<VariableLBL, int> >();
			equation->push_back(std::pair<VariableLBL, int>(constantLBL(), (*it1)->getPosition()));
			scattering.addEquation(*equation);
			delete equation;
			
		}
		else if ((*it1)->isStatement())
			throw Exception::UnexpectedScopNode("In ScopStatement::Traverse(...)::vect[i] can't be a ScopStatement.");
		else
			throw Exception::UnexpectedScopNode("In ScopStatement::Traverse(...)::vect[i] can't be something else than ScopTree node !!!");
	}
			
	// scattering add position for *this (the statement) 
	std::vector<std::pair<VariableLBL, int> > * equation;
	equation = new std::vector<std::pair<VariableLBL, int> >();
	equation->push_back(std::pair<VariableLBL, int>(constantLBL(), this->getPosition()));
	scattering.addEquation(*equation);
	delete equation;
}

template <class Function, class Expression, class VariableLBL>
Expression * ScopStatement<Function, Expression, VariableLBL>::getExpression() {
  return p_expression;
}

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isRoot() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isLoop() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isConditinnal() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isStatement() const { return true; }

