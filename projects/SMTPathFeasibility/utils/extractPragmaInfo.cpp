#include <string>
#include <locale>
#include <vector>
#include <iostream>
#include "rose.h"
//#include "extractPragmaInfo.h"
#include "utilHeader.h"


std::string get_pragma_string(SgPragmaDeclaration* prdecl) {
        ROSE_ASSERT(prdecl!=NULL);
        SgPragma* pr = prdecl->get_pragma();
        std::string prag = pr->get_pragma();
	prag.erase(remove_if(prag.begin(),prag.end(),::isspace),prag.end());
        return prag;

}

//std::string translateToS_Expression(std::string exprString) {
//	std::string result = "";
//	
//	return result;

//}

std::string getRHS(std::string rhs, std::set<std::string>& variables) {
        int k = 0;
        std::string s_expression;
        std::string rhslhs;
        std::string rhsrhs;
        std::string rhsop;
        int rhssize = rhs.size();
	
        while (std::isalnum(rhs[k]) || rhs[k]=='_') {
                rhslhs.push_back(rhs[k]);
	        k++;
        }
        while (!std::isalnum(rhs[k]) && rhs[k]!='_') {
                rhsop.push_back(rhs[k]);
                k++;
        }
        if (k > rhs.size()-1) {
               if (std::isalpha(rhslhs[0]) || rhslhs[0]=='_') {
			rhslhs = origNameToSMTName[rhslhs];
			if (variables.find(rhslhs) == variables.end()) {
			variables.insert(rhslhs);
			
			}
		} 
	       s_expression=rhslhs;
        }
        else {
                rhsrhs = rhs.substr(k,rhssize-k+1);
		if (std::isalpha(rhslhs[0]) || rhslhs[0]=='_') {
			rhslhs = origNameToSMTName[rhslhs];
			if (variables.find(rhslhs) == variables.end()) {
			variables.insert(rhslhs);
			}
		}
		if (rhsop == "%") {
			rhsop = "cmod";
		}
                s_expression = "(" + rhsop + " " + rhslhs + " " + getRHS(rhsrhs, variables) + ")";
        }
        return s_expression;
}


std::string translateToS_Expr(std::string pragma_string, std::set<std::string>& variables,std::vector<std::string>& s_expressions,bool &initPrag) {
	pragma_string.erase(remove_if(pragma_string.begin(), pragma_string.end(), ::isspace), pragma_string.end());
int len = pragma_string.size();
int start_position = 4;
initPrag = false;
std::string typeOfSMTPragma = "";
if (pragma_string.at(start_position-1) != '[') {
	std::string init_string;
	init_string = pragma_string.substr(start_position-1,5);
	ROSE_ASSERT(init_string == "init[");
	initPrag = true;
	//std::cout << "init_string: " << init_string << std::endl;
	start_position += 4;	
}
pragma_string = pragma_string.substr(start_position,len-5);
//std::cout << "pragma_string: " << pragma_string <<std::endl;
std::istringstream ts(pragma_string);
std::string token;
std::vector<std::string> tokens;
while (std::getline(ts,token,',')) {
        tokens.push_back(token);
}
std::stringstream returnstream;
for (int i = 0; i < tokens.size(); i++) {
        std::string lhs;
        std::string varOperator;
        std::string rhs;
        int token_size = tokens[i].size();
        std::string ithToken = tokens[i];
        int k = 0;
        while (std::isalnum(ithToken[k]) || ithToken[k]=='_') {
                lhs.push_back(ithToken[k]);
                k++;
        }
	
        while (!std::isalnum(ithToken[k]) && ithToken[k] != '_'){
                varOperator.push_back(ithToken[k]);
                k++;
        }
        rhs = tokens[i].substr(k,token_size-k);
        //std::stringstream returnstream;
	if (std::isalpha(lhs[0]) || lhs[0]=='_') {
		lhs = origNameToSMTName[lhs];
		if (variables.find(lhs) == variables.end()) {
		variables.insert(lhs);
		}
	}
	std::string returnstring = "(" + varOperator + " " + lhs + " " + getRHS(rhs,variables) + ")\n";
	s_expressions.push_back(returnstring);
	returnstream << "(" << varOperator << " " << lhs << " " << getRHS(rhs, variables) << ")" << std::endl;
	}
	return returnstream.str();
}



	
/*
int main(int argc, char** argv) {
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);
  std::string prag_string;
Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(proj,V_SgPragmaDeclaration);
  for (Rose_STL_Container<SgNode*>::iterator i = pragmaList.begin(); i != pragmaList.end(); i++) {
	SgPragmaDeclaration* prg = isSgPragmaDeclaration(*i);
	prag_string = get_pragma_string(prg);
	std::cout << "prag_string: " << prag_string << std::endl;
	std::string translation = translateToS_Expr(prag_string);	
	std::cout << "translation: " << translation << std::endl;
	// get value of variable from pragma

	std::vector<std::string> expressions = extractExpressions(pragma_string)
	for (int i = 0; i < expressions.size(); i++) {
		std::string ithExpr = translateToS_Expression(expressions[i]);
		S_Exprs.push_back(ithExpr);
	}

	}

  return 0;
}*/
