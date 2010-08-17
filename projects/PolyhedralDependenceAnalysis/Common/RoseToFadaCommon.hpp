
#include "rose.h"
#include "fada.h"

#include <vector>
#include <map>
#include <utility>

#ifndef __ROSE_TO_FADA_COMMON_
#define __ROSE_TO_FADA_COMMON_

namespace RoseToFada {

class FadaStatementAttribute : public AstAttribute {
	protected:
		int p_id;
		bool p_need_schedule;
		std::vector<std::string *> * p_iterators;
		fada::Condition * p_domain;
	
	public:
		FadaStatementAttribute(int id, bool need_schedule = false);
		
		int getID();
		
		bool needSchedule();
		
		void setIterators(std::vector<std::string *> * iterators);
		std::vector<std::string *> * getIterators();
		
		void setDomain(fada::Condition * domain);
		fada::Condition * getDomain();
};

fada::Statement * getFadaStatementByID(fada::Statement * root, int id);

class FadaRoseCrossContext {
	protected:
		fada::Program * p_fada_program;
		std::map<int, SgStatement *> p_rose_statement_mapped_by_fada_index;
		std::vector<std::string *> p_globals;
	
	public:
		FadaRoseCrossContext(fada::Program * program, SgFunctionDeclaration * func_decl);
		
		SgStatement * getSgStatementByID(int id);
		
		std::vector<std::string *> * getGlobals();
		
		std::vector<SgStatement *> * getStatements();
		
	protected:
		// recursivelly add statement from SgFunctionDeclaration argument of the constructor.
		void addSgStatement(SgStatement * statement);
	
		// return true if the statement has a FadaStatementAttribute (and so if the statement has been effectivelly added to this context)
		bool appendSgStatement(SgStatement * statement);
};

} 

#endif /* __ROSE_TO_FADA_COMMON_ */
