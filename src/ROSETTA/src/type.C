
#include "grammar.h"
#include "ROSETTA_macros.h"
#include "AstNodeClass.h"

// What should be the behavior of the default constructor for Grammar

void
Grammar::setUpTypes ()
   {
  // This function sets up the type system for the grammar.  In this case it implements the
  // C++ grammar, but this will be modified to permit all grammars to contain elements of the
  // C++ grammar.  Modified grammars will add and subtract elements from this default C++ grammar.

     NEW_TERMINAL_MACRO ( TypeUnknown         , "TypeUnknown",          "T_UNKNOWN" );
     NEW_TERMINAL_MACRO ( TypeChar            , "TypeChar",             "T_CHAR" );
     NEW_TERMINAL_MACRO ( TypeSignedChar      , "TypeSignedChar",       "T_SIGNED_CHAR" );
     NEW_TERMINAL_MACRO ( TypeUnsignedChar    , "TypeUnsignedChar",     "T_UNSIGNED_CHAR" );
     NEW_TERMINAL_MACRO ( TypeShort           , "TypeShort",            "T_SHORT" );
     NEW_TERMINAL_MACRO ( TypeSignedShort     , "TypeSignedShort",      "T_SIGNED_SHORT" );
     NEW_TERMINAL_MACRO ( TypeUnsignedShort   , "TypeUnsignedShort",    "T_UNSIGNED_SHORT" );
     NEW_TERMINAL_MACRO ( TypeInt             , "TypeInt",              "T_INT" );
     NEW_TERMINAL_MACRO ( TypeSignedInt       , "TypeSignedInt",        "T_SIGNED_INT" );
     NEW_TERMINAL_MACRO ( TypeUnsignedInt     , "TypeUnsignedInt",      "T_UNSIGNED_INT" );
     NEW_TERMINAL_MACRO ( TypeLong            , "TypeLong",             "T_LONG" );
     NEW_TERMINAL_MACRO ( TypeSignedLong      , "TypeSignedLong",       "T_SIGNED_LONG" );
     NEW_TERMINAL_MACRO ( TypeUnsignedLong    , "TypeUnsignedLong",     "T_UNSIGNED_LONG" );
     NEW_TERMINAL_MACRO ( TypeVoid            , "TypeVoid",             "T_VOID" );
     NEW_TERMINAL_MACRO ( TypeGlobalVoid      , "TypeGlobalVoid",       "T_GLOBAL_VOID" );
     NEW_TERMINAL_MACRO ( TypeWchar           , "TypeWchar",            "T_WCHAR" );
     NEW_TERMINAL_MACRO ( TypeFloat           , "TypeFloat",            "T_FLOAT" );
     NEW_TERMINAL_MACRO ( TypeDouble          , "TypeDouble",           "T_DOUBLE" );
     NEW_TERMINAL_MACRO ( TypeLongLong        , "TypeLongLong",         "T_LONG_LONG" );
     NEW_TERMINAL_MACRO ( TypeSignedLongLong  , "TypeSignedLongLong",   "T_SIGNED_LONG_LONG" );
     NEW_TERMINAL_MACRO ( TypeUnsignedLongLong, "TypeUnsignedLongLong", "T_UNSIGNED_LONG_LONG" );
     NEW_TERMINAL_MACRO ( TypeLongDouble      , "TypeLongDouble",       "T_LONG_DOUBLE" );
     NEW_TERMINAL_MACRO ( TypeFloat80         , "TypeFloat80",          "T_FLOAT80" );
     NEW_TERMINAL_MACRO ( TypeFloat128        , "TypeFloat128",         "T_FLOAT128" );
     NEW_TERMINAL_MACRO ( TypeString          , "TypeString",           "T_STRING" );
     NEW_TERMINAL_MACRO ( TypeBool            , "TypeBool",             "T_BOOL" );

  // Rasmussen (2/18/2020): Added TypeFixed for Jovial
     NEW_TERMINAL_MACRO ( TypeFixed           , "TypeFixed",            "T_FIXED" );

     //SK(08/20/2015): TypeMatrix to represent a Matlab matrix type
     NEW_TERMINAL_MACRO ( TypeMatrix          , "TypeMatrix",           "T_MATRIX" );

     //SK(08/20/2015): TypeTuple to represent the return type of a Matlab function that can return multiple types
     NEW_TERMINAL_MACRO ( TypeTuple           , "TypeTuple",            "T_TUPLE");

  // DQ (7/29/2014): Added nullptr type (I think we require this for C++11 support).
     NEW_TERMINAL_MACRO ( TypeNullptr         , "TypeNullptr",          "T_NULLPTR" );

     NEW_TERMINAL_MACRO ( TypeComplex         , "TypeComplex",          "T_COMPLEX" );
     NEW_TERMINAL_MACRO ( TypeImaginary       , "TypeImaginary",        "T_IMAGINARY" );
     NEW_TERMINAL_MACRO ( TypeDefault         , "TypeDefault",          "T_DEFAULT" );
     NEW_TERMINAL_MACRO ( PointerMemberType   , "PointerMemberType",    "T_MEMBER_POINTER" );
     NEW_TERMINAL_MACRO ( ReferenceType       , "ReferenceType",        "T_REFERENCE" );

  // DQ (7/29/2014): Added nullptr type (I think we required this for C++11 support..
     NEW_TERMINAL_MACRO ( RvalueReferenceType , "RvalueReferenceType",  "T_RVALUE_REFERENCE" );

  // DQ (8/2/2014): Adding support for C++11 decltype() (which should be an new SgType in the IR).
     NEW_TERMINAL_MACRO ( DeclType            , "DeclType",             "T_DECLTYPE" );

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     NEW_TERMINAL_MACRO ( TypeOfType          , "TypeOfType",             "T_TYPEOF_TYPE" );

     NEW_TERMINAL_MACRO ( TypeCAFTeam         , "TypeCAFTeam",          "T_CAFTEAM" );

  // DQ (3/24/2014): Adding support for 128 bit integers.
     NEW_TERMINAL_MACRO ( TypeUnsigned128bitInteger, "TypeUnsigned128bitInteger",  "T_UNSIGNED_128BIT_INTEGER" );
     NEW_TERMINAL_MACRO ( TypeSigned128bitInteger  , "TypeSigned128bitInteger"  ,  "T_SIGNED_128BIT_INTEGER" );

  // DQ (7/30/2014): Fixed spelling of T_LABEL tag.
  // DQ (2/1/2011): Added label type to support Fortran alternative return arguments in function declarations.
     NEW_TERMINAL_MACRO ( TypeLabel           , "TypeLabel",            "T_LABEL" );

  // DQ (5/7/2004): Made this a AstNodeClass, was previously a nonterminal 
  // with a TemplateInstantiationType derived from it.
  //   NEW_TERMINAL_MACRO ( ClassType           , "ClassType",            "T_CLASS" );

  // DQ (8/18/2011): Java specific support for generics.
     NEW_TERMINAL_MACRO ( JavaParameterizedType , "JavaParameterizedType", "T_JAVA_PARAM" );
     NEW_TERMINAL_MACRO ( JavaQualifiedType , "JavaQualifiedType", "T_JAVA_QUALIFIED" );
     NEW_TERMINAL_MACRO ( JavaWildcardType, "JavaWildcardType", "T_JAVA_WILD" );

  // DQ (2/10/2014): Added SgNamedType IR nodes for Philippe.
     NEW_TERMINAL_MACRO ( JavaUnionType     , "JavaUnionType", "T_JAVA_UNION" );
     NEW_TERMINAL_MACRO ( JavaParameterType , "JavaParameterType", "T_JAVA_PARAMETER" );

  // Rasmussen (5/10/2019): Added a Table type for Jovial (tables are structs with array dimensions)
     NEW_TERMINAL_MACRO ( JovialTableType , "JovialTableType", "T_JOVIAL_TABLE" );

     //
     // [DT] 5/11/2000 -- Added TemplateType.  Should it be called TemplateInstantiationType
     //      to maintain symmetry with TemplateInstantiationDecl and TemplateInstantiationSymbol?
     //
     //      Later, 5/11 -- Changed it to TemplateInstantiationType.
     //

  // DQ (5/7/2004): There is no need for a TemplateInstantiationType since once the template is
  // intantiated it actually has a more specific C++ type (class, function, member function, etc.)
  // But we do need a type for the TemplateDeclarationStatement since every sysmbol should have a type
  // as a general rule.  So cancle TemplateInstantiationType and build TemplateType instead.
  // NEW_TERMINAL_MACRO ( TemplateInstantiationType, "TemplateInstantiationType", "T_TEMPLATE_INSTANTIATION" );
     NEW_TERMINAL_MACRO ( TemplateType        , "TemplateType",         "T_TEMPLATE" );
     NEW_TERMINAL_MACRO ( EnumType            , "EnumType",             "T_ENUM" );
     NEW_TERMINAL_MACRO ( TypedefType         , "TypedefType",          "T_TYPEDEF" );

  // TV (04/11/2018): Introducing representation for non-real "stuff" (template parameters)
     NEW_TERMINAL_MACRO ( NonrealType, "NonrealType", "T_NONREAL");

  // TV (04/11/2018): Type for declaration of auto typed variables
     NEW_TERMINAL_MACRO ( AutoType        , "AutoType",         "T_AUTO" );

     NEW_TERMINAL_MACRO ( ModifierType        , "ModifierType",         "T_MODIFIER" );

  // DQ (4/14/2004): Support for new function modifiers (wrapper class design
  //                 is used here consistant with SgModifierType).
     NEW_TERMINAL_MACRO ( PartialFunctionModifierType, "PartialFunctionModifierType", "T_PARTIAL_FUNCTION_MODIFIER" );
     NEW_TERMINAL_MACRO ( ArrayType           , "ArrayType",            "T_ARRAY" );
     NEW_TERMINAL_MACRO ( TypeEllipse         , "TypeEllipse",          "T_ELLIPSE" );

  // FMZ (4/8/2009): Added for Cray Pointer
     NEW_TERMINAL_MACRO ( TypeCrayPointer           , "TypeCrayPointer",            "T_CRAY_POINTER" );

#if 0
     NEW_TERMINAL_MACRO ( PartialFunctionType , "PartialFunctionType",  "T_PARTIAL_FUNCTION" );
#else
     NEW_NONTERMINAL_MACRO (PartialFunctionType, PartialFunctionModifierType,
                           "PartialFunctionType","T_PARTIAL_FUNCTION", true);
#endif

  // It seems that ROSETTA has a bug in the copy constructor that forces us 
  // to specify product rules with more than one AstNodeClass or nonTerminal object 
  // (under some circomstances at least).
#define ROSETTA_BUG false
#if !ROSE_MICROSOFT_OS
#if ROSETTA_BUG
  // Dummy AstNodeClass so that we can add avoid the NonTerminal copy constructor (this may be a bug)
  // I think that this is not a problem now (Terminals can be reused in multiple BNF statements) though
  // except as a work around this makes little sense.
     NEW_TERMINAL_MACRO ( UnknownMemberFunctionType , "UnknownMemberFunctionType", "UnknownMemberFunctionTag" );
#endif
#endif

     NEW_NONTERMINAL_MACRO (MemberFunctionType,
                            PartialFunctionType /* | UnknownMemberFunctionType */,
                            "MemberFunctionType","T_MEMBERFUNCTION", true);
     NEW_NONTERMINAL_MACRO (FunctionType,
                            MemberFunctionType /* | UnknownMemberFunctionType */,
                            "FunctionType","T_FUNCTION", true);
     NEW_NONTERMINAL_MACRO (PointerType,
                            PointerMemberType,
                            "PointerType","T_POINTER", true);

  // Note that TemplateInstantiationType can't be both a child of the NamedType and ClassType
  // This is an error that ROSETTA currently does not catch and which I need to discuss with Danny Thorne
  // For the moment it seems that ROSETTA is by default ignoring this connection!
  // printf ("WARNING: TemplateInstantiationType specificed as a child of both NamedType and ClassType! \n");

  // NEW_NONTERMINAL_MACRO (NamedType,
  //                        ClassType | TemplateInstantiationType | EnumType | TypedefType,
  //                        "NamedType","T_NAME");
#if 0
  // DQ (2/10/2014): Original code
     NEW_NONTERMINAL_MACRO (NamedType,
                            ClassType | JavaParameterizedType | JavaQualifiedType | EnumType | TypedefType,
                            "NamedType","T_NAME", false);
#else
  // DQ (2/10/2014): Added SgNamedType IR nodes for Philippe.
  // Rasmussen (5/10/2019): Added a Table type for Jovial (tables are structs with array dimensions)
     NEW_NONTERMINAL_MACRO (ClassType,
                            JavaParameterType | JovialTableType,
                            "ClassType","T_CLASS", true);
     NEW_NONTERMINAL_MACRO (NamedType,
                            ClassType | EnumType | TypedefType | NonrealType |
                            JavaParameterizedType | JavaQualifiedType | JavaWildcardType,
                            "NamedType","T_NAME", false);
#endif
 
  // DQ (5/11/2011): This is no longer used, and has not be used since the 3rd rewite of the name qualification
  // support in 2007.  We are now working on the 4rh rewrite of this horrible subject and it is not clear if it
  // should be revived.  I would rather place the name qualification information into the constructs the reference
  // the type, instead of having the type re wrapped as if it were a SgPointerType, SgReferenceType, etc.
  // DQ (12/21/2005): Support for qualified named types (wraps SgType (always a SgNamedType) with SgQualifiedName)
     NEW_TERMINAL_MACRO ( QualifiedNameType        , "QualifiedNameType",         "T_QUALIFIED_NAME" );

  // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
     NEW_TERMINAL_MACRO ( TypeChar16           , "TypeChar16",            "T_CHAR16" );
     NEW_TERMINAL_MACRO ( TypeChar32           , "TypeChar32",            "T_CHAR32" );

  // DQ (5/7/2004): Added TemplateType to be derived from SgType (this leaves room later to 
  // build more specific types for template classes ? template function, etc. unless they 
  // should be derived from there non-template associated types as is done for the template 
  // instantiations; this is a judgement call).  For now we can define a TemplateDeclarationStatement 
  // to have a TemplateType and since all templates are the same (until they are instatiated, 
  // likely there should only be a single TemplateType).
     NEW_NONTERMINAL_MACRO (Type,
          TypeUnknown          | TypeChar                | TypeSignedChar            | TypeUnsignedChar     | 
          TypeShort            | TypeSignedShort         | TypeUnsignedShort         | TypeInt              | 
          TypeSignedInt        | TypeUnsignedInt         | TypeLong                  | TypeSignedLong       | 
          TypeUnsignedLong     | TypeVoid                | TypeGlobalVoid            | TypeWchar            |
          TypeFloat            | TypeDouble              | TypeLongLong              | TypeSignedLongLong   |
          TypeUnsignedLongLong | TypeSigned128bitInteger | TypeUnsigned128bitInteger | TypeFloat80          |
          TypeLongDouble       | TypeString              | TypeBool                  | PointerType          |
          ReferenceType        | NamedType               | ModifierType              | FunctionType         |
          ArrayType            | TypeEllipse             | TemplateType              | QualifiedNameType    |
          TypeComplex          | TypeImaginary           | TypeDefault               | TypeCAFTeam          |
          TypeCrayPointer      | TypeLabel               | JavaUnionType             | RvalueReferenceType  | 
          TypeNullptr          | DeclType                | TypeOfType                | TypeMatrix           |
          TypeTuple            | TypeChar16              | TypeChar32                | TypeFloat128         |
          TypeFixed            | AutoType,
        "Type","TypeTag", false);

     //SK(08/20/2015): TypeMatrix and TypeTuple for Matlab

#if 1
  // ***********************************************************************
  // ***********************************************************************
  //                 Header File Generation (Declarations)
  // ***********************************************************************
  // ***********************************************************************

  // Generate these function declarations in the classes derived from Type (but not on Type)
  // Type.setSubTreeFunctionPrototype( "HEADER", "../Grammar/Common.code" );
  // Type.excludeFunctionPrototype   ( "HEADER", "../Grammar/Common.code" );

  // Type.setFunctionPrototype ( "HEADER", "../Grammar/Common.code" );
     Type.setFunctionPrototype ( "HEADER", "../Grammar/Type.code" );
     Type.setFunctionSource    ( "SOURCE_MAIN_TYPE", "../Grammar/Type.code" );
     Type.setAutomaticGenerationOfConstructor(false);
  // DQ (12/4/2004): Now we automate the generation of the destructors
  // Type.setAutomaticGenerationOfDestructor (false);

     Type.setSubTreeFunctionPrototype               ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );
     Type.excludeFunctionPrototype                  ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );

  // FMZ (2/9/2009): Added a flag for CoArray
     Type.setDataPrototype("bool","isCoArray","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/7/2004): This functionality was added to EDG 1.4 (and has not yet been added to EDG_3.3)
  // The purpose it to allow types to be marked internally as being associated with a template 
  // argument.  There may be more information required to nail down more details in this mapping 
  // (pointer to the template, the template argument (in the instantiation), the matching template 
  // parameter (in the template), etc.  IR nodes specific to template parameters and template 
  // arguments have not yet been added and will be added incrementally as more of the template 
  // support is brought on line.
  // [DT] 8/14/2000 -- substitutedForTemplateParam
     Type.setDataPrototype("int","substitutedForTemplateParam","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // MK: Type.excludeDataPrototype ("int","substitutedForTemplateParam","= 0");

  // DQ (4/23/2014): I would like to make this just NO_TRAVERSAL so that we can debug the type traversal through base_type data members.
  // Reference to reference type
  // Type.setDataPrototype("SgReferenceType*","ref_to","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     Type.setDataPrototype("SgReferenceType*","ref_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (4/23/2014): I would like to make this just NO_TRAVERSAL so that we can debug the type traversal through base_type data members.
  // Reference to pointer type
  // Type.setDataPrototype("SgPointerType*","ptr_to","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     Type.setDataPrototype("SgPointerType*","ptr_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (4/23/2014): I would like to make this just NO_TRAVERSAL so that we can debug the type traversal through base_type data members.
  // Reference to modifier nodes (I forget the purpose of this)
  // Type.setDataPrototype("SgModifierNodes*","modifiers","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     Type.setDataPrototype("SgModifierNodes*","modifiers","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Reference to typedef type \attention{(need to check that these are fully resolved within mapping from EDG)}
#if 1
  // DQ (4/23/2014): I would like to make this just NO_TRAVERSAL so that we can debug the type traversal through base_type data members.
  // Type.setDataPrototype("SgTypedefSeq*","typedefs","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     Type.setDataPrototype("SgTypedefSeq*","typedefs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
  // #else
  //     Type.setDataPrototype("SgTypePtrList","typedefs","",
  //               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

  // DQ (7/29/2014): Adding support for C++11 rvalue references.
     Type.setDataPrototype("SgRvalueReferenceType*","rvalue_ref_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/2/2014): Adding support for C++11 decltype().
     Type.setDataPrototype("SgDeclType*","decltype_ref_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     Type.setDataPrototype("SgTypeOfType*","typeof_ref_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 1
  // DQ (10/5/2010): I think we can move back to an implementation with DEF_DELETE now (the other uninitialized memory problem was fixed).
  // DQ (10/3/2010): Note that without the NO_DELETE the Fortran jacobi.f file will sometimes fail.
  // DQ (10/2/10): This is the better place for the Fortran kind mechanism (only meaningful for Fortran)
  // DQ (9/17/2007): Support for Fortran kind mechanism
  // Type.setDataPrototype("SgExpression*","type_kind","= NULL", NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
  // Type.setDataPrototype("SgExpression*","type_kind","= NULL", NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     Type.setDataPrototype("SgExpression*","type_kind","= NULL",
            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
#endif
     
#if 1
  // DQ (11/1/2015): Build the access functions, but don't let the set_* access function set the "p_isModified" flag.
  // DQ (1/24/2006): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes.  Beata appears to use attributes
  // on SgTypes (even though they are shared; doesn't appear to be a problem for them).
  // Type.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
  //        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     Type.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);
     Type.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     Type.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

#ifdef BUILD_X_VERSION_TERMINALS

#error "DEAD CODE"

  // Reference to X version of type (special target type for associated grammar)
     Type.setDataPrototype("SgX_ReferenceType*","X_ref_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // Pointer to X version of type (special target type for associated grammar)
     Type.setDataPrototype("SgX_PointerType*","X_ptr_to","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef HL_GRAMMARS

#error "DEAD CODE"

     X_Type.setFunctionPrototype     ( "HEADER_X_TYPE",     "../Grammar/Type.code" );
     X_Type.setAutomaticGenerationOfCopyFunction(false);
     non_X_Type.setFunctionPrototype ( "HEADER_NON_X_TYPE", "../Grammar/Type.code" );
#endif

  // The only two types that don't have a get_mangled() member function
     PartialFunctionType.excludeFunctionPrototype   ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );
     NamedType.excludeFunctionPrototype             ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );
  // NamedType.setAutomaticGenerationOfCopyFunction(false);

  // TypeUnknown.setFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
  // TypeUnknown.setFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );


  // DQ (1/31/2006): Need to support addition of builtin_type pointers into the IR using ROSETTA
  // static $CLASSNAME* builtin_type;

  // Use simple "static CLASSNAME builtin_type;" on most classed derived from Type
     Type.setSubTreeFunctionPrototype ( "HEADER_COMMON_CREATE_TYPE", "../Grammar/Type.code" );
     Type.excludeFunctionPrototype    ( "HEADER_COMMON_CREATE_TYPE", "../Grammar/Type.code" );

     Type.setSubTreeFunctionSource    ( "SOURCE_COMMON_CREATE_TYPE", "../Grammar/Type.code" );
     Type.excludeFunctionSource       ( "SOURCE_COMMON_CREATE_TYPE", "../Grammar/Type.code" );

#if 0
  // DQ (10/12/2014): This is no longer used (commented out code).

  // DQ (12/26/2005): Support for builtin type traversal
     Type.setSubTreeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     Type.excludeFunctionPrototype    ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     Type.setSubTreeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     Type.excludeFunctionSource       ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // DQ (12/26/2005): This function has to handle all the different bit field lengths
     TypeInt.excludeFunctionSource          ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ReferenceType.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ReferenceType.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     RvalueReferenceType.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     RvalueReferenceType.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ModifierType.excludeFunctionPrototype  ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ModifierType.excludeFunctionSource     ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PointerType.excludeFunctionPrototype   ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PointerType.excludeFunctionSource      ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ArrayType.excludeFunctionPrototype     ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     ArrayType.excludeFunctionSource        ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // Rasmussen (2/18/2020): Added TypeFixed for Jovial
     TypeFixed.excludeFunctionPrototype     ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeFixed.excludeFunctionSource        ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // DQ (8/2/2014): Adding support for C++11 decltype().
     DeclType.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     DeclType.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     TypeOfType.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeOfType.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

     TemplateType.excludeFunctionPrototype        ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TemplateType.excludeFunctionSource           ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
  // TemplateType.excludeFunctionPrototype        ( "HEADER_CREATE_TYPE_WITH_PARAMETER", "../Grammar/Type.code" );

     FunctionType.excludeFunctionPrototype        ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     FunctionType.excludeFunctionSource           ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     MemberFunctionType.excludeFunctionPrototype  ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     MemberFunctionType.excludeFunctionSource     ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PartialFunctionType.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PartialFunctionType.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PointerMemberType.excludeFunctionPrototype   ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     PointerMemberType.excludeFunctionSource      ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     QualifiedNameType.excludeFunctionPrototype       ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     QualifiedNameType.excludeFunctionSource          ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // FMZ (4/8/2009): Added for Cray Pointer
     TypeCrayPointer.excludeFunctionPrototype   ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeCrayPointer.excludeFunctionSource      ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

  // DQ (6/18/2007): Not sure if we need this.
  // TemplateType.excludeFunctionPrototype      ( "HEADER_CREATE_TYPE_WITH_PARAMETER", "../Grammar/Type.code" );
  // TemplateType.excludeFunctionSource         ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

     TypeComplex.excludeFunctionPrototype       ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeComplex.excludeFunctionSource          ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

     TypeImaginary.excludeFunctionPrototype     ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeImaginary.excludeFunctionSource        ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );

     TypeString.excludeFunctionPrototype ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
     TypeString.excludeFunctionSource    ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
#endif

#if 1
     TypeComplex.excludeFunctionPrototype       ( "HEADER_COMMON_CREATE_TYPE", "../Grammar/Type.code" );
     TypeComplex.excludeFunctionSource          ( "SOURCE_COMMON_CREATE_TYPE", "../Grammar/Type.code" );

     TypeImaginary.excludeFunctionPrototype     ( "HEADER_COMMON_CREATE_TYPE", "../Grammar/Type.code" );
     TypeImaginary.excludeFunctionSource        ( "SOURCE_COMMON_CREATE_TYPE", "../Grammar/Type.code" );
#endif

  // DQ (8/17/2010): Don't use the static builtin type for the SgTypeString IR node.
     TypeString.excludeFunctionPrototype ( "HEADER_CREATE_TYPE_WITH_PARAMETER", "../Grammar/Type.code" );
  // TypeString.excludeFunctionPrototype ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );

  // DQ (10/12/2014): I now think that the builtin_type data member should not be marked as 
  // part of the type traversal (DEF_TRAVERSAL). It is a static data member used within the
  // management of types to define the required sharing that we seek for all types independent
  // (but related to) the global type table support added 2 years ago.  Fixing this might be
  // a better way to support the ATerm support in ROSE and eliminate the special case handling 
  // to exclude this from the successor containers that are computed in the grammar.C file in 
  // the Grammar::buildTreeTraversalFunctions() function.  I wish to discuss this internally 
  // before making the change.

     TypeUnknown.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeChar.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeSignedChar.setDataPrototype       ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsignedChar.setDataPrototype     ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeShort.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeSignedShort.setDataPrototype      ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsignedShort.setDataPrototype    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeInt.setDataPrototype              ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeSignedInt.setDataPrototype        ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsignedInt.setDataPrototype      ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeLong.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeSignedLong.setDataPrototype       ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsignedLong.setDataPrototype     ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeVoid.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeGlobalVoid.setDataPrototype       ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeWchar.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeFloat.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeDouble.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeLongLong.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeSignedLongLong.setDataPrototype   ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsignedLongLong.setDataPrototype ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (3/24/2014): Adding support for 128 bit integers.
     TypeSigned128bitInteger.setDataPrototype   ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeUnsigned128bitInteger.setDataPrototype   ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (2/16/2018): Adding support for char16_t and char32_t for C99 and C++11.
     TypeChar16.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeChar32.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     TypeCAFTeam.setDataPrototype ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // FMZ (4/8/2009): Added for Cray pointer
     TypeCrayPointer.setDataPrototype ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     TypeLongDouble.setDataPrototype       ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeFloat80.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeFloat128.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // This type now has a length parameter, so we cannot use a single static builtin_type to represent all of the variations.
  // TypeString.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (10/5/2010): Removed the lengthScalar and definedUsingScalarLength data members (no longer required).
  // DQ (8/6/2010): Add string length to type (this type is used only in Fortran support, as I recall, but might be used for string literals in all langauges).
  // A fixed length string is the most common form, but fortran also permits a variable length string so we have to support an expression as well.
     TypeString.setDataPrototype           ("SgExpression*", "lengthExpression"        , "= NULL" , CONSTRUCTOR_PARAMETER   , BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
  // TypeString.setDataPrototype           ("size_t"       , "lengthScalar"            , "= 0"    , CONSTRUCTOR_PARAMETER   , BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TypeString.setDataPrototype           ("bool"         , "definedUsingScalarLength", "= false", NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeBool.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (7/29/2014): Added nullptr type (I think we require this for C++11 support).
     TypeNullptr.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);


     TypeMatrix.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     TypeTuple.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     
     TypeTuple.setDataPrototype("SgTypePtrList", "types", "",
                                  NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeTuple.setFunctionPrototype     ("HEADER_TYPE_TUPLE", "../Grammar/Type.code" );
     TypeTuple.setFunctionSource     ("SOURCE_TYPE_TUPLE", "../Grammar/Type.code" );

     
     TypeDefault.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // PointerType.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // ReferenceType.setDataPrototype        ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     NamedType.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // ModifierType.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // FunctionType.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // ArrayType.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     TypeEllipse.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // TemplateType.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // QualifiedNameType.setDataPrototype    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // UnknownMemberFunctionType.setDataPrototype   ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
  // PartialFunctionType.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     PartialFunctionModifierType.setDataPrototype ("static SgPartialFunctionModifierType*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (2/1/2011): Added label type to support Fortran alternative return arguments in function declarations.
     TypeLabel.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (11/28/2011): Adding template declaration support.
  // TemplateType.setDataPrototype         ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (8/18/2011): Java specific support for generics.
  // JavaParameterizedType.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // DQ (8/25/2006): We can't specify an initializer if this is a static pointer type 
  // (since this triggers the output of the initialization code in the constructor).
  // DQ (8/10/2006): Added support for different kinds of complex types (float,double, and long double)
  // JJW (11/22/2008): Changed to a static variable in a function for the builtin complex and imaginary type caches

#define CUSTOM_CREATE_TYPE_MACRO(typeObjectName,sourceCodeName,parameterString) \
        typeObjectName.excludeFunctionPrototype ( "HEADER_COMMON_CREATE_TYPE", "../Grammar/Type.code" ); \
        typeObjectName.excludeFunctionSource    ( "SOURCE_COMMON_CREATE_TYPE", "../Grammar/Type.code" ); \
        typeObjectName.setFunctionPrototype     ( "HEADER_CREATE_TYPE_WITH_PARAMETER", "../Grammar/Type.code" ); \
        typeObjectName.editSubstitute           ( "CREATE_TYPE_PARAMETER", parameterString ); \
        typeObjectName.setFunctionSource        ( sourceCodeName, "../Grammar/Type.code" );

#if 0
     // DQ (10/12/2014): This is no longer used (commented out code).
        typeObjectName.setFunctionPrototype     ( "HEADER_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" ); \
        typeObjectName.setFunctionSource        ( "SOURCE_BUILTIN_TYPE_SUPPORT", "../Grammar/Type.code" );
#endif

     CUSTOM_CREATE_TYPE_MACRO(TypeInt,
            "SOURCE_CREATE_TYPE_FOR_TYPE_INT_TYPE",
            "int sz = 0");
     TypeInt.setFunctionPrototype("HEADER_BUILTIN_FOR_CREATE_TYPE_WITH_PARAMETER_TYPE_ARRAY","../Grammar/Type.code" );

     CUSTOM_CREATE_TYPE_MACRO(PointerType,
            "SOURCE_CREATE_TYPE_FOR_POINTER_TYPE",
            "SgType* type = NULL");
     CUSTOM_CREATE_TYPE_MACRO(PointerMemberType,
            "SOURCE_CREATE_TYPE_FOR_POINTER_MEMBER_TYPE",
            "SgType* type = NULL, SgType* class_type = NULL");
     CUSTOM_CREATE_TYPE_MACRO(ReferenceType,
            "SOURCE_CREATE_TYPE_FOR_REFERENCE_TYPE",
            "SgType* type = NULL");

     CUSTOM_CREATE_TYPE_MACRO(RvalueReferenceType,
            "SOURCE_CREATE_TYPE_FOR_RVALUE_REFERENCE_TYPE",
            "SgType* type = NULL");

  // DQ (8/2/2014): Adding support for C++11 decltype().
     CUSTOM_CREATE_TYPE_MACRO(DeclType,
            "SOURCE_CREATE_TYPE_FOR_DECL_TYPE",
            "SgExpression* expr = NULL");

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     CUSTOM_CREATE_TYPE_MACRO(TypeOfType,
            "SOURCE_CREATE_TYPE_FOR_TYPEOF_TYPE",
            "SgExpression* expr = NULL");

  // DQ (12/4/2011): Adding support for template declarations into the AST.
  // CUSTOM_CREATE_TYPE_MACRO(ClassType,"SOURCE_CREATE_TYPE_FOR_CLASS_TYPE","SgClassDeclaration* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(ClassType,"SOURCE_CREATE_TYPE_FOR_CLASS_TYPE","SgDeclarationStatement* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(JavaParameterizedType,
            "SOURCE_CREATE_TYPE_FOR_JAVA_PARAMETERIZED_TYPE",
            "SgClassDeclaration* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(JavaQualifiedType,
            "SOURCE_CREATE_TYPE_FOR_JAVA_QUALIFIED_TYPE",
            "SgClassDeclaration* decl = NULL");

  // DQ (2/10/2014): Added SgNamedType IR nodes for Philippe.
     CUSTOM_CREATE_TYPE_MACRO(JavaUnionType,
            "SOURCE_CREATE_TYPE_FOR_JAVA_UNION_TYPE",
            "SgClassDeclaration* decl = NULL");

     CUSTOM_CREATE_TYPE_MACRO(JavaParameterType,
            "SOURCE_CREATE_TYPE_FOR_JAVA_PARAMETER_TYPE",
            "SgClassDeclaration* decl = NULL");

     CUSTOM_CREATE_TYPE_MACRO(JovialTableType,
            "SOURCE_CREATE_TYPE_FOR_JOVIAL_TABLE_TYPE",
            "SgClassDeclaration* decl = NULL");

   // DQ (11/28/2011): Make this more like the NamedType internal support.
  // CUSTOM_CREATE_TYPE_MACRO(TemplateType,"SOURCE_CREATE_TYPE_FOR_TEMPLATE_TYPE","SgTemplateInstantiationDecl* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(TemplateType,"SOURCE_CREATE_TYPE_FOR_TEMPLATE_TYPE","SgTemplateDeclaration* decl = NULL");

     CUSTOM_CREATE_TYPE_MACRO(NonrealType,"SOURCE_CREATE_TYPE_FOR_NONREAL_TYPE","SgNonrealDecl* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(AutoType,"SOURCE_CREATE_TYPE_FOR_AUTO_TYPE","SgNode* node = NULL");

     CUSTOM_CREATE_TYPE_MACRO(JavaWildcardType,
            "SOURCE_CREATE_TYPE_FOR_JAVA_WILDCARD_TYPE",
            "SgClassDeclaration *decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(TemplateType,
            "SOURCE_CREATE_TYPE_FOR_TEMPLATE_TYPE",
            "SgTemplateInstantiationDecl* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(EnumType,
            "SOURCE_CREATE_TYPE_FOR_ENUM_TYPE",
            "SgEnumDeclaration* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(TypedefType,
            "SOURCE_CREATE_TYPE_FOR_TYPEDEF_TYPE",
            "SgTypedefDeclaration* decl = NULL");
     CUSTOM_CREATE_TYPE_MACRO(ModifierType,
            "SOURCE_CREATE_TYPE_FOR_MODIFIER_TYPE",
            "SgType* type = NULL, unsigned int x = 0");
     CUSTOM_CREATE_TYPE_MACRO(QualifiedNameType,
            "SOURCE_CREATE_TYPE_FOR_QUALIFIED_NAME_TYPE",
            "SgType* type = NULL, unsigned int x = 0");
     CUSTOM_CREATE_TYPE_MACRO(FunctionType,
            "SOURCE_CREATE_TYPE_FOR_FUNCTION_TYPE",
            "SgPartialFunctionType* type = NULL");
     CUSTOM_CREATE_TYPE_MACRO(MemberFunctionType,
            "SOURCE_CREATE_TYPE_FOR_MEMBER_FUNCTION_TYPE",
            "SgPartialFunctionType* type = NULL");
     CUSTOM_CREATE_TYPE_MACRO(PartialFunctionType,
            "SOURCE_CREATE_TYPE_FOR_PARTIAL_FUNCTION_TYPE",
            "SgType* type = NULL");
     CUSTOM_CREATE_TYPE_MACRO(ArrayType,
            "SOURCE_CREATE_TYPE_FOR_ARRAY_TYPE",
            "SgType* type = NULL, SgExpression* expr = NULL");

  // Rasmussen (2/18/2020): Added support for the create function for Jovial TypeFixed
     CUSTOM_CREATE_TYPE_MACRO(TypeFixed,
            "SOURCE_CREATE_TYPE_FOR_TYPE_FIXED",
            "SgExpression* scale = NULL, SgExpression* fraction = NULL");

  // DQ (8/17/2010): Added support for create function for StringType (Fortran specific)
     CUSTOM_CREATE_TYPE_MACRO(TypeString,
            "SOURCE_CREATE_TYPE_FOR_STRING_TYPE",
            "SgExpression* expr = NULL, size_t length = 0");

#if 0
  // DQ (8/27/2006): Complex types should just take an enum value to indicate their size (float, double, long double).
     CUSTOM_CREATE_TYPE_MACRO(TypeComplex,
            "SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE",
            "SgType* type = NULL, SgExpression* expr = NULL");
     CUSTOM_CREATE_TYPE_MACRO(TypeImaginary,
            "SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE",
            "SgType* type = NULL, SgExpression* expr = NULL");
#else
  // TypeComplex.excludeFunctionSource ("SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE", "../Grammar/Type.code" );
     TypeComplex.setFunctionSource     ("SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE", "../Grammar/Type.code" );
  // TypeImaginary.excludeFunctionSource ("SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE", "../Grammar/Type.code" );
     TypeImaginary.setFunctionSource   ("SOURCE_CREATE_TYPE_FOR_COMPLEX_TYPE", "../Grammar/Type.code" );
#endif

     TypeInt.setFunctionPrototype     ("HEADER_TYPE_INT_TYPE", "../Grammar/Type.code" );
  // TypeInt.excludeFunctionPrototype ( "HEADER_EXTRA_FUNCTIONS", "../Grammar/Type.code" );
  // TypeInt.setFunctionPrototype     ( "HEADER_CREATE_TYPE" , "../Grammar/Type.code" );
  // TypeInt.setFunctionSource        ( "SOURCE_BUILTIN_TYPE_ARRAY" , "../Grammar/Type.code" );

  // These classes have data fields
     TypeInt.setDataPrototype           ("int","field_size","= 0",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TypeMatrix.setDataPrototype           ("SgType*","base_type","= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
  // DQ (4/23/2014): I think this has to be defined as DEF_TRAVERSAL so that we can traverse the nested type.
  // This is required to support type transformations fo the shared memory DSL work. Likely also required for ReferenceType
  // and any other type with a base_type.
  // PointerType.setDataPrototype       ("SgType*","base_type","= NULL",
  //                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     PointerType.setDataPrototype       ("SgType*","base_type","= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     ReferenceType.setDataPrototype     ("SgType*","base_type","= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     RvalueReferenceType.setDataPrototype ("SgType*","base_type","= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/2/2014): Adding support for C++11 decltype().
     DeclType.setFunctionPrototype ("HEADER_DECL_TYPE", "../Grammar/Type.code" );
     DeclType.setDataPrototype ("SgExpression*","base_expression","= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/11/2014): We need to handle the case were the input is a function parameter code (number and level) 
  // and then store the associated type until we can translate the code into a parameter.  Then we need only
  // make sure that the expression type (if available) is consistant with the associated stored type.
  // If the expresssion's type is available, then the expression weill be stored, but if the expression's 
  // type is unavailable (either the expression is unavailable or the expression is a SgFunctionParameterRefExp
  // then the type is stored).  This is the more complex handling of stored type, but is hidden within the 
  // SgDeclType and the SgFunctionParameterRefExp, as required to present a simple API).
     DeclType.setDataPrototype ("SgType*","base_type","= NULL",
                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     DeclType.setFunctionPrototype ("HEADER_GET_BASE_TYPE", "../Grammar/Type.code" );

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     TypeOfType.setFunctionPrototype ("HEADER_TYPEOF_TYPE", "../Grammar/Type.code" );
     TypeOfType.setDataPrototype ("SgExpression*","base_expression","= NULL",
                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeOfType.setDataPrototype ("SgType*","base_type","= NULL",
                                CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeOfType.setFunctionPrototype ("HEADER_GET_BASE_TYPE", "../Grammar/Type.code" );

     PointerMemberType.setFunctionPrototype  ("HEADER_POINTER_MEMBER_TYPE", "../Grammar/Type.code" );
     PointerMemberType.setDataPrototype ("SgType*","class_type","= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/17/2019): To support SgPointerMemberType name qualification for the class type and the base type,
  // we need to add a name qualification field to the SgPointerMemberType which will allow the name qualification
  // to be computed for a set of types in a specific context (location in the code, e.g. statement) and then use 
  // the mechanism to generate a string for the type (as is used for template instantiations) and then put the
  // generated string into a map used for name qualificaiton of types.  The reason we can't just use a limited 
  // number of fields on each IR node that can reference a type is that there can be an abitrary number of
  // points in the nested representation of SgPointerMemberType(s) where name qualificaiton is required.
  // To this extent it is similar to the name qualification complexity of the template instantiations which
  // can have arbitrary numbers of template arguments with additional name qualification.

  // To support each context generating the name qualification for every part of type tree, we need to put
  // support for the name qualificaiton directly into the SgPointerMemberType.

#if 0
  // DQ (4/17/2019): This is needed to support pointers to member type in IR nodes that reference types.
     PointerMemberType.setDataPrototype ( "int", "name_qualification_length", "= 0",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/17/2019): This is needed to support pointers to member type in IR nodes that reference types.
     PointerMemberType.setDataPrototype("bool","type_elaboration_required","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/17/2019): This is needed to support pointers to member type in IR nodes that reference types.
     PointerMemberType.setDataPrototype("bool","global_qualification_required","= false",
                NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     NamedType.setFunctionPrototype ("HEADER_VIRTUAL_GET_NAME", "../Grammar/Type.code" );
     NamedType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );

  // DQ (7/18/2004): Added traversal of the class declaration in the named type
  // If we get to the type in a traversal then we want to traverse its members, 
  // else we would not have traversed the type!
  // NamedType.setDataPrototype     ("SgDeclarationStatement*", "declaration","= NULL",
  //      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     NamedType.setDataPrototype     ("SgDeclarationStatement*", "declaration","= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // class A { int i; };    // An "autonomous" decl
  // class B { int i; } b;  // Not "autonomous"
     NamedType.setDataPrototype     ("bool", "autonomous_declaration","= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // NamedType.setDataPrototype ( "static SgQualifiedNamePtrList", "defaultQualifiedNamePtrList", "",
  //           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // NamedType.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "= p_defaultQualifiedNamePtrList",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (7/16/2017): We need to mark that a named type is associated with a template parameter.
     NamedType.setDataPrototype     ("bool", "is_from_template_parameter","= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     ClassType.setFunctionPrototype ("HEADER_CLASS_TYPE", "../Grammar/Type.code" );
     ClassType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

  // DQ (3/20/2017): We need to support a flag to indicate packing in template parameters.
  // This is required only for the RoseExample tests using Boost 1.56 (no where else that I know of so far).
     ClassType.setDataPrototype     ("bool","packed","= false",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     NonrealType.setFunctionPrototype ( "HEADER_NONREAL_TYPE", "../Grammar/Type.code" );
     NonrealType.setFunctionPrototype ( "HEADER_GET_NAME",     "../Grammar/Type.code" );
//   NonrealType.excludeFunctionPrototype ( "HEADER_GET_NAME",     "../Grammar/Type.code" );
//   NonrealType.excludeFunctionPrototype ( "HEADER_GET_MANGLED",  "../Grammar/Type.code" );

     AutoType.setFunctionPrototype        ( "HEADER_AUTO_TYPE",   "../Grammar/Type.code" );
//   AutoType.excludeFunctionPrototype    ( "HEADER_GET_NAME",    "../Grammar/Type.code" );
//   AutoType.excludeFunctionPrototype    ( "HEADER_GET_MANGLED", "../Grammar/Type.code" );

     JavaParameterizedType.setFunctionPrototype ("HEADER_JAVA_PARAMETERIZED_TYPE", "../Grammar/Type.code" );
     JavaParameterizedType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );
     JavaParameterizedType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );
     JavaParameterizedType.setDataPrototype     ("SgNamedType*","raw_type","= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaParameterizedType.setDataPrototype     ("SgTemplateParameterList*","type_list","= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     JavaQualifiedType.setFunctionPrototype ("HEADER_JAVA_QUALIFIED_TYPE", "../Grammar/Type.code" );
     JavaQualifiedType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );
     JavaQualifiedType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );
     JavaQualifiedType.setDataPrototype     ("SgNamedType *","parent_type","= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaQualifiedType.setDataPrototype     ("SgNamedType *","type","= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     JavaWildcardType.setFunctionPrototype ("HEADER_JAVA_WILDCARD_TYPE", "../Grammar/Type.code" );
     JavaWildcardType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );
     JavaWildcardType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );
     JavaWildcardType.setDataPrototype     ("SgType*","bound_type","= NULL",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // JavaWildcardType.setDataPrototype     ("SgType*","extends_type","= NULL",
     //                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // JavaWildcardType.setDataPrototype     ("SgType*","super_type","= NULL",
     //                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaWildcardType.setDataPrototype     ("bool","is_unbound","= true",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaWildcardType.setDataPrototype     ("bool","has_extends","= false",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     JavaWildcardType.setDataPrototype     ("bool","has_super","= false",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/10/2014): Added SgNamedType IR nodes for Philippe.
     JavaUnionType.setFunctionPrototype ("HEADER_JAVA_UNION_TYPE", "../Grammar/Type.code" );
     JavaUnionType.setDataPrototype     ("SgTypePtrList","type_list","",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (3/22/2017): This is reserved for virtual function specification.
  // JavaUnionType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

     JavaParameterType.setFunctionPrototype ("HEADER_JAVA_PARAMETER_TYPE", "../Grammar/Type.code" );
     JavaParameterType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );
     JavaParameterType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

     JovialTableType.setFunctionPrototype   ("HEADER_JOVIAL_TABLE_TYPE", "../Grammar/Type.code" );
     JovialTableType.setFunctionPrototype   ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );
     JovialTableType.setFunctionPrototype   ("HEADER_GET_NAME", "../Grammar/Type.code" );

     JovialTableType.setDataPrototype ("SgType*"       , "base_type", "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL,  NO_DELETE);
     JovialTableType.setDataPrototype ("SgExprListExp*", "dim_info" , "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     JovialTableType.setDataPrototype ("int", "rank" , "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS,  NO_TRAVERSAL,  NO_DELETE);

   // TemplateInstantiationType.setFunctionPrototype ("HEADER_TEMPLATE_INSTANTIATION_TYPE", "../Grammar/Type.code" );
     TemplateType.setFunctionPrototype ("HEADER_TEMPLATE_TYPE", "../Grammar/Type.code" );
  // TemplateInstantiationType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

  // DQ (8/25/2012): Added support for name of template type. Note that this is the name "T1" in instantiation of a template 
  // using a template parameter.  This should likely be mapped back to the template parameter by position in the sequence of 
  // template parameters and the template declaration (OR just the template paramters only; I have not decided).
     TemplateType.setDataPrototype     ("SgName","name","= \"\"",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateType.setDataPrototype     ("int","template_parameter_position","= -1",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateType.setDataPrototype     ("int","template_parameter_depth","= -1",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     TemplateType.setDataPrototype     ("SgType *","class_type","= NULL",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     TemplateType.setDataPrototype     ("SgType *","parent_class_type","= NULL",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     TemplateType.setDataPrototype     ("SgTemplateParameter *","template_parameter","= NULL",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     TemplateType.setDataPrototype     ("SgTemplateArgumentPtrList", "tpl_args", "= SgTemplateArgumentPtrList()", NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TemplateType.setDataPrototype     ("SgTemplateArgumentPtrList", "part_spec_tpl_args", "= SgTemplateArgumentPtrList()", NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (3/18/2017): We need to support a flag to indicate packing in template parameters.
     TemplateType.setDataPrototype     ("bool","packed","= false",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Skip building a parse function for this AstNodeClass/nonterminal of the Grammar
     if (isRootGrammar() == false)
        {
          NamedType.excludeFunctionPrototype ( "HEADER_PARSER", "../Grammar/Node.code" );
          NamedType.excludeFunctionSource   ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro" );
        }
     
     EnumType.setFunctionPrototype ("HEADER_ENUM_TYPE", "../Grammar/Type.code" );
     EnumType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

  // DQ (6/30/2005): Added member function for stiping away typedefs and modifiers
  // TypedefType.setFunctionPrototype ("HEADER_TYPEDEF_TYPE", "../Grammar/Type.code" );

     TypedefType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );

  // This get_base_type function is not associated with a base_type data filed in this object
     TypedefType.setFunctionPrototype ("HEADER_GET_BASE_TYPE", "../Grammar/Type.code" );
     TypedefType.setFunctionPrototype ("HEADER_TYPEDEF_TYPE", "../Grammar/Type.code" );
     TypedefType.setDataPrototype     ("SgSymbol*", "parent_scope","= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Exclude the get_mangled function since we include it in the HEADER_MODIFIER_TYPE string
     ModifierType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     ModifierType.setFunctionPrototype ("HEADER_MODIFIER_TYPE", "../Grammar/Type.code" );

  // DQ (4/23/2014): I think this has to be defined as DEF_TRAVERSAL so that we can traverse the nested type.
  // This is required to support type transformations fo the shared memory DSL work. Likely also required for ReferenceType
  // and any other type with a base_type.
  // ModifierType.setDataPrototype     ("SgType*","base_type","= NULL",
  //                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierType.setDataPrototype     ("SgType*","base_type","= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (4/22/2004): Old way of handling modifiers
  // ModifierType.setDataPrototype     ("unsigned int","bitfield","= 0",
  //           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (4/13/2004): Added to support more uniform modifier handling and fix several bugs
  // Only cv-modifiers and restrict were ever in SAGE III previously
  // ModifierType.setDataPrototype     ("SgTypeModifier","typeModifier","= SgTypeModifier::e_unknown",
  //           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierType.setDataPrototype     ("SgTypeModifier","typeModifier","",
               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
     ModifierType.setDataPrototype     ("SgStorageModifier","storageModifier","= SgStorageModifier::e_unknown",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierType.setDataPrototype     ("SgAccessModifier","accessModifier","= SgAccessModifier::e_unknown",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierType.setDataPrototype     ("SgFunctionModifier","functionModifier","= SgFunctionModifier::e_unknown",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // OR we could use a SgDeclarationModifier!
     ModifierType.setDataPrototype     ("SgDeclarationModifier","declarationModifier","",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     ModifierType.setDataPrototype     ("SgSpecialFunctionModifier","specialFunctionModifier","",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#if 0
  // DQ (10/2/2010): The Fortran specific support for kind has been moved to the base class because so
  // many types in Fortran can have a kind parameter that is better to represent it in the base class.
  // DQ (12/1/2007): Support for Fortran kind mechanism (moved from SgType to SgModifierType)
     ModifierType.setDataPrototype("SgExpression*","type_kind","= NULL",
          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
  // DQ (12/1/2007): Added support for Fortran type parameter mechanism
  // Note that CHARACTER*52 becomes a statically types array of CHARACTER, but CHARACTER*52 becomes SGModifierType with a CHARACTER base type
  // ModifierType.setDataPrototype("SgExpression*","type_parameter","= NULL",
  //      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
#endif

  // DQ (8/12/2012): This is used as a wrapper to support type references that will be fixed up after the AST 
  // is build and all types exist. It is part of the new C++ support and required for types hidden in template 
  // instantiations that have not yet been processed yet.
     ModifierType.setDataPrototype ("char*", "frontend_type_reference" , "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (10/10/2006): The idea here is that these would be wrappers for existing types, 
  // but I think this was ultimately a problem to make it really work (because it could 
  // only replace SgType and not SgNamedType, for example; so it is not used as an IR 
  // node at present.
  // DQ (12/21/2005): Global qualification and qualified name handling are now represented explicitly in the AST
  // Exclude the get_mangled function since we include it in the HEADER_MODIFIER_TYPE string
  // QualifiedNameType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     QualifiedNameType.setFunctionPrototype ("HEADER_QUALIFIED_NAME_TYPE", "../Grammar/Type.code" );
     QualifiedNameType.setDataPrototype     ("SgType*","base_type","= NULL",
               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // QualifiedNameType.setDataPrototype ( "static SgQualifiedNamePtrList", "defaultQualifiedNamePtrList", "",
  //           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // QualifiedNameType.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "= p_defaultQualifiedNamePtrList",
  //           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     QualifiedNameType.setDataPrototype ( "SgQualifiedNamePtrList", "qualifiedNameList", "",
               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     FunctionType.setFunctionPrototype ("HEADER_FUNCTION_TYPE", "../Grammar/Type.code" );
     FunctionType.setDataPrototype     ("SgType*", "return_type","= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     FunctionType.setDataPrototype     ("bool", "has_ellipses","= true",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     FunctionType.setDataPrototype     ("SgType*", "orig_return_type","= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 1
  // DQ (7/20/2004): Modified to NOT traverse this object (later: likely OK to traverse)
  // FunctionType.setDataPrototype("SgFunctionParameterTypeList*", "arguments", "= NULL",
  //           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL);
     FunctionType.setDataPrototype("SgFunctionParameterTypeList*", "argument_list", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE);
#else
     FunctionType.setDataPrototype("SgTypePtrList", "arguments", "",
               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#if 0
  // DQ (12/21/2005): This global qualification is now replaced by a more general mechanism to handle
  // qualified names. Here we have a list of qualified names a global qualification is represented by 
  // a SgQualifiedName that contains a pointer to the SgGlobal scope. This is so variables such as 
  // "::X::Y::Z::variable" can be represented properly.
     FunctionType.setDataPrototype ( "SgQualifiedNamePtrList", "returnTypeQualifiedNameList", "",
               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // MK: FunctionType.excludeDataPrototype ("SgType*", "orig_return_type","= NULL");

  // Don't use the setDataPrototype method since the access functions require a reference return type
  // FunctionType.setDataPrototype       ("SgTypePtrList", "arguments","");

     MemberFunctionType.setFunctionPrototype ("HEADER_MEMBER_FUNCTION_TYPE", "../Grammar/Type.code" );        
     MemberFunctionType.setDataPrototype     ("SgType*", "class_type","= NULL",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     MemberFunctionType.setDataPrototype     ("unsigned int", "mfunc_specifier","= 0",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     MemberFunctionType.setDataPrototype     ("unsigned int", "ref_qualifiers","= 0",
                                              CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     PartialFunctionType.setFunctionPrototype ("HEADER_PARTIAL_FUNCTION_TYPE", "../Grammar/Type.code" );


  // DQ (3/22/2017): I think it is a mistake to include this this way. This is for the virtual 
  // function support and it is not a virtual function in SgArrayType.
  // ArrayType.setFunctionPrototype ("HEADER_GET_NAME", "../Grammar/Type.code" );
     ArrayType.setFunctionPrototype ("HEADER_ARRAY_TYPE", "../Grammar/Type.code" );

     ArrayType.setFunctionPrototype ("HEADER_GET_QUALIFIED_NAME", "../Grammar/Type.code" );

     ArrayType.setDataPrototype ("SgType*"      , "base_type", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/7/2007): I think that the delete operator should call delete for the index expression.
  // ArrayType.setDataPrototype ("SgExpression*", "index"    , "= NULL",
  //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     ArrayType.setDataPrototype ("SgExpression*", "index"    , "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);

  // DQ (8/15/2007): This could replace the "index" above, we need the list to handle Fortran, even 
  // though we only need a single expression for C and C++.
     ArrayType.setDataPrototype ("SgExprListExp*", "dim_info" , "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     ArrayType.setDataPrototype ("int", "rank" , "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // TV (09/21/2018): (ROSE-1391) to be used in unparser when referenced symbol is not accessible
     ArrayType.setDataPrototype ("int", "number_of_elements" , "= 0",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (2/12/2016): Adding support for Variable Length Arrays.
     ArrayType.setDataPrototype ("bool", "is_variable_length_array" , "= false",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Rasmussen (2/18/2020): Added TypeFixed for Jovial
     TypeFixed.setFunctionPrototype ("HEADER_TYPE_FIXED_TYPE", "../Grammar/Type.code" );
     TypeFixed.setDataPrototype ("SgExpression*", "scale", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);
     TypeFixed.setDataPrototype ("SgExpression*", "fraction", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, DEF_DELETE);

     TypeComplex.setFunctionPrototype ("HEADER_TYPE_COMPLEX_TYPE", "../Grammar/Type.code" );
     TypeComplex.setDataPrototype ("SgType*", "base_type", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/27/2006): Use the code from the complex class to define the imaginary class!
     TypeImaginary.setFunctionPrototype ("HEADER_TYPE_COMPLEX_TYPE", "../Grammar/Type.code" );
     TypeImaginary.setDataPrototype ("SgType*", "base_type", "= NULL",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/17/2010): Added support for string types for Fortran (in C/C++ they are just arrays of char).
     TypeString.setFunctionPrototype ("HEADER_TYPE_STRING_TYPE", "../Grammar/Type.code" );

  // DQ (12/26/2010): Added mechanism to store names of types that can't be identified in initial parsing (applicable only to Fortran).
     TypeDefault.setFunctionPrototype ("HEADER_TYPE_DEFAULT_TYPE", "../Grammar/Type.code" );
     TypeDefault.setDataPrototype ("SgName", "name" , "= \"\"",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (8/12/2012): I might not need this, since I have a more direct approach.
  // DQ (8/12/2012): I need a mechanism to store references to types (typically in template instantiations) 
  // that have not been seen yet and will be fixed up later.  I will use a SgDefaultType (used similarly in 
  // the Fortran support) and then fixed up in a seperate pass of AST post-processing.  At this point the
  // type_translation_cache map (of EDG types to ROSE types) should have a valid entry (because all of the
  // template instantiations will have been processed).  Note that this need not confuse the name mangling
  // since that can alternatively be done with either the typedef type of the typedef's base type.  It is
  // more important to support this where types are output (e.g as part of variable declarations) so that 
  // the type names being output will not be private. We have to store both sides of the reference so that
  // we can find the IR node in the ROSE AST to be reset (points to this SgTypeDefault IR node).  Then the
  // proper SgType node to use instead is found from:
  //      "type_translation_cache[(a_type_ptr)frontend_type_reference]".
  // This replacement must be done ast the AST has been build as a preliminary step in the AST post-processing.
  // All of the associated SgDefaultIr nodes can then be deleted.
  // I regret that I don't have a better solution for this problem, I don't like these solutions.  Maybe I 
  // will still have a better idea...
     TypeDefault.setDataPrototype ("SgNode*", "rose_node" , "= \"\"",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     TypeDefault.setDataPrototype ("void*", "frontend_type_reference" , "= \"\"",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (2/1/2011): Added label type to support Fortran alternative return arguments in function declarations.
     TypeLabel.setFunctionPrototype ("HEADER_TYPE_LABEL_TYPE", "../Grammar/Type.code" );
     TypeLabel.setDataPrototype ("SgName", "name" , "= \"\"", NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

  // Setup the source code common to all the derived classes
  // Type.setSubTreeFunctionSource ( "SOURCE", "../Grammar/sageCommon.code");
  // Type.excludeFunctionSource ( "SOURCE", "../Grammar/sageCommon.code");

  // Use the following constructor body for the constructors in the classes
  // derived from Type (except for a few classes which will be excluded)
     Type.editSubstituteSubTree ( "CONSTRUCTOR_BODY", "SOURCE_CONSTRUCTOR_BODY", "../Grammar/Type.code");

  // Setup the source for the get_mangled member function
     Type.setSubTreeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     Type.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

  // Use a different get_mangled member function for the PointerType and ReferenceType objects
     PointerType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     PointerType.setFunctionSource ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

     ReferenceType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     ReferenceType.setFunctionSource ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

     RvalueReferenceType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     RvalueReferenceType.setFunctionSource ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

  // DQ (8/2/2014): Adding support for C++11 decltype().
     DeclType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
  // TV (02/21/2019): We need a specific version of this function for the decltype() operator.
  // DeclType.setFunctionSource     ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     TypeOfType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
  // DQ (4/8/2015): We need a specific version of this function for the typeof() operator.
  // TypeOfType.setFunctionSource     ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

     ArrayType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
  // ArrayType.setFunctionSource ( "SOURCE_GET_MANGLED_BASE_TYPE", "../Grammar/Type.code");

  // We require a special function here which is included directly
     FunctionType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     TypedefType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     NamedType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     MemberFunctionType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     PartialFunctionType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     ModifierType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     QualifiedNameType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     ClassType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

//   NonrealType.excludeFunctionSource    ( "SOURCE_GET_NAME",    "../Grammar/Type.code");
     NonrealType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     NonrealType.setFunctionSource        ( "SOURCE_NONREAL_TYPE", "../Grammar/Type.code");

//   AutoType.excludeFunctionSource    ( "SOURCE_GET_NAME",    "../Grammar/Type.code");
     AutoType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     AutoType.setFunctionSource        ( "SOURCE_AUTO_TYPE", "../Grammar/Type.code");

     JavaParameterizedType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     JavaQualifiedType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     JavaWildcardType.excludeFunctionSource    ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

  // DQ (2/10/2014): Added SgNamedType IR nodes for Philippe.
     JavaUnionType.excludeFunctionSource     ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     JavaParameterType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     JovialTableType.excludeFunctionSource   ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

  // TemplateInstantiationType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     EnumType.excludeFunctionSource     ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");

     PointerMemberType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     PointerMemberType.setFunctionSource ( "SOURCE_POINTER_MEMBER_GET_MANGLED", "../Grammar/Type.code");

  // DQ (8/25/2012): Avoid the default generated get_mangled_name() function for this IR node.
     TemplateType.excludeFunctionSource ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");


  // Setup the mangled name function (a different entry specific to each class)
     TypeUnknown.editSubstitute( "MANGLED_ID_STRING", "Unknown" );
     TypeChar.editSubstitute( "MANGLED_ID_STRING", "c" );
     TypeSignedChar.editSubstitute( "MANGLED_ID_STRING", "si" );
     TypeUnsignedChar.editSubstitute( "MANGLED_ID_STRING", "Uc" );
     TypeShort.editSubstitute( "MANGLED_ID_STRING", "s" );
     TypeSignedShort.editSubstitute( "MANGLED_ID_STRING", "Ss" );
     TypeUnsignedShort.editSubstitute( "MANGLED_ID_STRING", "Us" );
     TypeInt.editSubstitute( "MANGLED_ID_STRING", "i" );
     TypeSignedInt.editSubstitute( "MANGLED_ID_STRING", "Si" );
     TypeUnsignedInt.editSubstitute( "MANGLED_ID_STRING", "Ui" );
     TypeLong.editSubstitute( "MANGLED_ID_STRING", "l" );
     TypeSignedLong.editSubstitute( "MANGLED_ID_STRING", "Sl" );
     TypeUnsignedLong.editSubstitute( "MANGLED_ID_STRING", "Ul" );
     TypeVoid.editSubstitute( "MANGLED_ID_STRING", "v" );
     TypeGlobalVoid.editSubstitute( "MANGLED_ID_STRING", "gv" );
     TypeWchar.editSubstitute( "MANGLED_ID_STRING", "wc" );
     TypeFixed.editSubstitute( "MANGLED_ID_STRING", "fx" );
     TypeFloat.editSubstitute( "MANGLED_ID_STRING", "f" );
     TypeDouble.editSubstitute( "MANGLED_ID_STRING", "d" );
     TypeLongLong.editSubstitute( "MANGLED_ID_STRING", "L" );
     TypeSignedLongLong.editSubstitute( "MANGLED_ID_STRING", "SL" );
     TypeUnsignedLongLong.editSubstitute( "MANGLED_ID_STRING", "UL" );

  // TV (12/29/2018): using literal suffixes
     TypeFloat80.editSubstitute( "MANGLED_ID_STRING", "w" );
     TypeFloat128.editSubstitute( "MANGLED_ID_STRING", "q" );

  // DQ (3/24/2014): Adding support for 128 bit integers.
     TypeSigned128bitInteger.editSubstitute( "MANGLED_ID_STRING", "SL128" );
     TypeUnsigned128bitInteger.editSubstitute( "MANGLED_ID_STRING", "UL128" );

  // DQ (2/16/2018): Adding support for char16_t and char32_t for C99 and C++11.
     TypeChar16.editSubstitute( "MANGLED_ID_STRING", "c16" );
     TypeChar32.editSubstitute( "MANGLED_ID_STRING", "c32" );

     TypeCAFTeam.editSubstitute( "MANGLED_ID_STRING", "s" );

     TypeLongDouble.editSubstitute( "MANGLED_ID_STRING", "ld" );
     TypeString.editSubstitute( "MANGLED_ID_STRING", "str" );
     TypeBool.editSubstitute( "MANGLED_ID_STRING", "b" );

  // DQ (7/29/2014): Added nullptr type (I think we require this for C++11 support).
     TypeNullptr.editSubstitute( "MANGLED_ID_STRING", "nullptr_t" );

     TypeMatrix.editSubstitute( "MANGLED_ID_STRING", "matrix_t" );
     TypeTuple.editSubstitute( "MANGLED_ID_STRING", "tuple_t" );
     
     TypeComplex.editSubstitute( "MANGLED_ID_STRING", "Complex" );
     TypeImaginary.editSubstitute( "MANGLED_ID_STRING", "Imaginary" );
  // TypeDefault.editSubstitute( "MANGLED_ID_STRING", "u" );
     PointerType.editSubstitute( "MANGLED_ID_STRING", "P" );
     ReferenceType.editSubstitute( "MANGLED_ID_STRING", "R" );
     RvalueReferenceType.editSubstitute( "MANGLED_ID_STRING", "Rvalue" );

  // DQ (8/2/2014): Adding support for C++11 decltype().
     DeclType.editSubstitute( "MANGLED_ID_STRING", "decltype" );

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     TypeOfType.editSubstitute( "MANGLED_ID_STRING", "typeOftype" );

  // ArrayType.editSubstitute( "MANGLED_ID_STRING", "A_" );
     TypeEllipse.editSubstitute( "MANGLED_ID_STRING", "e" );
  // JavaWildcardType.editSubstitute( "MANGLED_ID_STRING", "?" );

  // DQ (8/25/2012): Avoid the default generated get_mangled_name() function for this IR node.
  // DQ (5/11/2012): We need to define this.
  // TemplateType.editSubstitute( "MANGLED_ID_STRING", "TemplateType" );

  // DQ (5/11/2012): This should not be used, bu we should define this for all types.
     PartialFunctionModifierType.editSubstitute( "MANGLED_ID_STRING", "PartialFunctionType" );

     PartialFunctionType.setFunctionSource ( "SOURCE_PARTIAL_FUNCTION_TYPE", "../Grammar/Type.code");
     TypedefType.setFunctionSource         ( "SOURCE_TYPEDEF_TYPE", "../Grammar/Type.code");
     MemberFunctionType.setFunctionSource  ( "SOURCE_MEMBER_FUNCTION_TYPE", "../Grammar/Type.code");
     PointerMemberType.setFunctionSource   ( "SOURCE_POINTER_MEMBER_TYPE", "../Grammar/Type.code");

     TypeInt.setFunctionSource             ( "SOURCE_TYPE_INT_TYPE", "../Grammar/Type.code");
     NamedType.setFunctionSource           ( "SOURCE_NAMED_TYPE", "../Grammar/Type.code" );
     EnumType.setFunctionSource            ( "SOURCE_ENUM_TYPE", "../Grammar/Type.code");
     FunctionType.setFunctionSource        ( "SOURCE_FUNCTION_TYPE", "../Grammar/Type.code");

     ClassType.setFunctionSource             ( "SOURCE_CLASS_TYPE", "../Grammar/Type.code");
     JavaParameterizedType.setFunctionSource ( "SOURCE_JAVA_PARAMETERIZED_TYPE", "../Grammar/Type.code");
     JavaQualifiedType.setFunctionSource     ( "SOURCE_JAVA_QUALIFIED_TYPE", "../Grammar/Type.code");
     JavaWildcardType.setFunctionSource      ( "SOURCE_JAVA_WILDCARD_TYPE", "../Grammar/Type.code");

     JavaUnionType.setFunctionSource     ( "SOURCE_JAVA_UNION_TYPE", "../Grammar/Type.code");
     JavaParameterType.setFunctionSource ( "SOURCE_JAVA_PARAMETER_TYPE", "../Grammar/Type.code");

     JovialTableType.setFunctionSource   ( "SOURCE_JOVIAL_TABLE_TYPE", "../Grammar/Type.code");

     TemplateType.setFunctionSource        ( "SOURCE_TEMPLATE_TYPE", "../Grammar/Type.code");

     PointerType.setFunctionSource         ( "SOURCE_POINTER_TYPE", "../Grammar/Type.code");
     ReferenceType.setFunctionSource       ( "SOURCE_REFERENCE_TYPE", "../Grammar/Type.code");
     RvalueReferenceType.setFunctionSource ( "SOURCE_RVALUE_REFERENCE_TYPE", "../Grammar/Type.code");

  // DQ (8/2/2014): Adding support for C++11 decltype().
     DeclType.setFunctionSource ( "SOURCE_DECL_TYPE", "../Grammar/Type.code");

  // DQ (3/27/2015): Adding support for GNU C language extension "typeof" operator (works similar to decltype in C++11).
     TypeOfType.setFunctionSource ( "SOURCE_TYPEOF_TYPE", "../Grammar/Type.code");

     ArrayType.setFunctionSource           ( "SOURCE_ARRAY_TYPE", "../Grammar/Type.code");
     ModifierType.setFunctionSource        ( "SOURCE_MODIFIER_TYPE", "../Grammar/Type.code");
     QualifiedNameType.setFunctionSource   ( "SOURCE_QUALIFIED_NAME_TYPE", "../Grammar/Type.code");
     TypeCrayPointer.editSubstitute        ( "MANGLED_ID_STRING", "CrayPointer" );

  // DQ (8/17/2010): Added support for SgTypeString (used in Fortran).
     TypeString.setFunctionSource         ( "SOURCE_TYPE_STRING_TYPE", "../Grammar/Type.code");
  // DQ (8/17/2010): Added support for SgTypeString name mangling.
     TypeString.excludeFunctionSource     ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     TypeString.setFunctionSource         ( "SOURCE_GET_MANGLED_STRING_TYPE", "../Grammar/Type.code");

  // DQ (12/26/2010): Added support for names in TypeDefault (used to store parser information until
  // proper type can be constructed, e.g. after using statement is seen for function return types).
     TypeDefault.excludeFunctionSource     ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     TypeDefault.setFunctionSource         ( "SOURCE_TYPE_DEFAULT_TYPE", "../Grammar/Type.code");
  // TypeDefault.setFunctionSource         ( "SOURCE_GET_MANGLED_STRING_TYPE", "../Grammar/Type.code");

  // DQ (2/1/2011): Added label type to support Fortran alternative return arguments in function declarations.
     TypeLabel.excludeFunctionSource     ( "SOURCE_GET_MANGLED", "../Grammar/Type.code");
     TypeLabel.setFunctionSource         ( "SOURCE_TYPE_LABEL_TYPE", "../Grammar/Type.code");

#endif
   }



















