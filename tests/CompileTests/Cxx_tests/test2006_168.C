/*
Hi Dan,

I have the outliner working on ROSE's C++ test suite in all but two cases. The 
first is related to the SgStatementExpression question I sent in an earlier message.

The second is a little strange in that it occurs only sporadically when using the 
outliner's "pragma" interface. (In addition to being able to call a routine to 
outline the AST directly, one may also embed outlining pragmas in the source file, 
in the spirit of the OpenMP pragmas and Alin's original pragma interface.) The 
problem is that the #pragma sometimes appears in the AST in an unexpected location.

I've attached an example, simplified from CompileTests/Cxx_tests/test2004_09.C. 
My example has a sequence that looks like:

   variable-declaration;
 #pragma ...
   assignment;

However, the pragma ends up appearing before "variable-declaration" in the AST. 
I'm surprised because  pragmas otherwise generally do appear in the right place. 
The attached source file is the smallest version of test2004_09 that exhibits 
this behavior. I've attached both the unparsed output (from an identity translator) 
and a PDF dump of the AST as well.

Thanks,
--rich
*/

template < class T >
class ABC
   {
     public :
          T xyz;
   };

int foo()
{
  class ABC< int  > object1;

// This pragma will be unparsed into the wrong position (before the preceeding declarations).
#pragma rose_outline
  object1.xyz = 7;
  return 0;
}


