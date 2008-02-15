
#include "rose.h"

using namespace std;


// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#define DEBUG_NODEQUERY 0
// #include "arrayTransformationSupport.h"



// **********************************************************
//     Support for nested class: AstQuery<AstQuerySynthesizedAttributeType>::VariantVector
// **********************************************************

VariantVector::VariantVector ( const VariantT & X )
   {
#if 0
     switch (X)
	{
	  case V_SgNode:
		{
		  *this = VariantVector(V_SgLocatedNode) + 
			  VariantVector(V_SgType) +
			  VariantVector(V_SgSupport) +
			  VariantVector(V_SgSymbol);
		  insert(begin(),V_SgNode);
		  break;
		}

	  case V_SgLocatedNode:
		{
		  *this = VariantVector(V_SgStatement) + VariantVector(V_SgExpression);
		  insert(begin(),V_SgLocatedNode);
		  break;
		}

	  case V_SgType:
		{
	       // printf ("Building the vector of variants for the SgType \n");
		  *this = V_SgArrayType        + V_SgTypeComplex              + V_SgTypeImaginary     + VariantVector(V_SgFunctionType) + 
			  V_SgModifierType     + VariantVector(V_SgNamedType) + VariantVector(V_SgPointerType) +
			  V_SgReferenceType    + V_SgTypeBool                 + V_SgTypeChar          +
			  V_SgTypeDefault      + V_SgTypeDouble               + V_SgTypeEllipse       +
			  V_SgTypeFloat        + V_SgTypeGlobalVoid           + V_SgTypeInt           +
			  V_SgTypeLong         + V_SgTypeLongDouble           + V_SgTypeLongLong      +
			  V_SgTypeShort        + V_SgTypeSignedChar           + V_SgTypeSignedInt     +
			  V_SgTypeSignedLong   + V_SgTypeSignedShort          + V_SgTypeString        +
			  V_SgTypeUnknown      + V_SgTypeUnsignedChar         + V_SgTypeUnsignedInt   +
			  V_SgTypeUnsignedLong + V_SgTypeUnsignedLongLong     + V_SgTypeUnsignedShort +
			  V_SgTypeVoid         + V_SgTypeWchar;
		  insert(begin(),V_SgType);
		  break;
		}
#if 0
	  case V_SgClassType:
		{
		  *this = V_SgTemplateType;
		  insert(begin(),V_SgClassType);
		  break;
		}
#endif
	  case V_SgMemberFunctionType:
		{
		  *this = V_SgPartialFunctionType + V_SgUnknownMemberFunctionType;
		  insert(begin(),V_SgMemberFunctionType);
		  break;
		}

	  case V_SgFunctionType:
		{
		  *this = VariantVector(V_SgMemberFunctionType) + V_SgUnknownMemberFunctionType;
		  insert(begin(),V_SgFunctionType);
		  break;
		}

	  case V_SgPointerType:
		{
		  *this = V_SgPointerMemberType;
		  insert(begin(),V_SgPointerType);
		  break;
		}

	  case V_SgNamedType:
		{
		  *this = VariantVector(V_SgClassType) + V_SgEnumType + V_SgTypedefType;
		  insert(begin(),V_SgNamedType);
		  break;
		}

	  case V_SgInitializer:
		{
		  *this = V_SgAggregateInitializer + V_SgConstructorInitializer + V_SgAssignInitializer;
		  insert(begin(),V_SgInitializer);
		  break;
		}

	  case V_SgExpression:
		{
		  *this = VariantVector(V_SgUnaryOp) + VariantVector(V_SgBinaryOp) + V_SgExprListExp     +
			  V_SgVarRefExp              + V_SgClassNameRefExp         + V_SgFunctionRefExp  +
			  V_SgMemberFunctionRefExp   + VariantVector(V_SgValueExp) + V_SgFunctionCallExp +
			  V_SgSizeOfOp               + V_SgTypeIdOp                + V_SgConditionalExp  +
			  V_SgNewExp                 + V_SgDeleteExp               + V_SgThisExp         +
			  V_SgRefExp                 + V_SgInitializer             + V_SgVarArgStartOp   +
			  V_SgVarArgOp               + V_SgVarArgEndOp             + V_SgVarArgCopyOp    +
			  V_SgVarArgStartOneOperandOp;
		  insert(begin(),V_SgExpression);
		  break;
		}

	  case V_SgValueExp:
		{
		  *this = V_SgBoolValExp             + V_SgStringVal         + V_SgShortVal       +
			  V_SgCharVal                + V_SgUnsignedCharVal   + V_SgWcharVal       +
			  V_SgUnsignedShortVal       + V_SgIntVal            + V_SgEnumVal        +
			  V_SgUnsignedIntVal         + V_SgLongIntVal        + V_SgLongLongIntVal +
			  V_SgUnsignedLongLongIntVal + V_SgUnsignedLongVal   + V_SgFloatVal       + 
			  V_SgDoubleVal              + V_SgLongDoubleVal;
		  insert(begin(),V_SgValueExp);
		  break;
		}

	  case V_SgUnaryOp:
		{
	       // printf ("Building the vector of variants for the SgType \n");
		  *this = V_SgExpressionRoot + V_SgMinusOp         + V_SgUnaryAddOp      + 
			  V_SgNotOp          + V_SgPointerDerefExp + V_SgAddressOfOp     +
			  V_SgMinusMinusOp   + V_SgPlusPlusOp      + V_SgBitComplementOp +
			  V_SgCastExp        + V_SgThrowOp;
		  insert(begin(),V_SgUnaryOp);
		  break;
		}

	  case V_SgBinaryOp:
		{
	       // printf ("Building the vector of variants for the SgType \n");
		  *this = V_SgArrowExp         + V_SgDotExp        + V_SgDotStarOp       +
			  V_SgArrowStarOp      + V_SgEqualityOp    + V_SgLessThanOp      + 
			  V_SgGreaterThanOp    + V_SgNotEqualOp    + V_SgLessOrEqualOp   + 
			  V_SgGreaterOrEqualOp + V_SgAddOp         + V_SgSubtractOp      +
			  V_SgMultiplyOp       + V_SgDivideOp      + V_SgIntegerDivideOp +
			  V_SgModOp            + V_SgAndOp         + V_SgOrOp            +
			  V_SgBitXorOp         + V_SgBitAndOp      + V_SgBitOrOp         + 
			  V_SgCommaOpExp       + V_SgLshiftOp      + V_SgRshiftOp        +
			  V_SgPntrArrRefExp    + V_SgScopeOp       + V_SgAssignOp        +
			  V_SgPlusAssignOp     + V_SgMinusAssignOp + V_SgAndAssignOp     +
			  V_SgIorAssignOp      + V_SgMultAssignOp  + V_SgDivAssignOp     +
			  V_SgModAssignOp      + V_SgXorAssignOp   + V_SgLshiftAssignOp  +
			  V_SgRshiftAssignOp;
		  insert(begin(),V_SgBinaryOp);
		  break;
		}

	  case V_SgStatement:
		{
	       // printf ("Building the vector of variants for the SgStatment \n");
		  *this = VariantVector(V_SgScopeStatement)       + V_SgFunctionTypeTable            + 
			  VariantVector(V_SgDeclarationStatement) + VariantVector(V_SgExprStatement) + 
			  V_SgLabelStatement                      + V_SgCaseOptionStmt               +
			  V_SgTryStmt                             + V_SgDefaultOptionStmt            + 
			  V_SgBreakStmt                           + V_SgContinueStmt                 +
			  V_SgReturnStmt                          + V_SgGotoStatement                +
			  V_SgSpawnStmt                           + V_SgForInitStatement             +
			  V_SgCatchStatementSeq                   + V_SgClinkageStartStatement;
		  insert(begin(),V_SgStatement);
		  break;
		}

	  case V_SgScopeStatement:
		{
	       // printf ("Building the vector of variants for the SgScopeStatement \n");
		  *this = V_SgGlobal                        + V_SgBasicBlock                         + 
			  V_SgIfStmt                        + V_SgForStatement                       +
			  V_SgFunctionDefinition            + VariantVector(V_SgClassDefinition)     +
			  V_SgWhileStmt                     + V_SgDoWhileStmt                        + 
			  V_SgSwitchStatement               + V_SgCatchOptionStmt                    + 
			  V_SgNamespaceDeclarationStatement + V_SgNamespaceAliasDeclarationStatement + 
			  V_SgUsingDeclarationStatement;
		  insert(begin(),V_SgScopeStatement);
		  break;
		}

	  case V_SgDeclarationStatement:
		{
	       // printf ("Building the vector of variants for the SgDeclarationStatement \n");
		  *this = V_SgFunctionDeclaration   + V_SgVariableDeclaration             + 
			  V_SgVariableDefinition    + VariantVector(V_SgClassDeclaration) +
			  V_SgEnumDeclaration       + V_SgAsmStmt                         + 
			  V_SgTypedefDeclaration    + V_SgTemplateDeclaration             + 
			  V_SgFunctionParameterList + V_SgCtorInitializerList             +
			  V_SgPragmaDeclaration;
		  insert(begin(),V_SgDeclarationStatement);
		  break;
		}

	  case V_SgFunctionDeclaration:
		{
	       // printf ("Building the vector of variants for the SgFunctionDeclaration \n");
		  *this = VariantVector(V_SgMemberFunctionDeclaration) + 
			  V_SgTemplateInstantiationFunctionDecl;
		  insert(begin(),V_SgFunctionDeclaration);
		  break;
		}

	  case V_SgClassDeclaration:
		{
	       // printf ("Building the vector of variants for the SgClassDeclaration \n");
		  *this = V_SgTemplateInstantiationDecl;
		  insert(begin(),V_SgClassDeclaration);
		  break;
		}

	  case V_SgClassDefinition:
		{
	       // printf ("Building the vector of variants for the SgClassDefinition \n");
		  *this = V_SgTemplateInstantiationDefn;
		  insert(begin(),V_SgClassDefinition);
		  break;
		}

	  case V_SgMemberFunctionDeclaration:
		{
	       // printf ("Building the vector of variants for the SgMemberFunctionDeclaration \n");
		  *this = V_SgTemplateInstantiationMemberFunctionDecl;
		  insert(begin(),V_SgMemberFunctionDeclaration);
		  break;
		}

	  case V_SgSymbol:
		{
	       // printf ("Building the vector of variants for the SgSymbol \n");
		  *this = VariantVector(V_SgClassSymbol)    + V_SgDefaultSymbol      + 
			  V_SgEnumFieldSymbol               + V_SgEnumSymbol         + 
			  VariantVector(V_SgFunctionSymbol) + V_SgFunctionTypeSymbol + 
			  V_SgLabelSymbol                   + V_SgTemplateSymbol     + 
			  V_SgTypedefSymbol                 + V_SgVariableSymbol;
		  insert(begin(),V_SgSymbol);
		  break;
		}

#if 0
	  case V_SgClassSymbol:
		{
	       // printf ("Building the vector of variants for the SgClassSymbol \n");
		  *this = V_SgTemplateInstantiationSymbol;
		  insert(begin(),V_SgClassSymbol);
		  break;
		}
#endif
	  case V_SgFunctionSymbol:
		{
	       // printf ("Building the vector of variants for the SgFunctionSymbol \n");
		  *this = V_SgMemberFunctionSymbol;
		  insert(begin(),V_SgFunctionSymbol);
		  break;
		}

	  case V_SgSupport:
		{
	       // printf ("Building the vector of variants for the SgSupport \n");
		  *this = V_Sg_File_Info              + VariantVector(V_SgAttribute)  + V_SgBaseClass       + 
			  V_SgFile                    + V_SgFunctionParameterTypeList + V_SgInitializedName + 
			  VariantVector(V_SgModifier) + V_SgName                      + V_SgOptions         +
			  V_SgProject                 + V_SgSymbolTable               + V_SgTypedefSeq      +
			  V_SgUnparse_Info;
		  insert(begin(),V_SgSupport);
		  break;
		}

	  case V_SgAttribute:
		{
	       // printf ("Building the vector of variants for the SgAttribute \n");
		  *this = VariantVector(V_SgBitAttribute) + V_SgPragma;
		  insert(begin(),V_SgAttribute);
		  break;
		}

	  case V_SgBitAttribute:
		{
	       // printf ("Building the vector of variants for the SgBitAttribute \n");
		  *this = V_SgClassDecl_attr + V_SgFuncDecl_attr;
		  insert(begin(),V_SgBitAttribute);
		  break;
		}

	  case V_SgModifier:
		{
	       // printf ("Building the vector of variants for the SgModifier \n");
		  *this = V_SgModifierNodes     + V_SgTypeModifier           + V_SgStorageModifier  +
			  V_SgAccessModifier    + V_SgConstVolatileModifier  + V_SgFunctionModifier +
			  V_SgBaseClassModifier + V_SgElaboratedTypeModifier + V_SgDeclarationModifier + 
			  V_SgLinkageModifier   + V_SgUPC_AccessModifier     + V_SgSpecialFunctionModifier;
		  insert(begin(),V_SgModifier);
		  break;
		}

	  default:
		{
	       // else just add this element to the list
		  insert(begin(),X);
		}
	}
#else

	 int stopIndex =0;
	 int i         =0;

	 this->push_back(X);

	 do{
     	 SgNode::getClassHierarchySubTreeFunction( (*this)[i], *this);
		 i++;
	 }while( i < this->size() );
#endif

  // printVariantVector(*this);
  // printf ("End of VariantVector constructor \n");
  // ROSE_ASSERT (false);
   }

VariantVector::VariantVector ( const VariantVector & X )
   {
  // printf ("Top of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector copy constructor \n");
  // printVariantVector(*this);

  // if (X.size() > 0)
     insert(end(),X.begin(),X.end());

  // printf ("Bottom of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector copy constructor \n");
  // printVariantVector(*this);
   }


	VariantVector
VariantVector::operator= ( const VariantVector & X )
   {
  // printf ("Top of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector::operator= (X) \n");
  // printVariantVector(*this);

     erase(begin(),end());
     insert(begin(),X.begin(),X.end());

  // printf ("Bottom of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector::operator= (X) \n");
  // printVariantVector(*this);

     return *this;
   }

	VariantVector
operator+ ( VariantT lhs, VariantT rhs )
   {
  // printf ("In operator+ (VariantT,VariantT) \n");
     VariantVector returnVector(lhs);
     returnVector.push_back(rhs);
     return returnVector;
   }

	VariantVector
operator+ ( VariantT lhs, const VariantVector & rhs )
   {
  // printf ("In operator+ (VariantT,VariantVector) \n");
     VariantVector returnVector(lhs);
     returnVector.insert(returnVector.end(),rhs.begin(),rhs.end());
     return returnVector;
   }


	VariantVector
operator+ ( const VariantVector & lhs, VariantT rhs )
   {
  // printf ("In operator+ (VariantVector,VariantT) \n");
     VariantVector returnVector(lhs);
     returnVector.push_back(rhs);
     return returnVector;
   }


	VariantVector
operator+ ( const VariantVector & lhs, const VariantVector & rhs )
   {
  // printf ("In operator+ (VariantVector,VariantVector) \n");
     VariantVector returnVector(lhs);
     returnVector.insert(returnVector.end(),rhs.begin(),rhs.end());
     return returnVector;
   }

// DQ (4/7/2004): Added to support more general lookup of data in the AST (vector of variants)
	void 
VariantVector::printVariantVector ( const VariantVector & X )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector

     printf ("\n\n");
     printf ("In printVariantVector: \n");
     for (vector<VariantT>::const_iterator i = X.begin(); i != X.end(); i++)
	{
	  printf ("     node %s \n",getVariantName(*i).c_str());
	}
     printf ("\n\n");
   }






namespace AstQueryNamespace{
#if 0
     void Merge(Rose_STL_Container<bool> & mergeWith, Rose_STL_Container<bool> mergeTo ){
     // mergeWith.merge(mergeTo);
        mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
     }
#endif

     void Merge(Rose_STL_Container<int> & mergeWith, Rose_STL_Container<int>  mergeTo ){
     // mergeWith.merge(mergeTo);
        mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
     }

     void Merge(Rose_STL_Container<std::string> & mergeWith, Rose_STL_Container<std::string>  mergeTo ){
     // mergeWith.merge(mergeTo);
        mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
     }

     void Merge(Rose_STL_Container<SgNode*>& mergeWith, Rose_STL_Container<SgNode*>  mergeTo ){
     // mergeWith.merge(mergeTo);
        mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
     }

     void Merge(Rose_STL_Container<SgFunctionDeclaration*>& mergeWith, Rose_STL_Container<SgFunctionDeclaration*>  mergeTo ){;
     // mergeWith.merge(mergeTo);
        mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
     }

     void Merge(void* mergeWith, void* mergeTo ){}


     
}
// DQ (12/31/2005): This is OK if not declared in a header file


template<typename AstQuerySynthesizedAttributeType>
struct testFunctionals: public std::unary_function<SgNode*,std::list<AstQuerySynthesizedAttributeType> >{
  //When elementMatchCount==1 then a match has been made
     typedef std::list<AstQuerySynthesizedAttributeType> (*roseFunctionPointerOneParameter)  (SgNode *);
     roseFunctionPointerOneParameter queryFunctionOneParameter;

     testFunctionals(roseFunctionPointerOneParameter function){
	  queryFunctionOneParameter=function;
     }
     typename std::list<AstQuerySynthesizedAttributeType>  operator()(SgNode* node) {
	  return queryFunctionOneParameter(node);
     }
};

std::list<SgNode*> queryNodeAnonymousTypedef2(SgNode* node)
   {
     std::list<SgNode*> returnList;
     ROSE_ASSERT( node     != NULL );

     SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
     if (sageTypedefDeclaration != NULL)
	  if(isSgClassType(sageTypedefDeclaration->get_base_type()))
	       returnList.push_back(node);

     return returnList;
   } /* End function:queryNodeCLassDeclarationFromName() */

struct testFunctionals2: public std::binary_function<SgNode*,SgNode*, std::list<SgNode*> >{
     int y;
     void setPred(int x){
	  y=x;
     } 
     std::list<SgNode*>  operator()(SgNode* node,SgNode* test) const{
	  std::list<SgNode*>* x;
	  return *x;
     }
};

#if 0
using namespace std;
int foo(){
     using namespace AstQueryNamespace;
     AstQuery<testFunctionals<SgNode*> > test;
     test.setPredicate(new testFunctionals<SgNode*>(&queryNodeAnonymousTypedef2));
     AstQuery<testFunctionals<bool> > test2;
     AstQuery<testFunctionals<string> > test3;
     AstQuery<std::binder2nd<testFunctionals2 > > x;
};
#endif
