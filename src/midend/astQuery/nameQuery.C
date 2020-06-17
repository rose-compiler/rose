// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#include "nameQuery.h"
#define DEBUG_NAMEQUERY 0

// #include "arrayTransformationSupport.h"


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
// *****************************************************************
//                Interface function for Name Query
// *****************************************************************


NameQuerySynthesizedAttributeType
NameQuery::queryNameArgumentNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgFunctionDeclaration *sageFunctionDeclaration =
    isSgFunctionDeclaration (astNode);

  if (sageFunctionDeclaration != NULL)
    {

      typedef SgInitializedNamePtrList::iterator argumentIterator;
      SgInitializedNamePtrList sageNameList = sageFunctionDeclaration->get_args ();
      int countArguments = 0;
      for (argumentIterator i = sageNameList.begin();
           i != sageNameList.end(); ++i)
        {
          SgInitializedName* elementNode = *i;
          ROSE_ASSERT (elementNode != NULL);

          string sageArgument(elementNode->get_name().str());

          returnNameList.push_back(sageArgument.c_str());

          countArguments += 1;
        }
#if DEBUG_NAMEQUERY
      printf ("\nHere is a function declaration :Line = %d Columns = %d \n",
              Rose::getLineNumber (isSgLocatedNode (astNode)),
              Rose::getColumnNumber (isSgLocatedNode (astNode)));
      cout << "The filename is:" << Rose::getFileName (isSgLocatedNode (astNode)) << endl;
      cout << "The count of arguments is: " << countArguments << endl;
#endif
    }

  return returnNameList;
}                               /* End function queryNameArgumentNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameClassDeclarationNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_class)
      {

        string name = sageClassDeclaration->get_name ().str ();

#if DEBUG_NAMEQUERY
        printf ("In case: CLASS_DECL_STMT name = %s \n", name.c_str ());
#endif

        returnNameList.push_back (name);
      }


  return returnNameList;

}                               /* End function queryNameClassDeclarationNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameStructNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_struct)
      {
        string name = sageClassDeclaration->get_name ().str ();

#if DEBUG_NAMEQUERY
        printf ("In case: CLASS_DECL_STMT name = %s \n", name.c_str ());
#endif

        returnNameList.push_back (name);
      }


  return returnNameList;

}                               /* End function queryNameStructNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameUnionNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (astNode);

  if (sageClassDeclaration != NULL)
    if (sageClassDeclaration->get_class_type () ==
        SgClassDeclaration::e_union)
      {
        string name = sageClassDeclaration->get_name ().str ();

#if DEBUG_NAMEQUERY
        printf ("In case: CLASS_DECL_STMT name = %s \n", name.c_str ());
#endif

        returnNameList.push_back (name);
      }


  return returnNameList;

}                               /* End function queryNameUnionNames() */

NameQuerySynthesizedAttributeType
NameQuery::queryNameMemberFunctionDeclarationNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgMemberFunctionDeclaration *sageMemberFunctionDeclaration =
    isSgMemberFunctionDeclaration (astNode);

  if (sageMemberFunctionDeclaration != NULL)
    {
      string name = sageMemberFunctionDeclaration->get_name ().str ();

#if DEBUG_NAMEQUERY
      printf ("In case: CLASS_DECL_STMT name = %s \n", name.c_str ());
#endif

      returnNameList.push_back (name);
    }


  return returnNameList;

}                               /* End function queryNameMemberFunctionDeclarationNames() */



NameQuerySynthesizedAttributeType
NameQuery::queryNameFunctionDeclarationNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgFunctionDeclaration *sageFunctionDeclaration =
    isSgFunctionDeclaration (astNode);

  if (sageFunctionDeclaration != NULL)
    {
      string name = sageFunctionDeclaration->get_name ().str ();

#if DEBUG_NAMEQUERY
      printf ("In case: CLASS_DECL_STMT name = %s \n", name.c_str ());
#endif

      returnNameList.push_back (name);
    }


  return returnNameList;

}                               /* End function queryNameFunctionDeclarationNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameVariableTypeNames (SgNode * astNode)
{


  ROSE_ASSERT (astNode != 0);


  NameQuerySynthesizedAttributeType returnNameList;

  /*
     SgVarRefExp *sageVarRefExp = isSgVarRefExp (astNode);

     if(sageVarRefExp != NULL)
     {

     SgVariableSymbol* variableSymbol = sageVarRefExp->get_symbol();
     ROSE_ASSERT (variableSymbol != NULL);

     SgType* type = variableSymbol->get_type();
     ROSE_ASSERT (type != NULL);


     string typeName = TransformationSupport::getTypeName(type);
     ROSE_ASSERT (typeName.length() > 0);

     returnNameList.push_back(typeName);
     }
   */

  switch (astNode->variantT ())
    {

    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration *sageVariableDeclaration =
          isSgVariableDeclaration (astNode);
        ROSE_ASSERT (sageVariableDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

#if DEBUG_NAMEQUERY
        printf ("\nIn filename: %s ",
                Rose::getFileName (isSgLocatedNode (astNode)));
        printf ("\nHere is a variable :Line = %d Columns = %d \n",
                Rose::getLineNumber (isSgLocatedNode (astNode)),
                Rose::getColumnNumber (isSgLocatedNode (astNode)));
        //cout << "The typename of the variable is: " << typeName << endl;
#endif

        typedef SgInitializedNamePtrList::iterator variableIterator;
        SgType *typeNode;

        for (variableIterator variableListElement = sageInitializedNameList.begin ();
             variableListElement != sageInitializedNameList.end ();
             ++variableListElement)
          {
            SgInitializedName* elmVar = *variableListElement;

            typeNode = elmVar->get_type();
            ROSE_ASSERT (typeNode != NULL);
            returnNameList.push_back (TransformationSupport::getTypeName (typeNode));
#if DEBUG_NAMEQUERY
            cout << "The typeName of the variable is: " <<
              TransformationSupport::getTypeName (typeNode) << endl;
#endif
          }
        break;
      }                         /* End case V_SgVariableDeclaration */

    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
       {
      SgFunctionDeclaration * sageFunctionDeclaration =
        isSgFunctionDeclaration (astNode);
      ROSE_ASSERT (sageFunctionDeclaration != NULL);

      SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args ();
      SgType *typeNode;

#if DEBUG_NAMEQUERY
      printf ("\nIn filename: %s ",
              Rose::getFileName (isSgLocatedNode (astNode)));
      printf ("\nHere is a variable :Line = %d Columns = %d \n",
              Rose::getLineNumber (isSgLocatedNode (astNode)),
              Rose::getColumnNumber (isSgLocatedNode (astNode)));
      //cout << "The typename of the variable is: " << typeName << endl;
#endif


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
          returnNameList.push_back (TransformationSupport::getTypeName (typeNode));
#if DEBUG_NAMEQUERY
          cout << "The typeName of the variable is: " <<
            TransformationSupport::getTypeName (typeNode) << endl;
#endif
        }

      break;
       }
      default:
        {
       // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
        }
    }                           /* End switch case astNode */



  return returnNameList;

}                               /* End function queryNameVariableTypeNames() */




NameQuerySynthesizedAttributeType
NameQuery::queryNameVariableNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

// SgVarRefExp *sageVarRefExp = isSgVarRefExp (astNode);

  switch (astNode->variantT ())
    {
      /*
         case V_SgVarRefExp:
         {
         SgVarRefExp* sageVarRefExp = isSgVarRefExp(astNode);
         ROSE_ASSERT( sageVarRefExp != NULL);

         SgVariableSymbol* variableSymbol = sageVarRefExp->get_symbol();
         ROSE_ASSERT (variableSymbol != NULL);

         SgType* type = variableSymbol->get_type();
         ROSE_ASSERT (type != NULL);


         string typeName = TransformationSupport::getTypeName(type);
         ROSE_ASSERT (typeName.length() > 0);


         // Only define the variable name if we are using an object of array type
         SgInitializedName* initializedName = variableSymbol->get_declaration();
         ROSE_ASSERT (initializedName != NULL);
         SgName variableName = initializedName->get_name();
         printf("\nIn filename: %s ", Rose::getFileName(isSgLocatedNode(astNode)));
         printf ("\nHere is a variable :Line = %d Columns = %d \n", Rose:: getLineNumber (isSgLocatedNode(astNode) ), Rose:: getColumnNumber ( isSgLocatedNode(astNode) ));
         cout << "The typename of the variable is: " << typeName << endl;
         cout << "The name of the variable is: " << variableName.str() << endl;
         // copy the string to avoid corruption of the AST's version of the string
         string name = variableName.str();
         returnNameList.push_back (name);

         break;
         }
       */

    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration *sageVariableDeclaration =
          isSgVariableDeclaration (astNode);
        ROSE_ASSERT (sageVariableDeclaration != NULL);

        SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

#if DEBUG_NAMEQUERY
        printf ("\nIn filename: %s ",
                Rose::getFileName (isSgLocatedNode (astNode)));
        printf ("\nHere is a variable :Line = %d Columns = %d \n",
                Rose::getLineNumber (isSgLocatedNode (astNode)),
                Rose::getColumnNumber (isSgLocatedNode (astNode)));
        //cout << "The typename of the variable is: " << typeName << endl;
#endif

        typedef SgInitializedNamePtrList::iterator variableIterator;

        for (variableIterator variableListElement =
             sageInitializedNameList.begin ();
             variableListElement != sageInitializedNameList.end ();
             ++variableListElement)
          {
            SgInitializedName* elmVar = *variableListElement;
            ROSE_ASSERT (elmVar != NULL);
            string name = elmVar->get_name ().str ();
            ROSE_ASSERT (name.length () > 0);
            returnNameList.push_back (name);
#if DEBUG_NAMEQUERY
            cout << "The name of the variable is: " << name << endl;
#endif
          }
        break;
      }                         /* End case V_SgVariableDeclaration */

    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
       {
      SgFunctionDeclaration * sageFunctionDeclaration =
        isSgFunctionDeclaration (astNode);
      ROSE_ASSERT (sageFunctionDeclaration != NULL);

      SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args ();


      typedef SgInitializedNamePtrList::iterator variableIterator;

      for (variableIterator variableListElement =
           sageInitializedNameList.begin ();
           variableListElement != sageInitializedNameList.end ();
           ++variableListElement)
        {
          SgInitializedName* elmVar = *variableListElement;

          ROSE_ASSERT (elmVar != NULL);
          string name = elmVar->get_name ().str ();
          ROSE_ASSERT (name.length () > 0);
          returnNameList.push_back (name);
#if 1
          cout << "The name of the variable is: " << name << endl;
#endif
        }

      break;
        }
       
      default:
        {
       // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
        }
    }                           /* End switch case astNode */


  return returnNameList;

}                               /* End function queryNameVariableNames() */



NameQuerySynthesizedAttributeType
NameQuery::queryNameClassFieldNames (SgNode * astNode)
{
 ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

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

            SgVariableDeclaration *sageVariableDeclaration =
              isSgVariableDeclaration (listElement);

            if (sageVariableDeclaration != NULL)
              {


                typedef SgInitializedNamePtrList::iterator INITLI;

                SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

                for (INITLI i = sageInitializedNameList.begin ();
                     i != sageInitializedNameList.end (); ++i)
                  {
                    SgInitializedName* initializedListElement = *i;
                    ROSE_ASSERT (isSgInitializedName (initializedListElement) != NULL);

                    returnNameList.push_back (initializedListElement->get_name ().str ());

                  }             /* End iteration over declarationStatementPtrList */

              }                 /* End iteration over declarationStatementPtrList */
          }

      }
    }
  return returnNameList;

}                               /* End function queryClassFieldNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameUnionFieldNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

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

            SgVariableDeclaration *sageVariableDeclaration =
              isSgVariableDeclaration (listElement);

            if (sageVariableDeclaration != NULL)
              {


                typedef SgInitializedNamePtrList::iterator INITLI;

                SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

                for (INITLI i = sageInitializedNameList.begin ();
                     i != sageInitializedNameList.end (); ++i)
                  {
                    SgInitializedName* initializedListElement = *i;
                    ROSE_ASSERT (isSgInitializedName (initializedListElement) != NULL);

                    returnNameList.push_back (initializedListElement->get_name().str());

                  }             /* End iteration over declarationStatementPtrList */

              }                 /* End iteration over declarationStatementPtrList */
          }

        }
    }

  return returnNameList;

}                               /* End function queryUnionFieldNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameStructFieldNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

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

            SgVariableDeclaration *sageVariableDeclaration =
              isSgVariableDeclaration (listElement);

            if (sageVariableDeclaration != NULL)
              {


                typedef SgInitializedNamePtrList::iterator INITLI;

                SgInitializedNamePtrList sageInitializedNameList = sageVariableDeclaration->get_variables ();

                for (INITLI i = sageInitializedNameList.begin ();
                     i != sageInitializedNameList.end (); ++i)
                  {
                    SgInitializedName* initializedListElement = *i;
                    ROSE_ASSERT (isSgInitializedName (initializedListElement) != NULL);

                    returnNameList.push_back (initializedListElement->get_name ().str ());

                  }             /* End iteration over declarationStatementPtrList */

              }                 /* End iteration over declarationStatementPtrList */
          }

      }
    }
  return returnNameList;

}                               /* End function queryStructFieldNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameFunctionReferenceNames (SgNode * astNode)
{

  ROSE_ASSERT (astNode != 0);

  NameQuerySynthesizedAttributeType returnNameList;

  SgFunctionCallExp *sageFunctionCallExp = isSgFunctionCallExp (astNode);

  if (sageFunctionCallExp != NULL)
    {
      string functionName = "unknown";
      Rose_STL_Container< string > argumentNameList;
      string returnTypeName = "double";

      returnNameList.push_back (TransformationSupport::getFunctionName (sageFunctionCallExp));

    }


  return returnNameList;

}                               /* End function NameQuery::queryNameVariableNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryVariableNamesWithTypeName (SgNode * astNode, string matchingName)
{

  ROSE_ASSERT (astNode != 0);
  ROSE_ASSERT (matchingName.length () > 0);

  NameQuerySynthesizedAttributeType returnNameList;

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

#if DEBUG_NAMEQUERY
        printf ("\nIn filename: %s ",
                Rose::getFileName (isSgLocatedNode (astNode)));
        printf ("\nHere is a variable :Line = %d Columns = %d \n",
                Rose::getLineNumber (isSgLocatedNode (astNode)),
                Rose::getColumnNumber (isSgLocatedNode (astNode)));
        //cout << "The typename of the variable is: " << typeName << endl;
#endif
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
            string typeName = TransformationSupport::getTypeName(typeNode);

            if (typeName == matchingName)
              {
                string name = elmVar->get_name ().str ();
                ROSE_ASSERT (name.length () > 0);
                returnNameList.push_back (name);
#if DEBUG_NAMEQUERY
                cout << "The name of the variable is: " << name << endl;
#endif
              }
          }
        break;
      }                         /* End case V_SgVariableDeclaration */

    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
       {
      SgFunctionDeclaration * sageFunctionDeclaration =
        isSgFunctionDeclaration (astNode);
      ROSE_ASSERT (sageFunctionDeclaration != NULL);

      SgInitializedNamePtrList sageInitializedNameList = sageFunctionDeclaration->get_args ();

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
          string typeName = TransformationSupport::getTypeName (typeNode);

          if (typeName == matchingName)
            {
              string name = elmVar->get_name ().str ();
              ROSE_ASSERT (name.length () > 0);
              returnNameList.push_back (name);
#if DEBUG_NAMEQUERY
              cout << "The name of the variable is: " << name << endl;
#endif
            }
        }

      break;
       }

      default:
        {
       // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
        }
    }                           /* End switch case astNode */

  return returnNameList;

}                               /* End function NameQuery::queryNameVariableNames() */


NameQuerySynthesizedAttributeType
NameQuery::queryNameTypedefDeclarationNames (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NameQuerySynthesizedAttributeType returnNodeList;
  SgTypedefDeclaration *sageTypedefDeclaration =
    isSgTypedefDeclaration (astNode);

  if (sageTypedefDeclaration != NULL)
    returnNodeList.push_back (sageTypedefDeclaration->get_name ().str ());

  return returnNodeList;
}                               /* End function queryNameTypedefDeclarationNames */


// DQ (8/27/2006): This functionality already exists elsewhere
// It is a shame that it is recreated here as well !!!
NameQuerySynthesizedAttributeType
NameQuery::queryNameTypeName (SgNode * astNode)
{
  ROSE_ASSERT (astNode != NULL);
  string typeName = "";
  Rose_STL_Container< string > returnList;
  SgType *type = isSgType (astNode);

  // printf ("In TransformationSupport::getTypeName(): type->sage_class_name() = %s \n",type->sage_class_name());

  if (type != NULL)
    switch (type->variantT ())
      {
      case V_SgTypeComplex:
        typeName = "complex";
        break;
      case V_SgTypeImaginary:
        typeName = "imaginary";
        break;
      case V_SgTypeBool:
        typeName = "bool";
        break;
      case V_SgEnumType:
        typeName = "enum";
        break;
      case V_SgTypeChar:
        typeName = "char";
        break;
      case V_SgTypeVoid:
        typeName = "void";
        break;
      case V_SgTypeInt:
        typeName = "int";
        break;
      case V_SgTypeDouble:
        typeName = "double";
        break;
      case V_SgTypeFloat:
        typeName = "float";
        break;
      case V_SgTypeLong:
        typeName = "long";
        break;
      case V_SgTypeLongDouble:
        typeName = "long double";
        break;
      case V_SgTypeEllipse:
        typeName = "ellipse";
        break;
      case V_SgTypeGlobalVoid:
        typeName = "void";
        break;
      case V_SgTypeLongLong:
        typeName = "long long";
        break;
      case V_SgTypeShort:
        typeName = "short";
        break;
      case V_SgTypeSignedChar:
        typeName = "signed char";
        break;
      case V_SgTypeSignedInt:
        typeName = "signed int";
        break;
      case V_SgTypeSignedLong:
        typeName = "signed long";
        break;
      case V_SgTypeSignedShort:
        typeName = "signed short";
        break;
      case V_SgTypeString:
        typeName = "string";
        break;
      case V_SgTypeUnknown:
        typeName = "unknown";
        break;
      case V_SgTypeUnsignedChar:
        typeName = "unsigned char";
        break;
      case V_SgTypeUnsignedInt:
        typeName = "unsigned int";
        break;
      case V_SgTypeUnsignedLong:
        typeName = "unsigned long";
        break;
      case V_SgTypeUnsignedShort:
        typeName = "unsigned short";
        break;
      case V_SgTypeUnsignedLongLong:
        typeName = "unsigned long long";
        break;
      case V_SgReferenceType:
        {
          ROSE_ASSERT (isSgReferenceType (type)->get_base_type () != NULL);

          Rose_STL_Container< string > subTypeNames = queryNameTypeName (isSgReferenceType (type)->get_base_type ());

          typedef Rose_STL_Container< string >::iterator typeIterator;

          //This iterator will only contain one name
          for (typeIterator i = subTypeNames.begin ();
               i != subTypeNames.end (); ++i)
            {
              string e = *i;
              typeName = e;
              break;
            }

          break;
        }
      case V_SgPointerType:
        {
          ROSE_ASSERT (isSgPointerType (type)->get_base_type () != NULL);

          Rose_STL_Container< string > subTypeNames =
            queryNameTypeName (isSgPointerType (type)->get_base_type ());

          typedef Rose_STL_Container< string >::iterator typeIterator;

          //This iterator will only contain one name
          for (typeIterator i = subTypeNames.begin ();
               i != subTypeNames.end (); ++i)
            {
              string e = *i;
              typeName = e;
              break;
            }

          break;
        }
      case V_SgModifierType:
        {
          ROSE_ASSERT (isSgModifierType (type)->get_base_type () != NULL);

          Rose_STL_Container< string > subTypeNames =
            queryNameTypeName (isSgModifierType (type)->get_base_type ());

          typedef Rose_STL_Container< string >::iterator typeIterator;

          //This iterator will only contain one name
          for (typeIterator i = subTypeNames.begin ();
               i != subTypeNames.end (); ++i)
            {
              string e = *i;
              typeName = e;
              break;
            }
          break;
        }
      case V_SgNamedType:
        {
          SgNamedType *sageNamedType = isSgNamedType (type);
          ROSE_ASSERT (sageNamedType != NULL);
          typeName = sageNamedType->get_name ().str ();
          break;
        }
      case V_SgClassType:
        {
          SgClassType *sageClassType = isSgClassType (type);
          ROSE_ASSERT (sageClassType != NULL);
          typeName = sageClassType->get_name ().str ();
          break;
        }
      case V_SgTypedefType:
        {
          SgTypedefType *sageTypedefType = isSgTypedefType (type);
          ROSE_ASSERT (sageTypedefType != NULL);
          typeName = sageTypedefType->get_name ().str ();
          break;
        }
      case V_SgPointerMemberType:
        {
          SgPointerMemberType *pointerMemberType =
            isSgPointerMemberType (type);
          ROSE_ASSERT (pointerMemberType != NULL);
          SgClassType *classType =
            isSgClassType(pointerMemberType->get_class_type()->stripTypedefsAndModifiers());
          ROSE_ASSERT (classType != NULL);
          SgClassDeclaration *classDeclaration =
            isSgClassDeclaration(classType->get_declaration());
          ROSE_ASSERT (classDeclaration != NULL);
          typeName = classDeclaration->get_name ().str ();
          break;
        }
      case V_SgArrayType:
        {
          ROSE_ASSERT (isSgArrayType (type)->get_base_type () != NULL);


          Rose_STL_Container< string > subTypeNames =
            queryNameTypeName (isSgArrayType (type)->get_base_type ());

          typedef Rose_STL_Container< string >::iterator typeIterator;

          //This iterator will only contain one name
          for (typeIterator i = subTypeNames.begin ();
               i != subTypeNames.end (); ++i)
            {
              string e = *i;
              typeName = e;
              break;
            }
          break;
        }
      case V_SgFunctionType:
        {
          SgFunctionType *functionType = isSgFunctionType (type);
          ROSE_ASSERT (functionType != NULL);
          typeName = functionType->get_mangled_type ().str ();
          break;
        }
      case V_SgMemberFunctionType:
        {
          SgMemberFunctionType *memberFunctionType =
            isSgMemberFunctionType (type);
          ROSE_ASSERT (memberFunctionType != NULL);
          SgClassType *classType =
            isSgClassType(memberFunctionType->get_class_type()->stripTypedefsAndModifiers());
          ROSE_ASSERT (classType != NULL);
          SgClassDeclaration *classDeclaration =
            isSgClassDeclaration(classType->get_declaration());
          ROSE_ASSERT (classDeclaration != NULL);
          typeName = classDeclaration->get_name ().str ();
          break;
        }
      case V_SgTypeWchar:
        typeName = "wchar";
        break;
      case V_SgTypeDefault:
        typeName = "default";
        break;
      default:
        printf
          ("default reached in switch within TransformationSupport::getTypeName type->sage_class_name() = %s variant = %d \n",
           type->sage_class_name (), type->variant ());
        ROSE_ABORT ();
        break;
      }

  // Fix for purify problem report
  // typeName = Rose::stringDuplicate(typeName);

  if (typeName.size () > 0)
    returnList.push_back (typeName);
  //ROSE_ASSERT(typeName.c_str() != NULL);
  // return typeName;
  return returnList;
//return Rose::stringDuplicate(typeName.c_str());
}




std::function<Rose_STL_Container<std::string>(SgNode*) > NameQuery::getFunction(NameQuery::TypeOfQueryTypeOneParameter oneParam){
           NameQuery::roseFunctionPointerOneParameter __x; 
     switch (oneParam)
        {
          case UnknownListElementType:
             {
               printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
               ROSE_ASSERT (false);
             }
    case VariableNames:
      {
        __x = queryNameVariableNames;
        break;
      }
    case VariableTypeNames:
      {
        __x = queryNameVariableTypeNames;
        break;
      }
    case FunctionDeclarationNames:
      {
        __x =
          queryNameFunctionDeclarationNames;
        break;
      }
    case MemberFunctionDeclarationNames:
      {
        __x =
          queryNameMemberFunctionDeclarationNames;
        break;
      }
    case ClassDeclarationNames:
      {
        __x = queryNameClassDeclarationNames;
        break;
      }
    case ArgumentNames:
      {
        __x = queryNameArgumentNames;
        break;
      }
    case StructNames:
      {
        __x = queryNameStructNames;
        break;
      }
    case UnionNames:
      {
        __x = queryNameUnionNames;
        break;
      }
    case ClassFieldNames:
      {
        __x = queryNameClassFieldNames;
        break;
      }
    case StructFieldNames:
      {
        __x = queryNameStructFieldNames;
        break;
      }
    case UnionFieldNames:
      {
        __x = queryNameUnionFieldNames;
        break;
      }
    case FunctionReferenceNames:
      {
        __x =
          queryNameFunctionReferenceNames;
        break;
      }
    case TypedefDeclarationNames:
      {
        __x =
          queryNameTypedefDeclarationNames;
        break;
      }
    case TypeNames:
      {
        __x = queryNameTypeName;
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

std::function< Rose_STL_Container<std::string>(SgNode*, std::string) > NameQuery::getFunction(NameQuery::TypeOfQueryTypeTwoParameters twoParam){
     NameQuery::roseFunctionPointerTwoParameters __x;
     switch (twoParam)
        {
          case UnknownListElementTypeTwoParameters:
             {
               printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
               ROSE_ASSERT (false);
             }
          case VariableNamesWithTypeName:
             {
               __x =
                 queryVariableNamesWithTypeName;
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


          NameQuerySynthesizedAttributeType NameQuery::querySubTree 
                  ( SgNode * subTree,
                    NameQuery::TypeOfQueryTypeOneParameter elementReturnType,
                    AstQueryNamespace::QueryDepth defineQueryType){
                   return AstQueryNamespace::querySubTree(subTree, getFunction(elementReturnType), defineQueryType);
          }

       // get the SgNode's conforming to the test in querySolverFunction or
       // get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
          NameQuerySynthesizedAttributeType NameQuery::querySubTree 
                  ( SgNode * subTree,
                    std::string traversal,
                    NameQuery::roseFunctionPointerTwoParameters querySolverFunction,
                    AstQueryNamespace::QueryDepth defineQueryType){
                     return AstQueryNamespace::querySubTree(subTree, 
                                  std::bind(std::ptr_fun(querySolverFunction), std::placeholders::_1, traversal), defineQueryType);
          };
          NameQuerySynthesizedAttributeType NameQuery::querySubTree
                  ( SgNode * subTree,
                    std::string traversal,
                    NameQuery::TypeOfQueryTypeTwoParameters elementReturnType,
                    AstQueryNamespace::QueryDepth defineQueryType ){
                    return AstQueryNamespace::querySubTree(subTree, 
                                  std::bind(getFunction(elementReturnType), std::placeholders::_1 , traversal), defineQueryType);
          };



       // perform a query on a list<SgNode>
          NameQuerySynthesizedAttributeType NameQuery::queryNodeList 
                 ( Rose_STL_Container< SgNode * >nodeList,
                   NameQuery::roseFunctionPointerOneParameter querySolverFunction){
                 return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
                                 std::ptr_fun(querySolverFunction));
          };
          NameQuerySynthesizedAttributeType NameQuery::queryNodeList 
                 ( Rose_STL_Container<SgNode*> nodeList,
                   NameQuery::TypeOfQueryTypeOneParameter elementReturnType ){
                 return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),getFunction(elementReturnType));

          };

          NameQuerySynthesizedAttributeType
           NameQuery::querySubTree
           (SgNode * subTree,
            NameQuery::roseFunctionPointerOneParameter elementReturnType,
            AstQueryNamespace::QueryDepth defineQueryType 
           ){

            return  AstQueryNamespace::querySubTree(subTree,
                                  std::ptr_fun(elementReturnType),defineQueryType);

          };



          NameQuerySynthesizedAttributeType NameQuery::queryNodeList 
                 ( Rose_STL_Container<SgNode*> nodeList,
                   std::string targetNode,
                   NameQuery::roseFunctionPointerTwoParameters querySolverFunction ){
                return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
                             std::bind(std::ptr_fun(querySolverFunction), std::placeholders::_1, targetNode));
//                                  std::bind2nd(getFunction(elementReturnType),traversal), defineQueryType);

          };
          NameQuerySynthesizedAttributeType NameQuery::queryNodeList 
                 ( Rose_STL_Container<SgNode*> nodeList,
                   std::string targetNode,
                   NameQuery::TypeOfQueryTypeTwoParameters elementReturnType ){
                return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
                             std::bind(getFunction(elementReturnType), std::placeholders::_1, targetNode));

          };





  /********************************************************************************
   * The function
   *      NameQuerySynthesizedAttributeType queryMemoryPool ( SgNode * subTree,
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
   ********************************************************************************/
NameQuerySynthesizedAttributeType
  NameQuery::queryMemoryPool
    (
     std::string traversal,
     NameQuery::roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector)
   {
         return AstQueryNamespace::queryMemoryPool(
                                  std::bind(std::ptr_fun(querySolverFunction), std::placeholders::_1, traversal), targetVariantVector);

   };


/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*),
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
NameQuerySynthesizedAttributeType
  NameQuery::queryMemoryPool
    (
     std::string traversal,
     NameQuery::roseFunctionPointerOneParameter querySolverFunction, VariantVector* targetVariantVector)
   {
   return  AstQueryNamespace::queryMemoryPool(
                                  std::ptr_fun(querySolverFunction),targetVariantVector);


   };

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree, SgNode*,
 *                   TypeOfQueryTypeTwoParameters,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

NameQuerySynthesizedAttributeType
  NameQuery::queryMemoryPool
    (
     std::string traversal,
     NameQuery::TypeOfQueryTypeTwoParameters elementReturnType,
     VariantVector* targetVariantVector)
   {
 return AstQueryNamespace::queryMemoryPool( 
                                  std::bind(getFunction(elementReturnType), std::placeholders::_1, traversal), targetVariantVector);


   };

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   TypeOfQueryTypeOneParameter,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

NameQuerySynthesizedAttributeType
  NameQuery::queryMemoryPool
    (
     NameQuery::TypeOfQueryTypeOneParameter elementReturnType,
     VariantVector* targetVariantVector)
   {

     return AstQueryNamespace::queryMemoryPool(getFunction(elementReturnType), targetVariantVector);
 
    };




