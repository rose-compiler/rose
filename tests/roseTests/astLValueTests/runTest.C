#include <rose.h>
#include <iostream>
#include <boost/foreach.hpp>

int main(int argc, char ** argv)
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	
	// Only run the tests for lvalue
	TestLValues test;
	test.traverseInputFiles(project, preorder);
//	AstTests::runLValueTests(project);

	bool success = true;
	std::vector<SgFunctionDeclaration*> funs = SageInterface::querySubTree<SgFunctionDeclaration>(project, V_SgFunctionDeclaration);
	BOOST_FOREACH(SgFunctionDeclaration* fun, funs)
	{
		std::string funName = fun->get_qualified_name().getString();
		// every expr statement should be an lvalue
		if (funName == "HasLValues")
		{
			Rose_STL_Container<SgStatement*> bodystmts = fun->get_definition()->get_body()->get_statements();
			BOOST_FOREACH(SgStatement* stmt, bodystmts)
			{
				if (SgExprStatement* exprstmt = isSgExprStatement(stmt))
				{
					SgExpression* expr = exprstmt->get_expression();
					if (!expr->isLValue())
					{
						std::cerr << "Error, expression `" << expr->unparseToString() << "` is not an LValue." << std::endl;
						success = false;
					}
				}
			}
		}
		// no expr statement should be an lvalue
		else if (funName == "NoLValues")
		{
			Rose_STL_Container<SgStatement*> bodystmts = fun->get_definition()->get_body()->get_statements();
			BOOST_FOREACH(SgStatement* stmt, bodystmts)
			{
				if (SgExprStatement* exprstmt = isSgExprStatement(stmt))
				{
					SgExpression* expr = exprstmt->get_expression();
					if (expr->isLValue())
					{
						std::cerr << "Error, expression `" << expr->unparseToString() << "` is an LValue." << std::endl;
						success = false;
					}
				}
			}
		}
	}
	if (!success)
		return 1;

	return backend(project);
}

