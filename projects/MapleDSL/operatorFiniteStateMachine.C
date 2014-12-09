
#include "rose.h"

// #include "operatorEvaluation.h"

// This code will make calls to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
#include "operatorFiniteStateMachine.h"

// #include "dslSupport.h"

using namespace std;

// using namespace DSL_Support;


// **************************************************
// Discretization Finite State Machine Implementation
// **************************************************

DiscretizationFSM::DiscretizationFSM(int dim, DiscretizationFSM::DiscreticationKind_enum kind)
   {
     dimension               = dim;
     discreticationKind      = kind;
     builtFromOperatorParens = false;
     operatorParensArgument  = NULL;
   }

DiscretizationFSM
DiscretizationFSM::operator()(DiscretizationFSM* X)
   {
     builtFromOperatorParens = true;

     ROSE_ASSERT(X != NULL);

  // This must be initalized using a copy to define functional symantics.
     operatorParensArgument = new DiscretizationFSM(*X);

  // We might need to save the argument name so that we can support the Maple code generation.
     argumentName = "";

  // We might need the name as part of the Maple code generation.
  // discretizationName = X->discretizationName;
     operatorParensArgument->discretizationName = X->discretizationName;

     return *this;
   }

void DiscretizationFSM::display(const string & label)
   {
     printf ("In DiscretizationFSM::display(): label = %s \n",label.c_str());
     printf ("   --- discretizationName      = %s \n",discretizationName.c_str());
     printf ("   --- dimension               = %d \n",dimension);
     printf ("   --- discreticationKind      = %d \n",discreticationKind);
     printf ("   --- builtFromOperatorParens = %s \n",builtFromOperatorParens ? "true" : "false");
     printf ("   --- operatorParensArgument  = %p \n",operatorParensArgument);

     if (operatorParensArgument != NULL)
        {
          operatorParensArgument->display("operatorParensArgument");
        }
   }




// ********************************************
// Operator Finite State Machine Implementation
// ********************************************

OperatorFSM::OperatorFSM()
   {
  // Nothing to do here.
#if 0
     printf ("In OperatorFSM default constructor \n");
#endif
   }

OperatorFSM::OperatorFSM(DiscretizationFSM X)
   {
  // This is the simplest discretization operator (a first-order discretization).

#if 0
      printf ("In OperatorFSM constructor: OperatorFSM(DiscretizationFSM x) \n");
      printf ("   --- X.dimension = %3d \n",X.dimension);
#endif

  // vector<pair<Offset,double> > stencilPointList;
     discretizationList.push_back(X);
   }


OperatorFSM OperatorFSM::operator+(const OperatorFSM & X)
   {
  // Union operator
  // The semantics of this operator match that of the discretization operator used to specify the Maple DSL.

#if 0
      printf ("In OperatorFSM::operator+(): X.discretizationList.size() = %zu \n",X.discretizationList.size());
      printf ("In OperatorFSM::operator+(): X.operatorList.size()       = %zu \n",X.operatorList.size());
#endif

     for (size_t i = 0; i < X.discretizationList.size(); i++)
        {
#if 0
          printf ("In OperatorFSM::operator+(): In Loop: discretizationList.size() = %zu \n",discretizationList.size());
          printf ("In OperatorFSM::operator+(): In Loop: X.discretizationList.size() = %zu \n",X.discretizationList.size());
#endif
          discretizationList.push_back(X.discretizationList[i]);
#if 0
          printf ("In OperatorFSM::operator+(): In Loop: discretizationList.size() = %zu \n",discretizationList.size());
#endif
        }

     for (size_t i = 0; i < X.operatorList.size(); i++)
        {
#if 0
          printf ("In OperatorFSM::operator+(): In Loop: operatorList.size() = %zu \n",operatorList.size());
          printf ("In OperatorFSM::operator+(): In Loop: X.operatorList.size() = %zu \n",X.operatorList.size());
#endif
          operatorList.push_back(X.operatorList[i]);
#if 0
          printf ("In OperatorFSM::operator+(): In Loop: operatorList.size() = %zu \n",operatorList.size());
#endif
        }
#if 0
      printf ("Leaving OperatorFSM::operator+(): discretizationList.size() = %zu \n",discretizationList.size());
      printf ("Leaving OperatorFSM::operator+(): operatorList.size()       = %zu \n",operatorList.size());
#endif

     return *this;
   }

#if 0
OperatorFSM OperatorFSM::operator*(const OperatorFSM & X)
   {
  // Convolution operator

     printf ("operator convolution not defined yet! \n");
     ROSE_ASSERT(false);

     return *this;
   }
#endif

OperatorFSM OperatorFSM::operator=(const OperatorFSM & X)
   {
  // Assignment operator (operator=).
#if 0
     printf ("In OperatorFSM operator= \n");
#endif

     discretizationList = X.discretizationList;
     operatorList       = X.operatorList;

     operatorName       = X.operatorName;

#if 0
     printf ("Leaving OperatorFSM operator= \n");
#endif

     return *this;
   }

OperatorFSM::OperatorFSM (const OperatorFSM & X)
   {
  // Copy constructor (defined using the assignment operator).
#if 0
     printf ("In OperatorFSM copy constructor \n");
#endif
  // this->operator=(X);
     discretizationList = X.discretizationList;
     operatorList       = X.operatorList;

     operatorName       = X.operatorName;
   }

void
OperatorFSM::display(const std::string & label)
   {
     printf ("In OperatorFSM::display(): label = %s \n",label.c_str());

     printf ("   --- operatorName = %s \n",operatorName.c_str());

     printf ("   --- discretizationList (size = %zu): \n",discretizationList.size());
     for (size_t i = 0; i < discretizationList.size(); i++)
        {
       // DiscretizationFSM discretizationFSM  = discretizationList[i];
       // printf ("      --- discretizationFSM.dimension = %3d \n",discretizationFSM.dimension);

          printf ("   --- discretizationList[%zu]: \n",i);
          discretizationList[i].display("DiscretizationFSM element");
        }

     printf ("   --- operatorList (size = %zu): \n",operatorList.size());
     for (size_t i = 0; i < operatorList.size(); i++)
        {
          printf ("   --- operatorList[%zu]: \n",i);
          operatorList[i].display("OperatorFSM element");
        }
   }
