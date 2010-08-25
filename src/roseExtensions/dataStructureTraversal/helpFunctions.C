#include "sage3basic.h"
#include "helpFunctions.h"
/*
 * The function:
 *     leftTrim() 
 * takes as a parameter the string and  returns a copy of a string without 
 * leading spaces.
 */

string
leftTrim (string str)
{
  string whitespace = " \t\n\r";

  string s = str;

  if (whitespace.find (s.at (0)) != whitespace.length ())
    {
      // We have a string with leading blank(s)...

      int i = s.length ();
      unsigned int j;
      // Iterate from the far left of string until we
      // don't have any more whitespace...
      for (j = 0;
           (j != s.size ()) && (whitespace.find (s.at (j)) != string::npos);
           ++j)
        {
          //find the first none white space character
        }

      // Get the substring from the first non-whitespace
      // character to the end of the string...
      s = s.substr (j, i);
    }

  return s;
}

/*
 * The function:
 *       rightTrim() 
 * takes as a parameter a string and returns a copy of a string without 
 * trailing spaces.
 *
 */
string
rightTrim (string str)
{
  // We don't want to trip JUST spaces, but also tabs,
  // line feeds, etc.  Add anything else you want to
  // "trim" here in Whitespace
  string whitespace (" \t\n\r");

  string s (str);

  if (whitespace.find (s.at (s.size () - 1)) != string::npos)
    {
      // We have a string with trailing blank(s)...

      int j;
      // Iterate from the far right of string until we
      // don't have any more whitespace...
      for (j = s.size () - 1;
           (j != 0) && (whitespace.find (s.at (j)) != string::npos); --j)
        {
          //find the first none white space character
#ifdef DEBUG_CGRAPHPP
          cout << s.substr (0, j) << endl;
#endif
          
        }



      // Get the substring from the front of the string to
      // where the last non-whitespace character is...
      s = s.substr (0, j + 1);
    }

  return s;
}


/*
 * The function:
 *       Trim() 
 * takes a string as parameter and returns a copy of a string without leading 
 * or trailing spaces
 */

string
trim (string str)
{
  return rightTrim (leftTrim (str));
}

/*
 *  The function
 *      findScope()
 *  takes as a parameter a SgNode* which is a SgStatement*. It returns a SgNodePtrVector of all
 *  preceding scopes the SgStatement is in.
 *
 */
SgNodePtrVector
findScopes (SgNode * astNode)
{
  ROSE_ASSERT (isSgStatement (astNode));

  SgNodePtrVector returnVector;
  SgScopeStatement *currentScope;

  if (isSgScopeStatement (astNode))
    {
      currentScope = isSgScopeStatement (astNode);
      ROSE_ASSERT (currentScope != NULL);
      returnVector.push_back (astNode);
    }
  else
    {
      SgStatement *sageStatement = isSgStatement (astNode);
      ROSE_ASSERT (sageStatement != NULL);
      currentScope = sageStatement->get_scope ();
      ROSE_ASSERT (currentScope != NULL);
      returnVector.push_back (currentScope);
    }

  while (currentScope->variantT () != V_SgGlobal)
    {
      currentScope = currentScope->get_scope ();
      ROSE_ASSERT (currentScope != NULL);
      returnVector.push_back (currentScope);
    }

  //Must also include the Global Scopes of the other files in the project
  if (currentScope->variantT () == V_SgGlobal)
    {
      SgFile *sageFile = isSgFile ((currentScope)->get_parent ());
      ROSE_ASSERT (sageFile != NULL);
      SgProject *sageProject = isSgProject (sageFile->get_parent ());
      ROSE_ASSERT (sageProject != NULL);

      //Get a list of all files in the current project
      const SgFilePtrList sageFilePtrList = *sageProject->get_fileList ();

      //Iterate over the list of files to find all Global Scopes
      SgNodePtrVector globalScopes;
      for (unsigned int i = 0; i < sageFilePtrList.size (); i += 1)
        {
          const SgFile *sageFile = isSgFile (sageFilePtrList[i]);
          ROSE_ASSERT (sageFile != NULL);
          SgGlobal *sageGlobal = sageFile->get_root ();
          ROSE_ASSERT (sageGlobal != NULL);

          returnVector.push_back (sageGlobal);
        }
    }


  return returnVector;
};

list < SgNode * >queryNodeClassDeclarationFromTypedefName (SgNode * astNode,
                                                           SgNode * nameNode)
{
  NodeQuerySynthesizedAttributeType returnList;
  ROSE_ASSERT (nameNode != NULL);
  ROSE_ASSERT (nameNode != NULL);


  //finds the name which should be matched to 
  SgName *sageName = isSgName (nameNode);
  ROSE_ASSERT (sageName != NULL);
  string nameToMatch = sageName->str ();
  ROSE_ASSERT (nameToMatch.length () > 0);

  if (isSgType (astNode) != NULL)
    {
      /*SgTypedefType* sageTypedefType = isSgTypedefType(astNode);
         string name = TransformationSupport::getTypeName(sageTypedefType);
         ROSE_ASSERT( nameToMatch.length() > 0 );
         cout << nameToMatch << endl; */
#ifdef DEBUG_CGRAPHPP
      cout << TransformationSupport::getTypeName (isSgType (astNode)) << endl;
#endif
      if (TransformationSupport::getTypeName (isSgType (astNode)) ==
          nameToMatch)
        {
          returnList.push_back (astNode);
        }
/*
          if(nameToMatch == name){
                  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (sageTypedefType->get_declaration());
                  ROSE_ASSERT( sageClassDeclaration != NULL );
                  returnList.push_back(sageClassDeclaration);
          }*/
    }


  return returnList;



}

/*
 * The function
 *    findTypedefFromTypeName()
 * takes as a first parameter a vector<SgNode*> where SgNode*
 * is SgScopeStatement*, and as a second parameter it akes
 * a typename. It returns a unique instance of a typedef
 * corresponding to the typename in the scope. If it has
 * not found a corresponding typename it returns 0
* 
 *
 */
SgTypedefDeclaration *
findTypedefFromTypeName (SgNodePtrVector nodeVector, const string sageName)
{
  typedef SgNodePtrVector::iterator nodeIterator;
  list < SgNode * >tempNodeList, typedefDeclarationList,
    foundClassDeclarations;
  ROSE_ASSERT (sageName.length () > 0);

  for (nodeIterator i = nodeVector.begin (); i != nodeVector.end (); ++i)
    {
      ROSE_ASSERT (isSgScopeStatement (*i) != NULL);

      typedefDeclarationList =
        NodeQuery::querySubTree (*i,
                                 NodeQuery::TypedefDeclarations,
                                 NodeQuery::ChildrenOnly);

      list < SgNode * >::iterator j;

      for (j = typedefDeclarationList.begin ();
           j != typedefDeclarationList.end (); ++j)
        {
          // Take an action on each typedef declarations: generate a list of variable declarations
          ROSE_ASSERT ((*j) != NULL);

          // list<SgNode*> variableDeclarationList = NodeQuery::getTypeDefDeclarations(astNode,NodeQuery::ChildrenOnly);

          SgTypedefDeclaration *typedefDeclaration =
            isSgTypedefDeclaration (*j);
          ROSE_ASSERT (typedefDeclaration != NULL);

          string typeName = typedefDeclaration->get_name ().str ();

          if (typeName == sageName)
            return typedefDeclaration;
        }
    }

  return NULL;
}

/*
 *   The function
 *        findClassDeclarationsFromTypeName()
 *   takes as a first parameter a vector<SgNode*> where SgNode*
 *   is SgScopeStatement*, and as a second parameter it takes
 *   a typename. It returns a unique instance of a class 
 *   corresponding to the typename in the scope. 
 *
 */
list <
  SgNode * >findClassDeclarationsFromTypeName (SgNodePtrVector nodeVector,
                                               const string sageName)
{
  ROSE_ASSERT (sageName.length () > 0);

  typedef SgNodePtrVector::iterator nodeIterator;
  list < SgNode * >tempNodeList, typedefDeclarationList,
    foundClassDeclarations;

  //First step is to see if there exist a TypedefDeclaration which fits to this sageName
  /*                  SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(findTypedefFromTypeName(nodeVector,sageName));
     if(typedefDeclaration != NULL){
     SgTypedefType* typedefType = isSgTypedefType(typedefDeclaration->get_type());
     ROSE_ASSERT (typedefType != NULL);

     printf ("Found a typedef which maches the sageName.\n typedefType->get_name() = %s \n",typedefType->get_name().str());

     SgDeclarationStatement* declaration = typedefType->get_declaration();
     ROSE_ASSERT (declaration != NULL);
     printf ("declaration->sage_class_name() = %s \n",declaration->sage_class_name());

     //Makes sure that this is actually a class-typedef
     SgType* type = typedefDeclaration->get_base_type();
     ROSE_ASSERT (type != NULL);

     SgClassType* classType = isSgClassType(type);
     ROSE_ASSERT (classType != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
     ROSE_ASSERT (classDeclaration != NULL);

     tempNodeList.push_back(classDeclaration);
     }
   

  if (tempNodeList.empty () != true)
    {
      ROSE_ASSERT (tempNodeList.size () < 2);
      foundClassDeclarations.merge (tempNodeList);
    }
  */

  /* If the class declaraiton was not hidden in a typedef look for it 
   * elsewhere.
   * 
   */

 // if (tempNodeList.empty () == true)
    for (nodeIterator i = nodeVector.begin (); i != nodeVector.end (); ++i)
      {
        ROSE_ASSERT (isSgScopeStatement (*i) != NULL);
        tempNodeList =
          NodeQuery::querySubTree (*i, new SgName (sageName.c_str ()),
                                   NodeQuery::ClassDeclarationsFromTypeName,
                                   NodeQuery::ChildrenOnly);
        foundClassDeclarations.merge (tempNodeList);

        /*
           if (tempNodeList.empty () != true)
           {
           ROSE_ASSERT (tempNodeList.size () < 2);
           returnNode = isSgNode (*tempNodeList.begin ());
           ROSE_ASSERT (returnNode != NULL);
           foundClassDeclarations.push_back(returnNode);
           }
         */
      }

  return foundClassDeclarations;
}

SgNode *
findClassDeclarationFromType (SgNodePtrVector nodeVector, SgType * sageType)
{

  string sageName = TransformationSupport::getTypeName (sageType);
  ROSE_ASSERT (sageName.length () > 0);


  typedef SgNodePtrVector::iterator nodeIterator;
  SgNode *returnNode = NULL;
  list < SgNode * >tempNodeList;

  /* The traversal flag is set so that the class declaration pointer in 
   * the typedef is not traversed. Therefore I have to look for this
   * pointer myself.
   */

  if (isSgTypedefType (sageType) != NULL)
    {
      for (nodeIterator i = nodeVector.begin (); i != nodeVector.end (); ++i)
        {
          ROSE_ASSERT (isSgScopeStatement (*i) != NULL);

          tempNodeList =
            NodeQuery::querySubTree (*i, new SgName (sageName.c_str ()),
                                     queryNodeClassDeclarationFromTypedefName,
                                     NodeQuery::ChildrenOnly);
          if (tempNodeList.empty () != true)
            {
              ROSE_ASSERT (tempNodeList.size () < 2);
              returnNode = isSgNode (*tempNodeList.begin ());
              ROSE_ASSERT (returnNode != NULL);
            }
        }


      if (tempNodeList.empty () == true)
        cerr << "the tempNodeList is empty.\n";
      else
        cerr << "the tempNodeList is not empty.\n";
    }
  else
    {
      /* If the class declaraiton was not hidden in a typedef look for it 
       * elsewhere.
       * 
       */
      if (tempNodeList.empty () == true)
        for (nodeIterator i = nodeVector.begin (); i != nodeVector.end ();
             ++i)
          {
            ROSE_ASSERT (isSgScopeStatement (*i) != NULL);
            tempNodeList =
              NodeQuery::querySubTree (*i, new SgName (sageName.c_str ()),
                                       NodeQuery::ClassDeclarationFromName,
                                       NodeQuery::ChildrenOnly);

            if (tempNodeList.empty () != true)
              {
                ROSE_ASSERT (tempNodeList.size () < 2);
                returnNode = isSgNode (*tempNodeList.begin ());
                ROSE_ASSERT (returnNode != NULL);
              }
          }
    }

  return returnNode;
}
