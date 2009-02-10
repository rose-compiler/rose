// Const Cast
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_CONST_CAST_H
#define COMPASS_CONST_CAST_H

namespace CompassAnalyses
   { 
     namespace ConstCast
        { 
        /*! \brief Const Cast: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             {
                    SgType* toType;
                    SgType* fromType;
                    SgCastExp* IRnode;
  
               public:
                    CheckerOutput(SgType* to, SgType* from,
                              SgCastExp* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
                    int maximumConstCast;


               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_CONST_CAST_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Const Cast Analysis
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "compass.h"
// #include "constCast.h"
//#include "helpFunctions.h"
using namespace std;
namespace CompassAnalyses
   { 

 
		

     namespace ConstCast
        { 
          const std::string checkerName      = "ConstCast";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Casting the const away form a type is not allowed.";
          const std::string longDescription  = "Casting from a type to a type which is the same without a const midifier is not allowed.";

          SgType* findBaseType(SgType* sageType){
               ROSE_ASSERT( sageType != NULL);
               SgType* baseType = sageType;
		           
	             switch(sageType->variantT())
  		          {
        		    	case V_SgReferenceType: 
		  	             { 
	       		     	   baseType = isSgReferenceType(sageType)->get_base_type();
            		 		 break;
	                   }
	                case V_SgPointerType:
	                  {
	                   baseType =  isSgPointerType(sageType)->get_base_type(); 
	                   break;
	                   }
             			case V_SgTypedefType:
	                   {
		                while(isSgTypedefType(baseType) != NULL) 
		         	        baseType = isSgTypedefType(baseType)->get_base_type();
			              break;
			               }
			            default:
                    break; 
     	            };	
              	ROSE_ASSERT ( baseType  != NULL );
                return baseType;
          };       

          vector<SgType*> typeVectorFromType(SgType* sageType){
	           vector<SgType*> typeVector;
			
		       	ROSE_ASSERT (sageType != NULL);
			      SgType* baseType = sageType; 
		        SgType* previousBaseType = NULL;
           
            while(previousBaseType != baseType) 
		         {
              previousBaseType = baseType; 
            
                 switch(baseType->variantT()){
		                  case V_SgReferenceType:
                      case V_SgPointerType:
		                   	typeVector.push_back(baseType);
                      case V_SgTypedefType:
   		                  break;
                      default:
                          typeVector.push_back(baseType);
                          break;
            	      }
              baseType = findBaseType(baseType);
		          ROSE_ASSERT(baseType != NULL);

    		    }; 
		   
		     return typeVector;

         };

        } //End of namespace ConstCast.
   } //End of namespace CompassAnalyses.

                    
CompassAnalyses::ConstCast::
CheckerOutput::CheckerOutput(SgType* to, SgType* from,
                              SgCastExp* node)
  : OutputViolationBase(node,checkerName,shortDescription),
    toType(to), fromType(from), IRnode(node)

{};

CompassAnalyses::ConstCast::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ConstCast.YourParameter"]);

   }


void
CompassAnalyses::ConstCast::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
        SgCastExp* ce = isSgCastExp(node);
        if (ce == NULL) return;

        //std::cout << "Looking for cast exp" << std::endl;

        SgType* toType       = ce->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
        SgType* fromType     = ce->get_operand()->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);

        string  toTypeName ; 
        string  fromTypeName;
        string  constPrefix;

        toTypeName   = toType->unparseToString(); //TransformationSupport::getTypeName(toType);
        fromTypeName = fromType->unparseToString(); //TransformationSupport::getTypeName(fromType);
        constPrefix = "const ";

        //std::cout << "From:"<< fromTypeName << " To:" << toTypeName << std::endl;

        //Vector representing all the subtypes of a type, like e.g Foo* is [*, Foo]
        std::vector<SgType*> fromTypeVec = typeVectorFromType(fromType);
        std::vector<SgType*> toTypeVec   = typeVectorFromType(toType);

        bool isEqual = true;

        //Make sure that we have a const cast since we strip the modifier type
        //when checking for equivalence below
        if( fromTypeName.substr(0,constPrefix.length()) != constPrefix  ){
             isEqual = false;
        }

        //Check for equivalence between the types before and after casting
        if(fromTypeVec.size() == toTypeVec.size())
        for(size_t i = 0; i < fromTypeVec.size(); i++)
             {
               SgType* fromType     = fromTypeVec[i]->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_TYPEDEF_TYPE);
               SgType* toType       = toTypeVec[i]->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_TYPEDEF_TYPE);
               string  fromTypeName;
               string  toTypeName;

                     fromTypeName = fromType->unparseToString(); //TransformationSupport::getTypeName(fromType);
                     toTypeName   = toType->unparseToString(); //TransformationSupport::getTypeName(fromType);
                    //std::cout <<  toTypeVec[i]->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_TYPEDEF_TYPE)->class_name() << std::endl;
                    //std::cout << "From1 " << fromTypeName << " To1: " << toTypeName << std::endl;  

                    if( ( isSgPointerType(fromType) != NULL ) &&
                        ( isSgPointerType(toType)   != NULL ) ) {}
                    else if( ( isSgReferenceType(fromType) != NULL ) &&
                        ( isSgReferenceType(toType)   != NULL ) ) {}
                    else if( fromTypeName == toTypeName ){} 
                    /*else if( ( constPrefix.length()< fromTypeName.length() ) &&
                        ( toTypeName == fromTypeName.substr(constPrefix.length()) )   ){}*/
                    else isEqual = false;
        }
        else 
           isEqual = false;

        if(isEqual == true){

            output->addOutput(new CheckerOutput(toType, fromType, ce));
        }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ConstCast::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ConstCast::Traversal(params, output);
}

extern const Compass::Checker* const constCastChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ConstCast::checkerName,
        CompassAnalyses::ConstCast::shortDescription,
        CompassAnalyses::ConstCast::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
