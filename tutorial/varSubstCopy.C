#include <rose.h>

class SgVarSubstCopy : public SgCopyHelp
   {
     SgVariableSymbol *sourceSymbol;
     SgExpression *replacementExpression;

     public:

     SgVarSubstCopy(SgVariableSymbol *sourceSymbol, SgExpression *replacementExpression)
             : sourceSymbol(sourceSymbol), replacementExpression(replacementExpression)
                {}

     SgNode *copyAst(const SgNode *n)
        {
          if (const SgVarRefExp *vr = isSgVarRefExp(const_cast<SgNode *>(n)))
             {
               if (vr->get_symbol() == sourceSymbol)
                  {
                    return replacementExpression->copy(*this);
                  }
             }
          return n->copy(*this);
        }

   };

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

int main(int argc, char **argv)
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject *project = frontend(argc, argv);

  // Find the exampleFunction function
     SgGlobal *global = project->get_file(0).get_globalScope();
     SgFunctionSymbol *exampleFunctionSym = global->lookup_function_symbol("exampleFunction");
     ROSE_ASSERT(exampleFunctionSym != NULL);
     SgFunctionDeclaration *exampleFunctionDecl = exampleFunctionSym->get_declaration();
     ROSE_ASSERT(exampleFunctionDecl != NULL);
     SgFunctionDefinition *exampleFunctionDef = exampleFunctionDecl->get_definition();
     ROSE_ASSERT(exampleFunctionDef != NULL);

  // Find its first parameter
     SgInitializedName *firstParamName = exampleFunctionDecl->get_args().front();
     ROSE_ASSERT(firstParamName != NULL);
     SgVariableSymbol *firstParamSym = exampleFunctionDef->lookup_var_symbol(firstParamName->get_name());
     ROSE_ASSERT(firstParamSym != NULL);

  // Construct the expression to substitute for
     SgIntVal *twenty = new SgIntVal(SgNULL_FILE, 20);

  // Create our copy help mechanism with the required parameters
     SgVarSubstCopy ourCopyHelp(firstParamSym, twenty);

  // Do the copy
     SgNode *exampleFunctionDeclCopyNode = exampleFunctionDecl->copy(ourCopyHelp);
     ROSE_ASSERT(exampleFunctionDeclCopyNode != NULL);
     SgFunctionDeclaration *exampleFunctionDeclCopy = isSgFunctionDeclaration(exampleFunctionDeclCopyNode);
     ROSE_ASSERT(exampleFunctionDeclCopy != NULL);

  // Change the name of the new function
     exampleFunctionDeclCopy->set_name("exampleFunction20");
     global->get_symbol_table()->insert(exampleFunctionDeclCopy->get_name(), new SgFunctionSymbol(exampleFunctionDeclCopy));

  // Add function to global scope
     global->append_declaration(exampleFunctionDeclCopy);

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }
