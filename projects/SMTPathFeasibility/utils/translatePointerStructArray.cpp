#include "rose.h"

void getAllArrayAccess(SgNode* search_space, std::vector<Sg

void getAllDirectFieldAccess

void getAllIndirectFieldAccess

void getAllDeref(SgNode* search_space, std::vector<Sg 


void getAllVars(SgNode* search_space, std::vector<SgVariableSymbol*> &symbols) {
	std::vector<SgNode*> vars = SageInterface::NodeQuery(search_space,V_SgVarRefExp);
	std::vector<SgNode*>::iterator i = vars.begin();
	for (; i != vars.end(); i++) {
		SgVarRefExp* ith_var = isSgVarRefExp(*i);
		SgVariableSymbol* ith_var_sym =	ith_var->get_symbol();
		if (find(symbols.begin(), symbols.end(), ith_var_sym) == symbols.end()) {
	
			symbols.push_back(ith_var_sym);
		}
	}
	return;
}	
