#include "sage3basic.h"
#include "ControlStructure.h"
/*
 * The function
 *     queryNodePragmaDeclarationFromName()
 * takes as a first parameter a SgNode*. As a second parameter it takes
 * a SgNode* who must be of type SgName. The SgName contains a string which
 * should be the same as the left side in the pragma or a part of the left
 * side of the pragma. If the string is empty,
 * there will be an error message.
 *
 *        #pragma stringInSgNode = information
 *         
 */
list<SgNode*> queryNodePragmaDeclarationFromName2(SgNode* node, SgNode* nameNode){
   ROSE_ASSERT( nameNode != NULL );
   ROSE_ASSERT( node     != NULL );
  
  list<SgNode*> returnList;
  
  //finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ROSE_ASSERT( sageName != NULL );
  string nameToMatch = sageName->str();
  ROSE_ASSERT( nameToMatch.length() > 0 );

  if(node->variantT() == V_SgPragmaDeclaration){
       	SgPragmaDeclaration* sagePragmaDeclaration = isSgPragmaDeclaration(node);
	ROSE_ASSERT( sagePragmaDeclaration );
	ROSE_ASSERT( sagePragmaDeclaration->get_pragma() != NULL ); 
	ROSE_ASSERT( sagePragmaDeclaration->get_pragma()->get_pragma() );
        string pragmaDeclarationString =  sagePragmaDeclaration->get_pragma()->get_pragma();
        //extract the part before the leftmost = is pragmaDeclarationString
	pragmaDeclarationString = pragmaDeclarationString.substr(0,pragmaDeclarationString.find("="));
	//if the name-criteria is met accept node
	if(pragmaDeclarationString.find( nameToMatch ) != pragmaDeclarationString.length() ){
		cout << pragmaDeclarationString << endl;
		returnList.push_back(node);
	}
   }
   return returnList;
}
/*
 * The function 
 *     queryPragmaString()
 * teakes a SgNode* as a parameter and return a list<string>. It is a NameQuery
 * which query the AST or a subtree of the AST for the string of all the 
 * SgPragma's.
 * 
 */
list < string > queryFindPragmaString (SgNode * node)
{
  list < string > returnList;

  if (node->variantT () == V_SgPragmaDeclaration)
    {
      SgPragmaDeclaration *sagePragmaDeclaration = isSgPragmaDeclaration (node);
      ROSE_ASSERT (sagePragmaDeclaration);
      ROSE_ASSERT (sagePragmaDeclaration->get_pragma () != NULL);
      ROSE_ASSERT (sagePragmaDeclaration->get_pragma ()->get_pragma ());
      returnList.push_back (sagePragmaDeclaration->get_pragma ()->
			    get_pragma ());
    }
  return returnList;

}

/*
 * The function
 *    queryFindPragmaStringUnionControl()
 * inputs as a first parameter the string from a pragma. As a second option it
 * takes the name of a union. It returns a string which is the name of the variable
 * which controls which of the union fields was modified last. If the query is 
 * unsucsessful the returnvalue is an empty string of length 0. 
 * 
 */
string findPragmaStringUnionControl (string pragmaString,
                                     string unionName)
{
  string name_prefix = "print_";
  string name_postfix = "_variable";
  if (pragmaString.find (name_prefix + unionName + name_postfix) !=
      pragmaString.length ())
    {
      pragmaString =
	StringUtility::copyEdit (pragmaString,
                                 name_prefix + unionName + name_postfix, "");
      pragmaString = StringUtility::copyEdit (pragmaString, "=", "");

      return pragmaString;
    }
  else
    return "";
}				/* End function: findPragmaStringUnionControl() */


/*
 * The function
 *    queryFindPragmaStringUnionControl()
 * inputs as a first parameter the string from a pragma. As a second option it
 * takes the prefix and as a third a name. It 
 * returns a string which is the right hand side if the query is 
 * unsucsessful the returnvalue is an empty string of length 0. 
 *          #pragma prefix_name = RHS
 * 
 */
string
parsePragmaStringRHS (string pragmaString, string prefix, string name)
{
  ROSE_ASSERT (prefix.length () > 0);
  ROSE_ASSERT (name.length () > 0);
  if (pragmaString.find (prefix + name) != string::npos)
    {
/*      pragmaString =
	StringUtility::copyEdit (pragmaString, prefix + name, "");
      pragmaString = StringUtility::copyEdit (pragmaString, "=", "");*/
      pragmaString =
	pragmaString.substr (pragmaString.find ("=") + 1,
                             pragmaString.length ());

      return trim (pragmaString);
    }
  else
    return "";
}				/* End method: parsePragmaStringRHS() */

/*
 *  The function:
 *      parsePragmaStringLHS()
 *  takes as a first parameter a string from a pragma. As a second option it takes
 *  the prefix and the third is the terminator of the LHS. It returns a string which
 *  is the LHS of 
 *         #pragma prefix_returnString terminator 
 */

string
parsePragmaStringLHS (string pragmaString, string prefix, string terminator)
{
  ROSE_ASSERT (prefix.length () > 0);
  ROSE_ASSERT (terminator.length () > 0);
  if ((pragmaString.find (prefix) !=
       string::npos) & (pragmaString.find (terminator) != string::npos))
    {
      pragmaString = StringUtility::copyEdit (pragmaString, prefix, "");
      pragmaString = pragmaString.substr (0, pragmaString.find (terminator));
//             pragmaString = pragmaString.substr(pragmaString.find(prefix)+prefix.length(), 
//                           pragmaString.find(terminator));
      return trim (pragmaString);
    }
  else
    return "";

}				/* End method: parsePragmaStringLHS() */

string ControlStructureContainer::getPragmaString ()
{
  return comment;
}

void
ControlStructureContainer::setPragmaString (string tempComment)
{
  comment = tempComment;
}

SgNode *
ControlStructureContainer::getAssociatedStatement ()
{
  ROSE_ASSERT (associatedStatement != NULL);
  return associatedStatement;

}

void
ControlStructureContainer::setAssociatedStatement (SgNode * tempStatement)
{
  associatedStatement = tempStatement;
}

/*
 * The function
 *     queryFindCommentsInScope(string stringPrefixToMatch, string stringToMatch, SgScopeStatement* sageScopeStatement)
 * as a first parameter it takes a string which equals what it expect to find in the beginning of the interesting
 * comments. As a second parameter it takes a string which is what you would like to
 * find on the left side of "="
 *         //stringPrefixToMatch stringToMatch = extractionString
 *         or
 *         #pragma stringToMatch = extractionString
 * As a third parameter it takes a SgScopeStatement. The return value is a
 * map<> of SgStatements and their corresponding strings.
 *
 */

list <
  ControlStructureContainer *
  >queryFindCommentsInScope (const string stringPrefixToMatch,
                             const string stringToMatch,
			     SgScopeStatement * sageScopeStatement)
{
  ROSE_ASSERT (stringPrefixToMatch.length () > 0);
  ROSE_ASSERT (stringToMatch.length () > 0);
  ROSE_ASSERT (sageScopeStatement != NULL);

  list < ControlStructureContainer * >returnList;

  //find all pragmas who match the stringToMatch

   list < SgNode * >pragmaDeclarations =
    NodeQuery::querySubTree (sageScopeStatement,
                             new SgName (stringToMatch.c_str ()),
			     NodeQuery::PragmaDeclarationFromName);


/*  cout << "BEFORE LIST" << endl; 
  list < SgNode * >pragmaDeclarations = queryNodePragmaDeclarationFromName2(sageScopeStatement,
		                            new SgName(stringToMatch.c_str()));
  cout << "AFTER LIST" << endl;*/
  //return the pragmas in containers
/*  for (list < SgNode * >::iterator i = pragmaDeclarations.begin ();
       i != pragmaDeclarations.end (); ++i)
    {
      SgPragmaDeclaration *sagePragma = isSgPragmaDeclaration (*i);
      ROSE_ASSERT (sagePragma);
      ROSE_ASSERT (sagePragma->get_pragma () != NULL);
      ROSE_ASSERT (sagePragma->get_pragma ()->get_pragma ());

      ControlStructureContainer *container = new ControlStructureContainer ();
      container->setPragmaString (sagePragma->get_pragma ()->get_pragma ());
      container->setAssociatedStatement (sagePragma);
      returnList.push_back (container);
    }
*/

  //find all statements in the current scope
  if (sageScopeStatement->variantT () == V_SgClassDefinition)
    {
      SgDeclarationStatementPtrList statementsInScope =
	sageScopeStatement->getDeclarationList ();
      SgDeclarationStatementPtrList::iterator i;
      for (i = statementsInScope.begin (); i != statementsInScope.end (); i++)
	{

	  SgLocatedNode *locatedNode = isSgLocatedNode (*i);
	  ROSE_ASSERT (locatedNode != NULL);

	  //find all comments attached to current node.
	  AttachedPreprocessingInfoType *comments =
	    locatedNode->getAttachedPreprocessingInfo ();

	  if (comments != NULL)
	    {
	      //We need to find comments which fits the criteria    
	      printf ("Found attached comments (at %p of type: %s): \n",
		      locatedNode, locatedNode->sage_class_name ());
	      AttachedPreprocessingInfoType::iterator j;
	      for (j = comments->begin (); j != comments->end (); j++)
		{
		  ROSE_ASSERT ((*j) != NULL);
		  string comment = (*j)->getString ();
		  //see if comment begins with stringPrefixToMatch
		  string tempString = comment.substr (0, comment.find (' '));
		  if (tempString == stringPrefixToMatch)
		    {		//+stringPrefixToMatch ){
		      //cout << "Found string" << endl;

		      comment =
			StringUtility::copyEdit (comment, stringPrefixToMatch,
                                                 "");

		      //see if the comment has an element which matches the stringToMatch
		      if (comment.find (stringToMatch) != string::npos)
			{
			  //puit the matching comment into a container    
			  ControlStructureContainer *container =
			    new ControlStructureContainer ();
			  container->setPragmaString (comment);
			  container->setAssociatedStatement (locatedNode);

			  returnList.push_back (container);
			}
		    }
//                printf ("          Attached Comment (relativePosition=%s):\n %s\n Next comment: \n",
//                    ((*j)->relativePosition == PreprocessingInfo::before) ? "before" : "after",(*j)->getString());

		}
	    }
	}
    }
  else
    {
      // AS 12/18/03 PS!! The same as the above, but a different iterator. Will replace this when a
      // different solution has arisen. PS!! 
      SgStatementPtrList statementsInScope =
	sageScopeStatement->getStatementList ();
      SgStatementPtrList::iterator i;

      for (i = statementsInScope.begin (); i != statementsInScope.end (); i++)
	{

	  SgLocatedNode *locatedNode = isSgLocatedNode (*i);
	  ROSE_ASSERT (locatedNode != NULL);

	  //find all comments attached to current node.
	  AttachedPreprocessingInfoType *comments =
	    locatedNode->getAttachedPreprocessingInfo ();

	  if (comments != NULL)
	    {
	      //We need to find comments which fits the criteria    
	      //printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
	      AttachedPreprocessingInfoType::iterator j;
	      for (j = comments->begin (); j != comments->end (); j++)
		{
		  ROSE_ASSERT ((*j) != NULL);
		  string comment = (*j)->getString ();
		  //see if comment begins with stringPrefixToMatch
		  string tempString = comment.substr (0, comment.find (' '));
		  if (tempString == stringPrefixToMatch)
		    {		//+stringPrefixToMatch ){
		      comment =
			StringUtility::copyEdit (comment, stringPrefixToMatch,
                                                 "");

		      cout << "And the string is: " << comment << endl;

		      if (comment.find (stringToMatch) != string::npos)
			{
			  cout << "And even the string is matched." << endl;
			  //ROSE_ASSERT(comment.find('=') != comment.length());
			  //string variableName = comment.substr(comment.find(stringToMatch), comment.find('='));
			  cout << "And the string is a match" << endl;


			  //puit the matching comment into a container    
			  ControlStructureContainer *container =
			    new ControlStructureContainer ();
			  container->setPragmaString (comment);
			  container->setAssociatedStatement (locatedNode);

			  returnList.push_back (container);
			}
		    }
		  printf
		    ("          Attached Comment (relativePosition=%s):\n %s\n Next comment: \n",
                     ((*j)->relativePosition ==
                      PreprocessingInfo::before) ? "before" : "after",
                     (*j)->getString ());

		}
	    }
	}

    }
  return returnList;
}				/* End function:  queryFindCommentsInScope() */

/*
list  <ControlStructureContainer* > 
      queryFindCommentsInScope (const string stringPrefixToMatch,
                             const string stringToMatch,
                             SgScopeStatement * sageScopeStatement)
{
 
  ROSE_ASSERT (stringPrefixToMatch.length () > 0);
  ROSE_ASSERT (stringToMatch.length () > 0);
  ROSE_ASSERT (sageScopeStatement != NULL);

  list < ControlStructureContainer * >returnList;

  //find all pragmas who match the stringToMatch
// AS(01/26/04) REMOVED BECAUSE OF BUG
   list < SgNode * >pragmaDeclarations =
    NodeQuery::querySubTree (sageScopeStatement,
                             new SgName (stringToMatch.c_str ()),
                             NodeQuery::PragmaDeclarationFromName);

//  cout << "queryFindCommentsInScope is checkpoint 2" << endl;
//  list < SgNode * >pragmaDeclarations =
//       PragmaDeclarationFromName2(sageScopeStatement, 
//                           new SgName (stringToMatch.c_str ()));

  //return the pragmas in containers
  for (list < SgNode * >::iterator i = pragmaDeclarations.begin ();
       i != pragmaDeclarations.end (); ++i)
    {
      SgPragmaDeclaration *sagePragma = isSgPragmaDeclaration(*i);
      ROSE_ASSERT (sagePragma);
      ROSE_ASSERT (sagePragma->get_pragma () != NULL);
      ROSE_ASSERT (sagePragma->get_pragma ()->get_pragma ());

      ControlStructureContainer *container = new ControlStructureContainer ();
      container->setPragmaString (sagePragma->get_pragma ()->get_pragma ());
      container->setAssociatedStatement (sagePragma);
      returnList.push_back (container);
    }


  //find all statements in the current scope
  if (sageScopeStatement->variantT () == V_SgClassDefinition)
    {
      SgDeclarationStatementPtrList statementsInScope =
        sageScopeStatement->getDeclarationList ();
      SgDeclarationStatementPtrList::iterator i;
      for (i = statementsInScope.begin (); i != statementsInScope.end (); i++)
        {

          SgLocatedNode *locatedNode = isSgLocatedNode (*i);
          ROSE_ASSERT (locatedNode != NULL);

          //find all comments attached to current node.
          AttachedPreprocessingInfoType *comments =
            locatedNode->getAttachedPreprocessingInfo ();

          if (comments != NULL)
            {
              //We need to find comments which fits the crite
#ifdef DEBUG_CGRAPHPP
              printf ("Found attached comments (at %p of type: %s): \n",
                      locatedNode, locatedNode->sage_class_name ());
#endif
              
              AttachedPreprocessingInfoType::iterator j;
              for (j = comments->begin (); j != comments->end (); j++)
                {
                  ROSE_ASSERT ((*j) != NULL);
                  string comment = (*j)->getString ();
                  //see if comment begins with stringPrefixToMatch
                  string tempString = comment.substr (0, comment.find (' '));
                  if (tempString == stringPrefixToMatch)
                    {           //+stringPrefixToMatch ){
                      //cout << "Found string" << endl;

                      comment =
                        StringUtility::copyEdit (comment, stringPrefixToMatch, "");

                      //see if the comment has an element which matches the stringToMatch
                      if (comment.find (stringToMatch) != string::npos)
                        {
                          cout << endl <<  comment << "  " << endl;
                          //puit the matching comment into a container    
                          ControlStructureContainer *container =
                            new ControlStructureContainer ();
                          container->setPragmaString (comment);
                          container->setAssociatedStatement (locatedNode);

                          returnList.push_back (container);
                        }
                    }
//                printf ("          Attached Comment (relativePosition=%s):\n %s\n Next comment: \n",
//                    ((*j)->relativePosition == PreprocessingInfo::before) ? "before" : "after",(*j)->getString());

                }
            }
        }
    }
  else
    {
      // AS 12/18/03 PS!! The same as the above, but a different iterator. Will replace this when a
      // different solution has arisen. PS!! 
      SgStatementPtrList statementsInScope =
        sageScopeStatement->getStatementList ();
      SgStatementPtrList::iterator i;

      for (i = statementsInScope.begin (); i != statementsInScope.end (); i++)
        {

          SgLocatedNode *locatedNode = isSgLocatedNode (*i);
          ROSE_ASSERT (locatedNode != NULL);

          //find all comments attached to current node.
          AttachedPreprocessingInfoType *comments =
            locatedNode->getAttachedPreprocessingInfo ();

          if (comments != NULL)
            {
              //We need to find comments which fits the criteria    
              //printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
              AttachedPreprocessingInfoType::iterator j;
              for (j = comments->begin (); j != comments->end (); j++)
                {
                  ROSE_ASSERT ((*j) != NULL);
                  string comment = (*j)->getString ();
                  //see if comment begins with stringPrefixToMatch
                  string tempString = comment.substr (0, comment.find (' '));
                  if (tempString == stringPrefixToMatch)
                    {           //+stringPrefixToMatch ){
                      comment =
                        StringUtility::copyEdit (comment, stringPrefixToMatch,
                                                 "");

                      if (comment.find (stringToMatch) != comment.length ())
                        {
                          //ROSE_ASSERT(comment.find('=') != comment.length());
                          //string variableName = comment.substr(comment.find(stringToMatch), comment.find('='));

                          //puit the matching comment into a container    
                          ControlStructureContainer *container =
                            new ControlStructureContainer ();
                          container->setPragmaString (comment);
                          container->setAssociatedStatement (locatedNode);

                          returnList.push_back (container);
                        }
                    }
#ifdef DEBUG_CGRAPHPP
                  printf
                    ("          Attached Comment (relativePosition=%s):\n %s\n Next comment: \n",
                     ((*j)->relativePosition ==
                      PreprocessingInfo::before) ? "before" : "after",
                     (*j)->getString ());
#endif

                }
            }
        }

    }
  return returnList;
}       */                      /* End function:  queryFindCommentsInScope() */

string
checkPragmaRHSUnionControl (const list < SgNode * >unionFields,
			    const list < SgNode * >classFields,
			    const string pragmaRHS)
{
  string returnString = "";
cout << "BEFORE checkPragmaRHSUnionControl" << endl;
  const string leftCondition = "(";
  const string rightCondition = ")";
  string controlVariable = "";
  string condition = "";

  string substring = pragmaRHS;
  string subConditionToCheck;

  int leftBorder = 0;
  int rightBorder = 0;
  while (substring.find (leftCondition) != string::npos)
    {
      string tempString = "";
      leftBorder = substring.find (leftCondition);
      rightBorder = substring.find (rightCondition);

      subConditionToCheck =
	substring.substr (leftBorder + 1, rightBorder - 1);

      if (subConditionToCheck.find ("==") != string::npos)
	{
	  string controlVariableName =
	    subConditionToCheck.substr (0, subConditionToCheck.find ("=="));
	  string condition =
	    subConditionToCheck.substr (subConditionToCheck.find ("==") + 2,
					subConditionToCheck.length ());

	  //check to see if a a variable exist in the parent scope of the union with the name controlVariableName 
	  list < SgNode * >variableDeclaration =
	    NodeQuery::queryNodeList (classFields,
				      new SgName (controlVariableName.
						  c_str ()),
				      NodeQuery::VariableDeclarationFromName);
	  ROSE_ASSERT (variableDeclaration.empty () == false);

	  tempString =
	    " (classReference->" + controlVariableName + "==" + condition +
	    ") ";
	}
      else if (subConditionToCheck.find ("!=") != string::npos)
	{
	  string controlVariableName =
	    subConditionToCheck.substr (0, subConditionToCheck.find ("!="));
	  string condition =
	    subConditionToCheck.substr (subConditionToCheck.find ("!=") + 2,
					subConditionToCheck.length ());

	  //check to see if a a variable exist in the parent scope of the union with the name controlVariableName 
	  list < SgNode * >variableDeclaration =
	    NodeQuery::queryNodeList (classFields,
				      new SgName (controlVariableName.
						  c_str ()),
				      NodeQuery::VariableDeclarationFromName);
	  ROSE_ASSERT (variableDeclaration.empty () == false);

	  tempString =
	    " (classReference->" + controlVariableName + "!=" + condition +
	    ") ";
	}
      else
	{

	  cerr <<
	    "Case not implemented yet in (controlVariableName??condition). Terminating.\n";
	  cout << pragmaRHS << "\n";
	  exit (1);

	}
      //check to see if there exist any more control variables
      substring = substring.substr (rightBorder + 1, substring.length ());

      if (substring.find (leftCondition) != string::npos)
	{
	  if (substring.find ("&") != string::npos)
	    {
	      returnString = returnString + tempString + " & ";
	      substring =
		substring.substr (substring.find ("&") + 1,
				  substring.length ());
	    }
	  else if (substring.find ("|") != string::npos)
	    {
	      returnString = returnString + tempString + " | ";
	      substring =
		substring.substr (substring.find ("|") + 1,
				  substring.length ());

	    }
	  else
	    {
	      cerr << "Case not implemented yet in (--)?(--). Terminating\n";
	      cout << pragmaRHS << "\"\n" << substring << "\"\n";
	      cout << substring.
		find (leftCondition) << "::" << string::npos << "\n";
	      exit (1);
	    }

	}
      else
	returnString = returnString + tempString;
    }				/* end while */

cout << "AFTER checkPragmaRHSUnionControl" << endl;


  
  return "if(" + returnString + ")";
};				/* End function: checkPragmaRHSUnionControl() */


