// ROSE Translator to make sure that Fortran parameters come through as const
#include "rose.h"
#include <boost/foreach.hpp>

#ifdef WITH_DOT
struct ExtraNodeInfo
{
	std::string operator()(SgNode* node)
	{
//		AstDOTGenerationExtended_Defaults::ContainerSizeExtraNodeInfo cs;
		AstDOTGenerationExtended_Defaults::NamedExtraNodeInfo name;
		AstDOTGenerationExtended_Defaults::AddressExtraNodeInfo add;
		AstDOTGenerationExtended_Defaults::TypeExtraNodeInfo ty;
		AstDOTGenerationExtended_Defaults::LValueExtraNodeInfo lv;
		std::string extra;
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
#endif

int main(int argc, char* argv[])
{
	SgProject* project = frontend(argc, argv);

	std::vector<SgInitializedName*> names = SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName);
	BOOST_FOREACH(SgInitializedName* name, names)
	{
		if (name->get_qualified_name().getString() == "pi")
		{
			if (!SageInterface::isConstType(name->get_type()))
			{
				std::cerr << "Error, parameter 'pi' is not const." << std::endl;
				return 1;
			}
		}
	}



#ifdef WITH_DOT
	generateDOTExtended<ExtraNodeInfo, ExtraNodeOptions>( *project, std::string() );
#endif

	return 0;
}

