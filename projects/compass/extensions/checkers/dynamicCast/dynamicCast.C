// Dynamic Cast
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DYNAMIC_CAST_H
#define COMPASS_DYNAMIC_CAST_H

namespace CompassAnalyses
   { 
     namespace DynamicCast
        { 
        /*! \brief Dynamic Cast: Add your description here 
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
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_DYNAMIC_CAST_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Dynamic Cast Analysis
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "compass.h"
// #include "dynamicCast.h"
//#include "helpFunctions.h"



using namespace std;

namespace CompassAnalyses{

     namespace DynamicCast
        { 
          const std::string checkerName      = "DynamicCast";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "When downcasting C-style casts should not be used.";
          const std::string longDescription  = "When downcasting dynamic_cast<> should be used. If C-style casts are used an error is reported.";

      //The function hasVirtualMember(...) return true if the class declartion or one of
      //it's base classes has a virtual member.
      bool hasVirtualMember(SgClassDeclaration* clDecl)
        {  
          // tps (Aug 5, 2008), fixed this bug because it crashed.
          if (clDecl->get_definingDeclaration()==NULL) return false;
          SgClassDefinition* clDef = isSgClassDeclaration(clDecl->get_definingDeclaration())->get_definition();

          if(clDef == NULL)
              return false;
      
          SgDeclarationStatementPtrList& memb = clDef->get_members();
         
          for(SgDeclarationStatementPtrList::iterator i = memb.begin();
                   i != memb.end(); ++i )
            {
               SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(*i);
               if( funcDecl != NULL )
                   if( funcDecl->get_functionModifier().isVirtual() == true)
                      return true;
            }
 
          SgBaseClassPtrList&  baseCl = clDef->get_inheritances();

          for(SgBaseClassPtrList::iterator i = baseCl.begin();
                   i != baseCl.end(); ++i )
            {
               if( hasVirtualMember( (*i)->get_base_class() ) == true  )
                  return true;
            } 

          return false;
        }


     //The function 'isBaseClassOf(...)' returns true if the first paramater is a base
     //class of the second paramater.
     bool isBaseClassOf(SgClassDeclaration* base, SgClassDeclaration* clDecl)
        {  
          ROSE_ASSERT(base != NULL);
          ROSE_ASSERT(clDecl != NULL);

          if( clDecl->get_definingDeclaration() == NULL )
              return false;

          SgClassDefinition* clDef = isSgClassDeclaration(clDecl->get_definingDeclaration())->get_definition();

          if(clDef == NULL)
              return false;
      
         SgBaseClassPtrList&  baseClLst = clDef->get_inheritances();

          for(SgBaseClassPtrList::iterator i = baseClLst.begin();
                   i != baseClLst.end(); ++i )
            {
                
               SgClassDeclaration* baseCl = (*i)->get_base_class();

               if( base ==  baseCl )
                  return true;

               if( isBaseClassOf( baseCl, clDecl ) == true  )
                  return true;
            } 

          return false;
        }


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




        } //End of namespace DynamicCast.
   } //End of namespace CompassAnalyses.



CompassAnalyses::DynamicCast::
CheckerOutput::CheckerOutput (SgType* to, SgType* from,
                              SgCastExp* node)
   : OutputViolationBase(node,checkerName,shortDescription),
     toType(to), fromType(from), IRnode(node)
   {}


CompassAnalyses::DynamicCast::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DynamicCast.YourParameter"]);


   }

void
CompassAnalyses::DynamicCast::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
        SgCastExp* ce = isSgCastExp(node);
        if (ce == NULL) return;


        //If we already have a dynamic cast then no violation of the
        //rule is possible
        if(ce->cast_type() == SgCastExp::e_dynamic_cast) 
           return;


        //Type casted to
        SgType* toType       = ce->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
        string  toTypeName   = toType->unparseToString(); //TransformationSupport::getTypeName(toType);

        //Type casted from
        SgType* fromType     = ce->get_operand()->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
        string  fromTypeName = fromType->unparseToString(); //TransformationSupport::getTypeName(fromType);

        string  constPrefix = "const ";

        //std::cout << "From:"<< fromTypeName << " To:" << toTypeName << std::endl;

        //Vector representing all the subtypes of a type, like e.g Foo* is [*, Foo]
        std::vector<SgType*> fromTypeVec = typeVectorFromType(fromType);
        std::vector<SgType*> toTypeVec   = typeVectorFromType(toType);

        bool isDownCast = false;
        
        ROSE_ASSERT(fromTypeVec.size() > 0);
        ROSE_ASSERT(toTypeVec.size() > 0);

//        SgClassType* fromClType =  isSgClassType( toTypeVec[fromTypeVec.size()-1]);
//        SgClassType* toClType   =  isSgClassType( toTypeVec[toTypeVec.size()-1] );
          SgClassType* fromClType =  isSgClassType( fromType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE ) );
          SgClassType* toClType   =  isSgClassType( toType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE ) );


        if( ( fromClType != NULL ) && 
            ( toClType   != NULL ) &&
            ( hasVirtualMember(isSgClassDeclaration( fromClType->get_declaration() ) ) == true ) &&
            ( isBaseClassOf( isSgClassDeclaration(fromClType->get_declaration()), 
                             isSgClassDeclaration(toClType->get_declaration())   ) == true )
          ){
             isDownCast = true; 
           }

        if(isDownCast == true){
            output->addOutput(new CheckerOutput(toType, fromType, ce));
        }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DynamicCast::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DynamicCast::Traversal(params, output);
}

extern const Compass::Checker* const dynamicCastChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DynamicCast::checkerName,
        CompassAnalyses::DynamicCast::shortDescription,
        CompassAnalyses::DynamicCast::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
