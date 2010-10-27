#include "rose.h"
using namespace std;
// Must use memory pool traversal here
// AstSimple traversal will skip types
class visitorTraversal:public ROSE_VisitTraversal
{ 
  public:
    virtual void visit (SgNode * n);
};

#define GET_QUALIFIED_NAME(suffix) \
  Sg##suffix * target = isSg##suffix(n); \
  cout<< target->class_name()<<endl; \
  cout<<"\tunparsed string is "<< target->unparseToString()<<endl; \
  cout<<"\tqualified name is "<< target->get_qualified_name().getString()  <<endl;

void
visitorTraversal::visit (SgNode * n)
{
  ROSE_ASSERT (n != NULL);
  switch (n->variantT())
  { 
    //some of  SgScopeStatement
    case V_SgGlobal:
    case V_SgBasicBlock:
    case V_SgNamespaceDefinitionStatement:
    case V_SgClassDefinition:
    case V_SgFunctionDefinition:
    case V_SgTemplateInstantiationDefn:
      {
        GET_QUALIFIED_NAME(ScopeStatement)
        break;
      }
      //Some of SgDeclarationStatement
    case V_SgFunctionDeclaration:
      {
        GET_QUALIFIED_NAME(FunctionDeclaration)
        break;
      }
    case V_SgClassDeclaration:
      {
        GET_QUALIFIED_NAME(ClassDeclaration)
        break;
      }  
    case V_SgEnumDeclaration:
      {
        GET_QUALIFIED_NAME(EnumDeclaration)
        break;
      }
    case V_SgNamespaceDeclarationStatement:
      {
        GET_QUALIFIED_NAME(NamespaceDeclarationStatement)
        break;
      }
    case V_SgTemplateDeclaration:
      {
        GET_QUALIFIED_NAME(TemplateDeclaration)
        break;
      }
    case V_SgTemplateInstantiationDecl:
      {
        GET_QUALIFIED_NAME(TemplateInstantiationDecl)
        break;
      }
    case V_SgMemberFunctionDeclaration:
      {
        GET_QUALIFIED_NAME(MemberFunctionDeclaration)
        break;
      }
    case V_SgTypedefDeclaration:
      {
        GET_QUALIFIED_NAME(TypedefDeclaration)
        break;
      }
      // special case here 
    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration * target = isSgVariableDeclaration(n);
        SgQualifiedNamePtrList & n_list = target-> get_qualifiedNameList ();
        SgQualifiedNamePtrList::const_iterator iter = n_list.begin();
        for (; iter!=n_list.end(); iter++)
        {
          SgQualifiedName * q_name = *iter;
          ROSE_ASSERT (q_name != NULL);
          cout<< target->class_name()<<endl;
          cout<<"\tunparsed string is "<< target->unparseToString()<<endl;
        }
        break;
      }
      //Others   
    case V_SgInitializedName:
      {
        GET_QUALIFIED_NAME(InitializedName)
        break;
      }
    case V_SgNamedType:
      {
        GET_QUALIFIED_NAME(NamedType)
        break;
      }
    default:
      {
        break;
      }
  } // end switch
}

int main ( int argc, char** argv )
{
  SgProject *project = frontend (argc, argv);

  // Dump qualified names
  cout<<"----------- qualified names -------------"<<endl;
  visitorTraversal exampleTraversal;
  exampleTraversal.traverseMemoryPool();

  return backend (project);
}













