
#include "RoseToFadaCommon.hpp"
#include "FadaToPPL.hpp"

#include <vector>
#include <map>
#include <ostream>

#ifndef __SCHEDULE_PPL_
#define __SCHEDULE_PPL_

namespace Parma_Polyhedra_Library {
	class Variable;
	class Polyhedron;
	class Constraint;
	class Constraint_System;
}

namespace PolyhedralSchedule {

class ScheduleVariablesMappingPPL;

class ValidScheduleSpacePPL {
	protected:
		std::map<SgStatement *, ScheduleVariablesMappingPPL *> p_map_statement_variables_mapping;
		RoseToFada::FadaRoseCrossContext * p_context;
		int p_variables_counter;
		Parma_Polyhedra_Library::Polyhedron * p_polyhedron;
	
	public:
		ValidScheduleSpacePPL(
			RoseToFada::FadaRoseCrossContext * ctx,
			std::vector<FadaToPPL::PolyhedricDependence *> & dependences
		);
		
		void FillPolyhedron(std::vector<FadaToPPL::PolyhedricDependence *> & dependences);
		void AddDependenceToPolyhedron(FadaToPPL::PolyhedricDependence * dependence);
		
		int nextVariable();
		
		void print(std::ostream & out);
};

class ScheduleVariablesMappingPPL {
	protected:
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_iterators_coeficiant;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_globals_coeficiant;
		int p_constant_coeficiant;
	
	public:
		ScheduleVariablesMappingPPL(
			RoseToFada::FadaRoseCrossContext * ctx,
			SgStatement * statement,
			ValidScheduleSpacePPL * related_schedule_space
		);
		
		Parma_Polyhedra_Library::Variable * getIteratorCoeficiant(std::string & var);
		Parma_Polyhedra_Library::Variable * getGlobalCoeficiant(std::string & var);
		Parma_Polyhedra_Library::Variable * getConstantCoeficiant();
		
		friend void ValidScheduleSpacePPL::print(std::ostream & out);

};

}

#endif /* __SCHEDULE_PPL_ */
