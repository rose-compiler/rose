#include <rose.h>

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
               reachDefAnal (fa, head);
               graph.build (fa, reachDefAnal, aliasAnal);

               GraphDotOutput output (graph);
               output.writeToDOTFile (fname);
             }
   };

int
main (int argc, char *argv[])
   {
  // Construct AST
     SgProject sageProject (argc, argv);

  // Create a dot data flow graph for each function declaration
     int filenum = sageProject.numberOfFiles ();
     for (int i = 0; i < filenum; ++i)
        {
       // In each file find all declarations in global scope
          SgFile & sageFile = sageProject.get_file (i);
          SgGlobal *root = sageFile.get_root ();
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
               AstInterface fa (stmtsInBody);
            // Do alias analysis 
               StmtVarAliasCollect alias;
               alias (fa, funcDefn);

            // Generate dot graph representing the data flow graph
               std::string name = std::string (sageFile.get_sourceFileNameWithoutPath()) + ".dot";
               DefUseGraphToDOT op (alias);
               op (fa, funcDefn, name);
             }
        }

     return 0;
   }

