/**
 * \file src/common/PolyhedricDependency.tpp
 * \brief Implementation of Polyhedric Dependencies (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
void QuastToDependency(
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
	VariableLBL v,
	DependencyType type,
	QUAST<std::pair<Expression *, size_t> > * q,
	ChainedList<std::pair<Constraint, bool> > * conditions,
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res
) {
	if (q->isLeaf()) {
		Expression * from = q->Source().first;
		Expression * to = q->Leaf().first;
		if (to) {
			size_t st = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(to).getNumberOfIterators();
			size_t sf = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(from).getNumberOfIterators();
			size_t sg = polyhedral_program.getNumberOfGlobals();
			
			const ConstraintSystem & dom_to   = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(to).getDomainPolyhedron().minimized_constraints();
			const ConstraintSystem & dom_from = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(from).getDomainPolyhedron().minimized_constraints();
		
			Polyhedron dependency(st + sf + sg);
		
			LinearExpression_ppl le(0);
			ConstraintSystem::const_iterator it0;
			for (it0 = dom_to.begin(); it0 != dom_to.end(); it0++) {
				le += it0->inhomogeneous_term();
				for (int i = 0; (i < st) && (i < it0->space_dimension()); i++)
					le += it0->coefficient(VariableID(i)) * VariableID(i);
				for (int i = 0; (i < sg) && (st + i < it0->space_dimension()); i++)
					le += it0->coefficient(VariableID(st + i)) * VariableID(st + sf + i);
				if (it0->is_equality())
					dependency.refine_with_constraint(le == 0);
				else
					dependency.refine_with_constraint(le >= 0);
				le = LinearExpression_ppl::zero();
			}
			for (it0 = dom_from.begin(); it0 != dom_from.end(); it0++) {
				le += it0->inhomogeneous_term();
				for (int i = 0; (i < sf) && (i < it0->space_dimension()); i++)
					le += it0->coefficient(VariableID(i)) * VariableID(st + i);
				for (int i = 0; (i < sg) && (sf + i < it0->space_dimension()); i++)
					le += it0->coefficient(VariableID(sf + i)) * VariableID(st + sf + i);
				if (it0->is_equality())
					dependency.refine_with_constraint(le == 0);
				else
					dependency.refine_with_constraint(le >= 0);
				le = LinearExpression_ppl::zero();
			}
			
			ChainedList<std::pair<Constraint, bool> > * cond_it = conditions;
			while (cond_it != NULL) {			
				le += cond_it->getValue().first.inhomogeneous_term();
				for (int i = 0; (i < sf) && (i < cond_it->getValue().first.space_dimension()); i++)
					le += cond_it->getValue().first.coefficient(VariableID(i)) * VariableID(st + i);
				for (int i = 0; (i < sg) && (st + i < cond_it->getValue().first.space_dimension()); i++)
					le += cond_it->getValue().first.coefficient(VariableID(sf + i)) * VariableID(st + sf + i);
				if (cond_it->getValue().second)
					dependency.refine_with_constraint(le >= 0);
				else
					dependency.refine_with_constraint(le <= -1);
				le = LinearExpression_ppl::zero();
				
				cond_it = cond_it->getNext();
			}
			
			std::vector<LinearExpression_ppl>::const_iterator it2;
			size_t cnt = 0;
			for (it2 = q->Relation().begin(); it2 != q->Relation().end(); it2++) {
				le += it2->inhomogeneous_term();
				for (int i = 0; (i < sf) && (i < it2->space_dimension()); i++)
					le += it2->coefficient(VariableID(i)) * VariableID(st + i);
				for (int i = 0; (i < sg) && (sf + i < it2->space_dimension()); i++)
					le += it2->coefficient(VariableID(sf + i)) * VariableID(st + sf + i);
				dependency.refine_with_constraint(VariableID(cnt) == le);
				le = LinearExpression_ppl::zero();
				cnt++;
			}
			if (dependency.contains_integer_point())
				res->push_back(new Dependency<Function, Expression, VariableLBL>(
					polyhedral_program,
					from,
					to,
					q->Source().second,
					q->Leaf().second,
					type,
					v,
					dependency
				));
		}
	}
	else {
		ChainedList<std::pair<Constraint, bool> > * cond_t = conditions;
		ChainedList<std::pair<Constraint, bool> > * cond_f;
		ChainedList<std::pair<Constraint, bool> > * tmp;
		
		const std::vector<Constraint> & cond = q->Condition();
		std::vector<Constraint>::const_iterator it;
		for (it = cond.begin(); it != cond.end(); it++) {
			tmp = new ChainedList<std::pair<Constraint, bool> >(std::pair<Constraint, bool>(*it, true));
			tmp->setNext(cond_t);
			cond_t = tmp;
			
			cond_f = new ChainedList<std::pair<Constraint, bool> >(std::pair<Constraint, bool>(*it, false));
			cond_f->setNext(conditions);
			QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, type, q->Else(), cond_f, res);
			delete cond_f;
		}
		QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, type, q->Then(), cond_t, res);
		while (cond_t != conditions) {
			tmp = cond_t->getNext();
			delete cond_t;
			cond_t = tmp;
		}
	}
}

template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeDependencies(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res = new std::vector<Dependency<Function, Expression, VariableLBL> *>();
	
	polyhedral_program.finalize();
	
	std::vector<Dependency<Function, Expression, VariableLBL> *> * rar;
	std::vector<Dependency<Function, Expression, VariableLBL> *> * raw;
	std::vector<Dependency<Function, Expression, VariableLBL> *> * war;
	std::vector<Dependency<Function, Expression, VariableLBL> *> * waw;

	rar = ComputeRaR<Function, Expression, VariableLBL>(polyhedral_program);
	raw = ComputeRaW<Function, Expression, VariableLBL>(polyhedral_program);
	war = ComputeWaR<Function, Expression, VariableLBL>(polyhedral_program);
	waw = ComputeWaW<Function, Expression, VariableLBL>(polyhedral_program);
	
//	std::cout << "rar->size() = " << raw->size() << std::endl;
	res->insert(res->begin(), rar->begin(), rar->end());
	
//	std::cout << "raw->size() = " << raw->size() << std::endl;
	res->insert(res->begin(), raw->begin(), raw->end());
	
//	std::cout << "war->size() = " << raw->size() << std::endl;
	res->insert(res->begin(), war->begin(), war->end());
	
//	std::cout << "waw->size() = " << raw->size() << std::endl;
	res->insert(res->begin(), waw->begin(), waw->end());
	
	return res;
}

template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeRaR(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res = new std::vector<Dependency<Function, Expression, VariableLBL> *>();
	
	const std::vector<Expression *> & exp_l = polyhedral_program.getExpressions();
	
	std::vector<std::vector<LinearExpression_ppl> > * r_e1;
	std::vector<std::vector<LinearExpression_ppl> > * r_e2;
	std::vector<std::vector<LinearExpression_ppl> >::const_iterator av1, av2;
	VariableLBL v;
	QUAST<std::pair<Expression *, size_t> > * rar;
	size_t from_pos;
	size_t to_pos;
	
	typename std::vector<Expression *>::const_iterator e1, e2;
	//! parallel with adding regression on the 'res' vector
	for (e1 = exp_l.begin(); e1 != exp_l.end(); e1++) {
		const PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e1 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e1);
		//! parallel with adding regression on the 'res' vector
		for (size_t i = 1; i <= polyhedral_program.getNumberOfVariables(); i++) {
			v = polyhedral_program.getVariableByID(i);
			r_e1 = da_e1.getRead(v);

			from_pos = 0;
			//! parallel with adding regression on the 'res' vector
			for (av1 = r_e1->begin(); av1 != r_e1->end(); av1++) {
				rar = makeEmpty<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos));
				//! parallel with maxQuast regression on 'rar' Quast
				for (e2 = exp_l.begin(); e2 != exp_l.end(); e2++) {
					const PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e2 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e2);
					r_e2 = da_e2.getRead(v);
				
					to_pos = 0;
					//! parallel with maxQuast regression on 'rar' Quast
					for (av2 = r_e2->begin(); av2 != r_e2->end(); av2++) {
						rar = maxQuast<std::pair<Expression *, size_t> >(rar, computeMaxLex<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos), *av1, std::pair<Expression *, size_t>(*e2, to_pos), *av2));
						to_pos++;
					}
				}
				
				delete r_e2;
				QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, RaR, rar, NULL, res); //! here is the regression on 'res'
				delete rar;
				from_pos++;
			}
			delete r_e1;
		}
	}
	return res;
}

template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeRaW(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res = new std::vector<Dependency<Function, Expression, VariableLBL> *>();
	
	const std::vector<Expression *> & exp_l = polyhedral_program.getExpressions();
	
	std::vector<std::vector<LinearExpression_ppl> > * r_e1;
	std::vector<std::vector<LinearExpression_ppl> > * w_e2;
	std::vector<std::vector<LinearExpression_ppl> >::const_iterator av1, av2;
	VariableLBL v;
	QUAST<std::pair<Expression *, size_t> > * raw;
	size_t from_pos;
	size_t to_pos;
	
	typename std::vector<Expression *>::const_iterator e1, e2;
	//! parallel with adding regression on the 'res' vector
	for (e1 = exp_l.begin(); e1 != exp_l.end(); e1++) {
		PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e1 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e1);
		//! parallel with adding regression on the 'res' vector
		for (size_t i = 1; i <= polyhedral_program.getNumberOfVariables(); i++) {
			v = polyhedral_program.getVariableByID(i);
			r_e1 = da_e1.getRead(v);
			
			from_pos = 0;
			//! parallel with adding regression on the 'res' vector
			for (av1 = r_e1->begin(); av1 != r_e1->end(); av1++) {
				raw = makeEmpty<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos));
				//! parallel with maxQuast regression on 'raw' Quast
				for (e2 = exp_l.begin(); e2 != exp_l.end(); e2++) {
					PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e2 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e2);
					w_e2 = da_e2.getWrite(v);
				
					to_pos = 0;
					//! parallel with maxQuast regression on 'raw' Quast
					for (av2 = w_e2->begin(); av2 != w_e2->end(); av2++) {
						raw = maxQuast<std::pair<Expression *, size_t> >(raw, computeMaxLex<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos), *av1, std::pair<Expression *, size_t>(*e2, to_pos), *av2));
						to_pos++;
					}
				}
				
				delete w_e2;
				QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, RaW, raw, NULL, res); //! here is the regression on 'res'
				delete raw;
				from_pos++;
			}
			delete r_e1;
		}
	}
	
	return res;
}

template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeWaR(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res = new std::vector<Dependency<Function, Expression, VariableLBL> *>();
	
	const std::vector<Expression *> & exp_l = polyhedral_program.getExpressions();
	
	std::vector<std::vector<LinearExpression_ppl> > * w_e1;
	std::vector<std::vector<LinearExpression_ppl> > * r_e2;
	std::vector<std::vector<LinearExpression_ppl> >::const_iterator av1, av2;
	VariableLBL v;
	QUAST<std::pair<Expression *, size_t> > * war;
	size_t from_pos;
	size_t to_pos;
	
	typename std::vector<Expression *>::const_iterator e1, e2;
	//! parallel with adding regression on the 'res' vector
	for (e1 = exp_l.begin(); e1 != exp_l.end(); e1++) {
		PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e1 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e1);
		//! parallel with adding regression on the 'res' vector
		for (size_t i = 1; i <= polyhedral_program.getNumberOfVariables(); i++) {
			v = polyhedral_program.getVariableByID(i);
			w_e1 = da_e1.getWrite(v);

			from_pos = 0;
			//! parallel with adding regression on the 'res' vector
			for (av1 = w_e1->begin(); av1 != w_e1->end(); av1++) {
				war = makeEmpty<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos));
				//! parallel with maxQuast regression on 'war' Quast
				for (e2 = exp_l.begin(); e2 != exp_l.end(); e2++) {
					PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e2 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e2);
					r_e2 = da_e2.getRead(v);
				
					to_pos = 0;
					//! parallel with maxQuast regression on 'war' Quast
					for (av2 = r_e2->begin(); av2 != r_e2->end(); av2++) {
						war = maxQuast<std::pair<Expression *, size_t> >(war, computeMaxLex<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos), *av1, std::pair<Expression *, size_t>(*e2, to_pos), *av2));
						to_pos++;
					}
				}
				
				delete r_e2;
				QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, WaR, war, NULL, res); //! here is the regression on 'res'
				delete war;
				from_pos++;
			}
			delete w_e1;
		}
	}
	
	return res;
}

template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeWaW(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	std::vector<Dependency<Function, Expression, VariableLBL> *> * res = new std::vector<Dependency<Function, Expression, VariableLBL> *>();
	
	const std::vector<Expression *> & exp_l = polyhedral_program.getExpressions();
	
	std::vector<std::vector<LinearExpression_ppl> > * w_e1;
	std::vector<std::vector<LinearExpression_ppl> > * w_e2;
	std::vector<std::vector<LinearExpression_ppl> >::const_iterator av1, av2;
	VariableLBL v;
	QUAST<std::pair<Expression *, size_t> > * waw;
	size_t from_pos;
	size_t to_pos;
	
	typename std::vector<Expression *>::const_iterator e1, e2;
	//! parallel with adding regression on the 'res' vector
	for (e1 = exp_l.begin(); e1 != exp_l.end(); e1++) {
		PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e1 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e1);
		//! parallel with adding regression on the 'res' vector
		for (size_t i = 1; i <= polyhedral_program.getNumberOfVariables(); i++) {
			v = polyhedral_program.getVariableByID(i);
			w_e1 = da_e1.getWrite(v);

			from_pos = 0;
			//! parallel  with adding regression on the 'res' vector
			for (av1 = w_e1->begin(); av1 != w_e1->end(); av1++) {
				waw = makeEmpty<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos));
				//! parallel with maxQuast regression on 'waw' Quast
				for (e2 = exp_l.begin(); e2 != exp_l.end(); e2++) {
					PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & da_e2 = PolyhedricAnnotation::getDataAccess<Function, Expression, VariableLBL>(*e2);
					w_e2 = da_e2.getWrite(v);
				
					to_pos = 0;
					//! parallel with maxQuast regression on 'waw' Quast
					for (av2 = w_e2->begin(); av2 != w_e2->end(); av2++) {
						waw = maxQuast<std::pair<Expression *, size_t> >(waw, computeMaxLex<std::pair<Expression *, size_t> >(std::pair<Expression *, size_t>(*e1, from_pos), *av1, std::pair<Expression *, size_t>(*e2, to_pos), *av2));
						to_pos++;
					}
				}
				
				delete w_e2;
				QuastToDependency<Function, Expression, VariableLBL>(polyhedral_program, v, WaW, waw, NULL, res); //! here is the regression on 'res'
				delete waw;
				from_pos++;
			}
			delete w_e1;
		}
	}
	
	return res;
}

template <class Function, class Expression, class VariableLBL>
Dependency<Function, Expression, VariableLBL>::Dependency(
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
	Expression * from,
	Expression * to,
	size_t from_pos,
	size_t to_pos,
	DependencyType type,
	VariableLBL variable,
	Polyhedron & dependency
) :
	p_polyhedral_program(polyhedral_program),
	p_from(p_polyhedral_program.getStatementID(from)),
	p_to(p_polyhedral_program.getStatementID(to)),
	p_from_pos(from_pos),
	p_to_pos(to_pos),
	p_type(type),
	p_variable(p_polyhedral_program.getVariableID(variable)),
	p_dependency(dependency)
{}

template <class Function, class Expression, class VariableLBL>
Dependency<Function, Expression, VariableLBL>::Dependency(
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
	size_t from,
	size_t to,
	size_t from_pos,
	size_t to_pos,
	DependencyType type,
	size_t variable,
	Polyhedron & dependency
) :
	p_polyhedral_program(polyhedral_program),
	p_from(from),
	p_to(to),
	p_from_pos(from_pos),
	p_to_pos(to_pos),
	p_type(type),
	p_variable(variable),
	p_dependency(dependency)
{}


template <class Function, class Expression, class VariableLBL>
size_t Dependency<Function, Expression, VariableLBL>::getFrom() const { return p_from; }

template <class Function, class Expression, class VariableLBL>
size_t Dependency<Function, Expression, VariableLBL>::getTo() const { return p_to; }

template <class Function, class Expression, class VariableLBL>
DependencyType Dependency<Function, Expression, VariableLBL>::getType() const { return p_type; }

template <class Function, class Expression, class VariableLBL>
size_t Dependency<Function, Expression, VariableLBL>::getVariable() const { return p_variable; }

template <class Function, class Expression, class VariableLBL>
Polyhedron Dependency<Function, Expression, VariableLBL>::getPolyhedron() const { return p_dependency; }

template <class Function, class Expression, class VariableLBL>
void Dependency<Function, Expression, VariableLBL>::print(std::ostream & out) {
	out << "# Type" << std::endl;
	switch (p_type) {
		case RaR:
			out << "RaR" << std::endl;
			break;
		case RaW:
			out << "RaW" << std::endl;
			break;
		case WaR:
			out << "WaR" << std::endl;
			break;
		case WaW:
			out << "WaW" << std::endl;
			break;
	}
	
	out << "# From" << std::endl;
	out << p_from << std::endl;
	
	out << "# To" << std::endl;
	out << p_to << std::endl;
	
	out << "# On variable" << std::endl;
	out << p_variable << std::endl;
	
	out << "# Constraint" << std::endl;
	const ConstraintSystem & cs = p_dependency.minimized_constraints();
//	const ConstraintSystem & cs = p_dependency.constraints();
	ConstraintSystem::const_iterator it;
	
	size_t cs_size = 0;
	for (it = cs.begin(); it != cs.end(); it++) cs_size++;
	size_t st = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(p_polyhedral_program.getStatementById(p_to)).getNumberOfIterators();
	size_t sf = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(p_polyhedral_program.getStatementById(p_from)).getNumberOfIterators();
	size_t sg = p_polyhedral_program.getNumberOfGlobals();
	
	out << cs_size << " " << sf + st + sg + 2 << std::endl;
	
	out << "#       |" << std::setw(st * 3 + 1) << "to" << std::setw(st * 3 - 1) << "|"
					<< std::setw(sf * 3 + 2) << "from" << std::setw(sf * 3 - 2) << "|" << std::endl;
	
	out << "#eq/in";
	for (int i = 0; i < st; i++)
		out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_polyhedral_program.getStatementById(p_to), i));
	for (int i = 0; i < sf; i++)
		out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_polyhedral_program.getStatementById(p_from), i));
	for (int i = 0; i < sg; i++)
		out << std::setw(6) << toString(p_polyhedral_program.getGlobalById(i));
	out << std::setw(6) << "1" << std::endl;
	
	for (it = cs.begin(); it != cs.end(); it++) {
		if ((*it).is_equality())
			out << std::setw(6) << "0";
		else
			out << std::setw(6) << "1";
		for (int i = 0; i < sf + st + sg; i++)
			out << std::setw(6) << (*it).coefficient(VariableID(i));
		out << std::setw(6) << (*it).inhomogeneous_term() << std::endl;
	}
	out << std::endl;
	
	
}

