/*
  This file demonstrates a problem with ROSE 0.8.8a:
  The Boost Phoenix library V2.0 is using template template parameters. 
  The EDG-ROSE connection does not handle such parameters.

  Error: case tak_template reached in sage_form_template_args()
         ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:347: SgTemplateArgumentPtrList* sage_form_template_args(a_template_arg*, an_il_to_str_output_control_block*): Assertion `false' failed.

  Code from EDG-ROSE connection from sage_il_to_str.C, line 347:

                  case tak_template: // A template template argument.
                  {
                 // form_template(tap->variant.templ.ptr, octl);
                    printf ("Error: case tak_template reached in sage_form_template_args() \n");
                    ROSE_ASSERT (false);
                    break;
                  }


  Remark: Note that the Boost Phoenix Library V1.0 does work with the same user code.
          See test_phoenix_v1.cc

  Bug reported: Markus Schordan, 17-Aug-2006
*/

#include<iostream>
#include<algorithm>
#include<vector>
#include<list>

#include<boost/spirit/phoenix/core.hpp>
#include<boost/spirit/phoenix/function.hpp>
#include<boost/spirit/phoenix/operator.hpp>

using namespace boost;

int main()
{
	typedef double numeric_type;

	// container ======
	//
	std::vector<numeric_type> mycontainer(4);
	std::vector<numeric_type>::iterator it;

	std::for_each(mycontainer.begin(), mycontainer.end(), 
			phoenix::arg1 += 1.0 );

	return 0;
}
