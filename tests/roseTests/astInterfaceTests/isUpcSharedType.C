/*
 * Test if a variable declaration's type has a UPC shared modifier
 *
 * Liao, 7/1/2008
 */
#include "rose.h" 
#include <iostream>
int counter = 0;

using namespace SageInterface;
using namespace std;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    void virtual visit (SgNode* n)
    {
    if (isSgVariableDeclaration(n)!=NULL){
//        cout<<"Found SgVariable declaration !"<<endl;
        SgType* t = SageInterface::getFirstVarSym((isSgVariableDeclaration(n)))->get_type();
        if (SageInterface::hasUpcSharedType(t)) 
        {
          counter ++;
        }
      } //
    } //visit()
}; // class

int main(int argc, char* argv[])
{
  SgProject* project = frontend(argc, argv);
  ROSE_ASSERT(project !=NULL);
  visitorTraversal exampleTraversal;
  exampleTraversal.traverse(project,preorder);
 std::cout <<"Found "<<counter<<" shared declarations." <<std::endl;
 ROSE_ASSERT(counter==6);
  return backend(project);
}
