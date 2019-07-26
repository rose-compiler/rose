// This program checks Fortran and C files and declares if they
// have matching interfaces based on functions arguments.
//

#include "checkFortranInterfaces.h"

#define PRINT_TRAVERSAL 0

SgType* InterfaceChecker::fortran_to_c_type(SgType* f_type, SgExpression* kind_type, bool hasValue)
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

// TODO: convert for case in kind_name
   switch (f_type->variantT())
      {
      case V_SgTypeInt:
         {
            if (isDefaultKind || kind_name == "c_int") {
               c_type = SageBuilder::buildIntType();
            }
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
               c_type = SageBuilder::buildSignedCharType();
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

// TODO: implement (note fall through)
      case V_SgTypeBool:
      case V_SgTypeChar:
      default:
         {
            std::cout <<  "Type " << f_type->class_name() <<" is not yet implemented" << std::endl;
            break;
         }
      }

   if (hasValue == false) {
      c_base_type = c_type;
      c_type = SageBuilder::buildPointerType(c_base_type);
   }

   return c_type;
}

bool InterfaceChecker::compare_type_array(SgArrayType* f_array, SgArrayType* c_array)
{
   bool matches = false;
   bool hasValue = false;

   // compare arrays' base and kind type
   SgType* c_arr_type = c_array->get_base_type();
   SgType* f_arr_type = f_array->get_base_type();
   SgExpression* f_kind_type = f_arr_type->get_type_kind();
   SgDeclarationStatement* f_decl_stmt = f_arr_type->getAssociatedDeclaration();
   if (f_decl_stmt != NULL) {
      hasValue = f_decl_stmt->get_declarationModifier().get_typeModifier().isValue();
   }

   std::cout << "f_arr_type is " << f_arr_type->class_name() << ": " << f_arr_type << std::endl;
   SgType* f_to_c_type = fortran_to_c_type(f_arr_type, f_kind_type, hasValue);

   if (f_to_c_type != NULL) {
      if (f_to_c_type == c_arr_type) {
         int f_rank = f_array->get_rank();

         // checking fortran rank is equal to 1 to match c
         if (f_rank == 1) {
            // get c number of elements
            int c_arr_size = c_array->get_number_of_elements();

            // get number of elements for fortran through dim info
            SgExpressionPtrList & f_ptr_list = f_array->get_dim_info()->get_expressions();
            SgExpression* f_expr =  f_ptr_list[0];
            SgIntVal* f_int_val = isSgIntVal(f_expr);
            ROSE_ASSERT(f_int_val);

            int f_int_size = f_int_val->get_value();

            // comparing array size
            if (c_arr_size == f_int_size) {
               matches = true;
            }
         }
      }
   }
   
   return matches;
}

bool InterfaceChecker::compare_type_class(SgClassType* f_class, SgClassType* c_class)
{
   bool matches = false;

   std::cout << "f_class->class_name() is " << f_class->class_name()
             << ": " << f_class << " f_class->get_name() is " << f_class->get_name() << std::endl;
   std::cout << "c_class->class_name() is " << c_class->class_name()
             << ": " << c_class << " c_class->get_name() is " << c_class->get_name() << std::endl;

   SgDerivedTypeStatement* f_struct_decl = isSgDerivedTypeStatement(f_class->get_declaration());
   SgClassDeclaration* c_struct_decl = isSgClassDeclaration(c_class->get_declaration());
   ROSE_ASSERT(f_struct_decl && c_struct_decl);

   SgClassDeclaration* f_def_decl = isSgDerivedTypeStatement(f_struct_decl->get_definingDeclaration());
   SgClassDeclaration* c_def_decl = isSgClassDeclaration(c_struct_decl->get_definingDeclaration());
   ROSE_ASSERT(f_def_decl && c_def_decl);

   SgDeclarationStatementPtrList & f_decl_stmt_ptr_list = f_def_decl->get_definition()->get_members();
   SgDeclarationStatementPtrList & c_decl_stmt_ptr_list = c_def_decl->get_definition()->get_members();
   int f_size = f_decl_stmt_ptr_list.size();
   int c_size = c_decl_stmt_ptr_list.size();

   // WARNING: struct size has to be same and order of types has to be same
   if (f_size == c_size) {
      for (int i = 0; i < f_size; i++) {
         SgVariableDeclaration* f_var = isSgVariableDeclaration(f_decl_stmt_ptr_list[i]);
         SgVariableDeclaration* c_var = isSgVariableDeclaration(c_decl_stmt_ptr_list[i]);
         ROSE_ASSERT(f_var && c_var);

         SgInitializedName* f_name = f_var->get_definition()->get_vardefn();
         SgInitializedName* c_name = c_var->get_definition()->get_vardefn();

         matches = compare_types(f_name, c_name);

         if (!matches) {
            return matches;
         }
      }
   }
   else {
      std::cout << "ptr list size for structs do not match" << std::endl;
   }

   return matches;
}

bool InterfaceChecker::compare_types(SgInitializedName* f_name, SgInitializedName* c_name)
{
   bool matches = false;
   SgType* c_type = c_name->get_type();
   SgType* f_type = f_name->get_type();

   switch (f_type->variantT())
      {
      case V_SgArrayType:
         {
            SgArrayType* f_array = isSgArrayType(f_type);
            SgArrayType* c_array = isSgArrayType(c_type);
            ROSE_ASSERT(f_array && c_array);
            matches = compare_type_array(f_array, c_array);
            break;
         }
      case V_SgClassType:
         {
            SgClassType* f_class = isSgClassType(f_type);
            SgClassType* c_class = isSgClassType(c_type);
            ROSE_ASSERT(f_class && c_class);
            matches = compare_type_class(f_class, c_class);
            break;
         }
      case V_SgTypeFloat:
      case V_SgTypeComplex:
      case V_SgTypeBool:
      case V_SgTypeInt:
         {
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
                  matches = true;
               }
            }
            else {
               std::cout << "f_to_c_type is NULL, returning false" << std::endl;
            }
            break;
         }
      default:
         {
            std::cout << "compare_types(): Type " << f_type->class_name() << " not yet implemented." << std::endl;
            break;
         }
      }

   return matches;
}

bool InterfaceChecker::compare_decl(SgFunctionDeclaration* f_func_dec, SgFunctionDeclaration* c_func_dec)
{
   bool matches = false;
   const SgInitializedNamePtrList & f_name_list = f_func_dec->get_args();
   const SgInitializedNamePtrList & c_name_list = c_func_dec->get_args();

   // check number of arguments
   if (f_name_list.size() == c_name_list.size()) {
      // check return types
      SgType* f_return_type = f_func_dec->get_orig_return_type();
      SgType* c_return_type = c_func_dec->get_orig_return_type();

      if (f_return_type == c_return_type) {
         for (int i = 0; i < f_name_list.size(); i++) {
            if (compare_types(f_name_list[i], c_name_list[i])) {
               matches = true;
            }
            else {
               return false;
            }
         }
      }
      else {
         std::cout << "   compare_decl(): Different return types, returning false" << std::endl;
      }
   }
   else {
      std::cout << "   compare_decl(): Different number of arguments, returning false" << std::endl;
   }

   return matches;
}

bool InterfaceChecker::check_interface(mapType map_f, mapType map_c)
{
   bool eqv = false;

   // check to ensure neither map is empty
   if (map_f.size() != 0 && map_c.size() != 0) {
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
            std::cerr << "Error: Matching function name not found" << std::endl;
            return false;
         }
      }
   }
   else {
      std::cout << "WARNING: One or both of the maps are of size 0" << std::endl;
      std::cout << "map_f: size " << map_f.size() << ", map_c: size " << map_c.size() << std::endl;
   }

   return eqv;
}


// Constructor
Traversal::Traversal()
{
}


// Constructor
SynthesizedAttribute::SynthesizedAttribute()
{
}


// Constructor
InheritedAttribute::InheritedAttribute()
{
}


InheritedAttribute Traversal::evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute)
{
#if PRINT_TRAVERSAL
   std::cout << "InheritedAttribute():   astNode = " << astNode << " = " << astNode->class_name() << std::endl;
#endif

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

#if PRINT_TRAVERSAL
            SgInitializedNamePtrList & name_list = functionDeclaration->get_args();
            for (auto name: name_list)
            {
               std::cout << "\t   get_args() = " << name->get_type()->class_name() << " " <<name->get_name() << std::endl;
            }
#endif
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

         if (functionDeclaration != NULL) {
            std::string func_name = functionDeclaration->get_name().getString();
            map_f.insert(mapPair(func_name, functionDeclaration));

#if PRINT_TRAVERSAL
            SgInitializedNamePtrList & name_list = functionDeclaration->get_args();
            for (auto name: name_list)
            {
               std::cout << "\t   getargs() = " << name->get_type()->class_name() << " " <<name->get_name() << std::endl;
            }
#endif
         }

         break;
      }

      default:
      {
         break;
      }
   }

#if PRINT_TRAVERSAL
   if (functionDeclaration != NULL) {
      std::cout << "\tThe current size of map_c_or_cxx is " << map_c_or_cxx.size() << std::endl;
      std::cout << "\tThe current size of map_f is " << map_f.size() << std::endl;
   }
#endif

   return inheritedAttribute;
}


SynthesizedAttribute Traversal::evaluateSynthesizedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes)
{
   SynthesizedAttribute localResult;
#if PRINT_TRAVERSAL
   std::cout << "SynthesizedAttribute(): astNode = " << astNode << " = " << astNode->class_name() << std::endl;
#endif

   return localResult;
}
