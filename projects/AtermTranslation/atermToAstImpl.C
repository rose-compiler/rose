#include "rose.h"
#include "inlinerSupport.h"
// #include "initializeExplicitScopeData.h"
#include <stdio.h>
#include "aterm1.h"
#include "aterm2.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "atermTranslation.h"

using namespace std;

void FixSgTree(SgNode*);

// DQ (8/12/2004): This code identifies several problems with the
// curent unparser mechanims.
string myUnparse(SgNode* n) 
   {
     if (n == 0) 
          return "NULL";

     if (isSgInitializedName(n)) 
        {
          SgInitializedName* initname = isSgInitializedName(n);
          SgInitializer* init = initname->get_initializer();
          SgName name = initname->get_name();
          return string("") + initname->get_type()->unparseToString() + " " + (name.str() ? name.str() : "") + " " + (init ? string("= ") + init->unparseToString() : "");
        }
       else 
        {
          if (isSgFunctionParameterList(n)) 
             {
               SgFunctionParameterList* pl = isSgFunctionParameterList(n);
               string result = "(";
               for (SgInitializedNamePtrList::iterator i = pl->get_args().begin(); i != pl->get_args().end(); ++i)
                    result += string(i == pl->get_args().begin() ? "" : ", ") + myUnparse(*i);
               result += ")";
               return result;
             }
            else
             {
               if (isSgVariableDeclaration(n) && !(n->get_parent())) 
                  {
                    return "FIXME: vardecl";
                  }
                 else 
                  {
                    if (isSgClassDeclaration(n) && !(n->get_parent()))
                       {
                         return "FIXME: classdecl";
                       }
                      else 
                       {
                         if (isSgClassType(n) && !(isSgClassType(n)->get_declaration()->get_parent())) 
                            {
                              return "FIXME: classtype";
                            } 
                           else 
                            {
                              return n->unparseToString();
                            }
                       }
                  }
             }
        }
   }


string
aterm_type_name( ATerm term )
   {
  // Debugging support.

     string s;
     int atermType = ATgetType(term);

     switch (atermType)
        {
          case AT_APPL:        s = "AT_APPL";        break;
          case AT_INT:         s = "AT_INT";         break;
          case AT_REAL:        s = "AT_REAL";        break;
          case AT_LIST:        s = "AT_LIST";        break;
          case AT_PLACEHOLDER: s = "AT_PLACEHOLDER"; break;
          case AT_BLOB:        s = "AT_BLOB";        break;

          default:
             {
               printf ("In graph(ATerm): term = %p: default reached atermType = %d \n",term,atermType);
               ROSE_ASSERT(!"error to reach default in switch!");
             }
        }

     return s;
   }


vector<ATerm> getAtermList(ATerm ls) 
   {
     ATerm a, b;
     vector<ATerm> result;
     while (true) 
        {
          if (ATmatch(ls, "[]"))
               return result;
            else 
               if (ATmatch(ls, "[<term>, <list>]", &a, &b)) 
                  {
                    result.push_back(a);
                    ls = b;
                  } 
                 else
                    ROSE_ASSERT (!"getAtermList");
        }
   }


class LinkUpdater 
   {
     public:
          virtual void update(SgNode*) const = 0;
   };


template <class Sym, class Decl>
class SetDeclarationUpdate: public LinkUpdater 
   {
     Sym* sym;

     public:
          SetDeclarationUpdate(Sym* sym): sym(sym) {}

          virtual void update(SgNode* n) const 
             {
               sym->set_declaration(dynamic_cast<Decl*>(n));
             }
   };

// #define DEBUG

Sg_File_Info*
getAtermFileInfo(ATerm term) 
   {
     ATerm loc = ATgetAnnotation(term, ATmake("location"));
     if (loc == NULL) return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
     int line, col;
     char* filename;
     ROSE_ASSERT(ATmatch(loc, "Position(<str>, <int>, <int>)", &filename, &line, &col));

     return new Sg_File_Info(filename, line, col);
   }


class AtermToNodeConverter 
   {
     public:
          map<string, list<LinkUpdater*> > idUpdates;

       // map<string, SgNode*> targetLocations;
          map<string, SgScopeStatement*> translationScopeMap;
          map<string, SgType*> translationTypeMap;
          map<string, SgDeclarationStatement*> translationDeclarationMap;
          map<string, SgInitializedName*> translationInitializedNameMap;

          void updateScopeMap (string key, SgScopeStatement* scope);
          SgScopeStatement* lookupScope (string key);

          void updateDeclarationMap (string key, SgDeclarationStatement* scope);
          SgDeclarationStatement* lookupDeclaration (string key);

          void updateTypeMap (string key, SgType* scope);
          SgType* lookupType (string key);

          void updateInitializedNameMap (string key, SgInitializedName* scope);
          SgInitializedName* lookupInitializedName (string key);

          SgNode* convertAtermToNode(ATerm term);

   private:
          template <class TranslationMap, class TranslationMapElement>
          void updateMap_support (string key, TranslationMap & translationMap, TranslationMapElement* node);

          template <class TranslationMap, class TranslationMapElement>
          TranslationMapElement* lookupMap_support (string key, TranslationMap & tranlationMap);
   };


template <class TranslationMap, class TranslationMapElement>
void
AtermToNodeConverter::updateMap_support (string key, TranslationMap & translationMap, TranslationMapElement* node)
   {
  // targetLocations[scope_mangled_name_string] = global;
  // translationScopeMap[key] = scope;
     if (translationMap.find(key) == translationMap.end())
        {
          translationMap[key] = node;
        }
       else
        {
          if (node == translationMap[key])
             {
               printf ("Already present in translationMap: OK \n");
             }
            else
             {
               printf ("scope used to update translationScopeMap is different from that already present: node = %p = %s \n",node,node->class_name().c_str());
               printf ("   --- key = %s \n",key.c_str());
               printf ("   --- translationScopeMap[key] = %p = %s \n",translationMap[key],translationMap[key]->class_name().c_str());

               printf ("ERROR: attempt to update translationMap with a different scope \n");
               ROSE_ASSERT(false);
             }
        }
   }

void 
AtermToNodeConverter::updateScopeMap (string key, SgScopeStatement* scope)
   {
     updateMap_support(key,translationScopeMap,scope);
   }

void 
AtermToNodeConverter::updateDeclarationMap (string key, SgDeclarationStatement* decl)
   {
     updateMap_support(key,translationDeclarationMap,decl);
   }

void 
AtermToNodeConverter::updateTypeMap (string key, SgType* type)
   {
     updateMap_support(key,translationTypeMap,type);
   }

void 
AtermToNodeConverter::updateInitializedNameMap (string key, SgInitializedName* initializedName)
   {
     updateMap_support(key,translationInitializedNameMap,initializedName);
   }

template <class TranslationMap, class TranslationMapElement>
TranslationMapElement* 
AtermToNodeConverter::lookupMap_support (string key, TranslationMap & translationMap)
   {
     TranslationMapElement* returnScope = NULL;
     if (translationMap.find(key) != translationMap.end())
        {
          returnScope = translationMap[key];

       // Detect any NULL entries in the map.
          ROSE_ASSERT(returnScope != NULL);
        }
       else
        {
          printf ("key not present in translationMap (returning NULL might be OK) \n");
          ROSE_ASSERT(false);
        }

     return returnScope;
   }

SgScopeStatement* 
AtermToNodeConverter::lookupScope (string key)
   {
     SgScopeStatement* returnScope = NULL;
     returnScope = lookupMap_support<map<string, SgScopeStatement*>,SgScopeStatement>(key,translationScopeMap);
     return returnScope;
   }

SgDeclarationStatement* 
AtermToNodeConverter::lookupDeclaration (string key)
   {
     SgDeclarationStatement* returnDeclaration = NULL;
     returnDeclaration = lookupMap_support<map<string, SgDeclarationStatement*>,SgDeclarationStatement>(key,translationDeclarationMap);
     return returnDeclaration;
   }

SgType* 
AtermToNodeConverter::lookupType (string key)
   {
     SgType* returnType = NULL;
     returnType = lookupMap_support<map<string, SgType*>,SgType>(key,translationTypeMap);
     return returnType;
   }

SgInitializedName* 
AtermToNodeConverter::lookupInitializedName (string key)
   {
     SgInitializedName* returnInitializedName = NULL;
     returnInitializedName = lookupMap_support<map<string, SgInitializedName*>,SgInitializedName>(key,translationInitializedNameMap);
     return returnInitializedName;
   }


SgNode*
AtermToNodeConverter::convertAtermToNode(ATerm term) 
   {
     Sg_File_Info* fi = getAtermFileInfo(term);
     SgNode* result = 0;
     ATerm temp1, temp2, temp3, temp4, temp5;
     char* str;
     char* str2;
     int tempint;
     double tempdouble;

  // This has to be declared before the first goto.
     string unrecognizedAterm = "null";

#if 1
     string atermString = ATwriteToString(term);
     int arity = 0;
     printf ("In AtermToNodeConverter::convertAtermToNode(): arity = %d atermString = %s \n",arity,atermString.c_str());
#endif

     if (ATmatch(term, "NULL") || ATmatch(term, "\"NULL\"")) 
        {
#if 1
          printf ("Found NULL pointer! goto done \n");
#endif
          result = 0;
          goto done;
        }

#define DO_BINARY(key, name, t1, t2) \
     if (ATmatch(term, key "(<term>, <term>)", &temp1, &temp2)) \
        { \
          result = new name(fi, is##t1(convertAtermToNode(temp1)), is##t2(convertAtermToNode(temp2))); \
          goto done; \
        }

    DO_BINARY("Add", SgAddOp, SgExpression, SgExpression)
    DO_BINARY("Multiply", SgMultiplyOp, SgExpression, SgExpression)
    DO_BINARY("Divide", SgDivideOp, SgExpression, SgExpression)
    DO_BINARY("Less", SgLessThanOp, SgExpression, SgExpression)
    DO_BINARY("LessEqual", SgLessOrEqualOp, SgExpression, SgExpression)
    DO_BINARY("Greater", SgGreaterThanOp, SgExpression, SgExpression)
    DO_BINARY("GreaterEqual", SgGreaterOrEqualOp, SgExpression, SgExpression)
    DO_BINARY("Assign", SgAssignOp, SgExpression, SgExpression)
    DO_BINARY("AddAssign", SgPlusAssignOp, SgExpression, SgExpression)
    DO_BINARY("Index", SgPntrArrRefExp, SgExpression, SgExpression)
    DO_BINARY("Call", SgFunctionCallExp, SgExpression, SgExprListExp)
    DO_BINARY("Cast", SgCastExp, SgExpression, SgType)

#undef DO_BINARY

#define DO_BINARY_NOFILEINFO(key, name, t1, t2) \
     if (ATmatch(term, key "(<term>, <term>)", &temp1, &temp2)) \
        { \
          result = new name(is##t1(convertAtermToNode(temp1)), is##t2(convertAtermToNode(temp2))); \
          goto done; \
        }

     DO_BINARY_NOFILEINFO("Array", SgArrayType, SgType, SgExpression)
#undef DO_BINARY_NOFILEINFO

#define DO_UNARY(key, name, t1) \
     if (ATmatch(term, key "(<term>)", &temp1)) \
        { \
          result = new name(fi, is##t1(convertAtermToNode(temp1))); \
          goto done; \
        }

     DO_UNARY("Return", SgReturnStmt, SgExpression)
     DO_UNARY("ExprStmt", SgExprStatement, SgExpression)
     DO_UNARY("AssignInit", SgAssignInitializer, SgExpression)
  // DO_UNARY("Increment", SgPlusPlusOp, SgExpression)

#undef DO_UNARY

  // if (ATmatch(term, "Increment(<term>,<term>)", &str, &temp1))
     if (ATmatch(term, "Increment(<term>,<term>)", &temp1, &temp2))
        {
          bool postfix = false;
          bool prefix  = false;
          if (ATmatch(temp1,"Postfix"))
             {
               postfix = true;
#if 0
               printf ("Detected SgPlusPlusOp (Increment): Postfix \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
               if (ATmatch(temp1,"Prefix"))
                  {
                    prefix = true;
#if 0
                    printf ("Detected SgPlusPlusOp (Increment): Prefix \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }

          printf ("In ATmatch for SgPlusPlusOp: temp1: %s \n",(prefix ? "prefix" : (postfix ? "postfix" : "unknown")));
#if 0
          printf ("Detected SgPlusPlusOp (Increment): \n");
          ROSE_ASSERT(false);
#endif
          result = new SgPlusPlusOp(fi, isSgExpression(convertAtermToNode(temp2)), (prefix ? SgUnaryOp::prefix : (postfix ? SgUnaryOp::postfix : /* default*/ SgUnaryOp::postfix)) );
          goto done;
        }

     if (ATmatch(term, "ExprRoot(<term>)", &temp1)) 
        {
          result = convertAtermToNode(temp1);
          goto done;
        }

#define DO_UNARY_NOFILEINFO(key, name, t1) \
     if (ATmatch(term, key "(<term>)", &temp1)) \
        { \
          result = new name(is##t1(convertAtermToNode(temp1))); \
          goto done; \
        }

     DO_UNARY_NOFILEINFO("Pointer", SgPointerType, SgType)
     DO_UNARY_NOFILEINFO("Reference", SgReferenceType, SgType)
#undef DO_UNARY_NOFILEINFO

     if (ATmatch(term, "FunctionType(<term>, [<list>])", &temp1, &temp2)) 
        {
          SgFunctionType* ft = new SgFunctionType(isSgType(convertAtermToNode(temp1)));
          vector<ATerm> fargs = getAtermList(temp2);
          for (size_t i = 0; i < fargs.size(); ++i)
               ft->get_arguments().push_back(isSgType(convertAtermToNode(fargs[i])));
          result = ft;
          goto done;
        }

#define DO_NULLARY_NOFILEINFO(key, name) \
     if (ATmatch(term, key)) \
        { \
          result = new name(); \
          goto done; \
        }

     DO_NULLARY_NOFILEINFO("Int", SgTypeInt)
     DO_NULLARY_NOFILEINFO("Bool", SgTypeBool)
     DO_NULLARY_NOFILEINFO("Void", SgTypeVoid)
     DO_NULLARY_NOFILEINFO("Float", SgTypeFloat)
     DO_NULLARY_NOFILEINFO("Double", SgTypeDouble)
     DO_NULLARY_NOFILEINFO("Char", SgTypeChar)
     DO_NULLARY_NOFILEINFO("SignedChar", SgTypeSignedChar)
     DO_NULLARY_NOFILEINFO("Short", SgTypeShort)
     DO_NULLARY_NOFILEINFO("Long", SgTypeLong)
     DO_NULLARY_NOFILEINFO("LongLong", SgTypeLongLong)
     DO_NULLARY_NOFILEINFO("LongDouble", SgTypeLongDouble)
     DO_NULLARY_NOFILEINFO("UnsignedLong", SgTypeUnsignedLong)
     DO_NULLARY_NOFILEINFO("UnsignedShort", SgTypeUnsignedShort)
     DO_NULLARY_NOFILEINFO("UnsignedChar", SgTypeUnsignedChar)
     DO_NULLARY_NOFILEINFO("UnsignedInt", SgTypeUnsignedInt)
     DO_NULLARY_NOFILEINFO("UnsignedLongLong", SgTypeUnsignedLongLong)
     DO_NULLARY_NOFILEINFO("DefaultType", SgTypeDefault)

  // Adding more types (not names must match those used to generate the ATerms).
     DO_NULLARY_NOFILEINFO("SgTypeSignedShort", SgTypeSignedShort)
     DO_NULLARY_NOFILEINFO("SgTypeSignedInt", SgTypeSignedInt)
     DO_NULLARY_NOFILEINFO("SgTypeSignedLong", SgTypeSignedLong)

#undef DO_NULLARY_NOFILEINFO

#define DO_NULLARY(key, name) \
     if (ATmatch(term, key)) \
        { \
          result = new name(fi); \
          goto done; \
        }

     DO_NULLARY("Default", SgDefaultOptionStmt)
#undef DO_NULLARY

     if (ATmatch(term, "Project([<list>])", &temp1))
        {
          SgProject* proj = new SgProject();
          vector<ATerm> terms = getAtermList(temp1);

       // proj->set_fileList_ptr(new SgFilePtrList());
          ROSE_ASSERT(proj->get_fileList_ptr() != NULL);

          for (size_t i = 0; i < terms.size(); ++i)
             {
            // proj->get_fileList_ptr()->push_back(isSgFile(convertAtermToNode(terms[i])));
               proj->get_fileList_ptr()->get_listOfFiles().push_back(isSgFile(convertAtermToNode(terms[i])));
             }
          result = proj;
          goto done;
        }

  // DQ (8/12/2004): This does not work because SgFile() constructor is broken.
  // As a result the input Aterm must be edited to remove this node.
     if (ATmatch(term, "File(<str>, <term>)", &str, &temp1)) 
        {
       // SgFile* file = new SgFile();
          SgSourceFile* file = new SgSourceFile();
       // file->set_root(isSgGlobal(convertAtermToNode(temp1)));
          file->set_globalScope(isSgGlobal(convertAtermToNode(temp1)));
          string name = str; // FIXME: use this
          result = file;
          goto done;
        }

  // if (ATmatch(term, "Global([<list>])", &temp1))
     if (ATmatch(term, "Global(<str>, [<list>])", &str, &temp1))
        {
          SgGlobal* global = new SgGlobal(fi);
          vector<ATerm> terms = getAtermList(temp1);

          string scope_mangled_name_string = str;
          printf ("scope_mangled_name_string = %s \n",scope_mangled_name_string.c_str());

       // targetLocations[scope_mangled_name_string] = global;
       // translationScopeMap[scope_mangled_name_string] = global;
          updateScopeMap(scope_mangled_name_string,global);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          for (size_t i = 0; i < terms.size(); ++i) 
             {
               global->append_declaration(isSgDeclarationStatement(convertAtermToNode(terms[i])));
             }
          result = global;
          goto done;
        }

     if (ATmatch(term, "ExprList([<list>])", &temp1))
        {
          SgExprListExp* exprlist = new SgExprListExp(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               exprlist->get_expressions().push_back(isSgExpression(convertAtermToNode(terms[i])));
             }
          result = exprlist;
          goto done;
        }

     if (ATmatch(term, "Block([<list>])", &temp1))
        {
          SgBasicBlock* block = new SgBasicBlock(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               block->get_statements().push_back(isSgStatement(convertAtermToNode(terms[i])));
            // FIXME: add to symbol tables
             }
          result = block;
          goto done;
        }

  // DQ (9/28/2014): The ATerm appears to be associated with a different name than expected.
     if (ATmatch(term, "[Block([<list>])]", &temp1))
        {
          printf ("This is an alternative block! \n");
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          SgBasicBlock* block = new SgBasicBlock(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               block->get_statements().push_back(isSgStatement(convertAtermToNode(terms[i])));
            // FIXME: add to symbol tables
             }
          result = block;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          goto done;
        }

     if (ATmatch(term, "Function(<str>, <term>, [<list>], <term>)",&str, &temp1, &temp2, &temp3))
        {
          string funcname = str;
          SgType* return_type = isSgType(convertAtermToNode(temp1));
          ROSE_ASSERT (return_type);
          vector<ATerm> params = getAtermList(temp2);
          SgFunctionParameterList* pl = new SgFunctionParameterList(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          for (size_t i = 0; i < params.size(); ++i)
             {
               SgNode* argi = convertAtermToNode(params[i]);
               ROSE_ASSERT (isSgInitializedName(argi));
               pl->append_arg(isSgInitializedName(argi));
             }
          SgFunctionDefinition* def = isSgFunctionDefinition(convertAtermToNode(temp3));
          SgFunctionType* ft = new SgFunctionType(return_type, false);
          for (SgInitializedNamePtrList::iterator i = pl->get_args().begin(); i != pl->get_args().end(); ++i)
               ft->get_arguments().push_back((*i)->get_type());
          SgFunctionDeclaration* decl = new SgFunctionDeclaration(fi, funcname.c_str(), ft, 0);
          for (SgInitializedNamePtrList::iterator i = pl->get_args().begin(); i != pl->get_args().end(); ++i)
             {
               (*i)->set_definition(decl);
               decl->get_args().push_back(*i);
             }
          if (def)
               decl->set_definition(def);
          result = decl;
          goto done;
        }

     if (ATmatch(term, "MemberFunction(<str>, <term>, [<list>], <term>)",&str, &temp1, &temp2, &temp3))
        {
       // This case is mostly a copy fo the Function case (above).
          string funcname = str;
          SgType* return_type = isSgType(convertAtermToNode(temp1));
          ROSE_ASSERT (return_type);
          vector<ATerm> params = getAtermList(temp2);
          SgFunctionParameterList* pl = new SgFunctionParameterList(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          for (size_t i = 0; i < params.size(); ++i)
             {
               SgNode* argi = convertAtermToNode(params[i]);
               ROSE_ASSERT (isSgInitializedName(argi));
               pl->append_arg(isSgInitializedName(argi));
             }
          SgFunctionDefinition* def = isSgFunctionDefinition(convertAtermToNode(temp3));
          SgFunctionType* ft = new SgFunctionType(return_type, false);
          for (SgInitializedNamePtrList::iterator i = pl->get_args().begin(); i != pl->get_args().end(); ++i)
               ft->get_arguments().push_back((*i)->get_type());
          SgFunctionDeclaration* decl = new SgFunctionDeclaration(fi, funcname.c_str(), ft, 0);
          for (SgInitializedNamePtrList::iterator i = pl->get_args().begin(); i != pl->get_args().end(); ++i)
             {
               (*i)->set_definition(decl);
               decl->get_args().push_back(*i);
             }
          if (def)
               decl->set_definition(def);
          result = decl;
          goto done;
        }

     if (ATmatch(term, "FunctionDefinition(<term>)", &temp1))
        {
          SgFunctionDefinition* def = new SgFunctionDefinition(fi);
          def->set_body(isSgBasicBlock(convertAtermToNode(temp1)));
          result = def;
          goto done;
        }

     if (ATmatch(term, "ParameterList([<list>])", &temp1)) 
        {
          SgFunctionParameterList* parameter_list = new SgFunctionParameterList(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               parameter_list->get_args().push_back(isSgInitializedName(convertAtermToNode(terms[i])));
            // FIXME: add to symbol tables
             }
          result = parameter_list;
          goto done;
        }

     if (ATmatch(term, "InitName(<str>, <term>, <term>)",&str, &temp1, &temp2))
        {
          string varname = str;
          SgType* type = isSgType(convertAtermToNode(temp1));
          SgInitializer* init = isSgInitializer(convertAtermToNode(temp2));
          SgName* name = new SgName(varname.c_str());
          ROSE_ASSERT (name->str());
          result = new SgInitializedName(*name, type, init);
          goto done;
        }

     if (ATmatch(term, "Ellipses")) 
        {
          result = new SgInitializedName("", new SgTypeEllipse());
          goto done;
        }

     if (ATmatch(term, "IntC(<int>)", &tempint)) 
        {
          result = new SgIntVal(fi, tempint);
          goto done;
        }

     if (ATmatch(term, "DoubleC(<real>)", &tempdouble)) 
        {
          result = new SgDoubleVal(fi, tempdouble);
          goto done;
        }

     if (ATmatch(term, "UnsignedLongC(<str>)", &str)) 
        {
          istringstream s(str);
          unsigned long value;
          s >> value;
          ROSE_ASSERT (s.eof() && !s.fail());
          result = new SgUnsignedLongVal(fi, value);
          goto done;
        }

     if (ATmatch(term, "ExprList([<list>])", &temp1))
        {
          SgExprListExp* exprlist = new SgExprListExp(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               exprlist->append_expression(isSgExpression(convertAtermToNode(terms[i])));
             }
          result = exprlist;
          goto done;
        }

  // if (ATmatch(term, "ClassDefinition([<list>])", &temp1))
     if (ATmatch(term, "ClassDefinition(<str>, [<list>])", &str, &temp1))
        {
          SgClassDefinition* class_definition = new SgClassDefinition(fi);

          string scope_mangled_name_string = str;
          printf ("ClassDefinition: scope_mangled_name_string = %s \n",scope_mangled_name_string.c_str());

       // targetLocations[scope_mangled_name_string] = class_definition;
       // translationScopeMap[scope_mangled_name_string] = class_definition;
          updateScopeMap(scope_mangled_name_string,class_definition);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               SgDeclarationStatement* child = isSgDeclarationStatement(convertAtermToNode(terms[i]));
               ROSE_ASSERT (child);
               class_definition->append_member(child);
             }
          result = class_definition;
          goto done;
        }

     if (ATmatch(term, "Enum(<str>, [<list>])", &str, &temp1))
        {
          SgEnumDeclaration* enumdef = new SgEnumDeclaration(fi, str);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               SgInitializedName* child = isSgInitializedName(convertAtermToNode(terms[i]));
               ROSE_ASSERT (child);
               enumdef->append_enumerator(child);
             }
          result = enumdef;
          goto done;
        }

     if (ATmatch(term, "VarDecl([<list>])", &temp1)) 
        {
#if 1
          printf ("Matching VarDecl([<list>]) \n");
#endif
          SgVariableDeclaration* vardecl = new SgVariableDeclaration(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               SgInitializedName* child = isSgInitializedName(convertAtermToNode(terms[i]));
               ROSE_ASSERT (child);
               SgInitializer* init = child->get_initializer();
               vardecl->append_variable(child, init);
             }
          result = vardecl;
          goto done;
        }

     if (ATmatch(term, "Func(<str>)", &str)) 
        {
       // SgFunctionDeclaration* decl = isSgFunctionDeclaration(targetLocations[str]);
          SgDeclarationStatement* declaration_node = lookupDeclaration(str);
          SgFunctionDeclaration* decl = isSgFunctionDeclaration(declaration_node);
          ROSE_ASSERT (decl != NULL);
          SgFunctionSymbol* sym = new SgFunctionSymbol(decl);
          result = new SgFunctionRefExp(fi, sym);
          goto done;
        }

     if (ATmatch(term, "Var(<str>)", &str))
        {
       // SgInitializedName* decl = isSgInitializedName(targetLocations[str]);
          SgInitializedName* decl = lookupInitializedName(str);
          ROSE_ASSERT (decl != NULL);
          SgVariableSymbol* sym = new SgVariableSymbol(decl);
          result = new SgVarRefExp(fi, sym);
          goto done;
        }

     if (ATmatch(term, "Type(id(<str>))", &str))
        {
          cout << "Type ref to " << str << endl;

       // SgNode* body = targetLocations[str];
          SgDeclarationStatement* body = lookupDeclaration(str);

       // DQ (9/29/2014): Disabling this assertion.
       // DQ (9/18/2014): Need to allow this to be NULL for non-defining declarations associated with types.
       // I think this is when the reference to the type appears before the declaration in a class.
#if 0
          ROSE_ASSERT (body);
#endif
          if (body != NULL)
             {
          SgClassDeclaration* classdecl = isSgClassDeclaration(body);
          SgTypedefDeclaration* typedefdecl = isSgTypedefDeclaration(body);
          SgEnumDeclaration* enumdecl = isSgEnumDeclaration(body);
          if (classdecl)
               result = new SgClassType(classdecl);
            else if (typedefdecl)
               result = new SgTypedefType(typedefdecl);
            else if (enumdecl)
               result = new SgEnumType(enumdecl);
            else
               ROSE_ASSERT (!"Invalid type reference for id");
             }
            else
             {
               printf ("In ATmatch: Type(id(<str>)): targetLocations[str] == NULL: str = %s \n",str);

            // We need to know what kind of named type to build, so we can't mix then at the level where we build the aterms.

            // SgClassDeclaration (SgName name, SgClassDeclaration::class_types class_type, SgClassType *type, SgClassDefinition *definition)
               SgName name = "noname";
               SgClassDeclaration::class_types class_type = SgClassDeclaration::e_class;
               SgClassType *type = NULL;
               SgClassDefinition *definition = NULL;

               SgClassDeclaration* classdecl = new SgClassDeclaration(name,class_type,type,definition);

            // targetLocations[str] = classdecl;
               updateDeclarationMap(str,classdecl);

               result = new SgClassType(classdecl);
             }

          goto done;
        }

#if 0
  // I think this should not be used.
     if (ATmatch(term, "Type(<term>)", &temp1))
        {
       // For things like "typedef struct foo {int x;} bar;"
          SgClassType* nt = new SgClassType(isSgClassDeclaration(convertAtermToNode(temp1)));

       // FIXME add typedef and enum cases
          ROSE_ASSERT(nt != NULL);

          result = nt;
          goto done;
        }
#endif

     if (ATmatch(term, "TypedefType(id(<str>))", &str))
        {
          cout << "TypedefType(id(<str>)): Type ref to " << str << endl;
       // SgNode* typedefDeclaration_node = targetLocations[str];
          SgDeclarationStatement* typedefDeclaration_node = lookupDeclaration(str);

       // ROSE_ASSERT(typedefDeclaration_node != NULL);
          if (typedefDeclaration_node == NULL)
             {
               printf ("This might be a typedef generated to support a builtin type! \n");

            // Generate a typedef for the type and then the associated type.

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          printf ("typedefDeclaration_node = %p = %s \n",typedefDeclaration_node,typedefDeclaration_node->class_name().c_str());

          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefDeclaration_node);
          ROSE_ASSERT(typedefDeclaration != NULL);

          result = new SgTypedefType(typedefDeclaration);

#if 0
       // DQ (9/29/2014): Disabling this assertion.
       // DQ (9/18/2014): Need to allow this to be NULL for non-defining declarations associated with types.
       // I think this is when the reference to the type appears before the declaration in a class.
#if 0
          ROSE_ASSERT (body);
#endif
          if (body != NULL)
             {
          SgClassDeclaration* classdecl = isSgClassDeclaration(body);
          SgTypedefDeclaration* typedefdecl = isSgTypedefDeclaration(body);
          SgEnumDeclaration* enumdecl = isSgEnumDeclaration(body);
          if (classdecl)
               result = new SgClassType(classdecl);
            else if (typedefdecl)
               result = new SgTypedefType(typedefdecl);
            else if (enumdecl)
               result = new SgEnumType(enumdecl);
            else
               ROSE_ASSERT (!"Invalid type reference for id");
             }
            else
             {
               printf ("In ATmatch: Type(id(<str>)): targetLocations[str] == NULL: str = %s \n",str);

            // We need to know what kind of named type to build, so we can't mix then at the level where we build the aterms.

            // SgClassDeclaration (SgName name, SgClassDeclaration::class_types class_type, SgClassType *type, SgClassDefinition *definition)
               SgName name = "noname";
               SgClassDeclaration::class_types class_type = SgClassDeclaration::e_class;
               SgClassType *type = NULL;
               SgClassDefinition *definition = NULL;

               SgClassDeclaration* classdecl = new SgClassDeclaration(name,class_type,type,definition);

            // targetLocations[str] = classdecl;
               updateDeclarationMap(str,classdecl);

            // This is OK for now, but soon we want to be calling higher level functions (SageBuilder functions).
               result = new SgClassType(classdecl);
             }
#endif
          goto done;
        }

     if (ATmatch(term, "TypedefType(<term>)", &temp1))
        {
          SgNode* node = convertAtermToNode(temp1);
          ROSE_ASSERT(node != NULL);
          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
          ROSE_ASSERT(declaration != NULL);

          SgTypedefType* nt = new SgTypedefType(declaration);
          ROSE_ASSERT(nt != NULL);

          result = nt;
          goto done;
        }
     

     if (ATmatch(term, "ClassType(id(<str>))", &str))
  // if (ATmatch(term, "ClassType(<term>)", &temp1))
        {
          cout << "ClassType ref to " << str << endl;
       // SgNode* body = targetLocations[str];
          SgDeclarationStatement* body = lookupDeclaration(str);

       // DQ (9/29/2014): Disabling this assertion.
       // DQ (9/18/2014): Need to allow this to be NULL for non-defining declarations associated with types.
       // I think this is when the reference to the type appears before the declaration in a class.
#if 1
          ROSE_ASSERT (body);
#endif
          if (body != NULL)
             {
               SgClassDeclaration* classdecl = isSgClassDeclaration(body);
               if (classdecl)
                    result = new SgClassType(classdecl);
                 else
                    ROSE_ASSERT (!"Invalid type reference for id");
             }
            else
             {
               printf ("In ATmatch: ClassType(id(<str>)): targetLocations[str] == NULL: str = %s \n",str);
             }

          goto done;
        }

     if (ATmatch(term, "Class(<str>, <term>)", &str, &temp1))
        {
       // Note that the str value being used here is different from that used as a key in
       // the access of the declaration map in ATmatch(term, "ClassType(id(<str>))", &str).
       // I think the string used here is incorrect, not because it is the wrong class name, 
       // but because we need an additional string for the uniquely generatee name.

          SgClassDeclaration* decl = new SgClassDeclaration(fi, str);

          printf ("Processing ATerm for Class(<str>, <term>): str = %s \n",str);

       // Get the annotation we will use as a key in the updateDeclarationMap() function.
       // updateDeclarationMap(str,decl);
          ATerm idannot = ATgetAnnotation(term, ATmake("id"));
          if (idannot)
             {
#if 1
               printf ("Found an annotation \n");
#endif
               char* unique_id = NULL;
            // if (ATmatch("id(<str>)", &unique_id))
               if (ATmatch(idannot, "<str>", &unique_id))
                  {
                    printf ("unique_id = %s \n",unique_id);

                    updateDeclarationMap(unique_id,decl);

#if 1
                 // Not clear if I need to build the type directly here, I now think we need this step as well.
                 // ROSE_ASSERT(decl->get_type() != NULL);

                    SgClassType* classType = new SgClassType(decl);
                 // updateTypeMap(str,classType);
                    updateTypeMap(unique_id,classType);
#endif
                    decl->set_type(classType);
                    ROSE_ASSERT(decl->get_type() != NULL);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
               printf ("Error: annotation required to define key for updateDeclarationMap() \n");
               ROSE_ASSERT(false);
             }

          decl->set_definition(isSgClassDefinition(convertAtermToNode(temp1)));
          result = decl;
          goto done;
        }

  // if (ATmatch(term, "ClassFwd(<str>)", &str))
     if (ATmatch(term, "ClassFwd(<str>, <term>)", &str, &temp1))
        {
       // Get the saved scope and use the name to lookup the previously saved scope.
          char* scope_mangled_name = NULL;
          if (ATmatch(temp1, "ScopeRef(<str>)", &scope_mangled_name))
             {
               printf ("Match make for ScopeRef(<str>) \n");
             }
            else
             {
               printf ("Match NOT make for ScopeRef(<str>) \n");
             }

          printf ("In processing aterm ClassFwd: scope_mangled_name = %s \n",scope_mangled_name);

          string scope_mangled_name_string = scope_mangled_name;
       // SgNode* scope_node = targetLocations[scope_mangled_name_string];
          SgScopeStatement* scope_node = lookupScope(scope_mangled_name_string);
          ROSE_ASSERT(scope_node != NULL);

          printf ("In processing aterm ClassFwd: Building SgClassDeclaration for scope_node = %p = %s class name = %s \n",scope_node,scope_node->class_name().c_str(),str);

          SgScopeStatement* rose_scope = isSgScopeStatement(scope_node);
          ROSE_ASSERT(rose_scope != NULL);

          SgClassDeclaration::class_types class_kind = SgClassDeclaration::e_class;

       // SgClassDeclaration* decl = new SgClassDeclaration(fi, str);
          SgClassDeclaration* decl = new SgClassDeclaration(fi, str,class_kind,NULL,NULL);
          decl->set_scope(rose_scope);

          ROSE_ASSERT(decl->get_scope() != NULL);

          SgClassType* classType = new SgClassType(decl);
          decl->set_type(classType);

          ROSE_ASSERT(decl->get_type() != NULL);

          result = decl;
          goto done;
        }

     if (ATmatch(term, "Typedef(<str>, <term>)", &str, &temp1))
        {
       // SgType* base = isSgType(convertAtermToNode(temp1));
          SgNode* node = convertAtermToNode(temp1);
          ROSE_ASSERT(node != NULL);

          SgType* base = isSgType(node);
          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);

          ROSE_ASSERT(base != NULL || declaration != NULL);

          if (base != NULL)
             {
               printf ("Typedef(<str>, <term>): associated aterm is a SgType = %p = %s \n",node,node->class_name().c_str());
             }
            else
             {
               if (declaration != NULL)
                  {
                    printf ("Typedef(<str>, <term>): associated aterm is a SgDeclarationStatement = %p = %s \n",node,node->class_name().c_str());

                 // We can't call the non-virtual function in SgDeclarationStatement (perhaps 
                 // this should be fixed in ROSE, at the risk of it being harder to debug).
                 // base = declaration->get_type();

                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                    SgEnumDeclaration*  enumDeclaration  = isSgEnumDeclaration(declaration);
                    if (classDeclaration != NULL)
                       {
                         base = classDeclaration->get_type();
                         ROSE_ASSERT(base != NULL);
                       }
                      else
                       {
                         if (enumDeclaration != NULL)
                            {
                              base = enumDeclaration->get_type();
                              ROSE_ASSERT(base != NULL);
                            }
                           else
                            {
                              printf ("Neither a class nor enum declaration! \n");
                              ROSE_ASSERT(false);
                            }
                       }
                    ROSE_ASSERT(base != NULL);
                  }
                 else
                  {
                    printf ("Error: base type in typedef represented by node = %p = %s \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT((declaration != NULL && base != NULL) || base != NULL);

       // if (!base->get_typedefs())
          if (base != NULL && base->get_typedefs() == NULL)
             {
               base->set_typedefs(new SgTypedefSeq());
             }

          printf ("Building SgTypedefDeclaration from Aterm \n");

       // SgTypedefDeclaration* decl = new SgTypedefDeclaration(fi, str, base);
          SgTypedefDeclaration* decl = new SgTypedefDeclaration(fi, str, base, NULL, declaration, NULL);

          SgTypedefType* tt = new SgTypedefType(decl);
          if (base != NULL)
             {
               base->get_typedefs()->get_typedefs().push_back(tt);
             }

          decl->set_type(tt);

          result = decl;
          goto done;
        }

     if (ATmatch(term, "Const(<term>)", &temp1)) 
        {
       // SgModifierType* mt = new SgModifierType(isSgType(convertAtermToNode(temp1)));
          SgNode* node = convertAtermToNode(temp1);
          ROSE_ASSERT(node != NULL);
          printf ("convert ATerm to SgModifierType: node = %p = %s \n",node,node->class_name().c_str());
          SgType* type = isSgType(node);
          ROSE_ASSERT(type != NULL);
          SgModifierType* mt = new SgModifierType(type);

          mt->get_typeModifier().get_constVolatileModifier().setConst();
          result = mt;
          goto done;
        }

     if (ATmatch(term, "Volatile(<term>)", &temp1)) 
        {
          SgModifierType* mt = new SgModifierType(isSgType(convertAtermToNode(temp1)));
          mt->get_typeModifier().get_constVolatileModifier().setVolatile();
          result = mt;
          goto done;
        }

     if (ATmatch(term, "lazyWrap(<term>)", &temp1)) 
        {
       // This is a debugging step to handle some optionally generated untranslated subtrees, when lazyWrapping == true. 
#if 1
          printf ("Warning: detected generated generated lazyWrap(<term>): generating a SgNullStatement \n");
#endif
          SgNullStatement* ns = new SgNullStatement();
          result = ns;
          goto done;
        }

  // if (ATmatch(term, "VarDecl([<list>])", &temp1)) 
  // if (ATmatch(term, "VarDecl(NULL,InitName(<term>,<term>Int{[ptr,"0x0265ec60"],[id,"i__type"]},NULL){[ptr,"0x7f6b39d626d0"],[id,"xi_in_scope___global____initialized_name"]}){[ptr,"0x7f6b39b2b010"],[id,"__global___variable_declaration__variable_type_i_variable_name_x_variable_name_x__public_access_/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/projects/AtermTranslation/test2013_01.C_1__variable"]})", &temp1))
  // if (ATmatch(term, "VarDecl(<term>,<term>,<term>,<term>)", &temp1))
  // if (ATmatch(term, "VarDecl(<term>,<term>,<term>)", &temp1))
  // if (ATmatch(term, "VarDecl(<term>,<term>)", &temp1))
  // if (ATmatch(term, "VarDecl(<term>)", &temp1))
  // if (ATmatch(term, "VarDecl(<term>,<term>,<term>,<term>)", &temp1))
  // if (ATmatch(term, "VarDecl(<term>,[<list>])", &temp1)) 
     if (ATmatch(term, "VarDecl(<term>)", &temp1))
        {
#if 1
          printf ("Matching VarDecl(<term>) \n");
#endif
          SgNode* var = convertAtermToNode(temp1);
          SgInitializedName* initializedName = isSgInitializedName(var);
          ROSE_ASSERT (initializedName);
       // pl->append_arg(isSgInitializedName(argi));
       // result = SageBuilder::buildVariableDeclaration(initializedName);
          SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration();

#if 1
          printf ("Found correct form of VarDecl \n");
          ROSE_ASSERT(false);
#endif
        }

     if (ATmatch(term, "Label(<str>)", &str))
        {
          SgName name = str;
          SgLabelStatement* label = new SgLabelStatement(name,NULL);
          result = label;
          goto done;
        }

  // if (ATmatch(term, "For(<term>, <term>, <term>, <term>)", &temp1, &temp2, &temp3, &temp4))
  // if (ATmatch(term, "Global([<list>])", &temp1))
  // if (ATmatch(term, "For(<term>)", &temp1))
     if (ATmatch(term, "For(<term>, <term>, <term>, <term>, <term>)", &temp1, &temp2, &temp3, &temp4, &temp5))
        {
#if 1
          printf ("Identified SgForStatement: For(<term>, <term>, <term>, <term>, <term>) \n");
#endif
          SgForInitStatement* forInit = isSgForInitStatement(convertAtermToNode(temp1));
          ROSE_ASSERT(forInit != NULL);
          SgStatement* test = isSgStatement(convertAtermToNode(temp2));
          ROSE_ASSERT(test != NULL);
          SgExpression* increment = isSgExpression(convertAtermToNode(temp3));
          ROSE_ASSERT(increment != NULL);
          SgStatement* body = isSgStatement(convertAtermToNode(temp4));
          ROSE_ASSERT(body != NULL);

          result = new SgForStatement(forInit, test, increment, body);

          goto done;
        }

     if (ATmatch(term, "ForInit([<list>])", &temp1))
        {
#if 1
          printf ("Identified SgForInitStatement: ForInit([<list>]) \n");
#endif
          SgForInitStatement* for_init = new SgForInitStatement(fi);
          vector<ATerm> terms = getAtermList(temp1);
          for (size_t i = 0; i < terms.size(); ++i)
             {
               SgStatement* child = isSgStatement(convertAtermToNode(terms[i]));
               ROSE_ASSERT (child);
               for_init->append_init_stmt(child);
             }

          result = for_init;
          goto done;
        }

  // cerr << "Unrecognized term " << ATwriteToString(term) << endl;
     unrecognizedAterm = ATwriteToString(term);
     printf ("\n\nERROR: Unknown Aterm \n");
     printf ("ERROR: unrecognizedAterm = %s \n",unrecognizedAterm.c_str());
     printf ("DIAGNOSTIC: aterm_type_name = %s \n",aterm_type_name(term).c_str());
     ROSE_ASSERT (false);

done:
     ATerm idannot = ATgetAnnotation(term, ATmake("id"));
     if (idannot)
        {
#if 1
          printf ("Found an annotation \n");
#endif
          char* id;
          if (ATmatch(idannot, "<str>", &id))
             {
#if 1
                ROSE_ASSERT(result != NULL);
                printf ("Setting targetLocations map: id = %s result = %p = %s \n",id,result,result->class_name().c_str());
#endif
            // Organize as seperate translation maps to simplify the debugging (also allows the same key to be used for types and declarations, etc.).
            // targetLocations[id] = result;
               SgScopeStatement*       scope           = isSgScopeStatement(result);
               SgDeclarationStatement* declaration     = isSgDeclarationStatement(result);
               SgType*                 type            = isSgType(result);
               SgInitializedName*      initializedName = isSgInitializedName(result);
               if (scope != NULL)
                  {
                    updateScopeMap(id,scope);
                  }
                 else
                  {
                    if (declaration != NULL)
                       {
                         updateDeclarationMap(id,declaration);
                       }
                      else
                       {
                         if (type != NULL)
                            {
                              updateTypeMap(id,type);
                            }
                           else
                            {
                              if (initializedName != NULL)
                                 {
                                   updateInitializedNameMap(id,initializedName);
                                 }
                                else
                                 {
                                   printf ("Not updated in translation map: result = %p = %s \n",result,result->class_name().c_str());
                                 }
                            }
                       }
                  }
             }
            else
             {
               ROSE_ASSERT (!"id annotation has incorrect format");
             }
        }
       else
        {
#if 1
          printf ("No annotation found! \n");
#endif
        }

  // ROSE_ASSERT(result != NULL);
     if (result != NULL)
        {
          printf ("Leaving AtermToNodeConverter::convertAtermToNode(): result = %p = %s \n",result,result->class_name().c_str());
        }
       else
        {
          printf ("Leaving AtermToNodeConverter::convertAtermToNode(): result == NULL (This can sometimes be OK) \n");
        }

#ifdef DEBUG
     if (result)
        {
          FixSgTree(result);

          cout << "Resulting in " << (result ? result->sage_class_name() : "NULL") << ": " << /* myUnparse(result) << */ endl;
        }
#endif

     return result;
   }


SgNode* convertAtermToNode(ATerm term) 
   {
#if 0
     SgNode* root = NULL;

     printf ("In convertAtermToNode(): Where is this being called (same name and nearly the same signature as the function above) \n");
     ROSE_ASSERT(false);
#else
     AtermToNodeConverter converter;
     SgNode* root = converter.convertAtermToNode(term);

#if 0
  // I think we can skip this final step.
     for (map<string, list<LinkUpdater*> >::iterator i = converter.idUpdates.begin(); i != converter.idUpdates.end(); ++i) 
        {
          for (list<LinkUpdater*>::iterator j = i->second.begin(); j != i->second.end(); ++j) 
             {
               (*j)->update(converter.targetLocations[i->first]);
             }
        }
#endif
#endif
     return root;
   }


class FixFunctionDefinitionDeclPointersVisitor: public AstSimpleProcessing 
   {
     public:
          virtual void visit(SgNode* n) 
             {
               if (isSgFunctionDefinition(n) && isSgFunctionDeclaration(n->get_parent()))
                    isSgFunctionDefinition(n)->set_declaration(isSgFunctionDeclaration(n->get_parent()));
             }
   };


class FixScopePointers: public AstSimpleProcessing 
   {
     public:
          virtual void visit(SgNode* n)
             {
            // SgScopeStatement* scope = getScope(n);
               SgScopeStatement* scope = SageInterface::getScope(n);
               ROSE_ASSERT(scope != NULL);
               if (isSgInitializedName(n)) {
                 isSgInitializedName(n)->set_scope(scope);
               } else if (isSgQualifiedName(n)) {
                 isSgQualifiedName(n)->set_scope(scope);
               } else if (isSgEnumDeclaration(n)) {
                 isSgEnumDeclaration(n)->set_scope(scope);
               } else if (isSgTypedefDeclaration(n)) {
                 isSgTypedefDeclaration(n)->set_scope(scope);
               } else if (isSgTemplateDeclaration(n)) {
                 isSgTemplateDeclaration(n)->set_scope(scope);
               } else if (isSgClassDeclaration(n)) {
                 isSgClassDeclaration(n)->set_scope(scope);
               } else if (isSgFunctionDeclaration(n)) {
                 isSgFunctionDeclaration(n)->set_scope(scope);
               }
             }
   };


void cleanAstMadeFromAterm(SgNode* node) 
   {
     FixSgTree(node);
     FixFunctionDefinitionDeclPointersVisitor().traverse(node, preorder);
     FixScopePointers().traverse(node, preorder);
  // initializeExplicitScopes(node);

  // DQ (3/23/2013): These should not be required now, I think.
  // fixReturnStatements(node);
  // rebindVariableAndLabelReferences(node); // Should not be necessary now
   }
