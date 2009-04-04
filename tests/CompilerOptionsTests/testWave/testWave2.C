// A translator to dump preprocessing information in a file
// Liao
#include "rose.h"
#include <fstream>

using namespace std;

bool
sort_preprocessing_info(const PreprocessingInfo* left, const PreprocessingInfo* right )
{

  if(left->getLineNumber() < right->getLineNumber()) return true;
  if( (left->getLineNumber() == right->getLineNumber())
      && ( left->getColumnNumber() < right->getColumnNumber() ) )
    return true;

  return false;
};

class visitorTraversal:public AstSimpleProcessing
{
  public:
    std::ostream* outStream;
    virtual void visit (SgNode * n);
};

  void
visitorTraversal::visit (SgNode * n)
{
  // On each node look for any comments of CPP directives
  SgLocatedNode *locatedNode = isSgLocatedNode (n);
  if (locatedNode != NULL)
  {
    AttachedPreprocessingInfoType *comments =
      locatedNode->getAttachedPreprocessingInfo ();

    if (comments != NULL)
    {
      (*outStream) << "-----------------------------------------------\n";
      (*outStream) << "Found an IR node ( of type: " 
        << locatedNode->class_name () << ") in file "
        << locatedNode->get_file_info ()->get_filenameString () << "\n";
      int counter = 0;
 
     
      std::vector<PreprocessingInfo*> mapping;

      { //Get the preprocessing info for sorting
        AttachedPreprocessingInfoType::iterator i;

        for (i = comments->begin (); i != comments->end (); i++)
        {
          mapping.push_back(*i);
        }

      }

      //Need to sort in a predictable manner to get predictable output order of
      //preprocessing output
      std::sort( mapping.begin(), mapping.end(), std::ptr_fun(sort_preprocessing_info) );


      std::vector<PreprocessingInfo*>::iterator i;
      for (i = comments->begin (); i != comments->end (); i++)
      {
        (*outStream) << "with attached preprocessingInfo #"<< counter++ << " : classification "
          <<PreprocessingInfo::directiveTypeName ((*i)->getTypeOfDirective ()) 
          << " : position " << PreprocessingInfo::relativePositionName((*i)->getRelativePosition()) 
          << ": line-column: \n" << (*i)->getString () << "\n";
      }
    }
    else
    {
      //          printf ("No attached comments (at %p of type: %s): \n", locatedNode,
      //                  locatedNode->sage_class_name ());
    }
  }
}

  int
main (int argc, char *argv[])
{
  std::vector<std::string> newArgv(argv,argv+argc);
  newArgv.push_back("-rose:wave");

  SgProject *project = frontend (newArgv);
  visitorTraversal exampleTraversal;

  //Initializing output to File
  ofstream myfile;

  {
    // Assume that there is only one file
    std::string filename;
    for(int i = 0 ; i < project->numberOfFiles(); i++)
    {
      SgSourceFile* file = isSgSourceFile(&project->get_file(i));
      if( file != NULL)
        filename = file->getFileName();

    };

    ROSE_ASSERT(filename != "");

    filename+=".test2.out";
    filename = StringUtility::stripPathFromFileName(filename);


    //Output the macro diretives to the file
    myfile.open (filename.c_str());

    ROSE_ASSERT(myfile.is_open());

  }

  exampleTraversal.outStream = &myfile;
  exampleTraversal.traverse (project, preorder);

  return backend (project);
}
