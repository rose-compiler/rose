// A translator to dump preprocessing information within an input source file
// Liao
#include "rose.h"
using namespace std;

class visitorTraversal:public AstSimpleProcessing
{
  public:
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
      printf ("-----------------------------------------------\n");
      printf ("Found an IR node with preprocessing Info attached:\n");
      printf ("(memory address: %p Sage type: %s) in file \n%s (line %d column %d) \n",
          locatedNode, 
          locatedNode->class_name ().c_str (),
          (locatedNode->get_file_info ()->get_filenameString ()).c_str (),
          locatedNode->get_file_info ()->get_line(),
          locatedNode->get_file_info ()->get_col()         );
      int counter = 0;
      AttachedPreprocessingInfoType::iterator i;
      for (i = comments->begin (); i != comments->end (); i++)
      {
        printf("-------------PreprocessingInfo #%d ----------- : \n",counter++);
        printf("classification = %s:\n String format = %s\n",
                PreprocessingInfo::directiveTypeName((*i)->getTypeOfDirective ()). c_str (), 
                (*i)->getString ().c_str ());
        printf ("relative position is = ");
        if ((*i)->getRelativePosition () == PreprocessingInfo::inside)
          printf ("inside\n");
        else
          printf ("%s\n", \
              ((*i)->getRelativePosition () == PreprocessingInfo::before) ? "before" : "after");
      }
    }
  }
}

int
main (int argc, char *argv[])
{
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);
  visitorTraversal exampleTraversal;
  exampleTraversal.traverse (project, preorder);
  return backend (project);
}

