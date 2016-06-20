#include <rose.h>

#include <AstInterface_ROSE.h>
#include <GraphDotOutput.h>
#include <DefUseChain.h>


class DefUseGraphToDOT
   {
     AliasAnalysisInterface & aliasAnal;
     protected:
          DefaultDUchain graph;

     public:
          DefUseGraphToDOT (AliasAnalysisInterface & a):aliasAnal (a)
             {}

          void operator() (AstInterface & fa, SgNode * head, std::string fname)
             {
               ReachingDefinitionAnalysis reachDefAnal;
               reachDefAnal (fa, AstNodePtrImpl(head));
               graph.build (fa, reachDefAnal, aliasAnal);

               GraphDotOutput<DefaultDUchain> output (graph);
               output.writeToDOTFile (fname, "Reaching def");
             }
   };

int
main (int argc, char *argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Construct AST
     SgProject sageProject (argc, argv);

  // Create a dot data flow graph for each function declaration
     int filenum = sageProject.numberOfFiles ();
     for (int i = 0; i < filenum; ++i)
        {
       // In each file find all declarations in global scope
          SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
          SgGlobal *root = sageFile->get_globalScope();
          SgDeclarationStatementPtrList & declList = root->get_declarations ();
          for (SgDeclarationStatementPtrList::iterator p = declList.begin (); p != declList.end (); ++p)
             {
            // If the declaration statement is not defining function declaration then
            // continue to the next iterator element
               SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration (*p);
               if (funcDecl == 0)
                    continue;
               SgFunctionDefinition *funcDefn = funcDecl->get_definition ();
               if (funcDefn == 0)
                    continue;

               SgBasicBlock *stmtsInBody = funcDefn->get_body ();
            // ??
               AstInterfaceImpl faImpl (stmtsInBody);
               AstInterface fa (&faImpl);
            // Do alias analysis 
               StmtVarAliasCollect alias;
               alias (fa, AstNodePtrImpl(funcDefn));

            // Generate dot graph output for the data flow graph
               std::string name = std::string (sageFile->get_sourceFileNameWithoutPath()) + ".dot";
               DefUseGraphToDOT op (alias);
               op (fa, funcDefn, name);
             }
        }

     return 0;
   }

