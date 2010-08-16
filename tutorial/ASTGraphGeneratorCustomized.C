// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include "AstDOTGeneration.h"
#include "utility_functions.h"
#include <boost/foreach.hpp>

struct ExtraNodeInfo
{
	std::string operator()(SgNode* node)
	{
//		AstDOTGenerationExtended_Defaults::ContainerSizeExtraNodeInfo cs;
		AstDOTGenerationExtended_Defaults::NamedExtraNodeInfo name;
		AstDOTGenerationExtended_Defaults::AddressExtraNodeInfo add;
		AstDOTGenerationExtended_Defaults::TypeExtraNodeInfo ty;
		AstDOTGenerationExtended_Defaults::LValueExtraNodeInfo lv;
		std::string extra = "";
		     if (SgExpression* expr = isSgExpression(node))
		{
			extra += std::string("Mods: ") +
				     (SageInterface::isConstType    (expr->get_type()) ? "c" : "") +
				     (SageInterface::isVolatileType (expr->get_type()) ? "v" : "") +
				     (SageInterface::isReferenceType(expr->get_type()) ? "&" : "") +
				     "\\n";
			extra += (expr->isDefinable() ? "Definable\\n" : "!Definable\\n");
		}
		else if (SgPointerDerefExp* expr = isSgPointerDerefExp(node))
		{
		}
		else if (SgMinusMinusOp* expr = isSgMinusMinusOp(node))
		{
		}
		else if (SgPlusPlusOp* expr = isSgPlusPlusOp(node))
		{
		}
		else if (SgCastExp* expr = isSgCastExp(node))
		{
			switch (expr->cast_type())
			{
				case SgCastExp::e_C_style_cast:     extra += "C-style\\n";          break;
				case SgCastExp::e_const_cast:       extra += "const_cast\\n";       break;
				case SgCastExp::e_static_cast:      extra += "static_cast\\n";      break;
				case SgCastExp::e_dynamic_cast:     extra += "dynamic_cast\\n";     break;
				case SgCastExp::e_reinterpret_cast: extra += "reinterpret_cast\\n"; break;
				default: break;
			}
		}
		else if (SgArrowExp* expr = isSgArrowExp(node))
		{
		}
		else if (SgDotExp* expr = isSgDotExp(node))
		{
		}
		else if (SgDotStarOp* expr = isSgDotStarOp(node))
		{
		}
		else if (SgArrowStarOp* expr = isSgArrowStarOp(node))
		{
		}
		else if (SgCommaOpExp* expr = isSgCommaOpExp(node))
		{
		}
		else if (SgPntrArrRefExp* expr = isSgPntrArrRefExp(node))
		{
		}
		else if (SgScopeOp* expr = isSgScopeOp(node))
		{
		}
		else if (SgAssignOp* expr = isSgAssignOp(node))
		{
		}
		else if (SgPlusAssignOp* expr = isSgPlusAssignOp(node))
		{
		}
		else if (SgMinusAssignOp* expr = isSgMinusAssignOp(node))
		{
		}
		else if (SgAndAssignOp* expr = isSgAndAssignOp(node))
		{
		}
		else if (SgIorAssignOp* expr = isSgIorAssignOp(node))
		{
		}
		else if (SgMultAssignOp* expr = isSgMultAssignOp(node))
		{
		}
		else if (SgDivAssignOp* expr = isSgDivAssignOp(node))
		{
		}
		else if (SgModAssignOp* expr = isSgModAssignOp(node))
		{
		}
		else if (SgXorAssignOp* expr = isSgXorAssignOp(node))
		{
		}
		else if (SgLshiftAssignOp* expr = isSgLshiftAssignOp(node))
		{
		}
		else if (SgRshiftAssignOp* expr = isSgRshiftAssignOp(node))
		{
		}
		else if (SgPointerAssignOp* expr = isSgPointerAssignOp(node))
		{
		}
		else if (SgStringVal* expr = isSgStringVal(node))
		{
		}
		else if (SgVarRefExp* expr = isSgVarRefExp(node))
		{
		}
		else if (SgFunctionRefExp* expr = isSgFunctionRefExp(node))
		{
		}
		else if (SgMemberFunctionRefExp* expr = isSgMemberFunctionRefExp(node))
		{
		}
		else if (SgTypeIdOp* expr = isSgTypeIdOp(node))
		{
		}
		else if (SgConditionalExp* expr = isSgConditionalExp(node))
		{
		}

		else if (SgInitializedName* expr = isSgInitializedName(node))
		{
			extra += expr->get_type()->unparseToString() + "\\n";
		}
		return std::string("\\n") + name(node) + ty(node) + lv(node) + extra + add(node);
	}
};

struct ExtraNodeOptions
{
	std::string operator()(SgNode* node)
	{
		AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions eno;
		return eno(node);
	}
};

int main( int argc, char * argv[] )
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(project);
	if (mainDecl)
	{
		SgBasicBlock* body = mainDecl->get_definition()->get_body();
		std::vector<SgReferenceType*> referenceTypes = SageInterface::querySubTree<SgReferenceType>(body, V_SgReferenceType);
		
		BOOST_FOREACH(SgReferenceType* type, referenceTypes)
		{
			printf("%s\n", type->unparseToString().c_str());
		}

		BOOST_FOREACH(SgStatement* stmt, body->get_statements())
		{
			std::vector<SgReferenceType*> referenceTypes = SageInterface::querySubTree<SgReferenceType>(stmt, V_SgReferenceType);
			
			BOOST_FOREACH(SgReferenceType* type, referenceTypes)
			{
				printf("%d: %s : %s\n", stmt->get_file_info()->get_line(), stmt->unparseToString().c_str(), type->unparseToString().c_str());
			}
		}
	}

	// Generate a DOT file to use in visualizing the AST graph.
//	generateDOTExtended( *project, std::string(), AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );
//	generateDOTExtended( *project, std::string(), ExtraNodeInfo(), ExtraNodeOptions(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );
	generateDOTExtended<ExtraNodeInfo, ExtraNodeOptions>( *project, std::string() );

	return 0;
}

