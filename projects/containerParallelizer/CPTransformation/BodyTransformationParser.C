//****************************************************************
//   CPLUS2\PARSER_C.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997  Version 1.07
//      Eliminated redundant ";" in some places
//****************************************************************

#include "BodyTransformationCocoCodes.h"

// #include "AstNodeScanner.h"
#include <stdio.h>
#include <iostream>
#include <typeinfo>

#include "sage3.h"
#include "roseInternal.h"

#include "AstRestructure.h"

extern SgNode* currentAstNode;
extern AstRestructure* subst;

//typedef bool DerefExpAttribute;


// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;




#include "BodyTransformationParser.h"
#include "AstNodeScanner.h"

unsigned short int BodyTransformationParser::SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
  
  {0x0}
};

void BodyTransformationParser::Get()
{ do {
    Sym = Scanner->Get();
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      /* Empty Stmt */ ;
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void BodyTransformationParser::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int BodyTransformationParser::WeakSeparator(int n, int syFol, int repFol)
{ unsigned short int s[MAXSYM];
  int i;

  if (Sym == n) { Get(); return 1; }
  if (In(SymSet[repFol], Sym)) return 0;

  for (i = 0; i < MAXSYM; i++)
    s[i] = SymSet[0][i] | SymSet[syFol][i] | SymSet[repFol][i];
  GenError(n);
  while (!In(s, Sym)) Get();
  return In(SymSet[syFol], Sym);
}

void BodyTransformationParser::SgNode()
{
	if (Sym >= SgNameSym && Sym <= SgModifierNodesSym ||
	    Sym >= SgFileSym && Sym <= SgOptionsSym ||
	    Sym == SgBaseClassSym ||
	    Sym >= SgFunctionParameterTypeListSym && Sym <= SgClassDeclUnderscoreattrSym) {
		SgSupport();
	} else if (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	           Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	           Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym ||
	           Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgLocatedNode();
	} else if (Sym == SgVariableSymbolSym ||
	           Sym >= SgTemplateSymbolSym && Sym <= SgFunctionSymbolSym) {
		SgSymbol();
	} else GenError(197);
}

void BodyTransformationParser::SgSupport()
{
	switch (Sym) {
		case SgNameSym:  
			Get();
			{;
			SgName* astNode = dynamic_cast<class SgName*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgSymbolTableSym:  
			Get();
			{;
			SgSymbolTable* astNode = dynamic_cast<class SgSymbolTable*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgInitializedNameSym:  
			SgInitializedNameNT();
			break;
		case SgModifierNodesSym:  
			Get();
			{;
			SgModifierNodes* astNode = dynamic_cast<class SgModifierNodes*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFileSym:  
			SgFileNT();
			break;
		case SgProjectSym:  
			Get();
			{;
			SgProject* astNode = dynamic_cast<class SgProject*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			while (Sym == SgFileSym) {
				SgFileNT();
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgOptionsSym:  
			Get();
			{;
			SgOptions* astNode = dynamic_cast<class SgOptions*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBaseClassSym:  
			Get();
			{;
			SgBaseClass* astNode = dynamic_cast<class SgBaseClass*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgClassDeclarationNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFunctionParameterTypeListSym:  
			Get();
			{;
			SgFunctionParameterTypeList* astNode = dynamic_cast<class SgFunctionParameterTypeList*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPragmaSym: 
		case SgFuncDeclUnderscoreattrSym: 
		case SgClassDeclUnderscoreattrSym:  
			SgAttribute();
			break;
		default :GenError(198); break;
	}
}

void BodyTransformationParser::SgLocatedNode()
{
	DerefExpAttribute derefExpSpec;
	DerefExpSynAttribute derefExpSyn;
	if (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	    Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement(derefExpSpec, derefExpSyn);
	} else if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	           Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression(derefExpSpec, derefExpSyn);
	} else GenError(199);
}

void BodyTransformationParser::SgSymbol()
{
	switch (Sym) {
		case SgVariableSymbolSym:  
			Get();
			{;
			SgVariableSymbol* astNode = dynamic_cast<class SgVariableSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgTemplateSymbolSym:  
			Get();
			{;
			SgTemplateSymbol* astNode = dynamic_cast<class SgTemplateSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgTemplateDeclarationSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEnumSymbolSym:  
			Get();
			{;
			SgEnumSymbol* astNode = dynamic_cast<class SgEnumSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgEnumDeclarationSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEnumFieldSymbolSym:  
			Get();
			{;
			SgEnumFieldSymbol* astNode = dynamic_cast<class SgEnumFieldSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLabelSymbolSym:  
			Get();
			{;
			SgLabelSymbol* astNode = dynamic_cast<class SgLabelSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgLabelStatementSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDefaultSymbolSym:  
			Get();
			{;
			SgDefaultSymbol* astNode = dynamic_cast<class SgDefaultSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgTemplateInstantiationSymbolSym:  
			SgClassSymbol();
			break;
		case SgMemberFunctionSymbolSym: 
		case SgFunctionSymbolSym:  
			SgFunctionSymbolNT();
			break;
		case SgTypedefSymbolSym:  
			Get();
			{;
			SgTypedefSymbol* astNode = dynamic_cast<class SgTypedefSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgTypedefDeclarationSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(200); break;
	}
}

void BodyTransformationParser::SgInitializedNameNT()
{
	Expect(SgInitializedNameSym);
	{;
	SgInitializedName* astNode = dynamic_cast<class SgInitializedName*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	if (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	if (Sym >= SgAggregateInitializerSym && Sym <= SgAssignInitializerSym) {
		SgInitializer();
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgFileNT()
{
	Expect(SgFileSym);
	{;
	SgFile* astNode = dynamic_cast<class SgFile*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	SgGlobalNT();
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgClassDeclarationNT()
{
	if (Sym == SgClassDeclarationSym) {
		Get();
		{;
		SgClassDeclaration* astNode = dynamic_cast<class SgClassDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		if (Sym == SgTemplateInstantiationDefnSym ||
		    Sym == SgClassDefinitionSym) {
			SgClassDefinitionNT();
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgTemplateInstantiationDeclSym) {
		Get();
		{;
		SgTemplateInstantiationDecl* astNode = dynamic_cast<class SgTemplateInstantiationDecl*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		SgClassDefinitionNT();
		SgNode();
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(201);
}

void BodyTransformationParser::SgAttribute()
{
	if (Sym == SgPragmaSym) {
		Get();
		{;
		SgPragma* astNode = dynamic_cast<class SgPragma*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym >= SgFuncDeclUnderscoreattrSym && Sym <= SgClassDeclUnderscoreattrSym) {
		SgBitAttribute();
	} else GenError(202);
}

void BodyTransformationParser::SgGlobalNT()
{
	Expect(SgGlobalSym);
	{;
	SgGlobal* astNode = dynamic_cast<class SgGlobal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
	       Sym == SgClassDeclarationSym) {
		SgDeclarationStatement();
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgDeclarationStatement()
{
	switch (Sym) {
		case SgVariableDeclarationSym:  
			Get();
			{;
			SgVariableDeclaration* astNode = dynamic_cast<class SgVariableDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			while (Sym == SgInitializedNameSym) {
				SgInitializedNameNT();
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgVariableDefinitionSym:  
			Get();
			{;
			SgVariableDefinition* astNode = dynamic_cast<class SgVariableDefinition*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgInitializedNameNT();
			SgUnsignedLongValNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEnumDeclarationSym:  
			Get();
			{;
			SgEnumDeclaration* astNode = dynamic_cast<class SgEnumDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAsmStmtSym:  
			Get();
			{;
			SgAsmStmt* astNode = dynamic_cast<class SgAsmStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgTemplateDeclarationSym:  
			Get();
			{;
			SgTemplateDeclaration* astNode = dynamic_cast<class SgTemplateDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFunctionParameterListSym:  
			SgFunctionParameterListNT();
			break;
		case SgCtorInitializerListSym:  
			SgCtorInitializerListNT();
			break;
		case SgTemplateInstantiationDeclSym: 
		case SgClassDeclarationSym:  
			SgClassDeclarationNT();
			break;
		case SgMemberFunctionDeclarationSym: 
		case SgFunctionDeclarationSym:  
			SgFunctionDeclarationNT();
			break;
		case SgTypedefDeclarationSym:  
			Get();
			{;
			SgTypedefDeclaration* astNode = dynamic_cast<class SgTypedefDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			if (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
			    Sym == SgClassDeclarationSym) {
				SgDeclarationStatement();
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(203); break;
	}
}

void BodyTransformationParser::SgInitializer()
{
	DerefExpAttribute derefExpSpec;
	DerefExpSynAttribute derefExpSyn;
	if (Sym == SgAggregateInitializerSym) {
		Get();
		{;
		SgAggregateInitializer* astNode = dynamic_cast<class SgAggregateInitializer*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		if (Sym == SgExprListExpSym) {
			SgExprListExpNT(derefExpSpec, derefExpSyn);
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgConstructorInitializerSym) {
		SgConstructorInitializerNT();
	} else if (Sym == SgAssignInitializerSym) {
		Get();
		{;
		SgAssignInitializer* astNode = dynamic_cast<class SgAssignInitializer*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
		    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
			SgExpression(derefExpSpec, derefExpSyn);
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(204);
}

void BodyTransformationParser::SgBitAttribute()
{
	if (Sym == SgFuncDeclUnderscoreattrSym) {
		Get();
		{;
		SgFuncDecl_attr* astNode = dynamic_cast<class SgFuncDecl_attr*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgClassDeclUnderscoreattrSym) {
		Get();
		{;
		SgClassDecl_attr* astNode = dynamic_cast<class SgClassDecl_attr*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(205);
}

void BodyTransformationParser::SgStatement(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	switch (Sym) {
		case SgExprStatementSym:  
			Get();
			{;
			SgExprStatement* astNode = dynamic_cast<class SgExprStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLabelStatementSym:  
			Get();
			{;
			SgLabelStatement* astNode = dynamic_cast<class SgLabelStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCaseOptionStmtSym:  
			Get();
			{;
			SgCaseOptionStmt* astNode = dynamic_cast<class SgCaseOptionStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgTryStmtSym:  
			Get();
			{;
			SgTryStmt* astNode = dynamic_cast<class SgTryStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(SgCatchStatementSeqSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDefaultOptionStmtSym:  
			Get();
			{;
			SgDefaultOptionStmt* astNode = dynamic_cast<class SgDefaultOptionStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBreakStmtSym:  
			Get();
			{;
			SgBreakStmt* astNode = dynamic_cast<class SgBreakStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgContinueStmtSym:  
			Get();
			{;
			SgContinueStmt* astNode = dynamic_cast<class SgContinueStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgReturnStmtSym:  
			Get();
			{;
			SgReturnStmt* astNode = dynamic_cast<class SgReturnStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgGotoStatementSym:  
			Get();
			{;
			SgGotoStatement* astNode = dynamic_cast<class SgGotoStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgLabelStatementSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgSpawnStmtSym:  
			Get();
			{;
			SgSpawnStmt* astNode = dynamic_cast<class SgSpawnStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgFunctionCallExpNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPragmaDeclarationSym:  
			Get();
			{;
			SgPragmaDeclaration* astNode = dynamic_cast<class SgPragmaDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgPragmaSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCatchStatementSeqSym:  
			Get();
			{;
			SgCatchStatementSeq* astNode = dynamic_cast<class SgCatchStatementSeq*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
			       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
				SgStatement(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgClinkageStartStatementSym:  
			Get();
			{;
			SgClinkageStartStatement* astNode = dynamic_cast<class SgClinkageStartStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
#if 0
   // DQ (2/8/2004): These are no longer included in the SAGE III AST
		case SgIncludeDirectiveStatementSym: 
		case SgDefineDirectiveStatementSym: 
		case SgUndefDirectiveStatementSym: 
		case SgIfdefDirectiveStatementSym: 
		case SgIfndefDirectiveStatementSym: 
		case SgIfDirectiveStatementSym: 
		case SgElseDirectiveStatementSym: 
		case SgElseifDirectiveStatementSym: 
		case SgEndifDirectiveStatementSym: 
		case SgLineDirectiveStatementSym: 
		case SgErrorDirectiveStatementSym:  
			SgC_PreprocessorDirectiveStatem();
			break;
		case SgCUnderscoreStyleCommentStaSym: 
		case SgCxxStyleCommentStatementSym:  
			SgCommentStatement();
			break;
#endif
		case SgVariableDeclarationSym: 
		case SgVariableDefinitionSym: 
		case SgEnumDeclarationSym: 
		case SgAsmStmtSym: 
		case SgTypedefDeclarationSym: 
		case SgTemplateDeclarationSym: 
		case SgFunctionParameterListSym: 
		case SgCtorInitializerListSym: 
		case SgTemplateInstantiationDeclSym: 
		case SgMemberFunctionDeclarationSym: 
		case SgFunctionDeclarationSym: 
		case SgClassDeclarationSym:  
			SgDeclarationStatement();
			break;
		case SgGlobalSym: 
		case SgBasicBlockSym: 
		case SgIfStmtSym: 
		case SgForStatementSym: 
		case SgFunctionDefinitionSym: 
		case SgWhileStmtSym: 
		case SgDoWhileStmtSym: 
		case SgSwitchStatementSym: 
		case SgCatchOptionStmtSym: 
		case SgTemplateInstantiationDefnSym: 
		case SgClassDefinitionSym:  
			SgScopeStatement(derefExpSpec, derefExpSyn);
			break;
		case SgForInitStatementSym:  
			SgForInitStatementNT(derefExpSpec, derefExpSyn);
			break;
		default :GenError(206); break;
	}
}

void BodyTransformationParser::SgExpression(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	switch (Sym) {
		case SgVarRefExpSym:  
			Get();
			{;
			SgVarRefExp* astNode = dynamic_cast<class SgVarRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			
			      if(astNode->unparseToString()==iVarName) {
			        derefExpSyn.varNameMatches=true;
			        cout << "SgVarRefExp:" << astNode->unparseToString() << " : varname matches";
			      } else {
			        cout << "SgVarRefExp:" << astNode->unparseToString() << " : varname does NOT match";
			      }
			     cout << " Type:" << astNode->get_type()->unparseToString() << endl;
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgClassNameRefExpSym:  
			Get();
			{;
			SgClassNameRefExp* astNode = dynamic_cast<class SgClassNameRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFunctionRefExpSym:  
			Get();
			{;
			SgFunctionRefExp* astNode = dynamic_cast<class SgFunctionRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgMemberFunctionRefExpSym:  
			Get();
			{;
			SgMemberFunctionRefExp* astNode = dynamic_cast<class SgMemberFunctionRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			
			      /* check if deref operator of TYPE list! */ 
			      if(astNode->unparseToString()=="*") {// overloaded deref operator
			        derefExpSyn.isIteratorDeref=true; cout << "IteratorDeref:found: " << astNode->unparseToString() << endl;
			      } else {
			        cout << "IteratorDeref:NOT found: " << astNode->unparseToString() << endl;
			      };
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFunctionCallExpSym:  
			SgFunctionCallExpNT();
			break;
		case SgSizeOfOpSym:  
			Get();
			{;
			SgSizeOfOp* astNode = dynamic_cast<class SgSizeOfOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgConditionalExpSym:  
			Get();
			{;
			SgConditionalExp* astNode = dynamic_cast<class SgConditionalExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgNewExpSym:  
			Get();
			{;
			SgNewExp* astNode = dynamic_cast<class SgNewExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			if (Sym == SgExprListExpSym) {
				SgExprListExpNT(derefExpSpec, derefExpSyn);
			}
			SgConstructorInitializerNT();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDeleteExpSym:  
			Get();
			{;
			SgDeleteExp* astNode = dynamic_cast<class SgDeleteExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgThisExpSym:  
			Get();
			{;
			SgThisExp* astNode = dynamic_cast<class SgThisExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgRefExpSym:  
			Get();
			{;
			SgRefExp* astNode = dynamic_cast<class SgRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAggregateInitializerSym: 
		case SgConstructorInitializerSym: 
		case SgAssignInitializerSym:  
			SgInitializer();
			break;
		case SgBoolValExpSym: 
		case SgStringValSym: 
		case SgShortValSym: 
		case SgCharValSym: 
		case SgUnsignedCharValSym: 
		case SgWcharValSym: 
		case SgUnsignedShortValSym: 
		case SgIntValSym: 
		case SgEnumValSym: 
		case SgUnsignedIntValSym: 
		case SgLongIntValSym: 
		case SgLongLongIntValSym: 
		case SgUnsignedLongLongIntValSym: 
		case SgUnsignedLongValSym: 
		case SgFloatValSym: 
		case SgDoubleValSym: 
		case SgLongDoubleValSym:  
			SgValueExp();
			break;
		case SgArrowExpSym: 
		case SgDotExpSym: 
		case SgDotStarOpSym: 
		case SgArrowStarOpSym: 
		case SgEqualityOpSym: 
		case SgLessThanOpSym: 
		case SgGreaterThanOpSym: 
		case SgNotEqualOpSym: 
		case SgLessOrEqualOpSym: 
		case SgGreaterOrEqualOpSym: 
		case SgAddOpSym: 
		case SgSubtractOpSym: 
		case SgMultiplyOpSym: 
		case SgDivideOpSym: 
		case SgIntegerDivideOpSym: 
		case SgModOpSym: 
		case SgAndOpSym: 
		case SgOrOpSym: 
		case SgBitXorOpSym: 
		case SgBitAndOpSym: 
		case SgBitOrOpSym: 
		case SgCommaOpExpSym: 
		case SgLshiftOpSym: 
		case SgRshiftOpSym: 
		case SgPntrArrRefExpSym: 
		case SgScopeOpSym: 
		case SgAssignOpSym: 
		case SgPlusAssignOpSym: 
		case SgMinusAssignOpSym: 
		case SgAndAssignOpSym: 
		case SgIorAssignOpSym: 
		case SgMultAssignOpSym: 
		case SgDivAssignOpSym: 
		case SgModAssignOpSym: 
		case SgXorAssignOpSym: 
		case SgLshiftAssignOpSym: 
		case SgRshiftAssignOpSym:  
			SgBinaryOp(derefExpSpec, derefExpSyn);
			break;
		case SgExpressionRootSym: 
		case SgMinusOpSym: 
		case SgUnaryAddOpSym: 
		case SgNotOpSym: 
		case SgPointerDerefExpSym: 
		case SgAddressOfOpSym: 
		case SgMinusMinusOpSym: 
		case SgPlusPlusOpSym: 
		case SgBitComplementOpSym: 
		case SgCastExpSym: 
		case SgThrowOpSym:  
			SgUnaryOp(derefExpSpec, derefExpSyn);
			break;
		default :GenError(207); break;
	}
}

void BodyTransformationParser::SgExpressionRootNT()
{
	DerefExpAttribute derefExpSpec;
	DerefExpSynAttribute derefExpSyn;
	Expect(SgExpressionRootSym);
	{;
	SgExpressionRoot* astNode = dynamic_cast<class SgExpressionRoot*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgBasicBlockNT(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	Expect(SgBasicBlockSym);
	{;
	SgBasicBlock* astNode = dynamic_cast<class SgBasicBlock*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgFunctionCallExpNT()
{
	Expect(SgFunctionCallExpSym);
	{;
	SgFunctionCallExp* astNode = dynamic_cast<class SgFunctionCallExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	DerefExpAttribute derefExpSpec; derefExpSpec.withinFunctionCallExp=true;
	DerefExpSynAttribute derefExpSyn;
	Expect(LparenSym);
	if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression(derefExpSpec, derefExpSyn);
	}
	if (Sym == SgExprListExpSym) {
		SgExprListExpNT(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	if(derefExpSyn.lhsFunctionCallIsIteratorAccess && true/* ExprListExp is empty list */) {
	cout << "iterator-access found: " << astNode->unparseToString() << endl;
	subst->unparserReplace(astNode,"l[i]");
	};
	subst->unlock(astNode);
	};
}

#if 0
// DQ (2/8/2004): These are no longer included in the SAGE III AST
void BodyTransformationParser::SgC_PreprocessorDirectiveStatem()
{
	switch (Sym) {
		case SgIncludeDirectiveStatementSym:  
			Get();
			{;
			SgIncludeDirectiveStatement* astNode = dynamic_cast<class SgIncludeDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDefineDirectiveStatementSym:  
			Get();
			{;
			SgDefineDirectiveStatement* astNode = dynamic_cast<class SgDefineDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUndefDirectiveStatementSym:  
			Get();
			{;
			SgUndefDirectiveStatement* astNode = dynamic_cast<class SgUndefDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIfdefDirectiveStatementSym:  
			Get();
			{;
			SgIfdefDirectiveStatement* astNode = dynamic_cast<class SgIfdefDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIfndefDirectiveStatementSym:  
			Get();
			{;
			SgIfndefDirectiveStatement* astNode = dynamic_cast<class SgIfndefDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIfDirectiveStatementSym:  
			Get();
			{;
			SgIfDirectiveStatement* astNode = dynamic_cast<class SgIfDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgElseDirectiveStatementSym:  
			Get();
			{;
			SgElseDirectiveStatement* astNode = dynamic_cast<class SgElseDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgElseifDirectiveStatementSym:  
			Get();
			{;
			SgElseifDirectiveStatement* astNode = dynamic_cast<class SgElseifDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEndifDirectiveStatementSym:  
			Get();
			{;
			SgEndifDirectiveStatement* astNode = dynamic_cast<class SgEndifDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLineDirectiveStatementSym:  
			Get();
			{;
			SgLineDirectiveStatement* astNode = dynamic_cast<class SgLineDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgErrorDirectiveStatementSym:  
			Get();
			{;
			SgErrorDirectiveStatement* astNode = dynamic_cast<class SgErrorDirectiveStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(208); break;
	}
}
#endif

#if 0
// DQ (2/8/2004): These are no longer included in the SAGE III AST
void BodyTransformationParser::SgCommentStatement()
{
	if (Sym == SgCUnderscoreStyleCommentStaSym) {
		Get();
		{;
		SgC_StyleCommentStatement* astNode = dynamic_cast<class SgC_StyleCommentStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgCxxStyleCommentStatementSym) {
		Get();
		{;
		SgCxxStyleCommentStatement* astNode = dynamic_cast<class SgCxxStyleCommentStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(209);
}
#endif

void BodyTransformationParser::SgScopeStatement(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	switch (Sym) {
		case SgGlobalSym:  
			Get();
			{;
			SgGlobal* astNode = dynamic_cast<class SgGlobal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
			       Sym == SgClassDeclarationSym) {
				SgDeclarationStatement();
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIfStmtSym:  
			Get();
			{;
			SgIfStmt* astNode = dynamic_cast<class SgIfStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgStatement(derefExpSpec, derefExpSyn);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			if (Sym == SgBasicBlockSym) {
				SgBasicBlockNT(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgForStatementSym:  
			Get();
			{;
			SgForStatement* astNode = dynamic_cast<class SgForStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgForInitStatementNT(derefExpSpec, derefExpSyn);
			SgExpressionRootNT();
			SgExpressionRootNT();
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgWhileStmtSym:  
			Get();
			{;
			SgWhileStmt* astNode = dynamic_cast<class SgWhileStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgStatement(derefExpSpec, derefExpSyn);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDoWhileStmtSym:  
			Get();
			{;
			SgDoWhileStmt* astNode = dynamic_cast<class SgDoWhileStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgStatement(derefExpSpec, derefExpSyn);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgSwitchStatementSym:  
			Get();
			{;
			SgSwitchStatement* astNode = dynamic_cast<class SgSwitchStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			SgExpressionRootNT();
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCatchOptionStmtSym:  
			Get();
			{;
			SgCatchOptionStmt* astNode = dynamic_cast<class SgCatchOptionStmt*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(SgVariableDeclarationSym);
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgTemplateInstantiationDefnSym: 
		case SgClassDefinitionSym:  
			SgClassDefinitionNT();
			break;
		case SgFunctionDefinitionSym:  
			SgFunctionDefinitionNT();
			break;
		case SgBasicBlockSym:  
			SgBasicBlockNT(derefExpSpec, derefExpSyn);
			break;
		default :GenError(210); break;
	}
}

void BodyTransformationParser::SgForInitStatementNT(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	Expect(SgForInitStatementSym);
	{;
	SgForInitStatement* astNode = dynamic_cast<class SgForInitStatement*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgUnsignedLongValNT()
{
	Expect(SgUnsignedLongValSym);
	{;
	SgUnsignedLongVal* astNode = dynamic_cast<class SgUnsignedLongVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgFunctionParameterListNT()
{
	Expect(SgFunctionParameterListSym);
	{;
	SgFunctionParameterList* astNode = dynamic_cast<class SgFunctionParameterList*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgCtorInitializerListNT()
{
	Expect(SgCtorInitializerListSym);
	{;
	SgCtorInitializerList* astNode = dynamic_cast<class SgCtorInitializerList*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgFunctionDeclarationNT()
{
	if (Sym == SgFunctionDeclarationSym) {
		Get();
		{;
		SgFunctionDeclaration* astNode = dynamic_cast<class SgFunctionDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		SgFunctionParameterListNT();
		if (Sym == SgFunctionDefinitionSym) {
			SgFunctionDefinitionNT();
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgMemberFunctionDeclarationSym) {
		Get();
		{;
		SgMemberFunctionDeclaration* astNode = dynamic_cast<class SgMemberFunctionDeclaration*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		SgFunctionParameterListNT();
		if (Sym == SgFunctionDefinitionSym) {
			SgFunctionDefinitionNT();
		}
		SgCtorInitializerListNT();
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(211);
}

void BodyTransformationParser::SgClassDefinitionNT()
{
	if (Sym == SgClassDefinitionSym) {
		Get();
		{;
		SgClassDefinition* astNode = dynamic_cast<class SgClassDefinition*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
		       Sym == SgClassDeclarationSym) {
			SgDeclarationStatement();
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgTemplateInstantiationDefnSym) {
		Get();
		{;
		SgTemplateInstantiationDefn* astNode = dynamic_cast<class SgTemplateInstantiationDefn*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
		       Sym == SgClassDeclarationSym) {
			SgDeclarationStatement();
		}
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(212);
}

void BodyTransformationParser::SgFunctionDefinitionNT()
{
	DerefExpAttribute derefExpSpec;
	DerefExpSynAttribute derefExpSyn;
	Expect(SgFunctionDefinitionSym);
	{;
	SgFunctionDefinition* astNode = dynamic_cast<class SgFunctionDefinition*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	if (Sym == SgBasicBlockSym) {
		SgBasicBlockNT(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgExprListExpNT(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	Expect(SgExprListExpSym);
	{;
	SgExprListExp* astNode = dynamic_cast<class SgExprListExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	while (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	       Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgConstructorInitializerNT()
{
	DerefExpAttribute derefExpSpec;
	DerefExpSynAttribute derefExpSyn;
	Expect(SgConstructorInitializerSym);
	{;
	SgConstructorInitializer* astNode = dynamic_cast<class SgConstructorInitializer*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	if (Sym == SgExprListExpSym) {
		SgExprListExpNT(derefExpSpec, derefExpSyn);
	}
	Expect(RparenSym);
	subst->unlock(astNode);
	};
}

void BodyTransformationParser::SgValueExp()
{
	switch (Sym) {
		case SgBoolValExpSym:  
			Get();
			{;
			SgBoolValExp* astNode = dynamic_cast<class SgBoolValExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgStringValSym:  
			Get();
			{;
			SgStringVal* astNode = dynamic_cast<class SgStringVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgShortValSym:  
			Get();
			{;
			SgShortVal* astNode = dynamic_cast<class SgShortVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCharValSym:  
			Get();
			{;
			SgCharVal* astNode = dynamic_cast<class SgCharVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnsignedCharValSym:  
			Get();
			{;
			SgUnsignedCharVal* astNode = dynamic_cast<class SgUnsignedCharVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgWcharValSym:  
			Get();
			{;
			SgWcharVal* astNode = dynamic_cast<class SgWcharVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnsignedShortValSym:  
			Get();
			{;
			SgUnsignedShortVal* astNode = dynamic_cast<class SgUnsignedShortVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIntValSym:  
			Get();
			{;
			SgIntVal* astNode = dynamic_cast<class SgIntVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEnumValSym:  
			Get();
			{;
			SgEnumVal* astNode = dynamic_cast<class SgEnumVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnsignedIntValSym:  
			Get();
			{;
			SgUnsignedIntVal* astNode = dynamic_cast<class SgUnsignedIntVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLongIntValSym:  
			Get();
			{;
			SgLongIntVal* astNode = dynamic_cast<class SgLongIntVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLongLongIntValSym:  
			Get();
			{;
			SgLongLongIntVal* astNode = dynamic_cast<class SgLongLongIntVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnsignedLongLongIntValSym:  
			Get();
			{;
			SgUnsignedLongLongIntVal* astNode = dynamic_cast<class SgUnsignedLongLongIntVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnsignedLongValSym:  
			Get();
			{;
			SgUnsignedLongVal* astNode = dynamic_cast<class SgUnsignedLongVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgFloatValSym:  
			Get();
			{;
			SgFloatVal* astNode = dynamic_cast<class SgFloatVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDoubleValSym:  
			Get();
			{;
			SgDoubleVal* astNode = dynamic_cast<class SgDoubleVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLongDoubleValSym:  
			Get();
			{;
			SgLongDoubleVal* astNode = dynamic_cast<class SgLongDoubleVal*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(213); break;
	}
}

void BodyTransformationParser::SgBinaryOp(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	switch (Sym) {
		case SgArrowExpSym:  
			Get();
			{;
			SgArrowExp* astNode = dynamic_cast<class SgArrowExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDotExpSym:  
			Get();
			{;
			SgDotExp* astNode = dynamic_cast<class SgDotExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			bool lhsVarNameMatches=derefExpSyn.varNameMatches;
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			
			if(lhsVarNameMatches && derefExpSyn.isIteratorDeref) {
			derefExpSyn.lhsFunctionCallIsIteratorAccess=true;
			};
			subst->unlock(astNode);
			};
			break;
		case SgDotStarOpSym:  
			Get();
			{;
			SgDotStarOp* astNode = dynamic_cast<class SgDotStarOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgArrowStarOpSym:  
			Get();
			{;
			SgArrowStarOp* astNode = dynamic_cast<class SgArrowStarOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgEqualityOpSym:  
			Get();
			{;
			SgEqualityOp* astNode = dynamic_cast<class SgEqualityOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLessThanOpSym:  
			Get();
			{;
			SgLessThanOp* astNode = dynamic_cast<class SgLessThanOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgGreaterThanOpSym:  
			Get();
			{;
			SgGreaterThanOp* astNode = dynamic_cast<class SgGreaterThanOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgNotEqualOpSym:  
			Get();
			{;
			SgNotEqualOp* astNode = dynamic_cast<class SgNotEqualOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLessOrEqualOpSym:  
			Get();
			{;
			SgLessOrEqualOp* astNode = dynamic_cast<class SgLessOrEqualOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgGreaterOrEqualOpSym:  
			Get();
			{;
			SgGreaterOrEqualOp* astNode = dynamic_cast<class SgGreaterOrEqualOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAddOpSym:  
			Get();
			{;
			SgAddOp* astNode = dynamic_cast<class SgAddOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgSubtractOpSym:  
			Get();
			{;
			SgSubtractOp* astNode = dynamic_cast<class SgSubtractOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgMultiplyOpSym:  
			Get();
			{;
			SgMultiplyOp* astNode = dynamic_cast<class SgMultiplyOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDivideOpSym:  
			Get();
			{;
			SgDivideOp* astNode = dynamic_cast<class SgDivideOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIntegerDivideOpSym:  
			Get();
			{;
			SgIntegerDivideOp* astNode = dynamic_cast<class SgIntegerDivideOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgModOpSym:  
			Get();
			{;
			SgModOp* astNode = dynamic_cast<class SgModOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAndOpSym:  
			Get();
			{;
			SgAndOp* astNode = dynamic_cast<class SgAndOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgOrOpSym:  
			Get();
			{;
			SgOrOp* astNode = dynamic_cast<class SgOrOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBitXorOpSym:  
			Get();
			{;
			SgBitXorOp* astNode = dynamic_cast<class SgBitXorOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBitAndOpSym:  
			Get();
			{;
			SgBitAndOp* astNode = dynamic_cast<class SgBitAndOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBitOrOpSym:  
			Get();
			{;
			SgBitOrOp* astNode = dynamic_cast<class SgBitOrOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCommaOpExpSym:  
			Get();
			{;
			SgCommaOpExp* astNode = dynamic_cast<class SgCommaOpExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLshiftOpSym:  
			Get();
			{;
			SgLshiftOp* astNode = dynamic_cast<class SgLshiftOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgRshiftOpSym:  
			Get();
			{;
			SgRshiftOp* astNode = dynamic_cast<class SgRshiftOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPntrArrRefExpSym:  
			Get();
			{;
			SgPntrArrRefExp* astNode = dynamic_cast<class SgPntrArrRefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgScopeOpSym:  
			Get();
			{;
			SgScopeOp* astNode = dynamic_cast<class SgScopeOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAssignOpSym:  
			Get();
			{;
			SgAssignOp* astNode = dynamic_cast<class SgAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPlusAssignOpSym:  
			Get();
			{;
			SgPlusAssignOp* astNode = dynamic_cast<class SgPlusAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgMinusAssignOpSym:  
			Get();
			{;
			SgMinusAssignOp* astNode = dynamic_cast<class SgMinusAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAndAssignOpSym:  
			Get();
			{;
			SgAndAssignOp* astNode = dynamic_cast<class SgAndAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgIorAssignOpSym:  
			Get();
			{;
			SgIorAssignOp* astNode = dynamic_cast<class SgIorAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgMultAssignOpSym:  
			Get();
			{;
			SgMultAssignOp* astNode = dynamic_cast<class SgMultAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgDivAssignOpSym:  
			Get();
			{;
			SgDivAssignOp* astNode = dynamic_cast<class SgDivAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgModAssignOpSym:  
			Get();
			{;
			SgModAssignOp* astNode = dynamic_cast<class SgModAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgXorAssignOpSym:  
			Get();
			{;
			SgXorAssignOp* astNode = dynamic_cast<class SgXorAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgLshiftAssignOpSym:  
			Get();
			{;
			SgLshiftAssignOp* astNode = dynamic_cast<class SgLshiftAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgRshiftAssignOpSym:  
			Get();
			{;
			SgRshiftAssignOp* astNode = dynamic_cast<class SgRshiftAssignOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(214); break;
	}
}

void BodyTransformationParser::SgUnaryOp(DerefExpAttribute derefExpSpec, DerefExpSynAttribute& derefExpSyn)
{
	switch (Sym) {
		case SgExpressionRootSym:  
			SgExpressionRootNT();
			break;
		case SgMinusOpSym:  
			Get();
			{;
			SgMinusOp* astNode = dynamic_cast<class SgMinusOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgUnaryAddOpSym:  
			Get();
			{;
			SgUnaryAddOp* astNode = dynamic_cast<class SgUnaryAddOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgNotOpSym:  
			Get();
			{;
			SgNotOp* astNode = dynamic_cast<class SgNotOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPointerDerefExpSym:  
			Get();
			{;
			SgPointerDerefExp* astNode = dynamic_cast<class SgPointerDerefExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgAddressOfOpSym:  
			Get();
			{;
			SgAddressOfOp* astNode = dynamic_cast<class SgAddressOfOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgMinusMinusOpSym:  
			Get();
			{;
			SgMinusMinusOp* astNode = dynamic_cast<class SgMinusMinusOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgPlusPlusOpSym:  
			Get();
			{;
			SgPlusPlusOp* astNode = dynamic_cast<class SgPlusPlusOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgBitComplementOpSym:  
			Get();
			{;
			SgBitComplementOp* astNode = dynamic_cast<class SgBitComplementOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgCastExpSym:  
			Get();
			{;
			SgCastExp* astNode = dynamic_cast<class SgCastExp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression(derefExpSpec, derefExpSyn);
			}
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		case SgThrowOpSym:  
			Get();
			{;
			SgThrowOp* astNode = dynamic_cast<class SgThrowOp*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
			subst->lock(astNode);
			Expect(LparenSym);
			SgExpression(derefExpSpec, derefExpSyn);
			Expect(RparenSym);
			subst->unlock(astNode);
			};
			break;
		default :GenError(215); break;
	}
}

void BodyTransformationParser::SgClassSymbol()
{
#if 0
//	Expect(SgTemplateInstantiationSymbolSym);
	Expect(SgClassSymbolSym);
	{;
// DQ (5/7/2004): Modified to reflect removal of SgTemplateInstantiationSymbol
//	SgTemplateInstantiationSymbol* astNode = dynamic_cast<class SgTemplateInstantiationSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	SgClassSymbol* astNode = dynamic_cast<class SgClassSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
	subst->lock(astNode);
	Expect(LparenSym);
	SgClassDeclarationNT();
	Expect(RparenSym);
	subst->unlock(astNode);
	};
#endif
}

void BodyTransformationParser::SgFunctionSymbolNT()
{
	if (Sym == SgFunctionSymbolSym) {
		Get();
		{;
		SgFunctionSymbol* astNode = dynamic_cast<class SgFunctionSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		SgFunctionDeclarationNT();
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else if (Sym == SgMemberFunctionSymbolSym) {
		Get();
		{;
		SgMemberFunctionSymbol* astNode = dynamic_cast<class SgMemberFunctionSymbol*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node);
		subst->lock(astNode);
		Expect(LparenSym);
		SgFunctionDeclarationNT();
		Expect(RparenSym);
		subst->unlock(astNode);
		};
	} else GenError(216);
}



void BodyTransformationParser::Parse()
{ Scanner->Reset(); Get();
  SgNode();
}


