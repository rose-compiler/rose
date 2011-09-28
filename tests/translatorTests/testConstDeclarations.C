// This is a translator that demonstrates (or demonstrated) a bug in ROSE.
// This is is provided as part of regression tests on translators that 
// demonstrate bugs in ROSE and is different from the tests/CompileTests
// directory which demonstrates input codes that demonstrate bugs in ROSE.

#include "rose.h"
#include <boost/foreach.hpp>

// using namespace SageBuilder;
// using namespace SageInterface;

using namespace std;

int
main(int argc, char* argv[])
   {
     SgProject* project = frontend(argc, argv);

     std::vector<SgIfStmt*> ifs = SageInterface::querySubTree<SgIfStmt>(project, V_SgIfStmt);
     BOOST_FOREACH(SgIfStmt* if_stmt, ifs)
        {
          if (SgExpression *se = isSgExprStatement(if_stmt->get_conditional())->get_expression())
             {
               cout << "--->" << se->unparseToString() << "<---" << endl;

               Rose_STL_Container<SgNode*> variableList = NodeQuery::querySubTree(se, V_SgVarRefExp);
               for (Rose_STL_Container<SgNode*>::iterator i = variableList.begin(), end = variableList.end(); i != end; i++)
                  {
                    SgVarRefExp *varRef = isSgVarRefExp(*i);
                    SgVariableSymbol *currSym = varRef->get_symbol();
                    cout << "Looking at: --|" << currSym->get_name().str() << "|--" << endl;

                    SgDeclarationStatement *decl = currSym->get_declaration()->get_declaration();

                    cout << "declaration: " << decl->unparseToString() << endl;

                    SgConstVolatileModifier cvm = decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier();
                    bool constness = cvm.isConst();

                    cout << "constness via isConst(): " << constness << endl;
                    cout << cvm.displayString() << endl;
                  }
             }
        }

     return 0;
   }

