// Copy Constructor Const Arg
// Author: pants,,,
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_COPY_CONSTRUCTOR_CONST_ARG_H
#define COMPASS_COPY_CONSTRUCTOR_CONST_ARG_H

namespace CompassAnalyses
   { 
     namespace CopyConstructorConstArg
        { 
        /*! \brief Copy Constructor Const Arg: Add your description here 
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

// COMPASS_COPY_CONSTRUCTOR_CONST_ARG_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Copy Constructor Const Arg Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
// #include "copyConstructorConstArg.h"

namespace CompassAnalyses
   { 
     namespace CopyConstructorConstArg
        { 
          const std::string checkerName      = "CopyConstructorConstArg";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding copy constructors that do not take const references as arguments";
          const std::string longDescription  = "Finds copy constructors that do not take const references to class type as function parameters.";
        } //End of namespace CopyConstructorConstArg.
   } //End of namespace CompassAnalyses.

CompassAnalyses::CopyConstructorConstArg::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::CopyConstructorConstArg::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CopyConstructorConstArg.YourParameter"]);


   }

void
CompassAnalyses::CopyConstructorConstArg::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     
	bool is_const = false;
	bool is_copy = false;
	bool is_ref = false;
	// is it a class? get name and list of members
	SgClassDeclaration* cs = isSgClassDeclaration(node);
	if (!cs) return;
	
	std::string className = cs->get_name().str();
	
	SgClassType* clt = cs->get_type();

	SgClassDefinition* cl = cs->get_definition();
	if (!cl) return;
	
	SgDeclarationStatementPtrList members = cl->get_members();
	for (SgDeclarationStatementPtrList::iterator i = members.begin(); i != members.end(); i ++)
	  {
	    //find member functions and check if we have a constructor
	    SgFunctionDeclaration* fn = isSgFunctionDeclaration(*i);
	    if (fn)
	      {
		std::string funcName = fn->get_name().str();
		SgSpecialFunctionModifier mods = fn->get_specialFunctionModifier();
		
		if (mods.isConstructor())
		  {
		    //get the argument list from the constructor, make sure there is one arg.
		    SgInitializedNamePtrList fnArgs = fn->get_args();
		    if (fnArgs.size() == 1)
		      {

          //No reason for using an iterator when we only have one element
          SgInitializedNamePtrList::iterator j = fnArgs.begin();

          //AS(12/03/07) Rewrite to use AST constructs instead of the very bad string comparison
			    //some very janky string checking here.  Making sure arg is same as class, then checking for & in last place of type string for ref.
			    // if const is there then that's ok.

          SgInitializedName* argVar = isSgInitializedName (*j);
			    if (!argVar) return;
			    SgType *t = argVar->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE);

			    SgReferenceType* sgRef = isSgReferenceType(t);
          SgModifierType* mod_type = isSgModifierType(t->stripType(SgType::STRIP_REFERENCE_TYPE));
		     
			    if (sgRef)
			      {
				is_ref = true;

			      }
			   
          if(mod_type != NULL){
            if(mod_type->get_typeModifier().get_constVolatileModifier().isConst())
            {
        				is_const = true;
            }

          }
          
          SgClassType* fromClType =  isSgClassType( t->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_TYPEDEF_TYPE ) );

          if((fromClType!=NULL) and (fromClType->get_declaration()->get_firstNondefiningDeclaration() == clt->get_declaration()->get_firstNondefiningDeclaration()) )
			      {
				is_copy = true;
			      }
			    
			    if (is_copy and (!is_const and is_ref))
			      {
				//Sg_File_Info* start = cs->get_startOfConstruct();
				output->addOutput(new CheckerOutput(node));
			      }
			  //}
		      }
		  }
		
	      }
	  }
	
	
     
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::CopyConstructorConstArg::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CopyConstructorConstArg::Traversal(params, output);
}

extern const Compass::Checker* const copyConstructorConstArgChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::CopyConstructorConstArg::checkerName,
        CompassAnalyses::CopyConstructorConstArg::shortDescription,
        CompassAnalyses::CopyConstructorConstArg::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
