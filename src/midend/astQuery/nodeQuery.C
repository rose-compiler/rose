#include "sage3basic.h"

#include <boost/bind/bind.hpp>
#include "nodeQuery.h"

#define DEBUG_NODEQUERY 0

using namespace std;
using namespace AstQueryNamespace;

#include "queryVariant.C"

std::function<Rose_STL_Container<SgNode*>(SgNode*) >
NodeQuery::getFunction(TypeOfQueryTypeOneParameter oneParam)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  std::function<Rose_STL_Container<SgNode*>(SgNode*)> ptrFun;

  switch (oneParam)
  {
    case UnknownListElementType:
      {
        ASSERT_require2(false, "This is element number 0 in the list. It is not used to anything predefined.\n");
      }
    case TypedefDeclarations:
      {
        ptrFun = querySolverTypedefDeclarations;
        break;
      }
    case AnonymousTypedefs:
      {
        ptrFun = queryNodeAnonymousTypedef;
        break;
      }
    case AnonymousTypedefClassDeclarations:
      {
        ptrFun = queryNodeAnonymousTypedefClassDeclaration;
        break;
      }
    case VariableDeclarations:
      {
        ptrFun = querySolverVariableDeclarations;
        break;
      }
    case VariableTypes:
      {
        ptrFun = querySolverVariableTypes;
        break;
      }
    case FunctionDeclarations:
      {
        ptrFun =  querySolverFunctionDeclarations;
        break;
      }
    case MemberFunctionDeclarations:
      {
        ptrFun = querySolverMemberFunctionDeclarations;
        break;
      }
    case ClassDeclarations:
      {
        ptrFun = querySolverClassDeclarations;
        break;
      }
    case StructDeclarations:
      {
        ptrFun = querySolverStructDeclarations;
        break;
      }
    case UnionDeclarations:
      {
        ptrFun = querySolverUnionDeclarations;
        break;
      }
    case Arguments:
      {
        ptrFun = querySolverArguments;
        break;
      }
    case ClassFields:
      {
        ptrFun = querySolverClassFields;
        break;
      }
    case StructFields:
      {
        ptrFun = querySolverStructFields;
        break;
      }
    case UnionFields:
      {
        ptrFun = querySolverUnionFields;
        break;
      }
    case StructDefinitions:
      {
        ptrFun = querySolverStructDefinitions;
        break;
      }

    default:
      {
        ASSERT_require2(false, "This is an invalid member of the enum TypeOfQueryTypeOneParameter.");
      }
  } /* End switch-case */

  return ptrFun;
}

std::function< Rose_STL_Container<SgNode*>(SgNode*, SgNode*) >
NodeQuery::getFunction(TypeOfQueryTypeTwoParameters twoParam)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  std::function<Rose_STL_Container<SgNode*>(SgNode*,SgNode*)> ptrFun;

  switch (twoParam)
  {
    case UnknownListElementTypeTwoParameters:
      {
        ASSERT_require2(false, "This is element number 0 in the list. It is not used to anything predefined.\n");
      }
    case FunctionDeclarationFromDefinition:
      {
        ptrFun = querySolverFunctionDeclarationFromDefinition;
        break;
      }
    case ClassDeclarationFromName:
      {
        ptrFun = queryNodeClassDeclarationFromName;
        break;
      }
    case ClassDeclarationsFromTypeName:
      {
        ptrFun = queryNodeClassDeclarationsFromTypeName;
        break;
      }
    case PragmaDeclarationFromName:
      {
        ptrFun = queryNodePragmaDeclarationFromName;
        break;
      }
    case VariableDeclarationFromName:
      {
        ptrFun = queryNodeVariableDeclarationFromName;
        break;
      }
    default:
      {
        ASSERT_require2(false, "This is an invalid member of the enum  TypeOfQueryTypeOneParameter.\n");
      }
  }
  return ptrFun;
}

/*
 * The function
 *    queryNodeAnonymousTypedef()
 * is a NodeQuery which finds all Anonymous Typedefs is the scope.
 */
NodeQuerySynthesizedAttributeType NodeQuery::queryNodeAnonymousTypedef(SgNode* node)
{
  NodeQuerySynthesizedAttributeType returnList;
  ASSERT_not_null(node);

  SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
  if (sageTypedefDeclaration != nullptr)
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
  ASSERT_not_null(node);

  SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
  if (sageTypedefDeclaration != nullptr)
    if(isSgClassType(sageTypedefDeclaration->get_base_type()))
    {
      SgClassType* sageClassType = isSgClassType(sageTypedefDeclaration->get_base_type());
      SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration(sageClassType->get_declaration());
      ASSERT_not_null(sageClassDeclaration);

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
  ASSERT_not_null(nameNode);
  ASSERT_not_null(node);

  // finds the name which should be matched to
  SgName* sageName = isSgName(nameNode);
  ASSERT_not_null(sageName);
  std::string nameToMatch = sageName->str();
  ASSERT_require(nameToMatch.length() > 0);

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (node);

  if (sageClassDeclaration != nullptr)
  {
    std::string name = sageClassDeclaration->get_name ().str ();

    if ( name == nameToMatch )
      returnList.push_back(node);

  }

  return returnList;

} /* End function:queryNodeCLassDeclarationFromName() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverGrammarElementFromVariant (SgNode * astNode, VariantT targetVariant)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (astNode->variantT() == targetVariant) {
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
  ASSERT_not_null(nameNode);
  ASSERT_not_null(node);

  // finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ASSERT_not_null(sageName);
  std::string nameToMatch = sageName->str();
  ASSERT_require(nameToMatch.length() > 0);

  SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration (node);

  if (sageClassDeclaration != nullptr)
  {
    if(TransformationSupport::getTypeName(sageClassDeclaration->get_type()) == nameToMatch)
      returnList.push_back(node);
    else
    {
      SgClassDefinition* classDefinition = isSgClassDefinition(sageClassDeclaration->get_definition());
      ASSERT_not_null(classDefinition);

      SgBaseClassPtrList baseClassList = classDefinition->get_inheritances();

      typedef SgBaseClassPtrList::iterator SgBaseClassPtrListIterator;
      for( SgBaseClassPtrListIterator baseClassElm = baseClassList.begin();
          baseClassElm != baseClassList.end(); ++baseClassElm)
      {
        SgBaseClass* baseClass = *baseClassElm;
        sageClassDeclaration = baseClass->get_base_class();
        std::string typeName  = TransformationSupport::getTypeName ( sageClassDeclaration->get_type() );
        if( typeName == nameToMatch )
          returnList.push_back(node);
      }
    }
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
  ASSERT_not_null(nameNode);
  ASSERT_not_null(node);

  Rose_STL_Container<SgNode*> returnList;

  //finds the name which should be matched to 
  SgName* sageName = isSgName(nameNode);
  ASSERT_not_null(sageName);
  std::string nameToMatch = sageName->str();
  ASSERT_require(nameToMatch.length() > 0);

  if(node->variantT() == V_SgPragmaDeclaration){
    SgPragmaDeclaration* sagePragmaDeclaration = isSgPragmaDeclaration(node);
    ASSERT_not_null(sagePragmaDeclaration);
    ASSERT_not_null(sagePragmaDeclaration->get_pragma()); 

    std::string pragmaDeclarationString =  sagePragmaDeclaration->get_pragma()->get_pragma();
    // extract the part before the leftmost = is pragmaDeclarationString
    pragmaDeclarationString = pragmaDeclarationString.substr(0,pragmaDeclarationString.find("="));
    // if the name-criteria is met accept node
    if (pragmaDeclarationString.find( nameToMatch ) != pragmaDeclarationString.length()) {
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
  ASSERT_not_null(nameNode);
  ASSERT_not_null(astNode);

  Rose_STL_Container<SgNode*> returnList;

  if(astNode->variantT() == V_SgVariableDeclaration){

    SgName* sageName = isSgName(nameNode);
    ASSERT_not_null(sageName);
    std::string nameToMatch = sageName->str();
    ASSERT_require(nameToMatch.length() > 0);

    SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration(astNode);
    ASSERT_not_null(sageVariableDeclaration);
    ASSERT_not_null(sageVariableDeclaration->get_definition());

    SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

    //see if this variable declaration fits the criteria
    typedef SgInitializedNamePtrList::iterator variableIterator;

    for (variableIterator k = sageInitializedNameList.begin ();
        k != sageInitializedNameList.end(); ++k)
    {
      SgInitializedName* elmVar = *k;
      std::string name = elmVar->get_name().str();
      if(name == nameToMatch)
        returnList.push_back(astNode);
    }
  }

  return returnList;
}; /* End function: queryNodeVariableDeclarationFromName */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverArguments(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch (astNode->variantT ())
  {
    case V_SgMemberFunctionDeclaration:
    case V_SgFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration = isSgFunctionDeclaration(astNode);
        ASSERT_not_null(sageFunctionDeclaration);

        SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args();
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
        }
        break;
      }
    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  } /* End switch-case */

  return returnNodeList;
}

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverFunctionDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgFunctionDeclaration (astNode)) {
    returnNodeList.push_back (astNode);
  }
  return returnNodeList;
} /* End function querySolverFunctionDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverMemberFunctionDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgMemberFunctionDeclaration (astNode)) {
    returnNodeList.push_back (astNode);
  }
  return returnNodeList;
} /* End function querySolverMemberFunctionDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverVariableTypes (SgNode * astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch (astNode->variantT ())
  {
    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration (astNode);
        ASSERT_not_null(sageVariableDeclaration);

        SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

#if DEBUG_NODEQUERY
        printf ("\nIn filename: %s ",
            Rose::getFileName (isSgLocatedNode (astNode)));
        printf ("\nHere is a variable :Line = %d Columns = %d \n",
            Rose::getLineNumber (isSgLocatedNode (astNode)),
            Rose::getColumnNumber (isSgLocatedNode (astNode)));
#endif

        typedef SgInitializedNamePtrList::iterator variableIterator;
        SgType *typeNode;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          ASSERT_not_null(elmVar);
          typeNode = elmVar->get_type ();
          ASSERT_not_null(typeNode);
          returnNodeList.push_back (typeNode);
        }
        break;
      } /* End case V_SgVariableDeclaration */

    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration = isSgFunctionDeclaration (astNode);
        ASSERT_not_null(sageFunctionDeclaration);

        SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args();
        SgType* typeNode;

        typedef SgInitializedNamePtrList::iterator variableIterator;

        for (variableIterator variableListElement =
            sageInitializedNameList.begin ();
            variableListElement != sageInitializedNameList.end ();
            ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;
          ASSERT_not_null(elmVar);
          typeNode = elmVar->get_type ();
          ASSERT_not_null(typeNode);
          returnNodeList.push_back (typeNode);
        }
        break;
      }

    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  } /* End switch case astNode */

  return returnNodeList;
} /* End function querySolverType() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverVariableDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch (astNode->variantT ())
  {
    case V_SgVariableDeclaration:
      returnNodeList.push_back (astNode);
      break;
    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
      {
        SgFunctionDeclaration * sageFunctionDeclaration = isSgFunctionDeclaration(astNode);
        ASSERT_not_null(sageFunctionDeclaration);

        SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args();
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
          ASSERT_not_null(elmVar);
          returnNodeList.push_back (elmVar->get_declaration ());
        }
        break;
      }

    default:
      {
        // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
      }
  } /* End switch-case */

  return returnNodeList;
} /* End function querySolverVariableDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverClassDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != nullptr) {
    if (sageClassDeclaration->get_class_type() == SgClassDeclaration::e_class) {
      returnNodeList.push_back (astNode);
    }
  }
  return returnNodeList;
} /* End function querySolverClassDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != nullptr) {
    if (sageClassDeclaration->get_class_type() == SgClassDeclaration::e_struct) {
      returnNodeList.push_back (astNode);
    }
  }
  return returnNodeList;
}                               /* End function querySolverStructDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverUnionDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != nullptr) {
    if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_union) {
      returnNodeList.push_back (astNode);
    }
  }
  return returnNodeList;
} /* End function querySolverUnionDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverTypedefDeclarations(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (isSgTypedefDeclaration (astNode)) {
    returnNodeList.push_back (astNode);
  }
  return returnNodeList;
} /* End function querySolverTypedefDeclarations() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverClassFields(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);

  if (sageClassDefinition != nullptr)
  {
    ASSERT_not_null(sageClassDefinition->get_declaration ());
    if (sageClassDefinition->get_declaration ()->get_class_type () ==
        SgClassDeclaration::e_class)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList = sageClassDefinition->get_members ();

      typedef SgDeclarationStatementPtrList::iterator LI;
      for (LI i = declarationStatementPtrList.begin ();
          i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;
        if (isSgVariableDeclaration(listElement) != nullptr) {
          returnNodeList.push_back(listElement);
        }
      }
    }
  }
  return returnNodeList;
} /* End function querySolverClassFields() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructFields(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDefinition *sageClassDefinition = isSgClassDefinition(astNode);

  if (sageClassDefinition != nullptr)
  {
    ASSERT_not_null(sageClassDefinition->get_declaration ());
    if (sageClassDefinition->get_declaration ()->get_class_type () ==
        SgClassDeclaration::e_struct)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList = sageClassDefinition->get_members ();
      typedef SgDeclarationStatementPtrList::iterator LI;

      for (LI i = declarationStatementPtrList.begin ();
          i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;
        if (isSgVariableDeclaration (listElement) != nullptr) {
          returnNodeList.push_back (listElement);
        }
      }
    }
  }
  return returnNodeList;
} /* End function querySolverClassFields() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverUnionFields(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgClassDefinition* sageClassDefinition = isSgClassDefinition(astNode);

  if (sageClassDefinition != nullptr)
  {
    ASSERT_not_null(sageClassDefinition->get_declaration ());
    if (sageClassDefinition->get_declaration ()->get_class_type () == SgClassDeclaration::e_union)
    {
      SgDeclarationStatementPtrList declarationStatementPtrList = sageClassDefinition->get_members();
      typedef SgDeclarationStatementPtrList::iterator LI;

      for (LI i = declarationStatementPtrList.begin (); i != declarationStatementPtrList.end (); ++i)
      {
        SgNode *listElement = *i;
        if (isSgVariableDeclaration (listElement) != nullptr) {
          returnNodeList.push_back (listElement);
        }
      }
    }
  }
  return returnNodeList;
} /* End function querySolverUnionFields() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverStructDefinitions(SgNode* astNode)
{
  ASSERT_not_null(astNode);
  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;

  if (sageClassDefinition != nullptr)
  {
    SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration(sageClassDefinition->get_parent());
    ASSERT_not_null(sageClassDeclaration);

    if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_struct) {
      returnNodeList.push_back (astNode);
    }
  }
  return returnNodeList;
} /* End function querySolverClassFields() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySolverFunctionDeclarationFromDefinition(SgNode* astNode, SgNode* functionDefinition)
{
  ASSERT_not_null(astNode);
  NodeQuerySynthesizedAttributeType returnNodeList;
  SgMemberFunctionDeclaration *sageMemberFunctionDeclaration = isSgMemberFunctionDeclaration(astNode);

  if (sageMemberFunctionDeclaration != nullptr) {
    if (isSgClassDefinition (sageMemberFunctionDeclaration->get_parent ())) {
      if (isSgNode (sageMemberFunctionDeclaration->get_definition ()) == functionDefinition) {
        returnNodeList.push_back (astNode);
      }
    }
  }
  return returnNodeList;
} /* End function querySolverUnionFields() */

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree,
                        TypeOfQueryTypeOneParameter elementReturnType,
                        AstQueryNamespace::QueryDepth defineQueryType)
{
  return AstQueryNamespace::querySubTree(subTree, getFunction(elementReturnType), defineQueryType);
}

// get the SgNode's conforming to the test in querySolverFunction or
// get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree, SgNode* traversal,
                        roseFunctionPointerTwoParameters querySolverFunction,
                        AstQueryNamespace::QueryDepth defineQueryType)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  return AstQueryNamespace::querySubTree(subTree,
            std::bind(querySolverFunction, std::placeholders::_1,traversal), defineQueryType);
}

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree, SgNode* traversal,
                        TypeOfQueryTypeTwoParameters elementReturnType, AstQueryNamespace::QueryDepth defineQueryType)
{
  return AstQueryNamespace::querySubTree(subTree,
            std::bind(getFunction(elementReturnType),std::placeholders::_1,traversal), defineQueryType);
}

// perform a query on a list<SgNode>
Rose_STL_Container<SgNode*>
NodeQuery::queryNodeList(Rose_STL_Container<SgNode*>nodeList, Rose_STL_Container<SgNode*>(*querySolverFunction)(SgNode*))
{
  return AstQueryNamespace::queryRange<Rose_STL_Container<SgNode*>>(nodeList.begin(), nodeList.end(), querySolverFunction);
}

Rose_STL_Container<SgNode*>
NodeQuery::queryNodeList(Rose_STL_Container<SgNode*> nodeList, TypeOfQueryTypeOneParameter elementReturnType)
{
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),getFunction(elementReturnType));
}

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree, roseFunctionPointerOneParameter elementReturnType,
             AstQueryNamespace::QueryDepth defineQueryType)
{
// Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  return  AstQueryNamespace::querySubTree(subTree, elementReturnType, defineQueryType);
}

Rose_STL_Container<SgNode*>
NodeQuery::queryNodeList(Rose_STL_Container<SgNode*> nodeList, SgNode* targetNode,
             roseFunctionPointerTwoParameters querySolverFunction )
{
  return AstQueryNamespace::queryRange<SgNode*, Rose_STL_Container<SgNode*>>(nodeList.begin(),
             nodeList.end(), querySolverFunction, targetNode);
}

Rose_STL_Container<SgNode*>
NodeQuery::queryNodeList(Rose_STL_Container<SgNode*> nodeList, SgNode* targetNode,
             TypeOfQueryTypeTwoParameters elementReturnType )
{
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
             std::bind(getFunction(elementReturnType), std::placeholders::_1, targetNode));
}

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree, VariantVector targetVariantVector, AstQueryNamespace::QueryDepth defineQueryType)
{
  NodeQuerySynthesizedAttributeType returnList;

  AstQueryNamespace::querySubTree(subTree, boost::bind(querySolverGrammarElementFromVariantVector,
                                    boost::placeholders::_1, targetVariantVector, &returnList), defineQueryType);
  return returnList;
}

NodeQuerySynthesizedAttributeType
NodeQuery::queryNodeList(NodeQuerySynthesizedAttributeType nodeList, VariantVector targetVariantVector)
{
  NodeQuerySynthesizedAttributeType returnList;

  AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(), boost::bind(querySolverGrammarElementFromVariantVector,
                                    boost::placeholders::_1, targetVariantVector, &returnList));
  return returnList;
}

NodeQuerySynthesizedAttributeType
NodeQuery::querySubTree(SgNode* subTree, VariantT targetVariant, AstQueryNamespace::QueryDepth defineQueryType)
{
  return NodeQuery::querySubTree(subTree, VariantVector(targetVariant), defineQueryType);
}

NodeQuerySynthesizedAttributeType NodeQuery::queryNodeList ( NodeQuerySynthesizedAttributeType queryList, VariantT targetVariant)
{
  return NodeQuery::queryNodeList(queryList,VariantVector(targetVariant));
}

struct TypeQueryDummyFunctionalTest
{
  using result_type = void*;
  result_type operator()(SgNode* node, Rose_STL_Container<SgNode*>* ) const;
};

TypeQueryDummyFunctionalTest::result_type
TypeQueryDummyFunctionalTest::operator()(SgNode* node, Rose_STL_Container<SgNode*>* returnList) const
{
  returnList->push_back(node);
  return nullptr;
}

typedef SgNode* node_ptr;

struct TwoParamaters
{
  using result_type = void*;
  using first_argument_type = node_ptr;
  using second_argument_type = Rose_STL_Container<SgNode*>*;
  result_type operator()(first_argument_type node, const second_argument_type returnList) const
  {
    returnList->push_back(node);
    return nullptr;
  }
};

struct OneParamater
{
  using result_type = Rose_STL_Container<SgNode*>;
  result_type operator()(SgNode* node) const
  {
    result_type returnList;
    returnList.push_back(node);
    return returnList;
  }
};

Rose_STL_Container<SgNode*> NodeQuery::generateListOfTypes(SgNode* astNode)
{
  // Insert your own manipulation of the AST here...
  Rose_STL_Container<SgNode*> nodeList;

  bool useMemoryPool = (isSgProject(astNode) != nullptr);
  if (useMemoryPool == false)
  {
    // Check if this is a SgFile or SgGlobal where there is only a single SgFile in the SgProject!
    if (isSgFile(astNode) != nullptr || isSgGlobal(astNode) != nullptr)
    {
      SgProject* project = TransformationSupport::getProject(astNode);

      // 2nd chance to reset useMemoryPool and provide an optimized query for types!
      useMemoryPool = (project->numberOfFiles() == 1);
    }
  }

  if (useMemoryPool == true)
  {
    // Then just query the memory pool and provide a much faster query.
    TypeQueryDummyFunctionalTest funcTest;

    // Build the list of all IR node variants that are derived from SgType (including SgType)
    VariantVector ir_nodes (V_SgType);

    // Execute the query on each separate memory pool for the list of IR node variants
    AstQueryNamespace::queryMemoryPool(std::bind(funcTest,std::placeholders::_1,&nodeList),&ir_nodes);
  }
  else
  {
    // This operation is expensive and not always accurate (though only vacuious destructors might be missing).
    // As a result we would like to deprecate its use in ROSE (at least for V_SgType \n");
    // Get the types from the specified subtree

    // DQ (1/13/2011): This will only get a subset of types.
    nodeList = NodeQuery::querySubTree (astNode,V_SgType);
  }
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
NodeQuery::queryMemoryPool (SgNode* traversal, NodeQuery::roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  return AstQueryNamespace::queryMemoryPool(std::bind(querySolverFunction, std::placeholders::_1,traversal), targetVariantVector);
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
NodeQuery::queryMemoryPool(SgNode*, NodeQuery::roseFunctionPointerOneParameter querySolverFunction,
                           VariantVector* targetVariantVector)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  return AstQueryNamespace::queryMemoryPool(querySolverFunction, targetVariantVector);
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
NodeQuery::queryMemoryPool(SgNode* traversal, NodeQuery::TypeOfQueryTypeTwoParameters elementReturnType,
                           VariantVector* targetVariantVector)
{
  return AstQueryNamespace::queryMemoryPool(std::bind(getFunction(elementReturnType),std::placeholders::_1,traversal), targetVariantVector);
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
NodeQuery::queryMemoryPool(NodeQuery::TypeOfQueryTypeOneParameter elementReturnType, VariantVector* targetVariantVector)
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
