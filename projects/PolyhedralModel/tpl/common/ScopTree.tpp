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
ScopLoop<Function, Expression, VariableLBL>::ScopLoop(size_t position, ScopTree<Function, Expression, VariableLBL> * parent, VariableLBL iterator) :
	ScopTree<Function, Expression, VariableLBL>(position, parent),
	p_iterator(iterator),
	p_lb(),
	p_ub()
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
void ScopLoop<Function, Expression, VariableLBL>::addLowerBoundTerm(VariableLBL var, int coef) {
	typename std::map<VariableLBL, int>::iterator pair = p_lb.find(var);
	if (pair == p_lb.end())
		p_lb.insert(std::pair<VariableLBL, int>(var, coef));
	else
		pair->second += coef;
}

template <class Function, class Expression, class VariableLBL>
std::map<VariableLBL, int> & ScopLoop<Function, Expression, VariableLBL>::getLowerBound() { return p_lb; }

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::addUpperBoundTerm(VariableLBL var, int coef) {
	typename std::map<VariableLBL, int>::iterator pair = p_ub.find(var);
	if (pair == p_ub.end())
		p_ub.insert(std::pair<VariableLBL, int>(var, coef));
	else
		pair->second += coef;
}

template <class Function, class Expression, class VariableLBL>
std::map<VariableLBL, int> & ScopLoop<Function, Expression, VariableLBL>::getUpperBound() { return p_ub; }

template <class Function, class Expression, class VariableLBL>
void ScopLoop<Function, Expression, VariableLBL>::setIncrement(int inc) { p_inc = inc; }

template <class Function, class Expression, class VariableLBL>
int ScopLoop<Function, Expression, VariableLBL>::getIncrement() { return p_inc; }

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
			std::vector<std::pair<VariableLBL, int> > * e1;
			std::vector<std::pair<VariableLBL, int> > * e2;
			
			// domain add bound
			e1 = new std::vector<std::pair<VariableLBL, int> >(
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getUpperBound().begin(),
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getUpperBound().end()
			);
			e1->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), -1));
			e2 = new std::vector<std::pair<VariableLBL, int> >(
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getLowerBound().begin(),
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getLowerBound().end()
			);
			e2->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), -1));
			
			int inc = ((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIncrement();
			if (inc==1) {
				domain.addInequation(*e1, true);
				domain.addInequation(*e2, false);
			}
			else if (inc==-1) {
				domain.addInequation(*e1, false);
				domain.addInequation(*e2, true);
			}
			else
				throw Exception::OutOfScopRestrainedDefinition("Loop definition use an increment different from 1 or -1.");
			
			delete e1;
			delete e2;
			
			// scattering add position
			e1 = new std::vector<std::pair<VariableLBL, int> >();
			e1->push_back(std::pair<VariableLBL, int>(constantLBL(), (*it1)->getPosition()));
			scattering.addEquation(*e1);
			delete e1;
			
			// scattering add iterator
			/*e1 = new std::vector<std::pair<VariableLBL, int> >();*/
			e1 = new std::vector<std::pair<VariableLBL, int> >(
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getLowerBound().begin(),
				((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getLowerBound().end()
			);
			if (inc==1) {
				typename std::vector<std::pair<VariableLBL, int> >::iterator it2;
				for (it2 = e1->begin(); it2 != e1->end(); it2++)
					(*it2).second = -(*it2).second;
				e1->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), 1));
			}
			else if (inc==-1) {
				e1->push_back(std::pair<VariableLBL, int>(((ScopLoop<Function, Expression, VariableLBL>*)*it1)->getIterator(), -1));
			}
			scattering.addEquation(*e1);
			delete e1;
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
bool ScopStatement<Function, Expression, VariableLBL>::isRoot() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isLoop() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isConditinnal() const { return false; }

template <class Function, class Expression, class VariableLBL>
bool ScopStatement<Function, Expression, VariableLBL>::isStatement() const { return true; }

