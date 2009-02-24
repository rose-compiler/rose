#include <rose.h>
#include <iostream>

#include "config.h"


#include "broadway.h"

int main(int argc, char **argv)
{
  int n;
  
  // initialize project
  string projectName = "testProject"; 
// projectsRowdata prow( UNKNOWNID ,projectName, UNKNOWNID );
// projects.retrieveCreateByColumn( &prow, "name", projectName );
// long projectId = prow.get_id();

  for(n = 1; n < argc; n++) {
    Annotations * a = new Annotations(*(new string(argv[n])), NULL);

    for (procedures_map_cp p = a->procedures().begin();
	 p != a->procedures().end();
	 ++p)
      {
	procedureAnn * proc = p->second;

	cout << "procedure " << proc->name() << " (";

	for (var_list_cp p = proc->formal_params().begin();
	     p != proc->formal_params().end();
	     ++p)
	  {
	    annVariable * decl = *p;
	    if (p != proc->formal_params().begin())
	      cout << ", ";
	    cout << decl->name();
	  }
	cout << ")" << endl;
	cout << "{" << endl;

	cout << "  access { ";
	for (var_set_cp p = proc->uses().begin();
	     p != proc->uses().end();
	     ++p)
	  {
	    annVariable * decl = *p;
	    if (p != proc->uses().begin())
	      cout << ", ";
	    cout << decl->name();
	  }
	cout << "  }" << endl;
	
        int modifiedParams = 0;

	cout << "  modify { ";
	for (var_set_cp p = proc->defs().begin();
	     p != proc->defs().end();
	     ++p)
	  {
	    annVariable * decl = *p;
	    if (p != proc->defs().begin())
	      cout << ", ";
	    cout << decl->name();

            int found = -1;
            int pnum = 0;

	    for (var_list_cp fp = proc->formal_params().begin();
	       fp != proc->formal_params().end();
	       ++fp, ++pnum)
	    {
	       annVariable * param = *fp;
               if (param == decl) {
                  found = pnum;
                  modifiedParams++;
                  break;
               }
 	    }


          }

	cout << "  }" << endl;

	cout << "}" << endl;
	
      }

  //    a->print(cout);
//    delete a;
  }

  return( 0 );
}
