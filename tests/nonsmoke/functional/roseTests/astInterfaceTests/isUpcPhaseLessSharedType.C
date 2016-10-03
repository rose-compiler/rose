/*
 * Test if a variable declaration's type is a phase-less UPC shared type
 *  Also test isUpcPrivateToSharedType()
 * Liao, 7/1/2008
 */
#include "rose.h" 
#include <iostream>
int counter = 0;
int counter2 = 0;

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
          if (isUpcPhaseLessSharedType(t))
          {
       //   cout<<"a phase-less declaration: "<< 
        //  isSgVariableDeclaration(n)->unparseToString()<<endl;
            counter ++;
          }
          if (isUpcPrivateToSharedType(t)) counter2++;
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
 std::cout <<"Found "<<counter<<" phase-less shared declarations." <<std::endl;
 std::cout <<"Found "<<counter2<<" private-to-shared declarations." <<std::endl;
 int rt=  backend(project); // we want to see the unparse code even when the assertion fails.
 ROSE_ASSERT((counter==5)&&(counter2==1));
  return rt;
}
