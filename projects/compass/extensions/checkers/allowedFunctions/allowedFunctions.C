// Allowed Functions
// Author: Gary M. Yuan
// Date: 19-June-2008

#include "rose.h"
#include "compass.h"
#include "string_functions.h"

#ifndef COMPASS_ALLOWED_FUNCTIONS_H
#define COMPASS_ALLOWED_FUNCTIONS_H

using namespace rose;

namespace CompassAnalyses
   { 
     namespace AllowedFunctions
        { 
        /*! \brief Allowed Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(
                      SgNode* node, const std::string &what);
             };

       // Specification of Checker Traversal Implementation

          enum OP_CODES{ CONTINUE=0, GENERATE_CURRENT_LIST=1 };

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
               private:
                 std::vector< std::string > libraryPaths;
                 bool isGenerateCurrentListOfAllowedFunctions;
                 int allowedFunctionIndex;
                 std::string sourceDirectory;
                 std::ofstream *outf;
                 std::set<std::string> allowedFunctionSet;
                 std::vector<std::string> allowedNamespaces;
//                 StringUtility::FileNameClassification classification;

//               int parseParameter( const std::string & param );

                 void functionDeclarationHandler( 
                   const SgFunctionDeclaration *fdecl, 
                   std::string frefFileName,
                   SgNode *node );

                 void uniqueNameGenerator(
                   std::stringstream &ss,
                   const SgFunctionDeclaration *fdecl,
                   std::string &qname );

//               std::string getQualifiedNamespace( const std::string &fname );

                 std::string typeVariantT( SgType *type, int vT ); 

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n);//{ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ALLOWED_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Allowed Functions Analysis
// Author: Gary M. Yuan
// Date: 19-June-2008

#include <stdlib.h>
#include <errno.h>

#include <boost/algorithm/string.hpp>

#include "compass.h"
// #include "allowedFunctions.h"

using namespace rose::StringUtility;
using namespace boost::algorithm;

namespace CompassAnalyses
   { 
     namespace AllowedFunctions
        { 
          const std::string checkerName      = "AllowedFunctions";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that all function references and member function references are part of an accepted list of functions.";
          const std::string longDescription  = "This checker checks that all function references and member function references are part of an accepted list of functions defined in compass_parameters. These functions were built using this checker assuming that the input code was trusted source. All functions appearing in that source have been accepted.";
        } //End of namespace AllowedFunctions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AllowedFunctions::
CheckerOutput::CheckerOutput ( SgNode* node, const std::string & what )
   : OutputViolationBase(node,checkerName,shortDescription + " '" + what + "' is not allowed.")
   {}

CompassAnalyses::AllowedFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output), isGenerateCurrentListOfAllowedFunctions(false), allowedFunctionIndex(0), outf(0)
   {
     homeDir( sourceDirectory );
//     sourceDirectory = "/home/yuan5/ROSE/JUN1708/SRC/projects/compass";

     try
     {
       int index = atoi(
         inputParameters["AllowedFunctions.FunctionNum"].c_str());

       if( index >= 0 )
       {
         outf = new std::ofstream(
           inputParameters["AllowedFunctions.OutFile"].c_str(), std::ios::out );

         isGenerateCurrentListOfAllowedFunctions = true;
         allowedFunctionIndex = index;

         (*outf) << "Compass.RuleSelection="
                 << inputParameters["Compass.RuleSelection"]
                 << std::endl
                 << "AllowedFunctions.OutFile="
                 << inputParameters["AllowedFunctions.OutFile"]
                 << std::endl;
       } //if( index >= 0 )
       else
       {
         outf = new std::ofstream( "/dev/null", std::ios::app );
       }

       ROSE_ASSERT( outf != 0 && outf->good() == true );

       for( int i = 0; ; ++i )
       {
         std::stringstream ss;
         ss << "AllowedFunctions.Function" << i;
         allowedFunctionSet.insert(inputParameters[ss.str()]);

         if( allowedFunctionIndex > 0 )
         {
           (*outf) << ss.str() << "=" 
                   << inputParameters[ss.str()] << std::endl;
         } //if( allowedFunctionIndex > 0 ) */
       } //for
     } //try
     catch( const Compass::ParameterNotFoundException &e )
     {
     } //catch( const Compass::ParameterNotFoundException &e )

     try
     {
       for( int i = 0; ; ++i )
       {
         std::stringstream ss;
         ss << "AllowedFunctions.Namespace" << i;
         allowedNamespaces.push_back(inputParameters[ss.str()]);

         if( allowedFunctionIndex >= 0 )
         {
           (*outf) << ss.str() << "="
                   << inputParameters[ss.str()] << std::endl;
         } //if( allowedFunctionIndex > 0 ) */
       } //for
     } //try
     catch( const Compass::ParameterNotFoundException &e )
     {
     } //catch( const Compass::ParameterNotFoundException &e )

     try
     {
       for( int i = 0; ; ++i )
       {
         std::stringstream ss;
         ss << "AllowedFunctions.Library" << i;
         std::string path( inputParameters[ss.str()] );

         replace_all( path, "//", "/" );

         libraryPaths.push_back( path );

         char rpath[4096] = "\0";
         realpath(path.c_str(), rpath);
         libraryPaths.push_back( std::string(rpath) );

         if( allowedFunctionIndex >= 0 )
         {
           (*outf) << ss.str() << "=" << path << std::endl;
         } //if( allowedFunctionIndex >= 0 )
       } //for i
     } //try
     catch( const Compass::ParameterNotFoundException &e )
     {
     } //catch( const Compass::ParameterNotFoundException &e ) 
   }

void 
CompassAnalyses::AllowedFunctions::Traversal::
run(SgNode *n)
{
  this->traverse(n,preorder);

  if( isGenerateCurrentListOfAllowedFunctions == true )
    (*outf) << "AllowedFunctions.FunctionNum=" << allowedFunctionIndex << "\n";

  outf->close();
}

std::string CompassAnalyses::AllowedFunctions::Traversal::
typeVariantT( SgType *type, int vT )
{
  switch( vT )
  {
    case V_SgArrayType:
    {
      SgType *baseType = isSgArrayType(type)->get_base_type();
      ROSE_ASSERT(baseType != NULL);

      return this->typeVariantT( baseType, baseType->variantT() ) + "[]";
    } break;
    case V_SgMemberFunctionType:          //fall
    case V_SgPartialFunctionType:         //fall
    case V_SgPartialFunctionModifierType: //fall thru
    case V_SgFunctionType:
    {
      SgFunctionType *fType = isSgFunctionType(type);
      ROSE_ASSERT(fType != NULL);

      return fType->get_mangled_type().getString();
    } break;
    case V_SgModifierType:
    {
      SgType *baseType = isSgModifierType(type)->get_base_type();
      ROSE_ASSERT(baseType != NULL);

      return this->typeVariantT(baseType,baseType->variantT());
    } break;
    case V_SgClassType:    //fall
    case V_SgEnumType:     //fall
    case V_SgTypedefType:  //fall thru
    case V_SgNamedType:
    {
      SgNamedType *nType = isSgNamedType(type);
      ROSE_ASSERT(nType != NULL);

      return nType->get_name().getString();
    } break;
    case V_SgPointerMemberType:   //fall thru
    case V_SgPointerType:
    {
      SgType *baseType = isSgPointerType(type)->get_base_type();
      ROSE_ASSERT(baseType != NULL);

      return "*" + this->typeVariantT(baseType,baseType->variantT());
    } break;
    case V_SgQualifiedNameType:
    {
      SgType *baseType = isSgQualifiedNameType(type)->get_base_type();
      ROSE_ASSERT(baseType != NULL);

      return this->typeVariantT(baseType,baseType->variantT());
    } break;
    case V_SgReferenceType:
    {
      SgType *baseType = isSgReferenceType(type)->get_base_type();
      ROSE_ASSERT(baseType != NULL);

      return "&" + this->typeVariantT(baseType,baseType->variantT());
    } break;
    case V_SgTemplateType:
    {
      return "template<T>";
//      return isSgTemplateType(type)->get_mangled().getString();
    } break;
    case V_SgTypeBool: return "bool";
    case V_SgTypeChar: return "char";
    case V_SgTypeComplex: return "complex";
    case V_SgTypeDefault: return "default";
    case V_SgTypeDouble: return "double";
    case V_SgTypeEllipse: return "...";
    case V_SgTypeFloat: return "float";
    case V_SgTypeGlobalVoid: return "global void";
    case V_SgTypeImaginary: return "imaginary";
    case V_SgTypeInt: return "int";
    case V_SgTypeLong: return "long";
    case V_SgTypeLongDouble: return "long double";
    case V_SgTypeLongLong: return "long long";
    case V_SgTypeShort: return "short";
    case V_SgTypeSignedChar: return "signed char";
    case V_SgTypeSignedInt: return "signed int";
    case V_SgTypeSignedLong: return "signed long";
    case V_SgTypeSignedShort: return "signed short";
    case V_SgTypeString: return "string";
    case V_SgTypeUnknown: return isSgTypeUnknown(type)->get_mangled().getString();
    case V_SgTypeUnsignedChar: return "unsigned char";
    case V_SgTypeUnsignedInt: return "unsigned int";
    case V_SgTypeUnsignedLong: return "unsigned long";
    case V_SgTypeUnsignedLongLong: return "unsigned long long";
    case V_SgTypeUnsignedShort: return "unsigned short";
    case V_SgTypeVoid: return "void";
    case V_SgTypeWchar: return "wchar";
    default: break;
  } //switch( vT )

  std::cerr << "Got Unknown Variant: " << vT << std::endl;

  return "unknown";
}

void CompassAnalyses::AllowedFunctions::Traversal::
uniqueNameGenerator( 
  std::stringstream &ss, 
  const SgFunctionDeclaration *fdecl,
  std::string &qname )
{
  std::string qualifiedName( fdecl->get_qualified_name().str() );
  qname.assign(qualifiedName);

  SgFunctionType *fType = isSgFunctionType( fdecl->get_type() );
  ROSE_ASSERT(fType != NULL);

  SgType *fReturnType = fType->get_return_type();

  ROSE_ASSERT(fReturnType != NULL);

  std::string fReturnTypeName(
    typeVariantT(fReturnType, fReturnType->variantT()) );

  ss << fReturnTypeName << "," << qualifiedName << ",";

  const SgInitializedNamePtrList & arguments = fdecl->get_args();

  for( SgInitializedNamePtrList::const_iterator itr = arguments.begin();
       itr != arguments.end(); itr++ )
  {
    SgType *type = (*itr)->get_type();

    ss << this->typeVariantT(type, type->variantT()) << ",";
  } //for itr

  return;
}

void
CompassAnalyses::AllowedFunctions::Traversal::
functionDeclarationHandler( 
  const SgFunctionDeclaration *fdecl,
  std::string frefFileName,
  SgNode *node )
{
  ROSE_ASSERT(fdecl != NULL);

  StringUtility::FileNameClassification fdef_classification;
  StringUtility::FileNameClassification fref_classification;

  fref_classification = classifyFileName( frefFileName, sourceDirectory );


// CHECKS FOR USER DEFINED FUNCTIONS AND CALLS ORIGINATING FROM LIBRARIES
  if( fref_classification.getLocation() == FILENAME_LOCATION_LIBRARY )
    return;
  else if( fdecl->get_definition() != NULL )
  {
    fdef_classification = classifyFileName( 
      fdecl->get_definition()->getFilenameString(), sourceDirectory );

    if( fdef_classification.getLocation() == FILENAME_LOCATION_USER ) return;
  } //else if( fdecl->get_definition() != NULL )
  else
  {
    for( std::vector< std::string >::iterator itr = libraryPaths.begin();
         itr != libraryPaths.end(); itr++ )
    {
//      if( frefFileName.find( "include-staging" ) != std::string::npos )
//        std::cout << frefFileName << std::endl;
//      std::cout << *itr << "\t" << frefFileName << std::endl;
      if( frefFileName.find( *itr ) != std::string::npos ) return;
    } //for
  } //else
// CHECKS FOR USER DEFINED FUNCTIONS AND CALLS ORIGINATING FROM LIBRARIES END

//  std::cout << "RefFname: " << frefFileName << "\t" << fref_classification.getLibraryName() << std::endl;

  std::stringstream ss;
  std::string qname;
  this->uniqueNameGenerator(ss, fdecl,qname);
  std::set<std::string>::iterator afItr = allowedFunctionSet.find(ss.str());

  if( isGenerateCurrentListOfAllowedFunctions == true && 
      afItr == allowedFunctionSet.end() )
  {
    std::set<std::string>::iterator localAfItr = allowedFunctionSet.find(qname);

    if( localAfItr == allowedFunctionSet.end() )
    {
      (*outf) << "AllowedFunctions.Function" << allowedFunctionIndex << "=";
      (*outf) << ss.str() << std::endl;

      allowedFunctionSet.insert( ss.str() ); 

      allowedFunctionIndex++;
    } //if( localAfItr == allowedFunctionSet.end() )
  } //if( isGenerateCurrentListOfAllowedFunctions == true )
  else
  {
    if( afItr == allowedFunctionSet.end() )
    {
      std::vector<std::string>::iterator nsItr = allowedNamespaces.begin();
      for( ; nsItr != allowedNamespaces.end(); nsItr++ )
      {
        if( ss.str().find( *nsItr, 0 ) != std::string::npos ) break;
      } //for, failed to find in functions--checking namespaces/classes 

/*      SgFunctionDefinition *fdef = fdecl->get_definition();

      if( fdef != NULL )
      {
        fdef_classification = 
          classifyFileName( fdef->getFilenameString(), sourceDirectory );
      }

      if( fdef != NULL && 
          (fdef_classification.getLocation() == FILENAME_LOCATION_USER) )
      {
      } // */
      //else if( nsItr != allowedNamespaces.end() )
      if( nsItr != allowedNamespaces.end() )
      {
      } //else if, namespace entry
      else
      {
        //(08/11/08) tps: commented out the additional cout
        //std::cout << "RefFname: " << frefFileName << "\t" << fref_classification.getLibraryName() << std::endl;
        output->addOutput(new CheckerOutput(node, ss.str()));
      }

    } //if( afItr == allowedFunctionSet.end() )
  } //else, check for allowed function, namespace, class

  return;
} //functionReferenceHandler( const SgFunctionDeclaration *fdecl )

void
CompassAnalyses::AllowedFunctions::Traversal::
visit(SgNode* node)
   {
     switch( node->variantT() )
     {
       case V_SgFunctionRefExp:
       {
         SgFunctionRefExp *fref = isSgFunctionRefExp(node);

         ROSE_ASSERT(fref != NULL);

         SgFunctionDeclaration *fdecl = fref->get_symbol()->get_declaration();

         this->functionDeclarationHandler( 
           fdecl, fref->getFilenameString(), fref );
       } break; //case V_SgFunctionRefExp
       case V_SgMemberFunctionRefExp:
       {
         SgMemberFunctionRefExp *fref = isSgMemberFunctionRefExp(node);

         ROSE_ASSERT(fref != NULL);

         SgFunctionDeclaration *fdecl = fref->get_symbol()->get_declaration();

         this->functionDeclarationHandler(
           fdecl, fref->getFilenameString(), fref );
       } break; //case V_SgMemberFunctionRefExp
       default: break;
     } //switch( node->variantT() )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AllowedFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AllowedFunctions::Traversal(params, output);
}

extern const Compass::Checker* const allowedFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AllowedFunctions::checkerName,
        CompassAnalyses::AllowedFunctions::shortDescription,
        CompassAnalyses::AllowedFunctions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
