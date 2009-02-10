//#include "rose.h"
#include "helpFunctions.h"

#ifndef CONTROL_STRUCTURE_H
#define CONTROL_STRUCTURE_H
// using namespace std;
/*
 *  The class:
 *      ControlStructureContainer
 *  has the purpose to avail a unified treatment of controlstructures written
 *  as comments or pragmas.
 */
class ControlStructureContainer
{
public:
  ControlStructureContainer ()	//(string tempComment, SgNode* tempStatement)
  {
    /*
       comment = tempComment;
       associatedStatement = tempStatement;
     */
  }

  std::string
  getPragmaString ();
  void  setPragmaString (std::string tempComment);

  SgNode*  getAssociatedStatement ();
  void     setAssociatedStatement (SgNode * tempStatement);
  
private:
  std::string comment;
  SgNode* associatedStatement;
};

extern  std::string leftTrim (std::string str);
extern  std::string rightTrim (std::string str);
extern  std::string trim (std::string str);
extern  std::string checkPragmaRHSUnionControl (const Rose_STL_Container< SgNode * >unionFields,
			    const Rose_STL_Container< SgNode * >classFields,
			    const std::string pragmaRHS, const std::string controlString);
extern  std::string parsePragmaStringRHS (std::string pragmaString, std::string prefix, std::string name);
extern  std::string parsePragmaStringLHS (std::string pragmaString, std::string prefix, std::string terminator);
extern  Rose_STL_Container<ControlStructureContainer * > queryFindCommentsInScope (const std::string stringPrefixToMatch,
			  const std::string stringToMatch, 
			  SgScopeStatement * sageScopeStatement);
#endif
