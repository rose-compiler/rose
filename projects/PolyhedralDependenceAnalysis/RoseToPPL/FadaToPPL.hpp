
#include "rose.h"
#include "fada.h"

#include "RoseToFadaCommon.hpp"

#include <vector>
#include <map>

#ifndef __FADA_TO_PPL_
#define __FADA_TO_PPL_

namespace Parma_Polyhedra_Library {
	class Variable;
	class C_Polyhedron;
	class Constraint;
	class Constraint_System;
	class Linear_Expression;
}

namespace FadaToPPL {

class PolyhedricIterationDomain {
	protected:
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_iterators;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_globals;
		std::vector<std::string> p_iterator_list;
		Parma_Polyhedra_Library::C_Polyhedron * p_domain;
		
		Parma_Polyhedra_Library::Linear_Expression * parseExpression(fada::Expression * expression);
		void parseDomain(fada::Condition * condition, bool negation);
	
	public:
		PolyhedricIterationDomain(RoseToFada::FadaStatementAttribute * attr, RoseToFada::FadaRoseCrossContext * ctx);
		
		std::vector<std::string> * getIteratorsList();
		
		int getNbrIterator();
		std::string & getIteratorName(int idx);
		Parma_Polyhedra_Library::Variable * getIteratorVariable(int idx);
		Parma_Polyhedra_Library::Variable * getIteratorVariable(std::string & iterator);
		
		Parma_Polyhedra_Library::Variable * getGlobalVariable(std::string & globals);
		
		Parma_Polyhedra_Library::C_Polyhedron * getDomain();
};

class PolyhedricContext {
	protected:
		std::map<SgStatement *, PolyhedricIterationDomain *> p_iteration_domains;
		std::vector<std::string> p_globals;
	
	public:
		PolyhedricContext(RoseToFada::FadaRoseCrossContext * ctx);
		
		int getNbrGlobals();
		std::string & getGlobalName(int idx);
		std::vector<std::string> * getGlobals();
		
		std::vector<SgStatement *> * getStatements();
		
		PolyhedricIterationDomain * getDomain(SgStatement * statement);
};

class PolyhedricDependence {
	protected:
		SgStatement * p_source;
		SgStatement * p_destination;

		std::map<std::string, Parma_Polyhedra_Library::Variable> p_source_iterators;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_destination_iterators;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_globals;
		
		Parma_Polyhedra_Library::Constraint_System * p_minimized_constraints;

		Parma_Polyhedra_Library::C_Polyhedron * p_polyhedron;
		
		Parma_Polyhedra_Library::Constraint_System * createConstraints();

	public:
		PolyhedricDependence(SgStatement * source, SgStatement * destination,
						std::vector<std::string *> * source_iterators,
						std::vector<std::string *> * destination_iterators,
						std::vector<std::string *> * globals);

		void addConstraint(Parma_Polyhedra_Library::Constraint * constraint);
		
		Parma_Polyhedra_Library::Variable * getSourceIterator(std::string & var);
		Parma_Polyhedra_Library::Variable * getDestinationIterator(std::string & var);
		Parma_Polyhedra_Library::Variable * getGlobal(std::string & var);
		
		SgStatement * getSource();
		SgStatement * getDestination();
		
		int getNbrConstraints();
		Parma_Polyhedra_Library::Constraint_System * getConstraints();

		void printMinimized(std::ostream &);
		void print(std::ostream &);
};

std::vector<PolyhedricDependence *> * constructAllPolyhedricDependences(RoseToFada::FadaRoseCrossContext * ctx, fada::Program * program);

// Recursive function that traverse a Quast (Condition Tree) to generate all PolyhedricDependence
// --> look for statement
//  o  Create dependance
// <-- Add Condition to the dependence
std::vector<PolyhedricDependence *> * traverseQuast(RoseToFada::FadaRoseCrossContext * ctx, SgStatement * destination, fada::Quast * quast);

}

#endif /* __FADA_TO_POLYHEDRON_ */
