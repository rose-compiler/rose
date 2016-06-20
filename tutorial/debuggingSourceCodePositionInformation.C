// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

int
main ( int argc, char* argv[] )
   {
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In preprocessor.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     Rose_STL_Container<SgNode*> nodeList;
     nodeList = NodeQuery::querySubTree (project,V_SgForStatement);
     printf ("\nnodeList.size() = %zu \n",nodeList.size());

     Rose_STL_Container<SgNode*>::iterator i = nodeList.begin();
     while (i != nodeList.end())
        {
          Sg_File_Info & fileInfo = *((*i)->get_file_info());
          printf ("Query node = %p = %s in %s \n ----- at line %d on column %d \n",
              *i,(*i)->sage_class_name(),fileInfo.get_filename(),
               fileInfo.get_line(), fileInfo.get_col());
          i++;
        }

     if (project->get_verbose() > 0)
          printf ("Calling the backend() \n");

     return 0;
   }
