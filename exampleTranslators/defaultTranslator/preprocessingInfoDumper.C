// A translator to dump preprocessing information within an input source file
// Liao
#include "rose.h"
using namespace std;

using namespace SageInterface;

void prettyPrint(PreprocessingInfo* info )
{
  printf("%p classification = %s:\n String format = %s \n file name =%s line =%d \n", 
      info, PreprocessingInfo::directiveTypeName(info->getTypeOfDirective ()). c_str (), 
      info->getString ().c_str (), info->getFilename().c_str(), info->getLineNumber() );
  printf ("relative position is = ");
  if (info->getRelativePosition () == PreprocessingInfo::inside)
    printf ("inside\n");
  else
    printf ("%s\n", \
	(info->getRelativePosition () == PreprocessingInfo::before) ? "before" : "after");
}


// add a consistency checking: the list should have incremental line numbers
// return true if the preprocessing info list have strictly incremental line number
// TODO: what if the list contains things from different files: oops!!
bool checkIncrementalLineNumbers (vector<PreprocessingInfo*>& infoList)
{
  bool isFirst= true;
  int prev_line = -1; 

  for (auto i: infoList)
  {
    prettyPrint(i);
    if (isFirst) // first in the list. no checking with prev line number is needed.
    {
      isFirst = false;
    }
    else
    {
      if (prev_line > i->getLineNumber())
      {
        cerr<<"Error: find a directive with line number smaller than a previous directive's line number"<<endl;
	ROSE_ASSERT(false);
        return false;
      }
    }

    prev_line = i->getLineNumber();
  }
  
  return true; 
}


class visitorTraversal:public AstSimpleProcessing
{
  public:
    virtual void visit (SgNode * n);
};

void
visitorTraversal::visit (SgNode * n)
{
  // On each node look for any comments or CPP directives
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
	PreprocessingInfo * info = *i; 
	prettyPrint (info);
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

  printf ("Using preorder recursive call to preserve directive orders:\n ");
  vector<PreprocessingInfo*> infoList; 
  preOrderCollectPreprocessingInfo (project, infoList, 0);

  assert (checkIncrementalLineNumbers (infoList)==true);

  return backend (project);
}

