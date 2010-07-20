// Author: Markus Schordan
// $Id: AstDOTGeneration.h,v 1.4 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTDOTGENERATION_H
#define ASTDOTGENERATION_H

#include <set>
#include "DOTGeneration.h"
#include "roseInternal.h"
//#include "sage3.h"

class AstDOTGeneration : public DOTGeneration<SgNode*>
   {
     public:
          void generate(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");
          void generate(SgNode* node,  std::string filename, traversalType tt = TOPDOWNBOTTOMUP,std::string filenamePostfix = "");
          void generateInputFiles(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");
          void generateWithinFile(SgFile* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");

          void writeIncidenceGraphToDOTFile(SgIncidenceDirectedGraph* graph,  const std::string& filename);
          void addAdditionalNodesAndEdges(SgNode* node);

     protected:
          virtual DOTInheritedAttribute evaluateInheritedAttribute(SgNode* node, DOTInheritedAttribute ia);
          virtual DOTSynthesizedAttribute evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l);

          std::string additionalNodeInfo(SgNode* node);

       // DQ (11/1/2003) added mechanism to add node options (to add color, etc.)
          std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add edge options (to add color, etc.)
          std::string additionalEdgeInfo(SgNode* from, SgNode* to, std::string label);

       // DQ (3/5/2007) added mechanism to add edge options (to add color, etc.)
          std::string additionalEdgeOptions(SgNode* from, SgNode* to, std::string label);

       // DQ (7/27/2008): Added support to eliminate IR nodes in DOT graphs 
       // (to tailor the presentation of information about ASTs).
          bool commentOutNodeInGraph(SgNode* node);
   };


namespace AstDOTGenerationExtended_Defaults
{
	struct NamedExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			std::ostringstream ss;

			// add namespace name
			if (SgNamespaceDeclarationStatement* n = isSgNamespaceDeclarationStatement(node))
			{
				ss << n->get_qualified_name().str() << "\\n";
			}
			// add class name
			if (SgClassDeclaration* n = isSgClassDeclaration(node))
			{
				ss << n->get_qualified_name().str() << "\\n";
			}
			// add function name
			if (SgFunctionDeclaration* n = isSgFunctionDeclaration(node))
			{
				ss << n->get_qualified_name().str() << "\\n";
			}
			if (SgFunctionRefExp* n = isSgFunctionRefExp(node))
			{
				SgFunctionDeclaration* decl = n->getAssociatedFunctionDeclaration();
				if (decl) // it's null if through a function pointer
				{
					ss << decl->get_qualified_name().str() << "\\n";
				}
			}
			// add variable name
			if (SgInitializedName* n = isSgInitializedName(node))
			{
				ss << n->get_qualified_name().str() << "\\n";
			}
			if (SgVarRefExp* n = isSgVarRefExp(node))
			{
				SgVariableSymbol* sym = n->get_symbol();
				ss << sym->get_name().getString() << "\\n";
			}
			// add variable name
			if (SgVariableSymbol* n = isSgVariableSymbol(node))
			{
				ss << n->get_name().str() << "\\n";
			}

			return ss.str();
		}
	};

	struct AddressExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			std::ostringstream ss;

			ss << node << "\\n";

			return ss.str();
		}
	};

	struct ContainerSizeExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			std::ostringstream ss;
			// print number of max successors (= container size)
			AstSuccessorsSelectors::SuccessorsContainer c;
			AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
			ss << c.size() << "\\n";

			return ss.str();
		}
	};

	struct LValueExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			std::ostringstream ss;

			// adds whether or not it is an l-value
			if (SgExpression* n = isSgExpression(node))
			{
				ss << (n->isLValue() ? "L-Value" : "!L-Value") << "\\n";
			}

			return ss.str();
		}
	};

	struct TypeExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			std::ostringstream ss;

			if (SgExpression* n = isSgExpression(node))
			{
				ss << n->get_type()->unparseToString() << "\\n";
			}

			return ss.str();
		}
	};

	struct DefaultExtraNodeInfo
	{
		std::string operator()(SgNode* node)
		{
			ContainerSizeExtraNodeInfo cs;
			NamedExtraNodeInfo name;
			AddressExtraNodeInfo add;
			return std::string("\\n") + cs(node) + name(node) + add(node);
		}
	};


	struct DefaultExtraNodeOptions
	{
		std::string operator()(SgNode* node)
		{
			return std::string();
		}
	};

	struct DefaultExtraEdgeInfo
	{
		std::string operator()(SgNode* from, SgNode* to, std::string label)
		{
			return std::string();
		}
	};

	struct DefaultExtraEdgeOptions
	{
		std::string operator()(SgNode* node, SgNode* to, std::string label)
		{
			return std::string();
		}
	};
}

// King84 (14/7/2010) added mechanism to customize node options on demand
// Note: aditionalEdgeInfo andadditionalEdgeOptions are inherited.
// Note: EdgeInfo and EdgeOptions are not used because they come into play
//       for functions in the base class which are not virtual.
template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
class AstDOTGenerationExtended : public AstDOTGeneration
{
	protected:
		ExtraNodeInfo_t eni;
		ExtraNodeOptions_t eno;
		ExtraEdgeInfo_t eei;
		ExtraEdgeOptions_t eeo;
	public:
		AstDOTGenerationExtended(ExtraNodeInfo_t eni_ = ExtraNodeInfo_t(), ExtraNodeOptions_t eno_ = ExtraNodeOptions_t(), ExtraEdgeInfo_t eei_ = ExtraEdgeInfo_t(), ExtraEdgeOptions_t eeo_ = ExtraEdgeOptions_t())
			: eni(eni_), eno(eno_), eei(eei_), eeo(eeo_)
		{ }
//		virtual DOTInheritedAttribute evaluateInheritedAttribute(SgNode* node, DOTInheritedAttribute ia);
		virtual DOTSynthesizedAttribute evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l);
};

#include "AstDOTGenerationImpl.C"

#endif
