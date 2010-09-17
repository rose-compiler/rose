
// #include <cctype>
// #include <sstream>

#include "sage3basic.h"

using namespace std;



string
replaceNonAlphaNum (const string& s)
{
  ostringstream s_new;
  for (string::size_type i = 0; i < s.size (); ++i)
    {
      char s_i = s[i];
      if (!isalnum (s_i))
        s_new << '_' << (int)s_i;
      else
        s_new << s_i;
    }
  return s_new.str ();
}

string
trimSpaces (const string& s)
  {
    if (s.empty ())
      return s;
    
    string s_new (s);
    
    // strip trailing spaces
    string::size_type first_space = s_new.size ();
    ROSE_ASSERT (first_space >= 1);
    if (isspace (s_new[first_space-1]))
      {
        do
          first_space--;
        while (first_space > 0 && isspace (s_new[first_space-1]));
        ROSE_ASSERT (isspace (s_new[first_space]));
        s_new.erase (first_space); // erase to end of string
      }
    
    // strip leading spaces
    string::size_type first_nonspace = 0;
    while (first_nonspace < s_new.size () && isspace (s_new[first_nonspace]))
      first_nonspace++;
    if (first_nonspace)
      s_new.erase (0, first_nonspace);
    
    return s_new;
  }

#if 0
bool
isValidMangledName (string name)
   {
     if (name.empty () || isdigit (name[0]))
          return false;
    
     for (string::size_type i = 0; i < name.size (); ++i)
        {
          if (!isalnum (name[i]) && name[i] != '_')
               return false;
        }
  // Passed all above checks -- assume OK
     return true;
   }
#endif

string
joinMangledQualifiersToString (const string& base, const string& name)
{
  string mangled_name (base);
  if (!base.empty () && !name.empty ())
    mangled_name += "__scope__";
  mangled_name += name;
  return mangled_name;
}

SgName
joinMangledQualifiers (const SgName& base, const SgName& name)
   {
#if 0
  // DQ (8/25/2006): This is debugging code required to find a problem that 
  // results in a 480,000,000 character string used of a mangled name prefix 
  // within the boost_tests/test_boost_phoenix_v2.C
     printf ("base.getString ().size() = %ld name.getString ().size() = %ld \n",base.getString().size(),name.getString().size());

     const int MODERATE_SIZE = 1000;
     if (base.getString().size() > MODERATE_SIZE)
          printf ("base.getString() = %s \n",base.getString().c_str());

     const int MAX_SIZE = 10000;
     ROSE_ASSERT(base.getString().size() < MAX_SIZE);
     ROSE_ASSERT(name.getString().size() < MAX_SIZE);
#endif

     string mangled_name = joinMangledQualifiersToString (base.getString (),name.getString ());
     return SgName (mangled_name.c_str ());
   }

const SgFunctionDefinition *
findRootFunc (const SgScopeStatement* scope)
{
  if (scope) {
    if (scope->variantT () == V_SgFunctionDefinition)
      return isSgFunctionDefinition (scope);
    else
      return findRootFunc (scope->get_scope ());
  }
  // Not found.
  return NULL;
}


// size_t getLocalScopeNum ( SgFunctionDefinition* func_def, const SgScopeStatement* target)
size_t
getLocalScopeNum (const SgFunctionDefinition* func_def, const SgScopeStatement* target)
{
#if SKIP_BLOCK_NUMBER_CACHING
  // DQ (10/4/2006): This takes too long and stalls the compilation of 
  // some large codes (plum hall e.g. cvs06a/conform/ch7_22.c).  It is 
  // rewritten below to use a cache mechanisk link to a cache invalidation 
  // mechanism.

  // Preorder traversal to count the number of basic blocks preceeding 'target'
  class Traversal : public AstSimpleProcessing
  {
  public:
    Traversal (const SgScopeStatement* target)
      : target_ (target), count_ (0), found_ (false)
    {}
    void visit (SgNode* node)
    {
      if (!found_)
        {
          const SgScopeStatement* stmt = isSgScopeStatement (node);
          if (stmt)
            {
              count_++;
              found_ = (stmt == target_);
            }
        }
    }
    size_t count (void) const { return found_ ? count_ : 0; }
  private:
    const SgScopeStatement* target_; // Target scope statement
    size_t count_; // found_ ? number of target : number of last block seen
    bool found_; // true <==> target_ has been found
  };

  Traversal counter (target);
  counter.traverse (const_cast<SgFunctionDefinition *> (func_def), preorder);
  return counter.count ();
#else
  // DQ (10/6/2006): Implemented caching of computed lables for scopes in 
  // functions to avoid quadratic behavior of previous implementation.  The model
  // for this is the same a s what will be done to support caching of mangled names.
  // printf ("getLocalScopeNum calling func_def->get_scope_number(target)! \n");

     return func_def->get_scope_number(target);
#endif
}

string
mangleLocalScopeToString (const SgScopeStatement* scope)
{
  ostringstream mangled_name;
  if (scope)
    {
      // Find the function definition containing this scope.
      const SgFunctionDefinition* root_func = findRootFunc (scope);
      if (root_func)
        {
          size_t i = getLocalScopeNum (root_func, scope);
          ROSE_ASSERT (i > 0);
          mangled_name << "__SgSS" << i << "__";
        }
    }
  return mangled_name.str ();
}

string
mangleQualifiersToString (const SgScopeStatement* scope)
{
  string mangled_name;
  if (scope)
    {
      switch (scope->variantT ())
        {
        case V_SgClassDefinition:
        case V_SgTemplateInstantiationDefn:
          {
            const SgClassDefinition* def = isSgClassDefinition (scope);
            mangled_name = def->get_mangled_name ().getString ();
          }
          break;
        case V_SgNamespaceDefinitionStatement:
          {
            const SgNamespaceDefinitionStatement* def
              = isSgNamespaceDefinitionStatement (scope);
            mangled_name = def->get_mangled_name ().getString ();
          }
          break;
        case V_SgFunctionDefinition:
          // 'scope' is part of scope for locally defined classes
          {
            const SgFunctionDefinition* def = isSgFunctionDefinition (scope);
            ROSE_ASSERT (def);
            mangled_name = def->get_mangled_name ().getString ();
          }
          break;
        case V_SgCatchOptionStmt:
        case V_SgDoWhileStmt:
        case V_SgForStatement:
        case V_SgIfStmt:
        case V_SgSwitchStatement:
        case V_SgWhileStmt:
        case V_SgBasicBlock:
          {
         // printf ("In mangleQualifiersToString(scope = %p = %s) \n",scope,scope->class_name().c_str());

            const SgScopeStatement* stmt = isSgScopeStatement (scope);
            ROSE_ASSERT (stmt);
            string stmt_name = mangleLocalScopeToString (stmt);
            string par_scope_name =
              mangleQualifiersToString (scope->get_scope ());
            mangled_name = joinMangledQualifiersToString (par_scope_name,
                                                          stmt_name);
          }
          break;

     // DQ (2/22/2007): I'm not sure this is best, but we can leave it for now.
     // I expect that global scope should contribute to the mangled name to avoid
     // confusion with name of declarations in un-name namespaces for example.
        case V_SgGlobal: // Global scope has an 'empty' name
          break;

        default: // Embed the class name for subsequent debugging.
          {
            // Surrounding scope name
            string par_scope_name =
              mangleQualifiersToString (scope->get_scope ());

            // Compute a local scope name
            //! \todo Compute local scope names correctly (consistently).
            ostringstream scope_name;
            scope_name << scope->class_name ();

            // Build full name
            mangled_name = joinMangledQualifiersToString (par_scope_name,
                                                          scope_name.str ());
          }
          break;
        }
    }
  return mangled_name;
}


SgName
mangleQualifiers( const SgScopeStatement* scope )
   {
     string s = mangleQualifiersToString(scope);
     return SgName (s.c_str ());
   }

string
mangleTypesToString (const SgTypePtrList::const_iterator b,
                     const SgTypePtrList::const_iterator e)
   {
     string mangled_name;
     bool is_first = true;
     for (SgTypePtrList::const_iterator p = b; p != e; ++p)
        {
          const SgType* type_p = *p;
          ROSE_ASSERT (type_p);
          if (is_first)
             {
               is_first = false;
             }
            else
             {
               mangled_name += "__sep__"; // separator between argument types
             }

          ROSE_ASSERT(type_p != NULL);
          ROSE_ASSERT(const_cast<SgType *>(type_p) != NULL);
          SgName mangled_p = (const_cast<SgType *>(type_p))->get_mangled();

          mangled_name += string (mangled_p.str());
        }
     return mangled_name;
   }

SgName
mangleTypes (const SgTypePtrList::const_iterator b,
             const SgTypePtrList::const_iterator e)
  {
    string mangled_name_str = mangleTypesToString (b, e);
    return SgName (mangled_name_str.c_str ());
  }

string
mangleFunctionNameToString (const string& s, const string& ret_type_name )
  {
    string s_mangled (trimSpaces (s));

    // Special case: destructor names
    if (s[0] == '~')
      {
        s_mangled.replace (0, 1, "__dt");
        return s_mangled;
      }

    // Check for overloaded operators
    const string opstr ("operator");
    const string::size_type n_opstr = opstr.size ();
    const string newstr ("new");
    const string newarrstr ("new[]");
    const string delstr ("delete");
    const string delarrstr ("delete[]");

    if (s.substr (0, n_opstr) == opstr) // begins with "operator"
      {
        if (isspace (s[n_opstr]))
          {
            if (s.substr (n_opstr+1) == newstr) // is "operator new"
              s_mangled.replace (n_opstr, newstr.size ()+1, "__nw");
            else if (s.substr (n_opstr+1) == newarrstr) // "operator new[]"
              s_mangled.replace (n_opstr, newarrstr.size ()+1, "__na");
            else if (s.substr (n_opstr+1) == delstr) // is "operator delete"
              s_mangled.replace (n_opstr, delstr.size ()+1, "__dl");
            else if (s.substr (n_opstr+1) == delarrstr) // "operator delete[]"
              s_mangled.replace (n_opstr, delarrstr.size ()+1, "__da");
            else // Cast operators.
              s_mangled.replace (n_opstr, s.size () - n_opstr,
                                 "__opB__" + ret_type_name + "__opE__");
          }
        else // real operator (suffix after the substring "operator ")
          {
            string s_op = s.substr (n_opstr);

         // DQ (2/7/2006): Bug fix for case of function: operator_takes_lvalue_operand()
         // (this test appears in test2005_198.C).
            string s_op_mangled = s_op;
            if (s_op == "->")     s_op_mangled = "__rf";
            else if (s_op == "->*") s_op_mangled = "__rm";
            else if (s_op == "==")  s_op_mangled = "__eq";
            else if (s_op == "<")   s_op_mangled = "__lt";
            else if (s_op == ">")   s_op_mangled = "__gt";
            else if (s_op == "!=")  s_op_mangled = "__ne";
            else if (s_op == "<=")  s_op_mangled = "__le";
            else if (s_op == ">=")  s_op_mangled = "__ge";
            else if (s_op == "+")   s_op_mangled = "__pl";
            else if (s_op == "-")   s_op_mangled = "__mi";
            else if (s_op == "*")   s_op_mangled = "__ml";
            else if (s_op == "/")   s_op_mangled = "__dv";
            else if (s_op == "%")   s_op_mangled = "__md";
            else if (s_op == "&&")  s_op_mangled = "__aa";
            else if (s_op == "!")   s_op_mangled = "__nt";
            else if (s_op == "||")  s_op_mangled = "__oo";
            else if (s_op == "^")   s_op_mangled = "__er";
            else if (s_op == "&")   s_op_mangled = "__ad";
            else if (s_op == "|")   s_op_mangled = "__or";
            else if (s_op == ",")   s_op_mangled = "__cm";
            else if (s_op == "<<")  s_op_mangled = "__ls";
            else if (s_op == ">>")  s_op_mangled = "__rs";
            else if (s_op == "--")  s_op_mangled = "__mm";
            else if (s_op == "++")  s_op_mangled = "__pp";
            else if (s_op == "~")   s_op_mangled = "__co";
            else if (s_op == "=")   s_op_mangled = "__as";
            else if (s_op == "+=")  s_op_mangled = "__apl";
            else if (s_op == "-=")  s_op_mangled = "__ami";
            else if (s_op == "&=")  s_op_mangled = "__aad";
            else if (s_op == "|=")  s_op_mangled = "__aor";
            else if (s_op == "*=")  s_op_mangled = "__amu";
            else if (s_op == "/=")  s_op_mangled = "__adv";
            else if (s_op == "%=")  s_op_mangled = "__amd";
            else if (s_op == "^=")  s_op_mangled = "__aer";
            else if (s_op == "<<=") s_op_mangled = "__als";
            else if (s_op == ">>=") s_op_mangled = "__ars";
            else if (s_op == "()")  s_op_mangled = "__cl";
            else if (s_op == "[]")  s_op_mangled = "__xi";
            else
              {
             // DQ (1/8/2006): This is the case of a name that just happends to start with 
             // the work "operator" (e.g. operator_takes_lvalue_operand, in test2005_198.C)
             // the mangle form is just the unmodified function name.
             // rtmp = fname;
              }

         // DQ (2/7/2006): Bug fix for case of function such as operator_takes_lvalue_operand()
         // In the case of operator_takes_lvalue_operand() this should replace 
         // "_takes_lvalue_operand" with "_takes_lvalue_operand" (trivial case).
            s_mangled.replace (n_opstr, s_op.size (), s_op_mangled);
          }
      }
    // else, leave name as is.

    return s_mangled;
  }

SgName
mangleFunctionName (const SgName& n, const SgName& ret_type_name )
  {
    string s_mangled = mangleFunctionNameToString (n.getString (),
                                                   ret_type_name.str ());
    SgName n_mangled (s_mangled.c_str ());
    return n_mangled;
  }

string
mangleTemplateArgsToString (const SgTemplateArgumentPtrList::const_iterator b,
                            const SgTemplateArgumentPtrList::const_iterator e)
  {
    ostringstream mangled_name;
    bool is_first = true;
    for (SgTemplateArgumentPtrList::const_iterator i = b; i != e; ++i)
      {
        if (is_first)
          is_first = false;
        else // !is_first, so insert a "comma"
          mangled_name << "__sep__";
        
        const SgTemplateArgument* arg = *i;
        ROSE_ASSERT (arg);
        mangled_name << arg->get_mangled_name ().getString ();
      }
    return mangled_name.str ();
  }

#if 0
// DQ (2/7/2006): This function is not used or required.
SgName
mangleTemplateArgs (const SgTemplateArgumentPtrList::const_iterator b,
                    const SgTemplateArgumentPtrList::const_iterator e)
  {
    string s_mangled = mangleTemplateArgsToString (b, e);
    return SgName (s_mangled.c_str ());
  }
#endif
 
string
mangleTemplateToString (const string& templ_name,
                        const SgTemplateArgumentPtrList& templ_args,
                        const SgScopeStatement* scope)
   {
  // Mangle all the template arguments
     string args_mangled;
  // if (templ_args.begin() != templ_args.end())
     if (templ_args.empty() == true)
        {
          args_mangled = "unknown_arg";
        }
       else
        {
          args_mangled = mangleTemplateArgsToString (templ_args.begin(),templ_args.end());
        }

  // Compute the name qualification, if any.
     string scope_name;
     if (scope == NULL)
        {
          scope_name = "unknown_scope";
        }
       else
        {
          scope_name = mangleQualifiersToString (scope);
        }

  // Compute the final mangled name.
     string mangled_name = joinMangledQualifiersToString (scope_name, templ_name);
  // printf ("joinMangledQualifiersToString (scope_name, templ_name) : mangled_name = %s \n",mangled_name.c_str());

     if (mangled_name.empty() == true)
        {
          mangled_name = "unknown_template_name";
        }

     mangled_name += "__tas__" + args_mangled + "__tae__";

  // printf ("args_mangled = %s mangled_name = %s \n",args_mangled.c_str(),mangled_name.c_str());

     return mangled_name;
   }

SgName
mangleTemplate (const SgName& templ_name,
                const SgTemplateArgumentPtrList& templ_args,
                const SgScopeStatement* scope)
   {
     string mangled_name = mangleTemplateToString(templ_name.getString(),templ_args,scope);
  // printf ("In mangleTemplate(): mangled_name = %s \n",mangled_name.c_str());
     return SgName (mangled_name.c_str());
   }

string
mangleTemplateFunctionToString (const string& templ_name,
                                const SgTemplateArgumentPtrList& templ_args,
                                const SgFunctionType* func_type,
                                const SgScopeStatement* scope)
   {
  // Compute a mangled name for this function's type
     string type_name;
     string ret_type_name;
     if (func_type)
        {
          type_name = func_type->get_mangled ().getString ();
          const SgType* ret_type = func_type->get_return_type ();
          if (ret_type)
               ret_type_name = ret_type->get_mangled ().getString ();
        }
       else
        {
          type_name = "UNKNOWN_FUNCTION_TYPE";
        }

  // This function's name, transformed.
     string func_name = mangleFunctionNameToString (templ_name,ret_type_name);

  // Compute the final mangled name.
     string mangled_name = mangleTemplateToString (func_name, templ_args, scope) + "__" + type_name;

     return mangled_name;
   }

SgName
mangleTemplateFunction (const string& templ_name,
                        const SgTemplateArgumentPtrList& templ_args,
                        const SgFunctionType* func_type,
                        const SgScopeStatement* scope)
  {
    string mangled = mangleTemplateFunctionToString (templ_name,
                                                     templ_args,
                                                     func_type,
                                                     scope);
    return SgName (mangled.c_str ());
  }

/*! Mangles a value expression.
 *
 *  This template function is parameterized by a specific Sage III
 *  value type (derived from SgValueExp), and specifically relies on
 *  the 'get_value ()' member function.
 */
template <class SgValueExpType_>
string
mangleSgValueExp (const SgValueExpType_* expr)
  {
    // Verify that SgValueExpType_ descends from SgValueExp.
    ROSE_ASSERT (isSgValueExp (expr) || !expr);

    ostringstream mangled_name;
    if (expr)
      mangled_name << expr->get_value ();
    return mangled_name.str ();
  }

string
mangleSgValueExp (const SgBoolValExp* expr)
  {
    return (expr && expr->get_value ()) ? "true" : "false";
  }

string
mangleValueExp (const SgValueExp* expr)
  {
    string mangled_name;
    switch (expr->variantT ())
      {
      case V_SgBoolValExp:
        mangled_name =
          mangleSgValueExp<SgBoolValExp> (isSgBoolValExp (expr));
        break;
      case V_SgCharVal:
        mangled_name =
          mangleSgValueExp<SgCharVal> (isSgCharVal (expr));
        break;
      case V_SgDoubleVal:
        mangled_name =
          mangleSgValueExp<SgDoubleVal> (isSgDoubleVal (expr));
        break;
      case V_SgEnumVal:
        mangled_name =
          mangleSgValueExp<SgEnumVal> (isSgEnumVal (expr));
        break;
      case V_SgFloatVal:
        mangled_name =
          mangleSgValueExp<SgFloatVal> (isSgFloatVal (expr));
        break;
      case V_SgIntVal:
        mangled_name =
          mangleSgValueExp<SgIntVal> (isSgIntVal (expr));
        break;
      case V_SgLongDoubleVal:
        mangled_name =
          mangleSgValueExp<SgLongDoubleVal> (isSgLongDoubleVal (expr));
        break;
      case V_SgLongIntVal:
        mangled_name =
          mangleSgValueExp<SgLongIntVal> (isSgLongIntVal (expr));
        break;
     case V_SgLongLongIntVal: // Added by Liao, 2/10/2009
        mangled_name =
          mangleSgValueExp<SgLongLongIntVal> (isSgLongLongIntVal (expr));
        break;
     case V_SgShortVal:
        mangled_name =
          mangleSgValueExp<SgShortVal> (isSgShortVal (expr));
        break;
      case V_SgStringVal:
        mangled_name =
          mangleSgValueExp<SgStringVal> (isSgStringVal (expr));
        break;
      case V_SgUnsignedCharVal:
        mangled_name =
          mangleSgValueExp<SgUnsignedCharVal> (isSgUnsignedCharVal (expr));
        break;
      case V_SgUnsignedIntVal:
        mangled_name =
          mangleSgValueExp<SgUnsignedIntVal> (isSgUnsignedIntVal (expr));
        break;
      case V_SgUnsignedLongLongIntVal:
        mangled_name =
          mangleSgValueExp<SgUnsignedLongLongIntVal> (isSgUnsignedLongLongIntVal (expr));
        break;
      case V_SgUnsignedLongVal:
        mangled_name =
          mangleSgValueExp<SgUnsignedLongVal> (isSgUnsignedLongVal (expr));
        break;
      case V_SgUnsignedShortVal:
        mangled_name =
          mangleSgValueExp<SgUnsignedShortVal> (isSgUnsignedShortVal (expr));
        break;
      case V_SgWcharVal:
        mangled_name =
          mangleSgValueExp<SgWcharVal> (isSgWcharVal (expr));
        break;
      case V_SgCastExp:
        {
          const SgCastExp* cast_expr = isSgCastExp (expr); ROSE_ASSERT (cast_expr);
          const SgExpression* op = cast_expr->get_operand ();
          const SgType* cast_type = cast_expr->get_type ();
          mangled_name = "__Cstb__"
            + cast_type->get_mangled ().getString ()
            + mangleExpression (op)
            + "__Cste__";
        }
        break;
      default:
        std::cerr<<"Error! Unhandled case in mangleValueExp() for "<<expr->sage_class_name()<<std::endl; 
        ROSE_ASSERT (false); // Unhandled case.
        break;
      }
    return replaceNonAlphaNum (mangled_name);
  }

string
mangleExpression (const SgExpression* expr)
  {
    ostringstream mangled_name;
    if (expr)
      {
        // Handle value types and simple casts as a special case:
        const SgValueExp* val = isSgValueExp (expr);
        if (val)
          mangled_name << mangleValueExp (val);
        else // Punt on the rest.
          mangled_name << "EXPR";
      }
    return mangled_name.str ();
  }

bool
declarationHasTranslationUnitScope (const SgDeclarationStatement* decl)
   {
     ROSE_ASSERT(decl != NULL);
     SgNode *declParent = decl->get_parent();
     ROSE_ASSERT(declParent != NULL);
     VariantT declParentV = declParent->variantT();

     if (declParentV == V_SgGlobal || declParentV == V_SgNamespaceDefinitionStatement)
        {
       // If the declaration is static (in the C sense), it will have translation unit scope.
          if (decl->get_declarationModifier().get_storageModifier().isStatic())
               return true;

       // Likewise if the declaration is an inline function.
          if (const SgFunctionDeclaration *fnDecl = isSgFunctionDeclaration(decl))
             {
               if (fnDecl->get_functionModifier().isInline())
                    return true;
             }
        }

  // Likewise if the declaration is an anonymous namespace
     if (const SgNamespaceDeclarationStatement *nsDecl = isSgNamespaceDeclarationStatement(decl))
        {
          if (nsDecl->get_isUnnamedNamespace())
               return true;
        }

     return false;
   }

string
mangleTranslationUnitQualifiers (const SgDeclarationStatement* decl)
   {
     if (declarationHasTranslationUnitScope(decl))
        {
          return "_file_id_" + StringUtility::numberToString(SageInterface::getEnclosingFileNode(const_cast<SgDeclarationStatement *>(decl))->get_file_info()->get_file_id()) + "_";
        }
     else
        {
          return "";
        }
   }
