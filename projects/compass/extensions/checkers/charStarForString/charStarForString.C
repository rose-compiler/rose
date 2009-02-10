// Char Star For String
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_CHAR_STAR_FOR_STRING_H
#define COMPASS_CHAR_STAR_FOR_STRING_H

namespace CompassAnalyses
   { 
     namespace CharStarForString
        { 
        /*! \brief Char Star For String: Add your description here 
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
                    SgTypedefDeclaration* stringTypedef;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_CHAR_STAR_FOR_STRING_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Char Star For String Analysis
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "compass.h"
// #include "charStarForString.h"

using namespace std;

namespace CompassAnalyses
   {

      namespace CharStarForString
        { 

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

 
       const std::string checkerName      = "CharStarForString";

       // Descriptions should not include the newline character "\n".
       const std::string shortDescription = "char* should be used instead of std::string";
       const std::string longDescription  = "char* should be used instead of std::string";
     }//End of namespace CharStarForString.
   } //End of namespace CompassAnalyses.

CompassAnalyses::CharStarForString::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {
   }

CompassAnalyses::CharStarForString::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CharStarForString.YourParameter"]);
     stringTypedef = NULL;


   }

void
CompassAnalyses::CharStarForString::Traversal::
visit(SgNode* node)
   {
      SgTypedefDeclaration* typeDecl = isSgTypedefDeclaration(node);
 
      //Find the typedef for string
      if(stringTypedef == NULL)
       {
         //Find the typedef for std::string
         if(typeDecl != NULL)
          {
             if(typeDecl->get_name().getString() == "string")
                  stringTypedef = typeDecl;

         }
       }else{
             //Find variables with std::string type
             SgInitializedNamePtrList initPtrLst;

             if( SgVariableDeclaration* varDecl = isSgVariableDeclaration(node) )
                  initPtrLst =  varDecl->get_variables();
             else if(SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node))
                  initPtrLst =  funcDecl->get_args();

             for( SgInitializedNamePtrList::iterator iItr = initPtrLst.begin(); 
                  iItr != initPtrLst.end(); ++iItr ){
                  SgType* varType = (*iItr)->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
                  if( varType == stringTypedef->get_type())
                     output->addOutput(new CheckerOutput(*iItr));
             }

             //Handle typedefs as a special case a std::string is hidden by a 
             //typedef
             if(typeDecl != NULL)
               {
                  SgType* varType = typeDecl->get_base_type()->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
 
                if( varType == stringTypedef->get_type() )
                      output->addOutput(new CheckerOutput(typeDecl));
               }

      } 
     
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::CharStarForString::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CharStarForString::Traversal(params, output);
}

extern const Compass::Checker* const charStarForStringChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::CharStarForString::checkerName,
        CompassAnalyses::CharStarForString::shortDescription,
        CompassAnalyses::CharStarForString::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
