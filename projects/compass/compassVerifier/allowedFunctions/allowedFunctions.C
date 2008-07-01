// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Allowed Functions Analysis
// Author: Gary M. Yuan
// Date: 19-June-2008

#include "compass.h"
#include "allowedFunctions.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription), isGenerateCurrentListOfAllowedFunctions(false), allowedFunctionIndex(0), outf(0)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AllowedFunctions.YourParameter"]);

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

/*         if( allowedFunctionIndex > 0 )
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

/*         if( allowedFunctionIndex > 0 )
         {
           (*outf) << ss.str() << "="
                   << inputParameters[ss.str()] << std::endl;
         } //if( allowedFunctionIndex > 0 ) */
       } //for
     } //try
     catch( const Compass::ParameterNotFoundException &e )
     {
     } //catch( const Compass::ParameterNotFoundException &e )

     if( allowedFunctionIndex > 0 )
     {
       int i = 0;
       for( std::set<std::string>::iterator itr = allowedFunctionSet.begin();
            itr != allowedFunctionSet.end(); itr++ )
       {
         (*outf) << "AllowedFunctions.Function" << i++ 
                 << "=" << *itr << std::endl;
       } //for, itr functions

       i = 0;

       for( std::vector<std::string>::iterator itr = allowedNamespaces.begin();
            itr != allowedNamespaces.end(); itr++ )
       {
         (*outf) << "AllowedFunctions.Namespace" << i++
                 << "=" << *itr << std::endl;
       } //for, itr namespaces
     } //if( allowedFunctionIndex > 0 ), re-write existing allow list
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

/*std::string
CompassAnalyses::AllowedFunctions::Traversal::
getQualifiedNamespace( const std::string & fname )
{
  return fname.substr( 0, fname.find_last_of("::")-1 );
} //getQualifiedNamespace( const std::string & fname ) */

void
CompassAnalyses::AllowedFunctions::Traversal::
functionDeclarationHandler( 
  const SgFunctionDeclaration *fdecl,
  std::string frefFileName,
  SgNode *node )
{
  ROSE_ASSERT(fdecl != NULL);

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

      if( nsItr == allowedNamespaces.end() )
        output->addOutput(new CheckerOutput(node, ss.str()));
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
