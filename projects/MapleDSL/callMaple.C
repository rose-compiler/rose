
#include "rose.h"

#include "rose_config.h"

#ifdef USE_ROSE_MAPLE_SUPPORT
#include "maplec.h"

// Need SIGINT and signal handling details below.
#include <signal.h>

#include "mapleOperatorAPI.h"

// Need this for processing of comma seperated values in strings.
#include <sstream>

using namespace std;

// Define this as a global variable.
StencilOperator stencil(3,3,1);

// global variable used by queryInterrupt()
static int Interrupted = 0;

// interrupt signal handler: sets global variable when user hits Ctrl-C
void catch_intr( int signo )
   {
     Interrupted = TRUE;
     signal(SIGINT,catch_intr);
   }

// interrupt callback: stops computation when `Interrupted' is true
static M_BOOL M_DECL queryInterrupt( void *data )
   {
     if( Interrupted )
        {
          Interrupted = 0;
  	       return( TRUE );
        }
     return( FALSE );
   }

// callback used for directing result output
static void M_DECL textCallBack( void *data, int tag, char *output )
   {
     printf("%s\n",output);
   }


double returnCoefficient( MKernelVector kv, ALGEB coefficient_matrix,int i, int j)
   {
     printf ("In returnCoefficient() \n");

     M_INT maple_index_array[2];
     RTableData rtd1;
  // ALGEB maple_coeficient;
     
     double returnValue = 0.0;

     printf ("Calling ToMapleInteger() \n");

  // maple_index_array[0] = ToMapleInteger(kv,i);
  // maple_index_array[1] = ToMapleInteger(kv,j);
     maple_index_array[0] = i;
     maple_index_array[1] = j;

     printf ("Calling RTableSelect() \n");

  // double returnValue = MapleToFloat64(kv,equation);
     rtd1 = RTableSelect(kv,coefficient_matrix,maple_index_array);
  // maple_coeficient = RTableSelect(kv,coefficient_matrix,maple_index_array);

     printf ("DONE: Calling RTableSelect() \n");

  // double returnValue = MapleToFloat64(kv,rtd1);

     printf ("DONE: Calling MapleToFloat64() \n");

     return returnValue;
   }

// Specify a call back function in C/C++ that can be called from Maple.
// This function will be called from Maple and the input the the function
// will be a string passed as "args" (still unclear how "data" is used).
// Then this call back function can return a string which will be interpreted
// by Maple.
// To use this mechanism to get data from the coefficient array in Maple,
// we need to pass the index values and the matrix entry in the "args"
// array. Likely in a string such as: "1,2,3,-4.0" and then interpret it
// in the function to initialized the C/C++ data structure, such as:
// "stencil[1][2][3] = StencilPoint(/*bool isNonzero*/ true, /*double coefficientValue*/ -4.0);"
char* M_DECL callBackCallBack( void *data, char *args )
   {
  // Note that the args is a comma seperated list of integer index values 
  // (number of values equal to the dimension) and the associated coefficient value.

#if 1
     printf ("Inside of callBackCallBack(void *data, char *args) \n");
     printf ("   --- data = %p \n",data);
     printf ("   --- args = %s \n",args);
#endif

     vector<int> index_vector;

     string str = args;
     stringstream ss(str);

     int counter   = 1;
  // To be more general we could have the first element in the CSV list be the dimension.
     int dimension = 2;

     double coefficientValue = 0.0;

     bool isNonzero = false;

     double f;
     while (ss >> f)
        {
#if 1
       // printf ("ss = %s \n",ss.str());
          printf ("loop: counter = %d \n",counter);
#endif
       // vect.push_back(i);
          if (counter <= dimension)
             {
               int i = f;
               index_vector.push_back(i);
               printf ("value in args interpreted as an index value: i = %d \n",i);
               counter++;
             }
            else
             {
               coefficientValue = f;
               printf ("value in args interpreted as an index value: coefficientValue = %4.2f \n",coefficientValue);

            // Simple check for zero value (we ignore zero valued coefficients.
               if ((coefficientValue > -0.0001) && (coefficientValue < 0.0001))
                  {
                    isNonzero = false;
                  }
                 else
                  {
                    isNonzero = true;
                  }
             }

          if (ss.peek() == ',')
               ss.ignore();

        }
#if 0
     printf ("counter = %d \n",counter);
#endif

     for (int j = counter; j <= 3; j++)
        {
#if 0
          printf ("Fill in the rest of the values: j = %d index_vector.size() = %zu \n",j,index_vector.size());
#endif
       // Push one into remaining positions (will be translated to zero in next step when we translate the base of all index values).
          index_vector.push_back(1);
        }

#if 0
     printf ("Filled in the rest of the values index_vector.size() = %zu \n",index_vector.size());
#endif

     vector<int>::iterator itr = index_vector.begin();
     while(itr != index_vector.end())
        {
          printf ("  --- before translation: *itr = %d \n",*itr);
          *itr -= 1;
          printf ("  --- after translation: *itr = %d \n",*itr);
          itr++;
        }

#if 0
     itr = index_vector.begin();
     printf ("index_vector values: \n");
     while(itr != index_vector.end())
        {
          printf ("  --- *itr = %d \n",*itr);
          itr++;
        }

     printf ("coefficientValue = %4.2f \n",coefficientValue);
#endif

     ROSE_ASSERT(index_vector.size() == 3);

#if 0
     printf ("index_vector[0] = %d, index_vector[1] = %d, index_vector[2] = %d \n",index_vector[0],index_vector[1],index_vector[2]);
#endif

     ROSE_ASSERT(index_vector[0] < 3);
     ROSE_ASSERT(index_vector[1] < 3);
     ROSE_ASSERT(index_vector[2] < 3);

  // stencil[vect[0]][vect[2]][vect[3]] = StencilPoint(/*bool isNonzero*/ isNonzero, /*double coefficientValue*/ coefficientValue);
  // stencil(x,y,z) = StencilPoint(/*bool isNonzero*/ isNonzero, /*double coefficientValue*/ coefficientValue);
     stencil(index_vector[0],index_vector[1],index_vector[2]) = StencilPoint(/*bool isNonzero*/ isNonzero, /*double coefficientValue*/ coefficientValue);

  // We return a string that will be interpreted in Maple.
  // return "72;";
     return NULL;
   }


StencilOperator
callMaple ( const string & generatedCode )
   {
  // This function uses the prepared strings below as a template and inserts the generated 
  // code inbetween to form the Maple code to send to the Maple interpreter.

     string code;
     string codeFragment_part1 = "\
##\n\
## The following is preamble stuff for various functionality I often use\n\
##\n\
restart;\n\
with( plots ):\n\
with( LinearAlgebra ):\n\
with( CodeGeneration ):\n\
with( codegen, cost, optimize, makeproc ):\n\
\n\
##\n\
## The following are my generic definitions of the D operators in the x, and y directions\n\
##\n\
unassign( 'kx','ky','kz','ix','iy','iz' );\n\
dpx := w -> ( (subs({kx=ix+1},subs({ix=kx},w))-subs({kx=ix},  subs({ix=kx},w)))/dx ):\n\
dmx := w -> ( (subs({kx=ix},  subs({ix=kx},w))-subs({kx=ix-1},subs({ix=kx},w)))/dx ):\n\
dpy := w -> ( (subs({ky=iy+1},subs({iy=ky},w))-subs({ky=iy},  subs({iy=ky},w)))/dy ):\n\
dmy := w -> ( (subs({ky=iy},  subs({iy=ky},w))-subs({ky=iy-1},subs({iy=ky},w)))/dy ):\n";

     code += codeFragment_part1 + "\n## Generated Code from Maple DSL using ROSE\n" + generatedCode + "\n\n";

     string codeFragment_part2 = "\
## This is the application of the operator (again this is actually in the C++ code).\n\
v[ix,iy] := Lap2D( u[ix,iy] ):\n\
\n\
## now we generate the code\n\
alg_string := \n\
  C( (makeproc(v[ix,iy])),\n\
    precision=double,\n\
    optimize=false,\n\
    resultname=\"ret\",\n\
    declare = [ix::integer,iy::integer,\n\
               dx::float,dy::float,\n\
               ret::float,\n\
               u::float,v::float],\n\
    output = string ):\n\
\n\
\n\
## Write a file with the generated code\n\
fd := open( \"Lap2D.c\", WRITE ):\n\
fprintf( fd,alg_string ):\n\
close( fd ):\n\
## 42.0;\n\
\n\
## callBack(\"test\");\n\
## callBack(42);\n\
\n\
## Compute stencils\n\
stencilSize := 3:\n\
offset := (stencilSize-1)/2:\n\
  unassign( 'coeff_mat' );\n\
  coeff_mat := Matrix(stencilSize,stencilSize):\n\
\n\
  for gix from -offset by 1 to offset do \n\
  for giy from -offset by 1 to offset do \n\
    coeff_mat(gix+offset+1,giy+offset+1) := coeff( v[ix,iy], u[ix+gix,iy+giy] );\n\
  end do:\n\
  end do:\n\
## subs( {dx=1,dy=1},coeff_mat);\n\
##\n\
## Indexing in Maple starts at one, so we need the upper bound to be 3.\n\
for i to 3 do for j to 3 do callback(i,j,subs( {dx=1,dy=1},coeff_mat)(i,j)); od; od;\n";

     code += codeFragment_part2;

     char err[2048];  // command input and error string buffers
     MKernelVector kv;  // Maple kernel handle
     MCallBackVectorDesc cb = { textCallBack,
          0,   // errorCallBack not used 
          0,   // statusCallBack not used
          0,   // readLineCallBack not used
          0,   // redirectCallBack not used
          0,   // streamCallBack not used
          queryInterrupt, 
       // 0    // callBackCallBack not used
          callBackCallBack
        };
     ALGEB dag;  // eval result (Maple data-structure)

  // initialize Maple
     int argc = 0;
     char** argv = NULL;
     if ( (kv = StartMaple(argc,argv,&cb,NULL,NULL,err)) == NULL ) 
        {
          printf("Maple called from ROSE: Fatal error, %s\n",err);
          ROSE_ASSERT(false);
        }

  // We have to cast away const to form a C-style char* string for the Maple API.
     char* code_charString = const_cast<char*>(code.c_str());
#if 1
  // Output the code string we will use (debugging).
     printf ("code_charString = %s \n",code_charString);
#endif
  // call through to your callback handler to setup some data
  // EvalMapleStatement(kv,"callback(\"get_data\");");
     const char* callbackSetUpString = "callback(\"get_data\");";
     EvalMapleStatement(kv,const_cast<char*>(callbackSetUpString));

  // execute a callback at the beginning of each iteration of a loop.
  // EvalMapleStatement(kv,"for i to 10 do callback(i); eval(%)*x^2; od;");

  // dag = EvalMapleStatement(kv,expr);
  // dag = EvalMapleStatement(kv,code.c_str());
     dag = EvalMapleStatement(kv,code_charString);

#if 0
  // Call function back to maple
     ALGEB coefficient_matrix = dag;
     int i=0,j=0;
     double coefficient = returnCoefficient(kv,coefficient_matrix,i,j);

     printf ("coefficient = %f \n",coefficient);
#endif

     if ( dag && IsMapleStop(kv,dag) )
        {
          printf("Maple called from ROSE: Processing code string:: Fatal error, %s\n",err);
          ROSE_ASSERT(false);
        }

     printf ("Evaluation using Maple complete! \n");

  // **************************************************************************
  // Put the reading of the Maple expression to evaluate the coefficients here!
  // **************************************************************************

  // Assume a 2D array of stencil points.
  // StencilOperator stencil(3,3,1);

     return stencil;
   }

// endif for USE_ROSE_MAPLE_SUPPORT
#endif

