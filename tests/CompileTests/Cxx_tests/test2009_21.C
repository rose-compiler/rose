// This is a test code to support testing ROSE compiling ROSE.
// We can't put this into the test directory since it required 
// all the include files.

/*
Output from ROSE:
Handle the case where the class declaration has not yet been built!
Exiting at base of case to handle class declaration not yet seen!
lt-testAnalysis: /home/dquinlan/ROSE/svn-rose/src/frontend/CxxFrontend/EDG_SAGE_Connection/sage_gen_be.C:1306: SgExpression* sage_gen_routine_name(an_expr_node*): Assertion `false' failed.
gmake: *** [testForBug.o] Aborted
 */

// This bug requires this to be a template
template <class Container>
class ReadContainer
   {
     public:
          static bool get_val();
   };

class ParameterDeclaration
   {
     public:

          bool read() 
             {
               return ReadContainer<ParameterDeclaration>::get_val();
             }
   };
