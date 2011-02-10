#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#ifndef CONTROL_STRUCTURE_H
#define CONTROL_STRUCTURE_H

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
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
  ControlStructureContainer ()  //(string tempComment, SgNode* tempStatement)
  {
    /*
       comment = tempComment;
       associatedStatement = tempStatement;
     */
  }

  string
  getPragmaString ();
  void  setPragmaString (string tempComment);

  SgNode*  getAssociatedStatement ();
  void     setAssociatedStatement (SgNode * tempStatement);
  
private:
  string comment;
  SgNode* associatedStatement;
};

extern  string leftTrim (string str);
extern  string rightTrim (string str);
extern  string trim (string str);
extern  string checkPragmaRHSUnionControl (const list < SgNode * >unionFields,
                            const list < SgNode * >classFields,
                            const string pragmaRHS);
extern  string parsePragmaStringRHS (string pragmaString, string prefix, string name);
extern  string parsePragmaStringLHS (string pragmaString, string prefix, string terminator);
extern  list <ControlStructureContainer * > queryFindCommentsInScope (const string stringPrefixToMatch,
                          const string stringToMatch, 
                          SgScopeStatement * sageScopeStatement);
#endif
