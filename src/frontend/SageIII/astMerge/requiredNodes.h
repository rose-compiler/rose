#ifndef REQUIRED_NODES_H
#define REQUIRED_NODES_H

class RequiredIRNodes
   : // public CustomAstDOTGeneration,
     public SgSimpleProcessing
   {
  // The IR nodes accumulated to requiredNodesList will be emilinated from the delete list (if there is overlap)
     public:
       // list of nodes required in AST (these will be eliminated from the delete list)
          std::set<SgNode*> requiredNodesList;

     public:
          RequiredIRNodes() {};

      //! Required traversal function
          void visit (SgNode* node);

       // void display();

       // set<SgNode*> & get_requiredNodesList();
   };

std::set<SgNode*> buildRequiredNodeList ( SgNode* node );

#endif // REQUIRED_NODES_H

