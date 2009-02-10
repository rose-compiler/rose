// Assignment Return Const This
// Author: pants,,,
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H
#define COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H

namespace CompassAnalyses
   { 
     namespace AssignmentReturnConstThis
        { 
        /*! \brief Assignment Return Const This: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Assignment Return Const This Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
// #include "assignmentReturnConstThis.h"

namespace CompassAnalyses
   { 
     namespace AssignmentReturnConstThis
        { 
          const std::string checkerName      = "AssignmentReturnConstThis";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding assignment operators that do not return constant references to this";
          const std::string longDescription  = "Finds assignment operators (operator=) that have return type that is not a const reference to this or does return explicitly 'this' (and nothing else)";
        } //End of namespace AssignmentReturnConstThis.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AssignmentReturnConstThis::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::AssignmentReturnConstThis::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AssignmentReturnConstThis.YourParameter"]);


   }

void
CompassAnalyses::AssignmentReturnConstThis::Traversal::
visit(SgNode* node)
   {
     
          //
          // Implement your visitor here.
          // 
	bool is_const = false;
	bool is_ref = false;
	bool is_same_type = false;
	bool is_ret_this = true;
	std::string inputVar;
        std::string op = "operator=";
	std::string typeName;
	 SgMemberFunctionDeclaration *mfn =  isSgMemberFunctionDeclaration(node);
	 if (!mfn) return;
	 

	 std::string fName = mfn->get_name().str();
	 if (op == fName)
	   {
	     SgFunctionDefinition *fnDef = mfn->get_definition();
	     if (!fnDef)
	       {
		 return;
	       }
	     
	     std::string className = mfn->get_qualified_name().str();
	     int colon = className.rfind("::");
	     className =  "class " + className.substr(0,colon);
	     //cout << "class! " << className << endl;
	     
	     SgType *retType = mfn->get_orig_return_type();
	     SgReferenceType* sgRef = isSgReferenceType(retType);
	     if (sgRef)
	       {
		 is_ref = true;
	       }
	     typeName = retType->unparseToString();
	     
	     if (typeName.rfind("const",0) != std::string::npos)
	       {
		 is_const = true;
	       }
	     if (is_const and (typeName.length() + 5) >= className.length())
	       { 
		 //cout <<  "type ! " <<
		 //typeName.substr(6,className.length()) << endl;

		 //OK CHECK STRING CMP HERE
		 //cout << "setting sametype to true: " <<  << "||" <<  << endl;
		 if (typeName.substr(6, className.length()) == className)
		   {
		     is_same_type = true;
		   }
	       }
	     
	     Rose_STL_Container<SgNode*> returns = NodeQuery::querySubTree (mfn,V_SgReturnStmt);
	     
	     if (!returns.empty())
	       {
		 for (Rose_STL_Container<SgNode*>::iterator i = returns.begin(); i != returns.end(); i++)
		   {
		     if (!is_ret_this) break;
		     is_ret_this = false;
		     SgReturnStmt *ret = isSgReturnStmt(*i);
		     //ret->get_return_expr();
		     //std::vector< std::string > ted = ret->get_traversalSuccessorNamesContainer();
		     std::vector< SgNode * > kids = ret->get_traversalSuccessorContainer ();
		     if (kids.empty() or kids.size() > 1) break;
		     for (std::vector< SgNode * >::iterator j=kids.begin(); j!= kids.end(); j++)
		       {
			 
			 SgPointerDerefExp* deRef = isSgPointerDerefExp(*j);
			 if (!deRef) break;
			 std::vector< SgNode * > unRefs =  deRef->get_traversalSuccessorContainer ();
			 for (std::vector< SgNode * >::iterator k=unRefs.begin(); k!= unRefs.end(); k++)
			   {
			     SgThisExp *isItThis = isSgThisExp(*k);
			     if (isItThis) is_ret_this = true;
			     
			   }
		       }
		   }
	       }
	     else
	       {
		 //if no returns then we'll report it, don't like in
		 //explicit stuff.
		 is_ret_this = false;
	       }
	     std::string errString = className.substr(8)+"'s operator= ";
	     if (is_const and is_same_type and is_ref)// and is_ret_this)
	       {
		 //cout << "return type is pretty for " <<
		 //className.substr(8) << endl;
		 if (!is_ret_this)
		   {
		     errString +="returns something not *this or has no explicit returns"; 
		     output->addOutput(new CheckerOutput(node));
		   }
	       }
	     else
	       {
		 errString +=  "does not have return type const ";
		 errString +=  className.substr(8) + "&";
		 output->addOutput(new CheckerOutput(node));
		 //cout << "Failing class of " << className.substr(8) << endl;
	       }
	     //search for all returns and watch for return reference

	     
	   }


     
  // Implement your traversal here.  

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AssignmentReturnConstThis::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AssignmentReturnConstThis::Traversal(params, output);
}

extern const Compass::Checker* const assignmentReturnConstThisChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AssignmentReturnConstThis::checkerName,
        CompassAnalyses::AssignmentReturnConstThis::shortDescription,
        CompassAnalyses::AssignmentReturnConstThis::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
