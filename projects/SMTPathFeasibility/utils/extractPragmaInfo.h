#ifndef HH_EXTRACTPRAGMAINFO_HH
#define HH_EXTRACTPRAGMAINFO_HH
#include <vector>
extern std::map<std::string,std::string> origNameToSMTName;


std::string get_pragma_string(SgPragmaDeclaration*);
std::string getRHS(std::string,std::set<std::string>&);
std::string translateToS_Expr(std::string,std::set<std::string>&, std::vector<std::string>&);



#endif

