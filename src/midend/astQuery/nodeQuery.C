// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#include <boost/bind.hpp>

#include "nodeQuery.h"
#define DEBUG_NODEQUERY 0
// #include "arrayTransformationSupport.h"


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace AstQueryNamespace;

#include "queryVariant.C"

  std::pointer_to_unary_function<SgNode*, Rose_STL_Container<SgNode*> > 
NodeQuery::getFunction(TypeOfQueryTypeOneParameter oneParam)
{
  Rose_STL_Container<SgNode*> (*__x)(SgNode*); 
  switch (oneParam)
  {
    case UnknownListElementType:
      {
        printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
        ROSE_ASSERT (false);
      }
    case TypedefDeclarations:
      {
        __x = &querySolverTypedefDeclarations;
        break;
      }
    case AnonymousTypedefs:
      {
        __x = &queryNodeAnonymousTypedef;
        break;
      }
    case AnonymousTypedefClassDeclarations:
      {
        __x = &queryNodeAnonymousTypedefClassDeclaration;
        break;
      }
    case VariableDeclarations:
      {
        __x = &querySolverVariableDeclarations;
        break;
      }
    case VariableTypes:
      {
        __x = &querySolverVariableTypes;
        break;
      }
    case FunctionDeclarations:
      {
        __x = & querySolverFunctionDeclarations;
        break;
      }
    case MemberFunctionDeclarations:
      {
        __x = &querySolverMemberFunctionDeclarations;
        break;
      }
    case ClassDeclarations:
      {
        __x = &querySolverClassDeclarations;
        break;
      }
    case StructDeclarations:
      {
        __x = &querySolverStructDeclarations;
        break;
      }
    case UnionDeclarations:
      {
        __x = &querySolverUnionDeclarations;
        break;
      }
    case Arguments:
      {
        __x = &querySolverArguments;
        break;
      }
    case ClassFields:
      {
        __x = &querySolverClassFields;
        break;
      }
    case StructFields:
      {
        __x = &querySolverStructFields;
        break;
      }
    case UnionFields:
      {
        __x = &querySolverUnionFields;
        break;
      }
    case StructDefinitions:
      {
        __x = &querySolverStructDefinitions;
        break;
      }

    default:
      {
        printf ("This is an invalid member of the enum  TypeOfQueryTypeOneParameter.\n");
        ROSE_ASSERT (false);
      }
  } /* End switch-case */
  return std::ptr_fun(__x);

}

  std::pointer_to_binary_function<SgNode*, SgNode*, Rose_STL_Container<SgNode*> > 
NodeQuery::getFunction(TypeOfQueryTypeTwoParameters twoParam)
{
  Rose_STL_Container<SgNode*> (*__x)(SgNode*,SgNode*); 

  switch (twoParam)
  {
    case UnknownListElementTypeTwoParameters:
      {
        printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
        ROSE_ASSERT (false);
      }
    case FunctionDeclarationFromDefinition:
      {
        __x = &querySolverFunctionDeclarationFromDefinition;
        break;
      }
    case ClassDeclarationFromName:
      {
        __x = &queryNodeClassDeclarationFromName;
        break;
      }
    case ClassDeclarationsFromTypeName:
      {
        __x = &queryNodeClassDeclarationsFromTypeName;
        break;
      }
    case PragmaDeclarationFromName:
      {
        __x = &queryNodePragmaDeclarationFromName;
        break;
      }
    case VariableDeclarationFromName:
      {
        __x =  queryNodeVariableDeclarationFromName;
        break;
      }
    default:
      {
        printf ("This is an invalid member of the enum  TypeOfQueryTypeOneParameter.\n");
        ROSE_ASSERT (false);
      }
  }
  return std::ptr_fun(__x);
}





/*
 * The function
 *    queryNodeAnonymousTypedef()
 * is a NodeQuery which finds all Anonymous Typedefs is the scope.
 */
NodeQuerySynthesizedAttributeType NodeQuery::queryNodeAnonymousTypedef(SgNode* node)
{
  NodeQuerySynthesizedAttributeType returnList;
  ROSE_ASSERT( node     != NULL );

  SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
  if (sageTypedefDeclaration != NULL)
    if(isSgClassType(sageTypedefDeclaration->get_base_type()))
      returnList.push_back(node);

  return returnList;
} /* End function:queryNodeCLassDeclarationFromName() */
/*
 * The function
 *    queryNodeAnonymousTypedefClassDeclaration()
 * is a NodeQuery which finds all Anonymous Typedefs is the scope.
 */
NodeQuerySynthesizedAttributeType NodeQuery::queryNodeAnonymousTypedefClassDeclaration(SgNode* node)
{
  NodeQuerySynthesizedAttributeType returnList;
  ROSE_ASSERT( node     != NULL );

  SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
  if (sageTypedefDeclaration != NULL)
    if(isSgClassType(sageTypedefDeclaration->get_base_type()))
    {
      SgClassType* sageClassType = isSgClassType(sageTypedefDeclaration->get_base_type());
      SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration(sageClassType->get_declaration());
      ROSE_ASSERT(sageClassDeclaration != NULL);

      returnList.push_back(sageClassDeclaration);
    }

  return returnList;
} /* End function:queryNodeCLassDeclarationFromName() */


/*
 *   The function
 *      queryNodeBottomUpClassDeclarationFromName()
 *   takes as a first parameter a SgNode*. As a second parameter
 *   it takes a SgNode* which is a SgName* which contains the
 *   name the class should have. It returns a class declaration
 *   if it corresponds to the name.
 *    
 */

NodeQuerySynthesizedAttributeType NodeQuery::queryNodeClassDeclarationFromName(SgNode* node, SgNode* nameNode){
  NodeQuerySynthesizedAttributeType returnList;
  ROSE_ASSERT( nameNode != NULL );
  ROSE_ASSERT( node     != NULL );


  //finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ROSE_ASSERT( sageName != NULL );
  std::string nameToMatch = sageName->str();
  ROSE_ASSERT( nameToMatch.length() > 0 );

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (node);

  if (sageClassDeclaration != NULL)
  {

    std::string name = sageClassDeclaration->get_name ().str ();

    if( name == nameToMatch )
      returnList.push_back(node);

  }

  return returnList;

} /* End function:queryNodeCLassDeclarationFromName() */


// DQ (3/25/2004): Added to support more general lookup of data in the AST
  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverGrammarElementFromVariant (SgNode * astNode, VariantT targetVariant)
{
  ROSE_ASSERT (astNode != NULL);
  NodeQuerySynthesizedAttributeType returnNodeList;

  //     printf ("Looking for a targetVariant = %d = %s \n",targetVariant,getVariantName(targetVariant).c_str());
  if ( astNode->variantT() == targetVariant )
  {
    //printf ("astNode = %s FOUND \n",astNode->sage_class_name());
    returnNodeList.push_back (astNode);
  }

  return returnNodeList;
} /* End function querySolverUnionFields() */




/*
 *   The method
 *       queryNodeClassDeclarationsFromTypename()
 *   takes as a first parameter a SgNode*. As a second parameter
 *   it takes a SgNode* which is a SgName* which contains the
 *   typename the class should have. A SgNode* is returned if
 *   the base-type of the class corresponds to the typename.
 */

NodeQuerySynthesizedAttributeType NodeQuery::queryNodeClassDeclarationsFromTypeName(SgNode* node, SgNode* nameNode)
{
  NodeQuerySynthesizedAttributeType returnList;
  ROSE_ASSERT( nameNode != NULL );
  ROSE_ASSERT( node     != NULL );

  // finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ROSE_ASSERT( sageName != NULL );
  std::string nameToMatch = sageName->str();
  ROSE_ASSERT( nameToMatch.length() > 0 );

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (node);

  if (sageClassDeclaration != NULL)
  {
    if(TransformationSupport::getTypeName(sageClassDeclaration->get_type()) == nameToMatch)
      returnList.push_back(node);
    else
    {
      SgClassDefinition* classDefinition = isSgClassDefinition(sageClassDeclaration->get_definition());
      ROSE_ASSERT( classDefinition != NULL );

      // SgBaseClassList baseClassList = classDefinition->get_inheritances();
      SgBaseClassPtrList baseClassList = classDefinition->get_inheritances();

      typedef SgBaseClassPtrList::iterator SgBaseClassPtrListIterator;
      for( SgBaseClassPtrListIterator baseClassElm = baseClassList.begin();
          baseClassElm != baseClassList.end(); ++baseClassElm)
      {
        // SgBaseClass baseClass = *baseClassElm;
        SgBaseClass* baseClass = *baseClassElm;
        // sageClassDeclaration = baseClass.get_base_class();
        sageClassDeclaration = baseClass->get_base_class();
        std::string typeName  = TransformationSupport::getTypeName ( sageClassDeclaration->get_type() );
        if( typeName == nameToMatch )
          returnList.push_back(node);
      }
    }

    /*
       SgType* typeNode = sageClassDeclaration->get_type ();
       ROSE_ASSERT (typeNode != NULL);

       string currentTypeName  = "";
       string previousTypeName = ""; 

       do{
       previousTypeName = currentTypeName;
       currentTypeName  = TransformationSupport::getTypeName (typeNode);

       typeNode = typeNode->findBaseType();
       ROSE_ASSERT( typeNode != NULL );

       if( currentTypeName == nameToMatch ){
       returnList.push_back(node);
       break;
       }
       cout<< "\n\n The typenames is : " << currentTypeName << "\n\n" << previousTypeName << "\n\n";

       }while( previousTypeName != currentTypeName);
     */ 
  }

  return returnList;
} /* End function:queryNodeCLassDeclarationFromName() */


/*
 * The function
 *     queryNodePragmaDeclarationFromName()
 * takes as a first parameter a SgNode*. As a second parameter it takes
 * a SgNode* who must be of type SgName. The SgName contains a std::string which
 * should be the same as the left side in the pragma or a part of the left
 * side of the pragma. If the std::string is empty,
 * there will be an error message.
 *
 *        #pragma std::stringInSgNode = information
 *         
 */
Rose_STL_Container<SgNode*> NodeQuery::queryNodePragmaDeclarationFromName(SgNode* node, SgNode* nameNode){
  ROSE_ASSERT( nameNode != NULL );
  ROSE_ASSERT( node     != NULL );

  Rose_STL_Container<SgNode*> returnList;

  //finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ROSE_ASSERT( sageName != NULL );
  std::string nameToMatch = sageName->str();
  ROSE_ASSERT( nameToMatch.length() > 0 );

  if(node->variantT() == V_SgPragmaDeclaration){
    SgPragmaDeclaration* sagePragmaDeclaration = isSgPragmaDeclaration(node);
    ROSE_ASSERT( sagePragmaDeclaration );
    ROSE_ASSERT( sagePragmaDeclaration->get_pragma() != NULL ); 
    // ROSE_ASSERT( sagePragmaDeclaration->get_pragma()->get_pragma() );
    std::string pragmaDeclarationString =  sagePragmaDeclaration->get_pragma()->get_pragma();
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
 *  The function
 *      queryNodeVariableDeclarationFromName()
 *  takes as a first parameter a SgNode*, and as a second parameter a
 *  SgNode* which is of type SgName* and contains the name of the 
 *  interesting variable.
 * 
 */

Rose_STL_Container<SgNode*> NodeQuery::queryNodeVariableDeclarationFromName(SgNode* astNode, SgNode* nameNode){
  ROSE_ASSERT( nameNode != NULL );
  ROSE_ASSERT( astNode  != NULL );


  Rose_STL_Container<SgNode*> returnList;

  if(astNode->variantT() == V_SgVariableDeclaration){

    SgName* sageName = isSgName(nameNode);
    ROSE_ASSERT( sageName != NULL );
    std::string nameToMatch = sageName->str();
    ROSE_ASSERT( nameToMatch.length() > 0 );

    SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration(astNode);
    ROSE_ASSERT(sageVariableDeclaration != NULL);
    ROSE_ASSERT( sageVariableDeclaration->get_definition() != NULL );

    SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

    //see if this variable declaration fits the criteria
    typedef SgInitializedNamePtrList::iterator variableIterator;

    for (variableIterator k = sageInitializedNameList.begin ();
        k != sageInitializedNameList.end(); ++k)
    {
      SgInitializedName* elmVar = *k;
      std::string name = elmVar->get_name().str();
      ROSE_ASSERT (name.length () > 0);
      if(name == nameToMatch)
        returnList.push_back(astNode);

    }

  }

  return returnList;

}; /* End function: queryNodeVariableDeclarationFromName */


  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverArguments (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch (astNode->variantT ())
  {

    case V_SgMemberFunctionDeclaration:
    case V_SgFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration =
          isSgFunctionDeclaration (astNode);
        ROSE_ASSERT (sageFunctionDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList =
          sageFunctionDeclaration->get_args ();


        typedef SgInitializedNamePtrList::iterator variableIterator;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          //AS (9/26/03) I must put an object of type const SgDeclarationStatement into the list because
          //             I have no other way of finding the SgVariableDeclaration in the arguments. This is safe
          //             because arguments are Variable Declarations, but puts unwelcome limits on use of returned
          //             list because of it's constantness.
          returnNodeList.push_back (elmVar->get_declaration ());
          //returnNodeList.push_back (sageVariableDeclaration);

        }

        break;
      }
    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  }                             /* End switch-case */

  return returnNodeList;
}

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverFunctionDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgFunctionDeclaration (astNode))
    returnNodeList.push_back (astNode);

  return returnNodeList;
}                               /* End function querySolverFunctionDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverMemberFunctionDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgMemberFunctionDeclaration (astNode))
    returnNodeList.push_back (astNode);

  return returnNodeList;
}                               /* End function querySolverMemberFunctionDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverVariableTypes (SgNode * astNode)
{
  ROSE_ASSERT (astNode != NULL);
  NodeQuerySynthesizedAttributeType returnNodeList;
  /*
     SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration(astNode);

     if(sageVariableDeclaration != NULL)
     {
     SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables();


     printf ("\nHere is a function declaration :Line = %d Columns = %d \n", ROSE:: getLineNumber (isSgLocatedNode(astNode) ), ROSE:: getColumnNumber ( isSgLocatedNode(astNode) ));
     cout << "The filename is:" << ROSE::getFileName(isSgLocatedNode(astNode)) << endl;


     typedef SgInitializedNamePtrList::iterator LI;

     for ( LI i = sageInitializedNameList.begin(); i != sageInitializedNameList.end(); ++i) {
     SgType* sageElementType = i->get_type();
     ROSE_ASSERT( sageElementType != NULL);

     cout << "The class name is: " << sageElementType->sage_class_name() << endl;
     returnNodeList.push_back( sageElementType );

     }
     cout << endl << "End printout of this Initialized Name list" << endl;


     }

   */
  // SgVarRefExp *sageVarRefExp = isSgVarRefExp (astNode);

  switch (astNode->variantT ())
  {

    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration *sageVariableDeclaration =
          isSgVariableDeclaration (astNode);
        ROSE_ASSERT (sageVariableDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList =
          sageVariableDeclaration->get_variables ();

#if DEBUG_NODEQUERY
        printf ("\nIn filename: %s ",
            ROSE::getFileName (isSgLocatedNode (astNode)));
        printf ("\nHere is a variable :Line = %d Columns = %d \n",
            ROSE::getLineNumber (isSgLocatedNode (astNode)),
            ROSE::getColumnNumber (isSgLocatedNode (astNode)));
        //cout << "The typename of the variable is: " << typeName << endl;
#endif

        typedef SgInitializedNamePtrList::iterator variableIterator;
        SgType *typeNode;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          ROSE_ASSERT (elmVar != NULL);
          typeNode = elmVar->get_type ();
          ROSE_ASSERT (typeNode != NULL);
          returnNodeList.push_back (typeNode);
        }
        break;
      }                         /* End case V_SgVariableDeclaration */

    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration =
          isSgFunctionDeclaration (astNode);
        ROSE_ASSERT (sageFunctionDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList =
          sageFunctionDeclaration->get_args ();
        SgType *typeNode;

        typedef SgInitializedNamePtrList::iterator variableIterator;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          ROSE_ASSERT (elmVar != NULL);
          typeNode = elmVar->get_type ();
          ROSE_ASSERT (typeNode != NULL);
          returnNodeList.push_back (typeNode);

        }

        break;
      }

    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  }                             /* End switch case astNode */



  return returnNodeList;
}                               /* End function querySolverType() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverVariableDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch (astNode->variantT ())
  {

    case V_SgVariableDeclaration:
      returnNodeList.push_back (astNode);
      break;
    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration =
          isSgFunctionDeclaration (astNode);
        ROSE_ASSERT (sageFunctionDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList =
          sageFunctionDeclaration->get_args ();


        typedef SgInitializedNamePtrList::iterator variableIterator;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          //SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration((elmVar.get_declaration())->copy());
          //ROSE_ASSERT(sageVariableDeclaration != NULL); 

          //if( sageVariableDeclaration != NULL)

          //AS (9/26/03) I must put an object of type const SgDeclarationStatement into the list because
          //             I have no other way of finding the SgVariableDeclaration in the arguments. This is safe
          //             because arguments are Variable Declarations, but puts unwelcome limits on use of returned
          //             list because of it's constantness.
          ROSE_ASSERT (elmVar != NULL);
          returnNodeList.push_back (elmVar->get_declaration ());
          //returnNodeList.push_back (sageVariableDeclaration);

        }

        break;
      }

    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  }                             /* End switch-case */

  return returnNodeList;
}                               /* End function querySolverVariableDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverClassDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_class)
      returnNodeList.push_back (astNode);


  return returnNodeList;
}                               /* End function querySolverClassDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_struct)
      returnNodeList.push_back (astNode);


  return returnNodeList;
}                               /* End function querySolverStructDeclarations() */


  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverUnionDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_union)
      returnNodeList.push_back (astNode);


  return returnNodeList;
}                               /* End function querySolverUnionDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverTypedefDeclarations (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgTypedefDeclaration (astNode))
    returnNodeList.push_back (astNode);


  return returnNodeList;
}                               /* End function querySolverTypedefDeclarations() */

  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverClassFields (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);

  NodeQuerySynthesizedAttributeType returnNodeList;

  /*  cout << "The name of the node is: \" " << astNode->sage_class_name() << "\"\n";
      SgLocatedNode* sageLocatedNode = isSgLocatedNode(astNode);
      if(sageLocatedNode != NULL){
      cout << "The filename is: " << sageLocatedNode->getFileName() << " At line number :" << sageLocatedNode->get_file_info()->get_line() << "\n";
      } */
  // SgNode *sageReturnNode = NULL;

  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);

  if (sageClassDefinition != NULL)
  {
    ROSE_ASSERT (sageClassDefinition->get_declaration () != NULL);
    if (sageClassDefinition->get_declaration ()->get_class_type () ==
        SgClassDeclaration::e_class)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList =
        sageClassDefinition->get_members ();

      typedef SgDeclarationStatementPtrList::iterator LI;
      for (LI i = declarationStatementPtrList.begin ();
          i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;

        if (isSgVariableDeclaration (listElement) != NULL){
          /*    if(isSgVariableDeclaration(listElement)->get_name().str() != NULL)
                cout << "The name of the variable declaration is: \"" << isSgVariableDeclaration(listElement)->get_name().str() << "\"\n";
                else
                cout << "The name of the variable declaration is: \"\"\n";*/
          returnNodeList.push_back (listElement);
        }

      }
    }
  }
  return returnNodeList;
}                               /* End function querySolverClassFields() */


  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructFields (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NodeQuerySynthesizedAttributeType returnNodeList;

  // SgNode *sageReturnNode = NULL;

  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);

  if (sageClassDefinition != NULL)
  {
    ROSE_ASSERT (sageClassDefinition->get_declaration () != NULL);
    if (sageClassDefinition->get_declaration ()->get_class_type () ==
        SgClassDeclaration::e_struct)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList =
        sageClassDefinition->get_members ();

      typedef SgDeclarationStatementPtrList::iterator LI;

      for (LI i = declarationStatementPtrList.begin ();
          i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;

        if (isSgVariableDeclaration (listElement) != NULL)
          returnNodeList.push_back (listElement);

      }

    }
  }


  return returnNodeList;
}                               /* End function querySolverClassFields() */



  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverUnionFields (SgNode * astNode)
{


  ROSE_ASSERT (astNode != 0);

  NodeQuerySynthesizedAttributeType returnNodeList;

  // SgNode *sageReturnNode = NULL;

  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);

  if (sageClassDefinition != NULL)
  {
    ROSE_ASSERT (sageClassDefinition->get_declaration () != NULL);
    if (sageClassDefinition->get_declaration ()->get_class_type () ==
        SgClassDeclaration::e_union)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList =
        sageClassDefinition->get_members ();

      typedef SgDeclarationStatementPtrList::iterator LI;

      for (LI i = declarationStatementPtrList.begin ();
          i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;

        if (isSgVariableDeclaration (listElement) != NULL)
          returnNodeList.push_back (listElement);

      }

    }
  }


  return returnNodeList;

}                               /* End function querySolverUnionFields() */





  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructDefinitions (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (sageClassDefinition != NULL)
  {
    SgClassDeclaration *sageClassDeclaration =
      isSgClassDeclaration (sageClassDefinition->get_parent ());
    ROSE_ASSERT (sageClassDeclaration != NULL);

    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_struct)
      returnNodeList.push_back (astNode);
  }

  return returnNodeList;
}                               /* End function querySolverClassFields() */



  NodeQuerySynthesizedAttributeType
NodeQuery::querySolverFunctionDeclarationFromDefinition (SgNode * astNode,
    SgNode * functionDefinition)
{


  ROSE_ASSERT (astNode != 0);

  NodeQuerySynthesizedAttributeType returnNodeList;

  // SgNode *sageReturnNode = NULL;
  SgMemberFunctionDeclaration *sageMemberFunctionDeclaration =
    isSgMemberFunctionDeclaration (astNode);

  if (sageMemberFunctionDeclaration != NULL)
    if (isSgClassDefinition (sageMemberFunctionDeclaration->get_parent ()))
      if (isSgNode (sageMemberFunctionDeclaration->get_definition ()) ==
          functionDefinition)
        returnNodeList.push_back (astNode);

  return returnNodeList;

}                               /* End function querySolverUnionFields() */




NodeQuerySynthesizedAttributeType NodeQuery::querySubTree ( SgNode * subTree, TypeOfQueryTypeOneParameter elementReturnType, AstQueryNamespace::QueryDepth defineQueryType)
   {
#if 0
     printf ("Inside of NodeQuery::querySubTree #1 \n");
#endif
     return AstQueryNamespace::querySubTree(subTree, getFunction(elementReturnType), defineQueryType);
   }

// get the SgNode's conforming to the test in querySolverFunction or
// get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
NodeQuerySynthesizedAttributeType NodeQuery::querySubTree ( SgNode * subTree, SgNode * traversal, roseFunctionPointerTwoParameters querySolverFunction, AstQueryNamespace::QueryDepth defineQueryType)
   {
#if 0
     printf ("Inside of NodeQuery::querySubTree #2 \n");
#endif
     return AstQueryNamespace::querySubTree(subTree, std::bind2nd(std::ptr_fun(querySolverFunction),traversal), defineQueryType);
   }

NodeQuerySynthesizedAttributeType NodeQuery::querySubTree ( SgNode * subTree, SgNode * traversal, TypeOfQueryTypeTwoParameters elementReturnType, AstQueryNamespace::QueryDepth defineQueryType )
   {
#if 0
     printf ("Inside of NodeQuery::querySubTree #3 \n");
#endif
     return AstQueryNamespace::querySubTree(subTree, std::bind2nd(getFunction(elementReturnType),traversal), defineQueryType);
   }


// perform a query on a list<SgNode>
Rose_STL_Container<SgNode*> NodeQuery::queryNodeList ( Rose_STL_Container< SgNode * >nodeList, Rose_STL_Container< SgNode * > (*querySolverFunction)(SgNode*) )
   {
     return AstQueryNamespace::queryRange< Rose_STL_Container<SgNode*> >(nodeList.begin(), nodeList.end(), querySolverFunction);
   }

Rose_STL_Container<SgNode*> NodeQuery::queryNodeList ( Rose_STL_Container<SgNode*> nodeList, TypeOfQueryTypeOneParameter elementReturnType )
   {
     return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),getFunction(elementReturnType));
   }

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree (SgNode * subTree, roseFunctionPointerOneParameter elementReturnType, AstQueryNamespace::QueryDepth defineQueryType )
   {
#if 0
     printf ("Inside of NodeQuery::querySubTree #4 \n");
#endif
     return  AstQueryNamespace::querySubTree(subTree,std::ptr_fun(elementReturnType),defineQueryType);
   }


Rose_STL_Container<SgNode*> NodeQuery::queryNodeList ( Rose_STL_Container<SgNode*> nodeList, SgNode * targetNode, roseFunctionPointerTwoParameters querySolverFunction )
   {
     return AstQueryNamespace::queryRange<SgNode*, Rose_STL_Container<SgNode*> >(nodeList.begin(), nodeList.end(), querySolverFunction, targetNode);
  // std::bind2nd(getFunction(elementReturnType),traversal), defineQueryType);
   }

Rose_STL_Container<SgNode*> NodeQuery::queryNodeList ( Rose_STL_Container<SgNode*> nodeList, SgNode * targetNode, TypeOfQueryTypeTwoParameters elementReturnType )
   {
     return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),std::bind2nd(getFunction(elementReturnType), targetNode));
   }

// DQ (4/8/2004): Added query based on vector of variants

NodeQuerySynthesizedAttributeType NodeQuery::querySubTree ( SgNode * subTree, VariantVector targetVariantVector, AstQueryNamespace::QueryDepth defineQueryType)
   {
     NodeQuerySynthesizedAttributeType returnList;
#if 0
     printf ("Inside of NodeQuery::querySubTree #5 \n");
#endif
     AstQueryNamespace::querySubTree(subTree, boost::bind(querySolverGrammarElementFromVariantVector, _1,targetVariantVector,&returnList), defineQueryType);

     return returnList;
   }

NodeQuerySynthesizedAttributeType NodeQuery::queryNodeList ( NodeQuerySynthesizedAttributeType nodeList, VariantVector targetVariantVector)
   {
     NodeQuerySynthesizedAttributeType returnList;

     AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(), boost::bind(querySolverGrammarElementFromVariantVector, _1,targetVariantVector,&returnList));

     return returnList;
   }

// DQ (3/26/2004): Added query based on variant
NodeQuerySynthesizedAttributeType NodeQuery::querySubTree ( SgNode * subTree, VariantT targetVariant, AstQueryNamespace::QueryDepth defineQueryType )
   {
#if 0
     printf ("Inside of NodeQuery::querySubTree #6 \n");
#endif
     return NodeQuery::querySubTree(subTree, VariantVector(targetVariant), defineQueryType);
   }

NodeQuerySynthesizedAttributeType NodeQuery::queryNodeList ( NodeQuerySynthesizedAttributeType queryList, VariantT targetVariant)
   {
     return NodeQuery::queryNodeList(queryList,VariantVector(targetVariant));
   }

#if 0
// DQ (3/14/207): Older version using a return type of std::list
class TypeQueryDummyFunctionalTest :  public std::unary_function<SgNode*, std::list<SgNode*> > 
{
  public:
    result_type operator()(SgNode* node );
};

TypeQueryDummyFunctionalTest::result_type
TypeQueryDummyFunctionalTest::operator()(SgNode* node )
   {
     result_type returnType;
     returnType.push_back(node);
     return returnType; 
   }
#endif

class TypeQueryDummyFunctionalTest :  public std::binary_function<SgNode*, Rose_STL_Container<SgNode*>*, void* >
   {
     public:
          result_type operator()(SgNode* node, Rose_STL_Container<SgNode*>* ) const;
   };

TypeQueryDummyFunctionalTest::result_type
TypeQueryDummyFunctionalTest::operator()(SgNode* node, Rose_STL_Container<SgNode*>* returnList) const
   {
     returnList->push_back(node);

  // DQ (9/25/2007): Fixed up to return NULL pointer! Approved fix by Andreas.
     return NULL;
   }

typedef SgNode* node_ptr;

// Making this use a std::vector instead of std::list might make it more efficient as well.
class TwoParamaters :  public std::binary_function<node_ptr, Rose_STL_Container<SgNode*>* , void* >
{
  public:
    result_type operator()(first_argument_type node, const second_argument_type returnList ) const
    {
      // second_argument_type curr_list = (second_argument_type) returnList;
      returnList->push_back(node);
      return NULL;
    }
};

class OneParamater :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
  public:
    result_type operator()(SgNode* node ) const
    {
      Rose_STL_Container<SgNode*> returnList;
      returnList.push_back(node);
      return returnList;
    }
};




Rose_STL_Container<SgNode*> NodeQuery::generateListOfTypes ( SgNode* astNode )
{
  // Insert your own manipulation of the AST here...
#if 0
  printf ("\n\n");
  printf ("************************** \n");
  printf ("Generate list of types ... \n");
#endif

  Rose_STL_Container<SgNode*> nodeList;

  // if (isSgProject(astNode) != NULL || (isSgFile(astNode) != NULL && SgProject::numberOfFiles() == 1) )
  // if (isSgProject(astNode) != NULL)
  bool useMemoryPool = (isSgProject(astNode) != NULL);
  if (useMemoryPool == false)
  {
    // Check if this is a SgFile or SgGlobal where there is only a single SgFile in the SgProject!
    if (isSgFile(astNode) != NULL || isSgGlobal(astNode) != NULL)
    {
   // DQ (1/25/2011): We want to be able to use this functionality, it is not depreicated...
   // printf ("This is not a SgProject, but it is a SgFile or SgGlobal so check if this is an only file before using the memory pool! \n");
      SgProject* project = TransformationSupport::getProject(astNode);

      // 2nd chance to reset useMemoryPool and provide an optimized query for types!
      useMemoryPool = (project->numberOfFiles() == 1);
    }
  }

  if (useMemoryPool == true)
  {
    // Then just query the memory pool and provide a much faster query.
    // printf ("Using memory pool access to type information ... \n");

    TypeQueryDummyFunctionalTest funcTest;

    // Build the list of all IR node variants that are derived from SgType (including SgType)
    VariantVector ir_nodes (V_SgType);

    // Execute the query on each separate memory pool for the list of IR node variants

    // DQ (3/14/2007): The NodeQuery::queryMemoryPool() function assumes that the function return type is a std::list
    // so try to use the AstQueryNamespace::queryMemoryPool() function directly.
    // nodeList = NodeQuery::queryMemoryPool(funcTest,&ir_nodes);
    // AstQueryNamespace::queryMemoryPool(funcTest,nodeList,&ir_nodes);

    // DQ (2/16/2007): This is Andreas's fix for the performance problem represented by the previous 
    // implementat which built and returns STL lists by value with only a single IR node in the list.
    AstQueryNamespace::queryMemoryPool(std::bind2nd(funcTest,&nodeList),&ir_nodes); 
  }
  else
  {
    // This operation is expensive and not always accurate (though only vacuious destructors might be missing).
    // As a result we would like to depricate its use in ROSE (at least for V_SgType \n");
#if 0
    printf ("The use of this mechanism to get type information on arbitrary subtrees is depricated! (subtree at %p = %s) \n",astNode,astNode->class_name().c_str());
#endif

    // Get the types from the specified subtree
#if 1
 // DQ (1/13/2011): This will only get a subset of types.
    nodeList = NodeQuery::querySubTree (astNode,V_SgType);
#else
 // DQ (1/13/2011): But I don't think this is the correct approach...
    AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::ExtractTypes;
    nodeList = NodeQuery::querySubTree (astNode,V_SgType,defineQueryType);
#endif

#if 0
    printf ("/* AST Test: nodeList.size() = %zu */ \n",nodeList.size());
    printNodeList(nodeList);
    printf ("*** Sorted list *** \n");
#endif

    // DQ (9/25/2007): These operations don't exist for a std::vector and were used with we used std::list.
    // nodeList.sort();
    // nodeList.unique();
#if 0
    printf ("Skipping use of std::list<>::sort() and std::list<>::unique() now that we are using std::vector! (Is this a problem for NodeQuery::generateListOfTypes()?) \n");
#endif
  }

#if 0
  printNodeList(nodeList);
  printf ("DONE: Generate list of types ... \n");
  printf ("************************** \n\n\n");
#endif

  return nodeList;
}



/********************************************************************************
 * The function
 *      NodeQuerySynthesizedAttributeType queryMemoryPool ( SgNode * subTree,
 *                   Result (*x)(SgNode*,Arg), Arg x_arg,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * perform the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
  NodeQuerySynthesizedAttributeType
NodeQuery::queryMemoryPool ( SgNode * traversal, NodeQuery::roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector)
{
  return AstQueryNamespace::queryMemoryPool(std::bind2nd(std::ptr_fun(querySolverFunction),traversal), targetVariantVector);
}


/********************************************************************************
 * The function
 *      Result queryMemoryPool ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*),
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
  NodeQuerySynthesizedAttributeType
NodeQuery::queryMemoryPool ( SgNode * traversal, NodeQuery::roseFunctionPointerOneParameter querySolverFunction, VariantVector* targetVariantVector)
{
  return  AstQueryNamespace::queryMemoryPool(std::ptr_fun(querySolverFunction),targetVariantVector);
}

/********************************************************************************
 * The function
 *      Result queryMemoryPool ( SgNode * subTree, SgNode*,
 *                   TypeOfQueryTypeTwoParameters,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

  NodeQuerySynthesizedAttributeType
NodeQuery::queryMemoryPool (SgNode * traversal, NodeQuery::TypeOfQueryTypeTwoParameters elementReturnType, VariantVector* targetVariantVector)
{
  return AstQueryNamespace::queryMemoryPool(std::bind2nd(getFunction(elementReturnType),traversal), targetVariantVector);
}

/********************************************************************************
 * The function
 *      Result queryMemoryPool ( SgNode * subTree,
 *                   TypeOfQueryTypeOneParameter,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

  NodeQuerySynthesizedAttributeType
NodeQuery::queryMemoryPool ( NodeQuery::TypeOfQueryTypeOneParameter elementReturnType, VariantVector* targetVariantVector )
{
  return AstQueryNamespace::queryMemoryPool(getFunction(elementReturnType), targetVariantVector); 
}

  AstQueryNamespace::DefaultNodeFunctional::result_type 
NodeQuery::queryMemoryPool(VariantVector& targetVariantVector)
{
  DefaultNodeFunctional nodeFunc;
  return AstQueryNamespace::queryMemoryPool(nodeFunc, &targetVariantVector);
}


////////END INTERFACE FOR NAMESPACE NODE QUERY


