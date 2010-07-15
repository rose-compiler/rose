// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include "AstDOTGeneration.h"
#include "utility_functions.h"

struct ExtraNodeInfo
{
	std::string operator()(SgNode* node)
	{
		AstDOTGenerationExtended_Defaults::ContainerSizeExtraNodeInfo cs;
		AstDOTGenerationExtended_Defaults::NamedExtraNodeInfo name;
		AstDOTGenerationExtended_Defaults::AddressExtraNodeInfo add;
		AstDOTGenerationExtended_Defaults::LValueExtraNodeInfo lv;
		return std::string("\\n") + cs(node) + name(node) + lv(node) + add(node);
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

	// Generate a DOT file to use in visualizing the AST graph.
//	generateDOTExtended( *project, std::string(), AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );
//	generateDOTExtended( *project, std::string(), ExtraNodeInfo(), ExtraNodeOptions(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );
	generateDOTExtended<ExtraNodeInfo, ExtraNodeOptions>( *project, std::string() );

	return 0;
}

