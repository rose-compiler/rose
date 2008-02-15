// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Forbidden Functions Analysis
// Author: Gary Yuan
// Adapted From: forbiddenFunctions by Jeremiah J. Willcock, 05-September-2007
// Date: 28-December-2007

#include <sstream>

#include "compass.h"
#include "forbiddenFunctions.h"

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
    "Forbidden function '" + name + "' used. " + reason + ".")
   {
   } //CompassAnalyses::ForbiddenFunctions::CheckerOutput::CheckerOutput

CompassAnalyses::ForbiddenFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription), isCheckerFile(false)
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
  unsigned int index = param.find(',');

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
       case V_SgFile:
       {
         std::string currFileName = 
           isSgFile(node)->get_startOfConstruct()->get_filenameString();

         if( currFileName != previousFileName )
         {
           isCheckerFile = false;
           previousFileName = currFileName;
         } //if( currFileName != previousFileName )
       } break; //case V_SgFile
       case V_SgNamespaceDeclarationStatement:
       {
         if( isSgNamespaceDeclarationStatement(node)->get_name().getString() == 
             "CompassAnalyses" ) isCheckerFile = true;
       } break; //case V_SgNamespaceDeclarationStatement
       default:
         break;
     } //switch(node->variantT())

     if( isCheckerFile == true && sgName != NULL )
     {
       std::map<std::string, std::string>::const_iterator fItr = forbiddenFunctionMap.find(*sgName);

       if( fItr != forbiddenFunctionMap.end() ) 
       {
         output->addOutput(
           new CheckerOutput( node, fItr->first, fItr->second )
         ); //output->addOutput
       } //if
     } //if( sgName != NULL )

     return;
   } //End of the visit function.
