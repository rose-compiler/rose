// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include "linearizeAST.h"
#include <functional>
#include <ostream>
using namespace std;

#include <vector>

void
print_out_all_macros(std::ostream& outStream, SgNode* node){


};

class VisitEveryNode : public SgSimpleProcessing
   {
     private:
       std::ostream& outStream;
     public:
       VisitEveryNode(std::ostream& outS);
       // required visit function to define what is to be done
          void visit ( SgNode* astNode );
   };

VisitEveryNode::VisitEveryNode(std::ostream& outS)
  :outStream(outS)
{
};

void VisitEveryNode::visit ( SgNode* node )
   {
     //We don't want to unparse the whole file; this pulls in built-in functions which are different
     //between platforms
     if (isSgSourceFile(node) || isSgGlobal(node))
          return;
   
     std::vector<SgNode*> linearizedSubtree = linearize_subtree(node);

     outStream  << "Unparsed: " << node->class_name() << " " << node->unparseToString() << std::endl;
     outStream  << "          ";
     for (std::vector<SgNode*>::iterator it_sub =  linearizedSubtree.begin();
         it_sub != linearizedSubtree.end(); ++ it_sub){
       outStream << (*it_sub)->class_name() << " ";
       if(isSgValueExp(*it_sub) != NULL)
         outStream << (*it_sub)->unparseToString() + " ";
     }

     outStream << std::endl; 

   };




int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Assume that there is only one file
     std::string filename;
     for(int i = 0 ; i < project->numberOfFiles(); i++)
     {
       SgSourceFile* file = isSgSourceFile(&project->get_file(i));
       if( file != NULL)
         filename = file->getFileName();

     };

     ROSE_ASSERT(filename != "");

     filename+=".linearization.out";
     filename = StringUtility::stripPathFromFileName(filename);

     ofstream myfile;
     myfile.open (filename.c_str());

     ROSE_ASSERT(myfile.is_open());
     std::cout << "Outputing to the file " << filename << std::endl;





     VisitEveryNode aNode(myfile);
     aNode.traverseInputFiles(project, preorder);



//     return backend(project);

   };

