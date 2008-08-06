// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Byte By Byte Structure Comparison Analysis
// Author: Han Suk  Kim
// Date: 23-August-2007

#include "compass.h"
#include "byteByByteStructureComparison.h"

namespace CompassAnalyses
   { 
     namespace ByteByByteStructureComparison
        { 
          const std::string checkerName      = "ByteByByteStructureComparison";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Do not perform byte-by-byte comparisons between structures";
          const std::string longDescription  = "Structures may be padded with data to ensure that they are properly aligned in memory. The contents of the padding, and the amount of padding added is implementation defined. This can can lead to incorrect results when attempting a byte-by-byte comparison between structures.";
        } //End of namespace ByteByByteStructureComparison.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ByteByByteStructureComparison::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ByteByByteStructureComparison::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ByteByByteStructureComparison.YourParameter"]);

   }

void
CompassAnalyses::ByteByByteStructureComparison::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  


     if(isSgFunctionCallExp(node))
     {
       bool isFirstVarStruct = false;
       bool isSecondVarStruct = false;

       SgFunctionCallExp* callSite = isSgFunctionCallExp(node);
       ROSE_ASSERT(callSite != NULL);

       SgFunctionRefExp* function = isSgFunctionRefExp(callSite->get_function());

       if(function != NULL)
       {
         SgFunctionSymbol* functionSymbol = function->get_symbol();
    
         // first of all, check if the function call is "memcmp"
         if(functionSymbol->get_name() != NULL && functionSymbol->get_name() == "memcmp")
         {
           // then secondly, look at parameters
           SgExprListExp* args = callSite->get_args();

           if(args != NULL)
           {
             SgExpressionPtrList args_list = args->get_expressions();

             // peek first two parameters
             SgExpressionPtrList::iterator i = args_list.begin();

             SgExpression* first_arg = *i;
             i++;
             SgExpression* second_arg = *i;

             if(isSgCastExp(first_arg))
             {
               // go down 
               SgExpression* first_variable = isSgCastExp(first_arg)->get_operand_i();
               ROSE_ASSERT(first_variable != NULL);

               SgType* first_type = first_variable->get_type();
               first_type = first_type->findBaseType();

               if(isSgClassType(first_type))
                   //output->addOutput(new CheckerOutput(node));
                 isFirstVarStruct = true;

               //std::cout << "first: " << first_type->unparseToString() << std::endl;

             } else {
               SgType* first_type = first_arg->get_type();
               first_type = first_type->findBaseType();

               if(isSgClassType(first_type))
                   //output->addOutput(new CheckerOutput(node));
                 isFirstVarStruct = true;
               //std::cout << "first: " << first_type->unparseToString() << std::endl;
             }

             if(isSgCastExp(second_arg))
             {
               // go down 
               SgExpression* second_variable = isSgCastExp(second_arg)->get_operand_i();
               ROSE_ASSERT(second_variable != NULL);

               SgType* second_type = second_variable->get_type();
               second_type = second_type->findBaseType();

               if(isSgClassType(second_type))
                 isSecondVarStruct = true;
                   //output->addOutput(new CheckerOutput(node));

               //std::cout << "second: " << second_type->unparseToString() << std::endl;

             } else {
               //SgType* second_type = second_arg->get_symbol()->get_declaration()->get_typeptr();
               SgType* second_type = second_arg->get_type();
               second_type = second_type->findBaseType();

               if(isSgClassType(second_type))
                 isSecondVarStruct = true;
                   //output->addOutput(new CheckerOutput(node));
               //std::cout << "second: " << second_type->unparseToString() << std::endl;
             }

           }
         }
       }

       if(isFirstVarStruct && isSecondVarStruct)
         output->addOutput(new CheckerOutput(node));
     }

   } //End of the visit function.
   
