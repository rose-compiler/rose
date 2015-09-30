
#include "rose.h"
#include <fstream>
#include "AstProcessing.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void ReplaceAst( SgNode* parent, SgNode* orig, SgNode *n)
    {
      SgStatement *s = isSgStatement(orig), *ns = isSgStatement(n);
      if (s != 0) {
            SgStatement *p = isSgStatement( parent);
            assert(p != 0);
            p->replace_statement(s, ns);
      }
      else {
         SgExpression *e = isSgExpression(orig), *ne = isSgExpression(n);
         assert(e != 0 && ne != 0);
         SgExpression *p = isSgExpression(parent);
         assert(p != 0);
         p->replace_expression(e, ne);
      }
    }

void InterleaveSgTree( SgNode *r1, SgNode *r2)
{
    vector<SgNode*> children1 = r1->get_traversalSuccessorContainer() ;
    vector<SgNode*> children2 = r2->get_traversalSuccessorContainer() ;
    assert(children1.size() == children2.size());
    for (unsigned int i = 0; i < children1.size(); ++i) {
       ReplaceAst( r1, children1[i], children2[i]); 
       ReplaceAst( r2, children2[i], children1[i]); 
    } 
}

bool TestCopyRepl( SgProject & proj)
{
  int filenum = proj.numberOfFiles();

  for (int i = 0; i < filenum; ++i) {
  // SgFile &file = proj.get_file(i);
     SgSourceFile* file = isSgSourceFile(proj.get_fileList()[i]);
     SgGlobal *root = file->get_globalScope();
     SgTreeCopy tc;
     SgGlobal *rootCopy = static_cast<SgGlobal*>(root->copy(tc));

     
     file->unparse();

  // DQ (101/17/2005): Changed interface to use C++ strings
  // char *outname = file.get_unparse_output_filename();
  // string move1 = "mv " + string(outname) + " orig_" + string(outname);
     string outname = file->get_unparse_output_filename();
     string move1 = "mv " + outname + " orig_" + outname;
     bool fail = system(move1.c_str());
     assert(!fail);

     file->set_globalScope(rootCopy);
     file->unparse();
  // string move2 = "mv " + string(outname) + " copy_" + string(outname);
     string move2 = "mv " + outname + " copy_" + outname;
     fail = system(move2.c_str());
     assert(!fail);

  // string compare = "diff orig_" + string(outname) + " copy_" + string(outname) + "> diffout";
     string compare = "diff orig_" + outname + " copy_" + outname + "> diffout";
     fail = system(compare.c_str());
     assert(!fail);
 
     ifstream in("diffout");
     char c;
     in >> c;
     if (!in.eof()) 
        return false;

     InterleaveSgTree( root, rootCopy);
     file->unparse();
     string move3 = "mv " + outname + " repl_" + outname;
     fail = system(move3.c_str());
     assert(!fail);

     string compare2 = "diff orig_" + outname + " repl_" + outname + "> diffout2";
     fail = system(compare2.c_str());
     assert(!fail);

     ifstream in2("diffout2");
     in2 >> c;
     if (in2.eof()) {
        string cleanup = "rm orig_" + outname + "; rm copy_" + outname
                  + "; rm repl_" + outname + "; rm diffout; ";
        system(cleanup.c_str());
     }
     else
        return false;

     // delete root; 
  }
  return true;
}

int main ( int argc, char * argv[] )
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // Declare usage (if incorrect number of inputs):
     if (argc == 1)
        {
       // Print usage and exit with exit status == 1
          rose::usage (1);
        }

     SgProject sageProject (argc,argv);
     int EDG_FrontEndErrorCode = sageProject.get_frontendErrorCode();
     
  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);


     bool succ = TestCopyRepl( sageProject );
     if (succ)
         cerr << "successful test \n";
     else
         cerr << "failed test \n";

     return !succ;
   }

