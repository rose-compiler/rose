// This example code shows the traversal of IR types not available using the other traversal mechanism.
#include "rose.h"
using namespace std;

// CPP Macro to implement case for each IR node (we could alternatively use a visitor pattern and a function template, maybe?)
#define IR_NODE_VISIT_CASE(X) \
          case V_##X: \
             { \
               X* castNode = is##X(node); \
               int numberOfNodes   = castNode->numberOfNodes(); \
               int memoryFootprint = castNode->memoryUsage(); \
               printf ("count = %7d, memory use = %7d bytes, node name = %s \n",numberOfNodes,memoryFootprint,castNode->class_name().c_str());\
               break; \
             }

class RoseIRnodeVisitor : public ROSE_VisitTraversal {
     public:
          int counter;
          void visit ( SgNode* node);
          RoseIRnodeVisitor() : counter(0) {}
   };

void RoseIRnodeVisitor::visit ( SgNode* node)
   {
  // Using a classic visitor pattern should avoid all this casting,
  // but each function must be created separately (so it is wash if 
  // we want to do all IR nodes, as we do here).
     switch(node->variantT())
        {
          IR_NODE_VISIT_CASE(Sg_File_Info)
          IR_NODE_VISIT_CASE(SgPartialFunctionType)
          IR_NODE_VISIT_CASE(SgFunctionType)
          IR_NODE_VISIT_CASE(SgPointerType)
          IR_NODE_VISIT_CASE(SgFunctionDeclaration)
          IR_NODE_VISIT_CASE(SgFunctionSymbol)
          IR_NODE_VISIT_CASE(SgSymbolTable)
          IR_NODE_VISIT_CASE(SgInitializedName)
          IR_NODE_VISIT_CASE(SgStorageModifier)
          IR_NODE_VISIT_CASE(SgForStatement)
          IR_NODE_VISIT_CASE(SgForInitStatement)
          IR_NODE_VISIT_CASE(SgCtorInitializerList)
          IR_NODE_VISIT_CASE(SgIfStmt)
          IR_NODE_VISIT_CASE(SgExprStatement)
          IR_NODE_VISIT_CASE(SgTemplateDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationDecl)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationDefn)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationMemberFunctionDecl)
          IR_NODE_VISIT_CASE(SgClassSymbol)
          IR_NODE_VISIT_CASE(SgTemplateSymbol)
          IR_NODE_VISIT_CASE(SgMemberFunctionSymbol)

          default:
             {
#if 0
               printf ("Case not handled: %s \n",node->class_name().c_str());
#endif
             }
        }
   }


int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // ROSE visit traversal
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // ROSE visit traversal
     RoseIRnodeVisitor visitor;
     visitor.traverseRepresentativeIRnodes();
     printf ("Number of types of IR nodes (after building AST) = %d \n",visitor.counter);

#if 1
  // IR nodes statistics
     if (project->get_verbose() > 1)
          cout << AstNodeStatistics::IRnodeUsageStatistics();
#endif

     int errorCode = 0;
     errorCode = backend(project);

     return errorCode;
   }
