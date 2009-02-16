#ifndef mangling_support_INCLUDED
#define mangling_support_INCLUDED
std::string replaceNonAlphaNum (const std::string& s);

//! Returns the input std::string stripped of leading and trailing spaces.
std::string trimSpaces (const std::string& s);

#if 0
//! Returns true <==> the given mangled name is a valid identifier.
bool isValidMangledName (std::string name);
#endif

//! Joins two sets of mangled qualifiers to form a new mangled qualifier.
std::string joinMangledQualifiersToString (const std::string& base, const std::string& name);

//! Joins two sets of mangled qualifiers to form a new mangled qualifier.
SgName joinMangledQualifiers (const SgName& base, const SgName& name);

//! Returns the function definition containing the specified statement.
const SgFunctionDefinition* findRootFunc (const SgScopeStatement* scope);

// *****************************************************************
//         New code added to support better name mangling
// *****************************************************************

/*! Returns a unique positive integer ID to an SgScopeStatement within
 *  a function definition.
 *
 *  This routine computes a preorder numbering of the
 *  SgScopeStatements in the subtree rooted at 'func_def', and returns
 *  the number assigned to the 'target' scope statement, or 0 if the
 *  target could not be found in 'func_def'.
 */
size_t getLocalScopeNum (const SgFunctionDefinition* func_def, const SgScopeStatement* target);
// size_t getLocalScopeNum ( SgFunctionDefinition* func_def, const SgScopeStatement* target);

//! Produces a mangled name for a scope statement.
std::string mangleLocalScopeToString (const SgScopeStatement* scope);

/*! Mangles a chain of qualifiers followed from a given scope.
 *
 *  This routine starts at a given scope and collects all
 *  mangled scope names going 'up' through to global scope.
 */
std::string mangleQualifiersToString (const SgScopeStatement* scope);

/*! Mangles a chain of qualifiers, returning an SgName object.
 *  \sa mangleQualifiersToStd::String
 */
SgName mangleQualifiers (const SgScopeStatement* scope);

/*! Mangles a container of types, returning an STL std::string object.
 *
 *  \param[in] b  First element in the container to mangle.
 *  \param[in] e  End (last+1) element in the container to mangle.
 *  \returns An STL std::string containing all mangled names in the list,
 *  concatenated by "__sep__".
 */
std::string mangleTypesToString (const SgTypePtrList::const_iterator b, const SgTypePtrList::const_iterator e);

/*! Mangles a list of types, returning an SgName object.
 *  \sa mangleTypesToStd::String
 */
SgName mangleTypes (const SgTypePtrList::const_iterator b, const SgTypePtrList::const_iterator e);

/*! Returns a mangled form of special C++ function names (excl. qualifiers).
 *
 *  Adapted from SgType::mangledNameSupport. This routine considers these
 *  cases:
 *    - An overloaded operator, including 'new/delete' and 'new[]/delete[]'.
 *    - Overloaded casting operators: the caller should specify the optional
 *      return type of the function as a mangled name (see parameters).
 *    - A destructor (signaled by a '~' prefix).
 *
 *  The caller may omit the second parameter to this function, but
 *  that is not recommended.
 *
 *  \param[in] s              Unmangled function name.
 *  \param[in] ret_type_name  Return type of this function, as a mangled name.
 *  \returns A mangled std::string name.
 */
std::string mangleFunctionNameToString (const std::string& s, const std::string& ret_type_name = std::string (""));

/*! Returns a mangled form of a C++ function name, as an SgName object.
 *  \sa mangleFunctionNameToStd::String
 */
SgName mangleFunctionName (const SgName& n, const SgName& ret_type_name = SgName (""));

/*! Mangles a template argument container, returning an STL std::string.
 *
 *  \param[in] b  First element in the container to mangle.
 *  \param[in] e  End (last+1) in the container to mangle.
 *  \returns The specified arguments mangled and concatenated by "__sep__".
 */
std::string mangleTemplateArgsToString (const SgTemplateArgumentPtrList::const_iterator b, const SgTemplateArgumentPtrList::const_iterator e);

#if 0
// DQ (2/7/2006): This function is not used or required.
/*! Mangles a template argument list, returning an SgName object.
 *  \sa mangleTemplateArgsToStd::String
 */
SgName mangleTemplateArgs (const SgTemplateArgumentPtrList::const_iterator b, const SgTemplateArgumentPtrList::const_iterator e);
#endif
 
/*! Mangles an instantiated template.
 *
 *  \param[in] templ_name   Unmangled base name of the template. This std::string
 *                            should not contain the template arguments (i.e.,
 *                            should be 'foo' and not 'foo < int >').
 *  \param[in] templ_args   Container of template arguments.
 *  \param[in] scope        Scope of the function (e.g., via 'get_scope ()'),
 *                            used to get qualifiers.
 *  \returns Mangled name, including all parameter information.
 */
std::string mangleTemplateToString (const std::string& templ_name,
                               const SgTemplateArgumentPtrList& templ_args,
                               const SgScopeStatement* scope);

//! Mangles an instantiated template, returning an SgName object.
SgName mangleTemplate (const SgName& templ_name,
                       const SgTemplateArgumentPtrList& templ_args,
                       const SgScopeStatement* scope);

/*! Mangles an instantiated template function or template member function.
 *
 *  \param[in] templ_name   Unmangled base name of the function. This std::string
 *                            should not contain the template arguments (i.e.,
 *                            should be 'foo' and not 'foo < int >').
 *  \param[in] templ_args   Container of template arguments.
 *  \param[in] func_type    The function's type (e.g., via 'get_type ()')
 *  \param[in] scope        Scope of the function (e.g., via 'get_scope ()'),
 *                            used to get qualifiers.
 *  \returns Mangled name, including all parameter information.
 */
std::string
mangleTemplateFunctionToString (const std::string& templ_name,
                                const SgTemplateArgumentPtrList& templ_args,
                                const SgFunctionType* func_type,
                                const SgScopeStatement* scope);

/*! Mangles an instantiated template function or member function, returning
 *  a SgName object.
 *  \sa mangleTemplateFunctionToStd::String
 */
SgName
mangleTemplateFunction (const std::string& templ_name,
                        const SgTemplateArgumentPtrList& templ_args,
                        const SgFunctionType* func_type,
                        const SgScopeStatement* scope);

//! Mangles a boolean expression.
std::string mangleSgValueExp (const SgBoolValExp* expr);

//! Mangles a value expression, as used in a template argument.
std::string mangleValueExp (const SgValueExp* expr);

/*! Mangles a constant expression, needed for template arguments.
 *  \todo Find a better way to deal with general template expression
 *  arguments.
 */
std::string mangleExpression (const SgExpression* expr);


#if 0
// DQ (3/29/2006): I think this is only required interanlly in the mangledSupport.C file (where it is defined)

/*! Mangles a value expression.
 *
 *  This template function is parameterized by a specific Sage III
 *  value type (derived from SgValueExp), and specifically relies on
 *  the 'get_value ()' member function.
 */
template <class SgValueExpType_>
std::string
mangleSgValueExp (const SgValueExpType_* expr)
  {
    // Verify that SgValueExpType_ descends from SgValueExp.
    ROSE_ASSERT (isSgValueExp (expr) || !expr);

    ostringstream mangled_name;
    if (expr)
      mangled_name << expr->get_value ();
    return mangled_name.str ();
  }
#endif

#endif // mangling_support_INCLUDED
