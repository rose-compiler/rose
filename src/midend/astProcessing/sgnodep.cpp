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

#include "sgnodec.hpp"

// #include "sgnodes.hpp"
#include <stdio.h>
#include <iostream>
#include <typeinfo>

#include "sage3.h"
#include "roseInternal.h"

extern SgNode* currentAstNode;



#include "sgnodep.hpp"
#include "sgnodes.hpp"

unsigned short int sgnodeParser::SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
  
  {0x0}
};

void sgnodeParser::Get()
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

void sgnodeParser::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int sgnodeParser::WeakSeparator(int n, int syFol, int repFol)
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

void sgnodeParser::SgNode()
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

void sgnodeParser::SgSupport()
{
	switch (Sym) {
		case SgNameSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgSymbolTableSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgInitializedNameSym:  
			SgInitializedNameNT();
			break;
		case SgModifierNodesSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgFileSym:  
			SgFileNT();
			break;
		case SgProjectSym:  
			Get();
			Expect(LparenSym);
			while (Sym == SgFileSym) {
				SgFileNT();
			}
			Expect(RparenSym);
			break;
		case SgOptionsSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgBaseClassSym:  
			Get();
			Expect(LparenSym);
			SgClassDeclarationNT();
			Expect(RparenSym);
			break;
		case SgFunctionParameterTypeListSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
   // DQ (3/13/2004): Added case to support template parameters
		case SgTemplateParameterSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
   // DQ (3/13/2004): Added case to support template parameters
		case SgTemplateArgumentSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgPragmaSym: 
		case SgFuncDeclUnderscoreattrSym: 
		case SgClassDeclUnderscoreattrSym:  
			SgAttribute();
			break;
		default :GenError(198); break;
	}
}

void sgnodeParser::SgLocatedNode()
{
	if (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	    Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement();
	} else if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	           Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression();
	} else GenError(199);
}

void sgnodeParser::SgSymbol()
{
	switch (Sym) {
		case SgVariableSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgTemplateSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(SgTemplateDeclarationSym);
			Expect(RparenSym);
			break;
		case SgEnumSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(SgEnumDeclarationSym);
			Expect(RparenSym);
			break;
		case SgEnumFieldSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgLabelSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(SgLabelStatementSym);
			Expect(RparenSym);
			break;
		case SgDefaultSymbolSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
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
			Expect(LparenSym);
			Expect(SgTypedefDeclarationSym);
			Expect(RparenSym);
			break;
		default :GenError(200); break;
	}
}

void sgnodeParser::SgInitializedNameNT()
{
	Expect(SgInitializedNameSym);
	Expect(LparenSym);
	if (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	if (Sym >= SgAggregateInitializerSym && Sym <= SgAssignInitializerSym) {
		SgInitializer();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgFileNT()
{
	Expect(SgFileSym);
	Expect(LparenSym);
	SgGlobalNT();
	Expect(RparenSym);
}

void sgnodeParser::SgClassDeclarationNT()
{
	if (Sym == SgClassDeclarationSym) {
		Get();
		Expect(LparenSym);
		if (Sym == SgTemplateInstantiationDefnSym ||
		    Sym == SgClassDefinitionSym) {
			SgClassDefinitionNT();
		}
		Expect(RparenSym);
	} else if (Sym == SgTemplateInstantiationDeclSym) {
		Get();
		Expect(LparenSym);
		SgClassDefinitionNT();
		SgNode();
		Expect(RparenSym);
	} else GenError(201);
}

void sgnodeParser::SgAttribute()
{
	if (Sym == SgPragmaSym) {
		Get();
		Expect(LparenSym);
		Expect(RparenSym);
	} else if (Sym >= SgFuncDeclUnderscoreattrSym && Sym <= SgClassDeclUnderscoreattrSym) {
		SgBitAttribute();
	} else GenError(202);
}

void sgnodeParser::SgGlobalNT()
{
	Expect(SgGlobalSym);
	Expect(LparenSym);
	while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
	       Sym == SgClassDeclarationSym) {
		SgDeclarationStatement();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgDeclarationStatement()
{
	switch (Sym) {
		case SgVariableDeclarationSym:  
			Get();
			Expect(LparenSym);
			while (Sym == SgInitializedNameSym) {
				SgInitializedNameNT();
			}
			Expect(RparenSym);
			break;
		case SgVariableDefinitionSym:  
			Get();
			Expect(LparenSym);
			SgInitializedNameNT();
			SgUnsignedLongValNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgEnumDeclarationSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgAsmStmtSym:  
			Get();
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgTemplateDeclarationSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
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
			Expect(LparenSym);
			if (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
			    Sym == SgClassDeclarationSym) {
				SgDeclarationStatement();
			}
			Expect(RparenSym);
			break;
		default :GenError(203); break;
	}
}

void sgnodeParser::SgInitializer()
{
	if (Sym == SgAggregateInitializerSym) {
		Get();
		Expect(LparenSym);
		if (Sym == SgExprListExpSym) {
			SgExprListExpNT();
		}
		Expect(RparenSym);
	} else if (Sym == SgConstructorInitializerSym) {
		SgConstructorInitializerNT();
	} else if (Sym == SgAssignInitializerSym) {
		Get();
		Expect(LparenSym);
		if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
		    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
			SgExpression();
		}
		Expect(RparenSym);
	} else GenError(204);
}

void sgnodeParser::SgBitAttribute()
{
	if (Sym == SgFuncDeclUnderscoreattrSym) {
		Get();
		Expect(LparenSym);
		Expect(RparenSym);
	} else if (Sym == SgClassDeclUnderscoreattrSym) {
		Get();
		Expect(LparenSym);
		Expect(RparenSym);
	} else GenError(205);
}

void sgnodeParser::SgStatement()
{
	switch (Sym) {
		case SgExprStatementSym:  
			Get();
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgLabelStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgCaseOptionStmtSym:  
			Get();
			Expect(LparenSym);
			SgBasicBlockNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgTryStmtSym:  
			Get();
			Expect(LparenSym);
			SgBasicBlockNT();
			Expect(SgCatchStatementSeqSym);
			Expect(RparenSym);
			break;
		case SgDefaultOptionStmtSym:  
			Get();
			Expect(LparenSym);
			SgBasicBlockNT();
			Expect(RparenSym);
			break;
		case SgBreakStmtSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgContinueStmtSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgReturnStmtSym:  
			Get();
			Expect(LparenSym);
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgGotoStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(SgLabelStatementSym);
			Expect(RparenSym);
			break;
		case SgSpawnStmtSym:  
			Get();
			Expect(LparenSym);
			SgFunctionCallExpNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgPragmaDeclarationSym:  
			Get();
			Expect(LparenSym);
			Expect(SgPragmaSym);
			Expect(RparenSym);
			break;
		case SgCatchStatementSeqSym:  
			Get();
			Expect(LparenSym);
			while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
			       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
				SgStatement();
			}
			Expect(RparenSym);
			break;
		case SgClinkageStartStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
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
			SgScopeStatement();
			break;
		case SgForInitStatementSym:  
			SgForInitStatementNT();
			break;
		default :GenError(206); break;
	}
}

void sgnodeParser::SgExpression()
{
	switch (Sym) {
		case SgVarRefExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgClassNameRefExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgFunctionRefExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgMemberFunctionRefExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgFunctionCallExpSym:  
			SgFunctionCallExpNT();
			break;
		case SgSizeOfOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgConditionalExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgNewExpSym:  
			Get();
			Expect(LparenSym);
			if (Sym == SgExprListExpSym) {
				SgExprListExpNT();
			}
			SgConstructorInitializerNT();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgDeleteExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgThisExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgRefExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
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
			SgBinaryOp();
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
			SgUnaryOp();
			break;
		default :GenError(207); break;
	}
}

void sgnodeParser::SgExpressionRootNT()
{
	Expect(SgExpressionRootSym);
	Expect(LparenSym);
	if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgBasicBlockNT()
{
	Expect(SgBasicBlockSym);
	Expect(LparenSym);
	while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgFunctionCallExpNT()
{
	Expect(SgFunctionCallExpSym);
	Expect(LparenSym);
	if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression();
	}
	if (Sym == SgExprListExpSym) {
		SgExprListExpNT();
	}
	Expect(RparenSym);
}

#if 0
   // DQ (2/8/2004): These are no longer included in the SAGE III AST
void sgnodeParser::SgC_PreprocessorDirectiveStatem()
{
	switch (Sym) {
		case SgIncludeDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgDefineDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUndefDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgIfdefDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgIfndefDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgIfDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgElseDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgElseifDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgEndifDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgLineDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgErrorDirectiveStatementSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		default :GenError(208); break;
	}
}
#endif

#if 0
   // DQ (2/8/2004): These are no longer included in the SAGE III AST
void sgnodeParser::SgCommentStatement()
{
	if (Sym == SgCUnderscoreStyleCommentStaSym) {
		Get();
		Expect(LparenSym);
		Expect(RparenSym);
	} else if (Sym == SgCxxStyleCommentStatementSym) {
		Get();
		Expect(LparenSym);
		Expect(RparenSym);
	} else GenError(209);
}
#endif

void sgnodeParser::SgScopeStatement()
{
	switch (Sym) {
		case SgGlobalSym:  
			Get();
			Expect(LparenSym);
			while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
			       Sym == SgClassDeclarationSym) {
				SgDeclarationStatement();
			}
			Expect(RparenSym);
			break;
		case SgIfStmtSym:  
			Get();
			Expect(LparenSym);
			SgStatement();
			SgBasicBlockNT();
			if (Sym == SgBasicBlockSym) {
				SgBasicBlockNT();
			}
			Expect(RparenSym);
			break;
		case SgForStatementSym:  
			Get();
			Expect(LparenSym);
			SgForInitStatementNT();
			SgExpressionRootNT();
			SgExpressionRootNT();
			SgBasicBlockNT();
			Expect(RparenSym);
			break;
		case SgWhileStmtSym:  
			Get();
			Expect(LparenSym);
			SgStatement();
			SgBasicBlockNT();
			Expect(RparenSym);
			break;
		case SgDoWhileStmtSym:  
			Get();
			Expect(LparenSym);
			SgStatement();
			SgBasicBlockNT();
			Expect(RparenSym);
			break;
		case SgSwitchStatementSym:  
			Get();
			Expect(LparenSym);
			SgBasicBlockNT();
			SgExpressionRootNT();
			Expect(RparenSym);
			break;
		case SgCatchOptionStmtSym:  
			Get();
			Expect(LparenSym);
			Expect(SgVariableDeclarationSym);
			SgBasicBlockNT();
			Expect(RparenSym);
			break;
		case SgTemplateInstantiationDefnSym: 
		case SgClassDefinitionSym:  
			SgClassDefinitionNT();
			break;
		case SgFunctionDefinitionSym:  
			SgFunctionDefinitionNT();
			break;
		case SgBasicBlockSym:  
			SgBasicBlockNT();
			break;
		default :GenError(210); break;
	}
}

void sgnodeParser::SgForInitStatementNT()
{
	Expect(SgForInitStatementSym);
	Expect(LparenSym);
	while (Sym >= SgExprStatementSym && Sym <= SgTemplateInstantiationDefnSym ||
	       Sym >= SgClassDeclarationSym && Sym <= SgClassDefinitionSym) {
		SgStatement();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgUnsignedLongValNT()
{
	Expect(SgUnsignedLongValSym);
	Expect(LparenSym);
	Expect(RparenSym);
}

void sgnodeParser::SgFunctionParameterListNT()
{
	Expect(SgFunctionParameterListSym);
	Expect(LparenSym);
	while (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgCtorInitializerListNT()
{
	Expect(SgCtorInitializerListSym);
	Expect(LparenSym);
	while (Sym == SgInitializedNameSym) {
		SgInitializedNameNT();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgFunctionDeclarationNT()
{
	if (Sym == SgFunctionDeclarationSym) {
		Get();
		Expect(LparenSym);
		SgFunctionParameterListNT();
		if (Sym == SgFunctionDefinitionSym) {
			SgFunctionDefinitionNT();
		}
		Expect(RparenSym);
	} else if (Sym == SgMemberFunctionDeclarationSym) {
		Get();
		Expect(LparenSym);
		SgFunctionParameterListNT();
		if (Sym == SgFunctionDefinitionSym) {
			SgFunctionDefinitionNT();
		}
		SgCtorInitializerListNT();
		Expect(RparenSym);
	} else GenError(211);
}

void sgnodeParser::SgClassDefinitionNT()
{
	if (Sym == SgClassDefinitionSym) {
		Get();
		Expect(LparenSym);
		while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
		       Sym == SgClassDeclarationSym) {
			SgDeclarationStatement();
		}
		Expect(RparenSym);
	} else if (Sym == SgTemplateInstantiationDefnSym) {
		Get();
		Expect(LparenSym);
		while (Sym >= SgVariableDeclarationSym && Sym <= SgFunctionDeclarationSym ||
		       Sym == SgClassDeclarationSym) {
			SgDeclarationStatement();
		}
		Expect(RparenSym);
	} else GenError(212);
}

void sgnodeParser::SgFunctionDefinitionNT()
{
	Expect(SgFunctionDefinitionSym);
	Expect(LparenSym);
	if (Sym == SgBasicBlockSym) {
		SgBasicBlockNT();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgExprListExpNT()
{
	Expect(SgExprListExpSym);
	Expect(LparenSym);
	while (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
	       Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
		SgExpression();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgConstructorInitializerNT()
{
	Expect(SgConstructorInitializerSym);
	Expect(LparenSym);
	if (Sym == SgExprListExpSym) {
		SgExprListExpNT();
	}
	Expect(RparenSym);
}

void sgnodeParser::SgValueExp()
{
	switch (Sym) {
		case SgBoolValExpSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgStringValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgShortValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgCharValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUnsignedCharValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgWcharValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUnsignedShortValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgIntValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgEnumValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUnsignedIntValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgLongIntValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgLongLongIntValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUnsignedLongLongIntValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgUnsignedLongValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgFloatValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgDoubleValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		case SgLongDoubleValSym:  
			Get();
			Expect(LparenSym);
			Expect(RparenSym);
			break;
		default :GenError(213); break;
	}
}

void sgnodeParser::SgBinaryOp()
{
	switch (Sym) {
		case SgArrowExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgDotExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgDotStarOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgArrowStarOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgEqualityOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgLessThanOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgGreaterThanOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgNotEqualOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgLessOrEqualOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgGreaterOrEqualOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgAddOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgSubtractOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgMultiplyOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgDivideOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgIntegerDivideOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgModOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgAndOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgOrOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgBitXorOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgBitAndOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgBitOrOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgCommaOpExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgLshiftOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgRshiftOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgPntrArrRefExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgScopeOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgPlusAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgMinusAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgAndAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgIorAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgMultAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgDivAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgModAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgXorAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgLshiftAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		case SgRshiftAssignOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			SgExpression();
			Expect(RparenSym);
			break;
		default :GenError(214); break;
	}
}

void sgnodeParser::SgUnaryOp()
{
	switch (Sym) {
		case SgExpressionRootSym:  
			SgExpressionRootNT();
			break;
		case SgMinusOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgUnaryAddOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgNotOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgPointerDerefExpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgAddressOfOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgMinusMinusOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgPlusPlusOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgBitComplementOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		case SgCastExpSym:  
			Get();
			Expect(LparenSym);
			if (Sym >= SgVarRefExpSym && Sym <= SgSizeOfOpSym ||
			    Sym >= SgConditionalExpSym && Sym <= SgThrowOpSym) {
				SgExpression();
			}
			Expect(RparenSym);
			break;
		case SgThrowOpSym:  
			Get();
			Expect(LparenSym);
			SgExpression();
			Expect(RparenSym);
			break;
		default :GenError(215); break;
	}
}

void sgnodeParser::SgClassSymbol()
{
	Expect(SgTemplateInstantiationSymbolSym);
	Expect(LparenSym);
	SgClassDeclarationNT();
	Expect(RparenSym);
}

void sgnodeParser::SgFunctionSymbolNT()
{
	if (Sym == SgFunctionSymbolSym) {
		Get();
		Expect(LparenSym);
		SgFunctionDeclarationNT();
		Expect(RparenSym);
	} else if (Sym == SgMemberFunctionSymbolSym) {
		Get();
		Expect(LparenSym);
		SgFunctionDeclarationNT();
		Expect(RparenSym);
	} else GenError(216);
}



void sgnodeParser::Parse()
{ Scanner->Reset(); Get();
  SgNode();
}


