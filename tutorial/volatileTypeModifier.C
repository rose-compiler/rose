#include "rose.h"

using namespace std;

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
  // The "volatile" madifier is in the type of the SgInitializedName
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL) 
        {
          printf ("Found a SgInitializedName = %s \n",initializedName->get_name().str());
          SgType* type = initializedName->get_type();

          printf ("   initializedName: type = %p = %s \n",type,type->class_name().c_str());
          SgModifierType* modifierType = isSgModifierType(type);
          if (modifierType != NULL)
             {
               bool isVolatile = modifierType->get_typeModifier().get_constVolatileModifier().isVolatile();
               printf ("   initializedName: SgModifierType: isVolatile = %s \n",(isVolatile == true) ? "true" : "false");
             }

          SgModifierNodes* modifierNodes = type->get_modifiers();
          printf ("   initializedName: modifierNodes = %p \n",modifierNodes);
          if (modifierNodes != NULL)
             {
               SgModifierTypePtrVector modifierList = modifierNodes->get_nodes();
               for (SgModifierTypePtrVector::iterator i = modifierList.begin(); i != modifierList.end(); i++)
                  {
                    printf ("initializedName: modifiers: i = %s \n",(*i)->class_name().c_str());
                  }
             }
        }

  // Note that the "volatile" madifier is not in the SgVariableDeclaration nor the SgVariableDefinition
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
     if (variableDeclaration != NULL) 
        {
          bool isVolatile = variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile();
          printf ("SgVariableDeclaration: isVolatile = %s \n",(isVolatile == true) ? "true" : "false");
          SgVariableDefinition* variableDefinition = variableDeclaration->get_definition();
       // printf ("variableDefinition = %p \n",variableDefinition);
          if (variableDefinition != NULL)
             {
               bool isVolatile = variableDefinition->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile();
               printf ("SgVariableDefinition: isVolatile = %s \n",(isVolatile == true) ? "true" : "false");
             }
        }
   }

// must have argc and argv here!!
int main(int argc, char * argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject *project = frontend (argc, argv);

     visitorTraversal myvisitor;
     myvisitor.traverseInputFiles(project,preorder);

     return backend(project);
   }

