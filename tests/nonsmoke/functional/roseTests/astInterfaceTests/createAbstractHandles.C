/*
test code for creating abstract handles with qualified name style
This is useful to uniquely identify AST nodes during transformations.

by Liao, 3/6/2014
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "AstPDFGeneration.h"

using namespace std;
using namespace AbstractHandle;

// a global handle for the current file
//static abstract_handle* file_handle = NULL;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* n)
{
  SgLocatedNode* lnode= isSgLocatedNode(n);

  if (lnode)
  {
#if 0    
    cout<<"Creating handles for a loop construct..."<<endl;
    //Create an abstract node
    abstract_node* anode= buildroseNode(forloop);

    //Create an abstract handle from the abstract node
    //Using source position specifiers by default
    abstract_handle * ahandle = new abstract_handle(anode);
    cout<<ahandle->toString()<<endl;

    // Create handles based on numbering specifiers within the file
    abstract_handle * bhandle = new abstract_handle(anode,e_numbering,file_handle);
#endif
    AbstractHandle::abstract_handle * bhandle = SageInterface::buildAbstractHandle(lnode);
    string h_str = bhandle->toString();
    cout<< h_str <<endl;

#if 0 // this takes so long
    SgNode* grabbed_node = SageInterface::getSgNodeFromAbstractHandleString (h_str);
    if ( grabbed_node != lnode )
    {
      cout<<"Warning: node grabbed from a handle is different from the original "<<endl;
      cout<<"original node is:"<<lnode<< " of class_name="<< lnode->class_name()<<endl;
      cout<<"grabbed node  is:"<<grabbed_node << " of class_name="<< grabbed_node ->class_name() <<endl; 
      ROSE_ASSERT ( grabbed_node == lnode );
    }
#endif
  }
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  cout << "[Dumping input file's AST to a PDF for debugging ...]" << endl;
  AstPDFGeneration pdf;
//  pdf.generate(project); // takes forever
  pdf.generateInputFiles(project);

  //Generate a file handle
#if 0
  abstract_node * file_node = buildroseNode((project->get_fileList())[0]);
  file_handle = new abstract_handle(file_node);
#endif

  //Generate handles for language constructs
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);


  // A new way: using iterator to handle all nodes
  // this takes a very long time, like 5+ minutes on tux385.
  RoseAst ast(project);
  for(RoseAst::iterator i = ast.begin(); i != ast.end(); ++i)
  {
    // narrow down to template instantion member function only to save time
    //    if(SgTemplateInstantiationMemberFunctionDecl* funDef = isSgTemplateInstantiationMemberFunctionDecl(*i)){
    //      SgTemplateMemberFunctionDeclaration* funDec = funDef->get_templateDeclaration();
    if(SgFunctionDefinition* funDef = isSgFunctionDefinition(*i)){
      SgFunctionDeclaration* funDec = funDef->get_declaration();

      // test generating an abstract handle from an AST node
      //  abstract_node* anode = buildroseNode(funDec);
      //  abstract_handle ahandle(anode);
      //  using sage interface function to speedup things: internal map to avoid redundant creation.
      AbstractHandle::abstract_handle * ahandle = SageInterface::buildAbstractHandle(funDec);
      string h_str = ahandle->toString(); 
      cout<< h_str <<endl;

#if 0 // this takes so long
      // test parsing the string and find its corresponding AST node.
      // this interface function can only return the first found match.
      // For a declaration with multiple nodes (defining vs. nondefining), there may be a difference.
      // As a result, we get the first nondefining declaration and compare them.
      // Direct comparisio between two declaration nodes may fail.
      SgNode* grabbed_node = SageInterface::getSgNodeFromAbstractHandleString (h_str);
      SgFunctionDeclaration* grabbed_decl = isSgFunctionDeclaration (grabbed_node);
      if ( grabbed_decl != funDec )
      {
        cout<<"Warning: node grabbed from a handle is different from the original "<<endl;
        cout <<"original node is:"<<funDec << " of class_name="<< funDec->class_name()<<endl;
        cout <<"grabbed node  is:"<<grabbed_node <<endl; 
        //ROSE_ASSERT ( grabbed_decl-> get_firstNondefiningDeclaration() == funDec ->get_firstNondefiningDeclaration()  );
      }
#endif
    } // end if func definition
  } // end ast iterator



  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

