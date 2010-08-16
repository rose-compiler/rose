
#include "RoseToFadaCommon.hpp"

namespace RoseToFada {

/* class FadaStatementAttribute */

FadaStatementAttribute::FadaStatementAttribute(int id, bool need_schedule):
	p_id(id),
	p_need_schedule(need_schedule)
{
//	std::cout << "new Attribute: " << p_id << "." << std::endl;
}
		
int FadaStatementAttribute::getID() { return p_id; }

bool FadaStatementAttribute::needSchedule() { return p_need_schedule; }
		
void FadaStatementAttribute::setIterators(std::vector<std::string *> * iterators) { p_iterators = iterators; }
std::vector<std::string *> * FadaStatementAttribute::getIterators() { return p_iterators; }

void FadaStatementAttribute::setDomain(fada::Condition * domain) { p_domain = domain; }
fada::Condition * FadaStatementAttribute::getDomain() { return p_domain; }

/* function getFadaStatementByID(...) */

fada::Statement * getFadaStatementByID(fada::Statement * root, int id) {
	if (!root)
		return NULL;

	if (root->GetID() == id)
		return root;
	
	std::vector<Statement * > * vect;	
	std::vector<Statement * >::iterator it;
	fada::Statement * tmp;
	
	vect = root->GetSurroundedStatements();
	for (it = vect->begin(); it != vect->end(); it++) {
		tmp = getFadaStatementByID(*it, id);
		if (tmp)
			return tmp;
	}
	
	vect = root->GetElseSurroundedStatements();
	for (it = vect->begin(); it != vect->end(); it++) {
		tmp = getFadaStatementByID(*it, id);
		if (tmp)
			return tmp;
	}
	
	return NULL;
}

/* FadaRoseCrossContext */

FadaRoseCrossContext::FadaRoseCrossContext(fada::Program * program, SgFunctionDeclaration * func_decl) :
	p_fada_program(program),
	p_rose_statement_mapped_by_fada_index(),
	p_globals()
{
	std::vector<std::string> * globals = p_fada_program->GetGlobalParameters();
	std::vector<std::string>::iterator it;
	for (it = globals->begin(); it != globals->end(); it++) {
//		if (!( (*it).size() == 1 && (*it)[0] >= '0' && (*it)[0] <= '9' ))
		if (!( (*it)[0] >= '0' && (*it)[0] <= '9' ))
			p_globals.push_back(new std::string(*it));
	}
	
	SgFunctionDefinition * func_def = isSgFunctionDefinition(func_decl->get_definition());
	ROSE_ASSERT(func_def != NULL);

	SgBasicBlock * bb = isSgBasicBlock(func_def->get_body());
	ROSE_ASSERT(bb != NULL);
	
	addSgStatement(bb);
}
		
SgStatement * FadaRoseCrossContext::getSgStatementByID(int id) { return p_rose_statement_mapped_by_fada_index[id]; }
		
std::vector<std::string *> * FadaRoseCrossContext::getGlobals() { return &p_globals; }

std::vector<SgStatement *> * FadaRoseCrossContext::getStatements() {
	std::vector<SgStatement *> * res = new std::vector<SgStatement *>();
	std::map<int, SgStatement *>::iterator it;
	for (it = p_rose_statement_mapped_by_fada_index.begin(); it != p_rose_statement_mapped_by_fada_index.end(); it++)
		res->push_back(it->second);
	return res;
}


void FadaRoseCrossContext::addSgStatement(SgStatement * statement) {
//	if (appendSgStatement(statement))	
//		std::cout << "Append a statement !" << std::endl;
	appendSgStatement(statement);
	
	switch (statement->variantT()) {
		case V_SgBasicBlock:
		{
			SgBasicBlock * bb_stmt = isSgBasicBlock(statement);
			ROSE_ASSERT(bb_stmt != NULL);
			SgStatementPtrList & stmt_list = bb_stmt->get_statements();
			SgStatementPtrList::iterator stmt_it;
			for (stmt_it = stmt_list.begin(); stmt_it != stmt_list.end(); stmt_it++) {
				addSgStatement(*stmt_it);
			}
			break;
		}
		case V_SgForStatement:
		{
			SgForStatement * for_stmt = isSgForStatement(statement);
			ROSE_ASSERT(for_stmt != NULL);
			addSgStatement(for_stmt->get_loop_body());
			break;
		}
		case V_SgExprStatement:
//			appendSgStatement(statement);
			break;
		case V_SgVariableDeclaration:
			break;
		default:
			std::cerr << "Error in FadaRoseCrossContext::addSgStatement(...): Unsupported Sage statement !" << std::endl;
			ROSE_ASSERT(false);
	}
}

bool FadaRoseCrossContext::appendSgStatement(SgStatement * statement) {
	AstAttribute * attrib = statement->getAttribute("FadaStatement");
	
	if (!attrib) {
		return false;
	}
	
	FadaStatementAttribute * attribute = dynamic_cast<FadaStatementAttribute *>(attrib);
		
	if (!attribute) {
		std::cerr << "Error in FadaRoseCrossContext::addSgStatement(...): Unable to cast Statement Attribute !" << std::endl;
		ROSE_ASSERT(false);
	}
			
	fada::Statement * fada_statement = getFadaStatementByID(p_fada_program->GetSyntaxTree(), attribute->getID());
			
	if (!fada_statement) {
		std::cerr << "Error in FadaRoseCrossContext::appendSgStatement(...): Referenced id ( = " << attribute->getID() << ") doesn't match with a Fada's statement !" << std::endl;
		ROSE_ASSERT(false);
	}
			
	std::vector<std::string *> * iterators_ptr = new std::vector<std::string *>();
	std::vector<std::string> * iterators = fada_statement->GetReferences()->GetCounters();
	std::vector<std::string>::iterator it;
	for (it = iterators->begin(); it != iterators->end(); it++) {
		iterators_ptr->push_back(new std::string(*it));
	}
		
	attribute->setIterators(iterators_ptr);
	
	attribute->setDomain(fada_statement->GetReferences()->GetDomain());
	
	p_rose_statement_mapped_by_fada_index.insert(std::pair<int, SgStatement *>(attribute->getID(), statement));
	
	return true;
}

}
