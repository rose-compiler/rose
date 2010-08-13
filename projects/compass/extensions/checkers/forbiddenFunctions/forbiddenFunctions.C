// Forbidden Functions
// Author: Gary Yuan
// Date: 28-December-2007

#include "rose.h"
#include "compass.h"
#include <map>
#include <string>


#ifndef COMPASS_FORBIDDEN_FUNCTIONS_H
#define COMPASS_FORBIDDEN_FUNCTIONS_H

namespace CompassAnalyses
   { 
     namespace ForbiddenFunctions
        { 
        /*! \brief Forbidden Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(
                      SgNode* node, 
                      const std::string &, 
                      const std::string & );
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
               private:
	         int foundFunction;
                 Compass::OutputObject* output;
                 std::map<std::string,std::string> forbiddenFunctionMap;
                 void parseParameter( const std::string & param );
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
	       void run(SgNode* n){ foundFunction=0;this->traverse(n, preorder); 
		 //std::cerr << "  forbiddenFunctions : functions traversed = " << 
		 //  RoseBin_support::ToString(foundFunction) << std::endl;
	       }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FORBIDDEN_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Forbidden Functions Analysis
// Author: Gary Yuan
// Adapted From: forbiddenFunctions by Jeremiah J. Willcock, 05-September-2007
// Date: 28-December-2007

#include <sstream>

#include "compass.h"
// #include "forbiddenFunctions.h"

namespace CompassAnalyses
   { 
     namespace ForbiddenFunctions
        { 
          const std::string checkerName      = "ForbiddenFunctions";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Checks for calls to forbidden functions.";
          const std::string longDescription  = "Checks for calls to forbidden functions as specified in compass_parameters using only the non-overloaded name of the function.";
        } //End of namespace ForbiddenFunctions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ForbiddenFunctions::CheckerOutput::CheckerOutput( 
  SgNode* node, 
  const std::string & name,
  const std::string & reason ) : OutputViolationBase(node,checkerName,
    "Forbidden function '" + name + "' used. "+reason + ".")
   {
   } //CompassAnalyses::ForbiddenFunctions::CheckerOutput::CheckerOutput

CompassAnalyses::ForbiddenFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
     try
     {
       for( int i = 0; ; ++i )
       {
         std::stringstream ss;
         ss << "ForbiddenFunctions.Function" << i;
         parseParameter(inputParameters[ss.str()]);
       } //for, i
     }//try
     catch( const Compass::ParameterNotFoundException & e )
     {
     } //catch( const Compass::ParameterNotFoundException & e )

   } //CompassAnalyses::ForbiddenFunctions::Traversal::Traversal()

void CompassAnalyses::ForbiddenFunctions::Traversal::parseParameter(
  const std::string & param )
{
  std::string namePart;
  std::string name;
  std::string reason;
  size_t index = param.find(',');

  if( index == std::string::npos ) 
  {
    namePart = param;
  } //if( index == std::string::npos ), No reason field
  else 
  {
    namePart = param.substr(0, index);
    reason = param.substr(index + 1);
  } //else

  // Trim spaces
  {
    std::istringstream is(namePart);
    is >> name;
  }

  if( name == "" ) 
  {
    throw Compass::ParseError(param, "non-blank function name");
  } //if( name == "" )

  forbiddenFunctionMap.insert( std::make_pair(name, reason) );
} //CompassAnalyses::ForbiddenFunctions::parseParameter

void
CompassAnalyses::ForbiddenFunctions::Traversal::
visit(SgNode* node)
   {
     std::string *sgName = NULL;

     if (isSgFunctionDefinition(node))
       foundFunction++;

     switch(node->variantT())
     {
       case V_SgFunctionRefExp:
       {
         sgName = new std::string( isSgFunctionRefExp(node)->get_symbol()->get_declaration()->get_name().getString() );
       } break; //case V_SgFunctionRefExp
       case V_SgMemberFunctionRefExp:
       {
         sgName = new std::string( isSgMemberFunctionRefExp(node)->get_symbol()->get_declaration()->get_name().getString() );
       } break; //case V_SgMemberFunctionRefExp
       default:
         break;
     } //switch(node->variantT())

     if( sgName != NULL )
     {
       std::map<std::string, std::string>::const_iterator fItr = forbiddenFunctionMap.find(*sgName);

       if( fItr != forbiddenFunctionMap.end() )
       {
	 SgNode* parent = node->get_parent();
	 while (!isSgFunctionDeclaration(parent) && !isSgGlobal(parent)) 
	   parent=parent->get_parent();
	 std::string funcname="";
	 if (isSgFunctionDeclaration(parent))
	   funcname=isSgFunctionDeclaration(parent)->get_name();
	 std::string reason="\tin function: "+funcname+" \treason: "+fItr->second;

         output->addOutput(
           new CheckerOutput( node, fItr->first, reason )
         ); //output->addOutput
       } //if
     } //if( sgName != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ForbiddenFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ForbiddenFunctions::Traversal(params, output);
}

extern const Compass::Checker* const forbiddenFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ForbiddenFunctions::checkerName,
        CompassAnalyses::ForbiddenFunctions::shortDescription,
        CompassAnalyses::ForbiddenFunctions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
