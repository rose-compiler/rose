// This program checks Fortran and C files and declares if they
// have matching interfaces based on functions arguments.
//

#include "rose.h"

typedef std::map <std::string, SgFunctionDeclaration*> mapType;
typedef std::pair<std::string, SgFunctionDeclaration*> mapPair;

class InterfaceChecker
{
  public:

   SgType* fortran_to_c_type(SgType* f_type, SgExpression* kind_type, bool hasValue)
   {
      SgType* c_type = NULL;
      SgType* c_base_type = NULL;
      bool isDefaultKind = false;
      std::string kind_name = "";
      
      SgVarRefExp* kind_exp = isSgVarRefExp(kind_type);
      if (kind_exp == NULL) {
         isDefaultKind = true;
      }
      else {
         SgVariableSymbol* kind_symbol = kind_exp->get_symbol();
         ROSE_ASSERT(kind_symbol);
         kind_name = kind_symbol->get_name().str();
      }

      // ISSUE: C_LONG and c_long both work, but for the others I've tested
      // only the lowercase version works
      switch (f_type->variantT())
      {
         case V_SgTypeInt:
            {
               if (isDefaultKind || kind_name == "c_int") {
                  c_type = SageBuilder::buildIntType();
               }
               // need to check caps
               else if (kind_name == "c_short") {
                  c_type = SageBuilder::buildShortType();
               }
               else if (kind_name == "c_long") {
                  c_type = SageBuilder::buildLongType();
               }
               else if (kind_name == "c_long_long") {
                  c_type = SageBuilder::buildLongLongType();
               }
               else if (kind_name == "c_signed_char") {
                  //                  c_type = SageBuilder::buildSignedCharType();
                  c_type = SageBuilder::buildUnsignedCharType();
               }
               else if (kind_name == "c_size_t") {
                  c_type = SageBuilder::buildLongType();
               }

#if 0
               else if (kind_name == "c_int8_t") {
               }
               else if (kind_name == "c_int16_t") {
               }
               else if (kind_name == "c_int32_t") {
               }
               else if (kind_name == "c_int64_t") {
               }
               else if (kind_name == "c_int_lease8_t") {
               }
               else if (kind_name == "c_int_lease16_t") {
               }
               else if (kind_name == "c_int_lease32_t") {
               }
               else if (kind_name == "c_int_lease64_t") {
               }
               else if (kind_name == "c_int_fast8_t") {
               }
               else if (kind_name == "c_int_fast16_t") {
               }
               else if (kind_name == "c_int_fast32_t") {
               }
               else if (kind_name == "c_int_fast64_t") {
               }
               else if (kind_name == "c_intmax_t") {
               }
               else if (kind_name == "c_intptr_t") {
               }
               else if (kind_name == "c_ptrdiff_t") {
               }
#endif
               else {
                  std::cout << "Type " << kind_name << " is not implemented" << std::endl;
               }

               break;
            }
         case V_SgTypeFloat:
            {
               if (isDefaultKind || kind_name == "c_float") {
                  c_type = SageBuilder::buildFloatType();
               }
               else if (kind_name == "c_double") {
                  c_type = SageBuilder::buildDoubleType();
               }
               else if (kind_name == "c_long_double") {
                  c_type = SageBuilder::buildLongDoubleType();
               }
               else {
                  std::cout << "Type " << kind_name << " is not implemented" << std::endl;
               }
               break;
            }
         case V_SgTypeComplex:
            {
               // what should the default kind be?
               if (isDefaultKind) {
                  c_type = SageBuilder::buildComplexType();
               }

#if 0
               else if (kind_name == "c_float_complex") {
               }
               else if (kind_name == "c_double_complex") {
               }
               else if (kind_name == "c_long_double_complex") {
               }

#endif
               else {
                  std::cout << "Type " << kind_name << " is not implemented" << std::endl;
               }
               break;
            }
         case V_SgTypeBool:
            {
               break;
            }
         case V_SgTypeChar:
            {
               break;
            }
         default:
            {
               std::cout << "Type " << " is not implemented" << std::endl;
               break;
            }
      }

      if (hasValue == false) {
         c_base_type = c_type;
         c_type = SageBuilder::buildPointerType(c_base_type);
      }

      return c_type;
   }
   
   bool compare_types(SgInitializedName* f_name, SgInitializedName* c_name)
   {
      SgType* c_type = c_name->get_type();
      SgType* f_type = f_name->get_type();
      SgExpression* f_kind_type = f_type->get_type_kind();
      bool hasValue = false;
      SgVariableDeclaration* var_dec = isSgVariableDeclaration(f_name->get_declaration());
      if (var_dec!= NULL) {
         SgTypeModifier type_mod = var_dec->get_declarationModifier().get_typeModifier();
         hasValue = type_mod.isValue();
      }

      SgType* f_to_c_type = fortran_to_c_type(f_type, f_kind_type, hasValue);

      if (f_to_c_type != NULL) {
         std::cout << "f_to_c_type is " << f_to_c_type->class_name() << ": " << f_to_c_type << std::endl;
         if (f_to_c_type == c_type) {
            std::cout << "f_to_c_type is equal to c_type, returning true" << std::endl;
            return true;
         }
      }
      else {
         std::cout << "f_to_c_type is NULL, returning false" << std::endl;
         return false;
      }

      std::cout << "\t compare_types(): Types don't match, returning false" << std::endl;

      return false;
   }
   
   bool compare_decl(SgFunctionDeclaration* f_func_dec, SgFunctionDeclaration* c_func_dec)
   {
      // check for number of arguments
      if (f_func_dec->get_args().size() != c_func_dec->get_args().size()) {
         std::cout << "   compare_decl(): Different number of arguments, returning false" << std::endl;
         return false;
      }

      // check for matching return types
      SgType* f_return_type = f_func_dec->get_orig_return_type();
      SgType* c_return_type = c_func_dec->get_orig_return_type();

      if (f_return_type != c_return_type) {
         std::cout << "   compare_decl(): Different return types, returning false" << std::endl;
         return false;
      }
      else {
         std::cout << "   compare_decl(): Return types match" << std::endl;
      }

      const SgInitializedNamePtrList & f_name_list = f_func_dec->get_args();
      const SgInitializedNamePtrList & c_name_list = c_func_dec->get_args();
      bool found = false;

   // TODO - make sure same # of arguments
      // Is done at the top of the function
   //--------------------------
      int i = 0;
      BOOST_FOREACH(SgInitializedName* f_name, f_name_list)
      {
         if (compare_types(f_name, c_name_list[i])) {
            found = true;
         }
         else {
            std::cout << "   compare_decl(): returning false" << std::endl;
            return false;
         }
         i++;
      }

      std::cout << "   compare_decl(): returning true" << std::endl;
      return found;
   }

   // return type?!?
   bool check_interface(mapType map_f, mapType map_c)
   {
      bool eqv = false;

      // check to ensure neither map is empty
      if (map_f.size() == 0 || map_c.size() == 0) {
         // Could make WARNING: in red
         std::cout << "WARNING: One or both of the maps are of size 0" << std::endl;
         std::cout << "map_f: size " << map_f.size() << ", map_c: size " << map_c.size() << std::endl;
         return false;
      }
      
      mapType::iterator itr;
      for (itr = map_f.begin(); itr != map_f.end(); ++itr) {
         std::string bind_func_name = itr->second->get_binding_label();
         std::string c_func_name;
         mapType::iterator got_it = map_c.find(bind_func_name);
         

         if (got_it != map_c.end()) {
            c_func_name = got_it->first;
            if (compare_decl(itr->second, got_it->second) == true) {
               std::cout << "check_interface(): function declarations match" << std::endl;
               eqv = true;
            }
            else {
               std::cerr << "Error: Function names matches but function declarations do not match" << std::endl;
               return false;
            }
         }
         else {
            std::cout << "Error: Matching function name not found" << std::endl;
         }
      }

      return eqv;
   }

};


// Inherited attribute (see ROSE Tutorial (Chapter 9)).
class InheritedAttribute
{
  public:
   InheritedAttribute();
};

// Constructor (not really needed)
InheritedAttribute::InheritedAttribute()
{
}

// Synthesized attribute (see ROSE Tutorial (Chapter 9)).
class SynthesizedAttribute
{
  public:
   SynthesizedAttribute();
};

// Constructor
SynthesizedAttribute::SynthesizedAttribute()
{
}

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
{
  public:

   mapType map_c_or_cxx;
   mapType map_f;

   mapType getFortranMap() { return map_f       ; }
   mapType getCMap()       { return map_c_or_cxx; }

   Traversal();

       // Functions required
   InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, InheritedAttribute inheritedAttribute);
   SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList);
};


InheritedAttribute
Traversal::evaluateInheritedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute)
{
   std::cout << "InheritedAttribute():   astNode = " << astNode << " = " << astNode->class_name() << std::endl;

   SgFunctionDeclaration* functionDeclaration = NULL;

   switch (SageInterface::getEnclosingFileNode(astNode)->get_inputLanguage())
   {
   case SgFile::e_C_language:
   case SgFile::e_Cxx_language:
      {
         functionDeclaration = isSgFunctionDeclaration(astNode);
         if (functionDeclaration != NULL) {
            std::string func_name = functionDeclaration->get_name().getString();
            map_c_or_cxx.insert(mapPair(func_name, functionDeclaration));
            
            SgInitializedNamePtrList & name_list = functionDeclaration->get_args();
      
            BOOST_FOREACH(SgInitializedName* name, name_list)
            {
               std::cout << "\t   get_args() = " << name->get_type()->class_name() << " " <<name->get_name() << std::endl;
            }

         }

         break;
      }

      case SgFile::e_Fortran_language:
      {
         SgInterfaceBody* interfaceBody = isSgInterfaceBody(astNode);
         functionDeclaration = isSgFunctionDeclaration(astNode);

         if (interfaceBody != NULL) {
            functionDeclaration = interfaceBody->get_functionDeclaration();
         }

         if (interfaceBody != NULL || functionDeclaration != NULL) {
            std::string func_name = functionDeclaration->get_name().getString();
            map_f.insert(mapPair(func_name, functionDeclaration));

            SgInitializedNamePtrList & name_list = functionDeclaration->get_args();
      
            BOOST_FOREACH(SgInitializedName* name, name_list)
            {
               std::cout << "\t   getargs() = " << name->get_type()->class_name() << " " <<name->get_name() << std::endl;
            }
         }

         break;
      }

      default:
      {
         break;
      }
   }

   if (functionDeclaration != NULL) {
      std::cout << "\tThe current size of map_c_or_cxx is " << map_c_or_cxx.size() << std::endl;
      std::cout << "\tThe current size of map_f is " << map_f.size() << std::endl;
   }

   return inheritedAttribute;
}


SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes)
   {
     SynthesizedAttribute localResult;

     std::cout << "SynthesizedAttribute(): astNode = " << astNode << " = " << astNode->class_name() << std::endl;

     return localResult;
   }


Traversal::Traversal()
   {
   }


int
main ( int argc, char* argv[] )
   {
  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal astTraversal;

  // Call the traversal starting at the project (root) node of the AST
     astTraversal.traverseInputFiles(project,inheritedAttribute);

     std::cout << "\n" << std::endl;

     mapType map_f = astTraversal.getFortranMap();
     mapType map_c_or_cxx = astTraversal.getCMap();

     InterfaceChecker check;
     if (check.check_interface(map_f, map_c_or_cxx)) {
        std::cout << "END OF PROGRAM: map's functions match" << std::endl;
     }
     else {
        std::cout << "END OF PROGRAM: map's functions do not match" << std::endl;
     }

  // return 0;
     return backendCompilesUsingOriginalInputFile(project);
   }
