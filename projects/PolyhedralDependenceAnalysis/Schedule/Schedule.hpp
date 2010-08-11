
#include "FadaToPPL.hpp"

#include <vector>
#include <map>
#include <ostream>

#ifndef __SCHEDULE_PPL_
#define __SCHEDULE_PPL_

namespace Parma_Polyhedra_Library {
	class Variable;
	class C_Polyhedron;
	class Constraint;
	class Constraint_System;
}

namespace PolyhedralSchedule {

class ScheduleVariablesMappingPPL;
class ProgramSchedule;

class ValidScheduleSpacePPL {
	protected:
		std::map<SgStatement *, ScheduleVariablesMappingPPL *> p_map_statement_variables_mapping;
		FadaToPPL::PolyhedricContext * p_context;
		int p_variables_counter;
		int p_lambdas_counter;
		Parma_Polyhedra_Library::C_Polyhedron * p_polyhedron;
	
	public:
		ValidScheduleSpacePPL(
			FadaToPPL::PolyhedricContext * ctx,
			std::vector<FadaToPPL::PolyhedricDependence *> * dependences
		);
		
		void FillPolyhedron(std::vector<FadaToPPL::PolyhedricDependence *> * dependences);
		void AddDependenceToPolyhedron(FadaToPPL::PolyhedricDependence * dependence);
		
		int nextVariable();
		
		void print(std::ostream & out, bool not_minimized = false);
		
		std::vector<ProgramSchedule *> * generateAllValidSchedules();
		
		void bounding(int min_param_bound = -2, int max_param_bound = 2, int lambda_bound = 1);
		void playWithGenerator();
};

class ScheduleVariablesMappingPPL {
	protected:
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_iterators_coeficiant;
		std::map<std::string, Parma_Polyhedra_Library::Variable> p_globals_coeficiant;
		int p_constant_coeficiant;
	
	public:
		ScheduleVariablesMappingPPL(
			FadaToPPL::PolyhedricContext * ctx,
			SgStatement * statement,
			ValidScheduleSpacePPL * related_schedule_space
		);
		
		Parma_Polyhedra_Library::Variable * getIteratorCoeficiant(std::string & var);
		Parma_Polyhedra_Library::Variable * getGlobalCoeficiant(std::string & var);
		Parma_Polyhedra_Library::Variable * getConstantCoeficiant();
		
		friend std::vector<ProgramSchedule *> * ValidScheduleSpacePPL::generateAllValidSchedules();
		friend void ValidScheduleSpacePPL::print(std::ostream & out, bool not_minimized);
		friend void ValidScheduleSpacePPL::playWithGenerator();

};

class StatementSchedule {
	protected:
		std::vector<int> p_iterators_coeficiants;
		std::vector<int> p_globals_coeficiants;
		int p_constant;
	
	public:
		StatementSchedule():
			p_iterators_coeficiants(),
			p_globals_coeficiants(),
			p_constant(0)
		{}
		
		void setIteratorCoef(int val) { p_iterators_coeficiants.push_back(val); }
		void setGlobalCoef(int val)   { p_globals_coeficiants.push_back(val); }
		void setConstant(int val) { p_constant = val; }
		
		int getIteratorCoef(int idx) { return p_iterators_coeficiants[idx]; }
		int getGlobalCoef(int idx)   { return p_globals_coeficiants[idx]; }
		int getConstant() { return p_constant; }
		
		void print(FadaToPPL::PolyhedricContext * ctx, FadaToPPL::PolyhedricIterationDomain * stmt_info, std::ostream & out) {
			for (int i = 0; i < stmt_info->getNbrIterator(); i++) {
				out << getIteratorCoef(i) << " * " << stmt_info->getIteratorName(i) << " + ";
			}
			for (int i = 0; i < ctx->getNbrGlobals(); i++) {
				out << getGlobalCoef(i) << " * " << ctx->getGlobalName(i) << " + ";
			}
			out << getConstant();
		}
		
		friend bool operator == (StatementSchedule & s1, StatementSchedule & s2);
};
bool operator == (StatementSchedule & s1, StatementSchedule & s2);

class ProgramSchedule {
	protected:
		std::map<SgStatement *, StatementSchedule *> p_statement_schedules;
		
	public:
		ProgramSchedule() : p_statement_schedules() {}
		
		StatementSchedule * addStatement(SgStatement * statement) {
			StatementSchedule * tmp = new StatementSchedule();
			p_statement_schedules.insert(std::pair<SgStatement *, StatementSchedule *>(statement, tmp));
			return tmp;
		}
		
		StatementSchedule * getSchedule(SgStatement * statement) {
			std::map<SgStatement *, StatementSchedule *>::iterator it = p_statement_schedules.find(statement);
			if (it == p_statement_schedules.end())
				return NULL;
			else
				return it->second;
		}
		
		void print(FadaToPPL::PolyhedricContext * ctx, std::ostream & out) {
			std::map<SgStatement *, StatementSchedule *>::iterator it;
			int cnt = 0;
			for (it = p_statement_schedules.begin(); it != p_statement_schedules.end(); it++) {
				out << cnt++ << ": ";
				it->second->print(ctx, ctx->getDomain(it->first), out);
				out << ";" << std::endl;
			}
		}
		
		friend bool operator == (ProgramSchedule & s1, ProgramSchedule & s2);
};
bool operator == (ProgramSchedule & s1, ProgramSchedule & s2);

}

#endif /* __SCHEDULE_PPL_ */
