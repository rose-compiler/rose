
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

const SgFunctionDefinition*
findRootFunc (const SgScopeStatement* scope)
   {
  // DQ (12/13/2011): This function is being called recursively (infinite recursion) for test2011_187.C (added support for SgTemplateFunctionDefinition).

  // printf ("Inside of findRootFunc(scope = %p) \n",scope);

     if (scope != NULL)
        {
          if (scope->variantT () == V_SgFunctionDefinition)
             {
               return isSgFunctionDefinition (scope);
             }
            else
             {
               if (scope->variantT () == V_SgTemplateFunctionDefinition)
                  {
                    return isSgTemplateFunctionDefinition (scope);
                  }
                 else
                  {
                 // DQ (12/13/2011): Adding test for improperly set scope.
                 // printf ("In findRootFunc(): scope = %p = %s \n",scope,scope->class_name().c_str());

                    SgScopeStatement* nextOuterScope = scope->get_scope();
                    ROSE_ASSERT(nextOuterScope != NULL);
#if 0
                    printf ("nextOuterScope = %p = %s \n",nextOuterScope,nextOuterScope->class_name().c_str());
#endif
                    ROSE_ASSERT(nextOuterScope != scope);

                    return findRootFunc(scope->get_scope());
                  }
             }
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
// DQ (3/20/2011): Make this a valid ostringstream.
// ostringstream mangled_name = "";
  ostringstream mangled_name;
// ROSE_ASSERT(scope != NULL);
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
#if 0
     printf ("In manglingSupport.C: mangleQualifiersToString(const SgScopeStatement*): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif

  // DQ (3/14/2012): I would like to make this assertion (part of required C++ support).
     ROSE_ASSERT(scope != NULL);

  // DQ (3/19/2011): Make this a valid string.
  // string mangled_name = "";
     string mangled_name = "";
     if (scope != NULL)
        {
          switch (scope->variantT ())
             {
            // DQ (9/27/2012): Added this case to be the same as that for SgClassDefinition (removed case below).
               case V_SgTemplateClassDefinition:

               case V_SgClassDefinition:
               case V_SgTemplateInstantiationDefn:
                  {
                    const SgClassDefinition* def = isSgClassDefinition (scope);
                    mangled_name = def->get_mangled_name().getString();

                    const SgClassDeclaration* classDeclaration = def->get_declaration();
                    string name = classDeclaration->get_name().str();
#if 0
                    printf ("In manglingSupport.C: mangleQualifiersToString(): mangled name for scope = %p = %s is: %s \n",scope,scope->class_name().c_str(),mangled_name.c_str());
                    printf ("In manglingSupport.C: mangleQualifiersToString(): class declaration = %p = %s class name = %s \n",classDeclaration,classDeclaration->class_name().c_str(),name.c_str());
#endif
                    break;
                  }

               case V_SgNamespaceDefinitionStatement:
                  {
                    const SgNamespaceDefinitionStatement* def = isSgNamespaceDefinitionStatement (scope);
                    mangled_name = def->get_mangled_name().getString();
#if 0
                    printf ("In manglingSupport.C: mangleQualifiersToString(): mangled name for scope = %p = %s is: %s \n",scope,scope->class_name().c_str(),mangled_name.c_str());
#endif
                    break;
                  }

            // DQ (9/27/2012): Added this case to be the same as that for SgFunctionDefinition (removed case below).
               case V_SgTemplateFunctionDefinition:

               case V_SgFunctionDefinition:
                  {
                 // 'scope' is part of scope for locally defined classes
                    const SgFunctionDefinition* def = isSgFunctionDefinition (scope);
                    ROSE_ASSERT (def);
                    mangled_name = def->get_mangled_name().getString();
                    break;
                  }

               case V_SgCatchOptionStmt:
               case V_SgDoWhileStmt:
               case V_SgForStatement:
               case V_SgIfStmt:
               case V_SgSwitchStatement:
               case V_SgWhileStmt:
               case V_SgBasicBlock:
                  {
                 // printf ("In mangleQualifiersToString(scope = %p = %s) \n",scope,scope->class_name().c_str());

                    const SgScopeStatement* stmt = isSgScopeStatement(scope);
                    ROSE_ASSERT (stmt != NULL);
                    string stmt_name      = mangleLocalScopeToString (stmt);
                    string par_scope_name = mangleQualifiersToString (scope->get_scope());

                    mangled_name = joinMangledQualifiersToString (par_scope_name,stmt_name);
                    break;
                  }

           // DQ (2/22/2007): I'm not sure this is best, but we can leave it for now.
           // I expect that global scope should contribute to the mangled name to avoid
           // confusion with name of declarations in un-name namespaces for example.
               case V_SgGlobal: // Global scope has an 'empty' name
                  {
                 // I think there is nothing to do for this case.
                    break;
                  }

#if 0
               case V_SgTemplateFunctionDefinition:
                  {
#if 1
                 // DQ (8/26/2012): Decrease the volume of warnings from this part of the code.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    static int count = 0;
                    if (count++ % 100 == 0)
                       {
                         printf ("WARNING: In mangleQualifiersToString(const SgScopeStatement*): Case SgTemplateFunctionDefinition not implemented \n");
                       }
#endif
#else
                    printf ("WARNING: In mangleQualifiersToString(const SgScopeStatement*): Case SgTemplateFunctionDefinition not implemented \n");
#endif
                    break;
                  }
#endif
#if 0
            // DQ (5/12/2012): Implement this case later, but it does not cause anything to fail presently (I think).
               case V_SgTemplateClassDefinition:
                  {
#if 1
                 // DQ (8/26/2012): Decrease the volume of warnings from this part of the code.
                    static int count = 0;
                    if (count++ % 100 == 0)
                       {
                         printf ("WARNING: In mangleQualifiersToString(const SgScopeStatement*): Case SgTemplateClassDefinition not implemented \n");
                       }
#else
                    printf ("WARNING: In mangleQualifiersToString(const SgScopeStatement*): Case SgTemplateClassDefinition not implemented \n");
#endif
                    break;
                  }
#endif

            // DQ (3/14/2012): I think that defaults should be resurced for errors, and not proper handling of unexpected cases.
               default: // Embed the class name for subsequent debugging.
                  {
                 // DQ (3/19/2014): Supress this output, but permit it with verbose level == 1.
                    if (SgProject::get_verbose() > 0)
                       {
                         printf ("WARNING: In mangleQualifiersToString(const SgScopeStatement*): case of scope = %p = %s not handled (default reached) \n",scope,scope->class_name().c_str());
                       }

                 // DQ (1/12/13): Assert that this is not a previously deleted IR node (which will have the name = "SgNode").
                    ROSE_ASSERT(scope->class_name() != "SgNode");

                 // DQ (1/12/13): Added assertion.
                    ROSE_ASSERT(scope->get_scope() != NULL);

                 // Surrounding scope name
                    string par_scope_name = mangleQualifiersToString (scope->get_scope());

                 // Compute a local scope name
                 //! \todo Compute local scope names correctly (consistently).
                    ostringstream scope_name;
                    scope_name << scope->class_name ();
AstRegExAttribute *attribute = (AstRegExAttribute *) scope -> getAttribute("name");
if (attribute) {
scope_name << "_" << attribute -> expression;
}

                 // Build full name
                    mangled_name = joinMangledQualifiersToString (par_scope_name,scope_name.str ());
                    break;
                  }
             }
        }

  // DQ (5/31/2012): Added test for template brackets that are caught later in AstConsistencyTests.
  // Make sure that there is no template specific syntax included in the mangled name
  // if ( mangled_name.find('<') != string::npos )
     if (SageInterface::hasTemplateSyntax(mangled_name) == true)
        {
       // string name = classDeclaration->get_name().str();
       // printf ("In mangleQualifiersToString(): scope = %p = %s unmangled name = %s check mangled class name = %s \n",scope,scope->class_name().c_str(),name.c_str(),mangled_name.c_str());
          printf ("In mangleQualifiersToString(): scope = %p = %s check mangled class name = %s \n",scope,scope->class_name().c_str(),mangled_name.c_str());
        }

  // ROSE_ASSERT(mangled_name.find('<') == string::npos);
     ROSE_ASSERT(SageInterface::hasTemplateSyntax(mangled_name) == false);

     return mangled_name;
   }


SgName
mangleQualifiers( const SgScopeStatement* scope )
   {
  // DQ (3/14/2012): I think we have top assert this here, though it appears to have been commented out.
  // This may become a part of a future set of language dependent assertions in the AST Build Interface
  // since it is more relevant for C++ than for other languges.
  // DQ (3/19/2011): I think that we want a valid scope else there is no proper pointer to the generated string.
  // ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(scope != NULL);

  // DQ (1/12/13): Assert that this is not a previously deleted IR node (which will have the name = "SgNode").
     ROSE_ASSERT(scope->class_name() != "SgNode");

  // DQ (2/17/2014): Adding debugging code (issue with new options to gnunet).
  // DQ (1/12/13): Added assertion.
     if (scope->get_scope() == NULL)
        {
#if 0
          printf ("Warning: In mangleQualifiers(): (scope->get_scope() == NULL): scope = %p = %s = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
#if 0
          scope->get_file_info()->display("Warning: scope->get_scope() == NULL: debug \n");
#endif
       // DQ (2/17/2014): In this case there is no mangled name.
          return SgName("");
        }
     ROSE_ASSERT(scope->get_scope() != NULL);

     string s = mangleQualifiersToString(scope);

#if 0
     printf ("In manglingSupport.C: mangleQualifiers(const SgScopeStatement*): returning s = %s \n",s.c_str());
#endif

     return SgName(s.c_str());
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
          ROSE_ASSERT (type_p != NULL);

       // if (is_first)
          if (is_first == true)
             {
               is_first = false;

            // DQ (5/11/2012): Make the mangled names a little more clear.
               mangled_name += "_";
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

  // DQ (5/11/2012): Make the mangled names a little more clear.
     mangled_name += "_";

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
  // DQ (7/24/2012): This causes a problem for the isspace(s[n_opstr]) test below (likely not the cause of the problem).
     string s_mangled (trimSpaces (s));
  // string s_mangled(s);

#if 0
     printf ("In mangleFunctionNameToString(s = %s ret_type_name = %s) \n",s.c_str(),ret_type_name.c_str());
#endif

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
#if 0
        printf ("In mangleFunctionNameToString(): Found operator syntax isspace(s[n_opstr]) = %s \n",isspace(s[n_opstr]) ? "true" : "false");
#endif
        if (isspace(s[n_opstr]))
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
#if 0
            printf ("In mangleFunctionNameToString(): s_op = %s \n",s_op.c_str());
#endif
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
#if 0
            printf ("In mangleFunctionNameToString(): Before s_mangled.replace(): s_mangled = %s \n",s_mangled.c_str());
#endif
         // DQ (2/7/2006): Bug fix for case of function such as operator_takes_lvalue_operand()
         // In the case of operator_takes_lvalue_operand() this should replace 
         // "_takes_lvalue_operand" with "_takes_lvalue_operand" (trivial case).
            s_mangled.replace (n_opstr, s_op.size (), s_op_mangled);
#if 0
            printf ("In mangleFunctionNameToString(): After s_mangled.replace(): s_mangled = %s \n",s_mangled.c_str());
#endif
          }
      }
    // else, leave name as is.
     else
      {
#if 0
        printf ("In mangleFunctionNameToString(): No operator syntax found \n");
#endif
      }

#if 0
  // DQ (7/24/2012): Added test (failing for test2004_141.C); fails later in AST consistency tests.
     ROSE_ASSERT(s_mangled.find('&') == string::npos);
     ROSE_ASSERT(s_mangled.find('*') == string::npos);
#endif

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
mangleTemplateArgsToString (const SgTemplateArgumentPtrList::const_iterator b,const SgTemplateArgumentPtrList::const_iterator e)
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
          ROSE_ASSERT (arg != NULL);

          mangled_name << arg->get_mangled_name().getString();
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
          args_mangled = mangleTemplateArgsToString(templ_args.begin(),templ_args.end());
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

  // DQ (5/31/2012): This should be a valid name to be a template.
     ROSE_ASSERT(templ_name.empty() == false);

  // DQ (5/31/2012): Find locations where this is set and include template syntax.
  // ROSE_ASSERT(templ_name.find('<') == string::npos);
  // ROSE_ASSERT(SageInterface::hasTemplateSyntax(templ_name) == false);

     if (func_type != NULL)
        {
          type_name = func_type->get_mangled().getString();

          const SgType* ret_type = func_type->get_return_type ();
          if (ret_type != NULL)
             {
               ret_type_name = ret_type->get_mangled ().getString ();
             }
        }
       else
        {
          type_name = "UNKNOWN_FUNCTION_TYPE";
        }

  // This function's name, transformed.
     string func_name = mangleFunctionNameToString(templ_name,ret_type_name);

#if 0
  // DQ (7/24/2012): Added test (failing for test2004_141.C); fails later in AST consistency tests.
     ROSE_ASSERT(func_name.find('&') == string::npos);
     ROSE_ASSERT(func_name.find('*') == string::npos);
#endif

  // Compute the final mangled name.
     string mangled_name = mangleTemplateToString(func_name,templ_args,scope) + "__" + type_name;

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

   // DQ (7/21/2012): Added support for IR node not seen previously except in new C++11 work.
      case V_SgTemplateParameterVal:
        mangled_name = "unsupported_SgTemplateParameterVal"; // mangleSgValueExp<SgTemplateParameterVal> (isSgTemplateParameterVal(expr));
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

  // DQ (9/8/2014): This now fails as a result of new template declaration handling.
     if (declParent == NULL)
        {
#if 0
          printf ("WARNING: In declarationHasTranslationUnitScope(): (declParent == NULL): decl = %p = %s (using non-defining declaration parent) \n",decl,decl->class_name().c_str());
#endif
          ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
          declParent = decl->get_firstNondefiningDeclaration()->get_parent();
        }
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
  // DQ (4/30/2012): I think there is a problem with this code.  Declarations can be the same even when they 
  // appear in different files or in different locations in the same file (or translation unit) and the inclusion 
  // of this information in the mangled name will prevent them from beeing seen as the same.  I am not sure
  // why this code is required and so we need to discuss this point.  
  // Additionally, when the file_id() is a negative number (e.g. for compiler generated code) then the name
  // mangling also fails the test (for mangled names to be useable as identifiers) since "-" appears in the name.

     if (declarationHasTranslationUnitScope(decl) == true)
        {
       // TV (04/22/11): I think 'decl' will refer to the same file_id than is enclosing file.
       //         And as in EDGrose file and global scope are linked after the building of the global scope...
       // return "_file_id_" + StringUtility::numberToString(SageInterface::getEnclosingFileNode(const_cast<SgDeclarationStatement *>(decl))->get_file_info()->get_file_id()) + "_";
       // return "_file_id_" + StringUtility::numberToString(decl->get_file_info()->get_file_id()) + "_";

       // DQ (4/30/2012): Modified this code, but I would like to better understand why it is required.
          string returnString = "";
          int fileIdNumber = decl->get_file_info()->get_file_id();
          if (fileIdNumber >= 0)
             {
               returnString = "_file_id_" + StringUtility::numberToString(fileIdNumber) + "_";
             }
            else
             {
            // Put "_minus" into the generated name.
               returnString = "_file_id_minus_" + StringUtility::numberToString(abs(fileIdNumber)) + "_";
             }

          return returnString;
        }
       else
        {
          return "";
        }
   }
