
#include <fstream>
#include <string>
class VisPromelaAST
{
	public:
	VisPromelaAST(SgProject *proj);
	void generate(std::string filename);
	protected:
	SgProject * proj;
};
