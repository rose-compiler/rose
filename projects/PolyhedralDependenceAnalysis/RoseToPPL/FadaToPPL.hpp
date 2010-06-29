
#include "rose.h"
#include "fada.h"

#include "RoseToFadaCommon.hpp"

#ifndef __FADA_TO_PPL_
#define __FADA_TO_PPL_

namespace Parma_Polyhedra_Library {
	class Variable;
	class Polyhedron;
	class Constraint;
	class Constraint_System;
}

namespace FadaToPPL {

class PolyhedricDependence {
	protected:
		SgStatement * p_source;
		SgStatement * p_destination;

		std::map<std::string, Parma_Polyhedra_Library::Variable> p_source_iterators;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_destination_iterators;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_globals;
		
		Parma_Polyhedra_Library::Constraint_System * p_minimized_constraints;

		Parma_Polyhedra_Library::Polyhedron * p_polyhedron;
		
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

// Recursive function that traverse a Quast (Condition Tree) to generate all PolyhedricDependence
// --> look for statement
//  o  Create dependance
// <-- Add Condition to the dependence
std::vector<PolyhedricDependence *> * traverseQuast(RoseToFada::FadaRoseCrossContext * ctx, SgStatement * destination, fada::Quast * quast);

}

#endif /* __FADA_TO_POLYHEDRON_ */
