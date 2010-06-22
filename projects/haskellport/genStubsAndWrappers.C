#include "rose.h"
#include <sageDoxygen.h>
#include <iostream>

using namespace std;

/*
   This program generates "stubs" and "wrappers" using the supplied
   header file. Here a wrapper refers to a C wrapper around a C++
   function, and a stub refers to a Haskell interface to the C wrapper,
   Both stubs and wrappers are necessary because Haskell's foreign
   function interface does not support C++, only C.

   We do this by traversing the ROSE AST and performing automatic
   code generation.  We are supported by the ROSE.CxxContainers
   module which wraps the C++ container classes.
 */

//! This class defines all aspects of how to present the given API to the user.
class APIFilter
   {

     public:
          //! Defines which classes to export.
          virtual bool exportClass(SgClassDeclaration *klass) = 0;
          //! Defines which methods to export.
          virtual bool exportMethod(SgMemberFunctionDeclaration *fn) = 0;
          //! Defines which enums to export.
          virtual bool exportEnum(SgEnumDeclaration *enumDecl) = 0;
          //! Derives a Haskell name for the given method.
          virtual string hsMethodName(SgMemberFunctionDeclaration *fn, bool isOverloaded) = 0;

   };

APIFilter *globFilter;

//! True if the given class has a variant associated with it.
bool
hasVariantForClass(SgGlobal *glob, SgClassDeclaration *decl)
   {
     SgEnumFieldSymbol *efield = glob->get_symbol_table()->find_enum_field("V_" + decl->get_name());
     return (efield != 0);
   }

bool
isOverriddenVirtualFunctionHere(const SgName &n, SgFunctionType *ft, SgClassDefinition *clsDef)
   {
     if (SgFunctionSymbol *fs = clsDef->lookup_function_symbol(n, ft))
        {
          if (fs->get_declaration()->get_functionModifier().isVirtual()) return true;
        }
     SgBaseClassPtrList &bases = clsDef->get_inheritances();
     for (SgBaseClassPtrList::iterator i = bases.begin(); i != bases.end(); ++i)
        {
          SgClassDefinition *baseDef = isSgClassDeclaration((*i)->get_base_class()->get_definingDeclaration())->get_definition();
          ROSE_ASSERT(baseDef != NULL);
          if (isOverriddenVirtualFunctionHere(n, ft, baseDef)) return true;
        }
     return false;
   }

//! True if this function ifs simply overriding a virtual function defined in a superclass.  Used to avoid exporting virtual function overrides unnecessarily.
bool
isOverridingVirtualFunction(SgMemberFunctionDeclaration *fn)
   {
     if (!fn->get_functionModifier().isVirtual()) return false;
     SgClassDefinition *clsDef = isSgClassDefinition(fn->get_scope());
     ROSE_ASSERT(clsDef != NULL);
     SgBaseClassPtrList &bases = clsDef->get_inheritances();
     for (SgBaseClassPtrList::iterator i = bases.begin(); i != bases.end(); ++i)
        {
          SgClassDefinition *baseDef = isSgClassDeclaration((*i)->get_base_class()->get_definingDeclaration())->get_definition();
          ROSE_ASSERT(baseDef != NULL);
          if (isOverriddenVirtualFunctionHere(fn->get_name(), fn->get_type(), baseDef)) return true;
        }
     return false;
   }

string
haddockEscape(string desc)
   {
  // Escape the special characters used by Haddock
  // http://www.haskell.org/haddock/doc/html/ch03s08.html#id289762
     desc = StringUtility::copyEdit(desc, "\\", "\\\\");
     desc = StringUtility::copyEdit(desc, "/", "\\/");
     desc = StringUtility::copyEdit(desc, "'", "\\'");
     desc = StringUtility::copyEdit(desc, "`", "\\`");
     desc = StringUtility::copyEdit(desc, "\"", "\\\"");
     desc = StringUtility::copyEdit(desc, "@", "\\@");
     desc = StringUtility::copyEdit(desc, "<", "\\<");
     desc = StringUtility::copyEdit(desc, ">", "\\>");
     desc = StringUtility::copyEdit(desc, "*", "\\*");
     desc = StringUtility::copyEdit(desc, "-", "\\-");
     return desc;
   }

static string haskellFFItype(SgDeclarationStatement *ds, SgType *t, char &quantifier);

class Sage3Filter : public APIFilter
   {

     private:
          set<string> usedNames;

          string mkFreshName(string origName)
             {
               if (usedNames.find(origName) != usedNames.end())
                  {
                    for (int i = 2;; i++)
                       {
                         stringstream newNameSS;
                         newNameSS << origName << i;
                         string newName = newNameSS.str();
                         if (usedNames.find(newName) == usedNames.end())
                            {
                              usedNames.insert(newName);
                              return newName;
                            }
                       }
                  }
               else
                  {
                    usedNames.insert(origName);
                    return origName;
                  }
             }


     public:
          bool exportClass(SgClassDeclaration *klass)
             {
               SgGlobal *glob = isSgGlobal(klass->get_scope());
               if (!glob) return false;
               string nameStr = klass->get_name().getString();
               return nameStr.substr(0, 2) == "Sg" && nameStr != "SgName" && hasVariantForClass(glob, klass);
             }

          bool exportMethod(SgMemberFunctionDeclaration *fn)
             {
               SgFunctionType *fnType = isSgFunctionType(fn->get_type());
               char quantifier = 'a';
               if (fn->get_name() == "get_dlls") return false; // skip a weird overloaded function: SgAsmGenericHeader::get_dlls
            // SgUnparse_Info::set_array_index_list is the only function that
            // has a direct Sg... (other than SgName) parameter (as opposed to
            // a pointer or reference).  It isn't implemented anyway so we
            // don't lose anything
               if (fn->get_name() == "set_array_index_list") return false;
               if (isOverridingVirtualFunction(fn)) return false;
               return fnType && !haskellFFItype(fn, fnType, quantifier).empty();
             }

          bool exportEnum(SgEnumDeclaration *enumDecl)
             {
               return (enumDecl->get_name() == "VariantT");
             }


          string hsMethodName(SgMemberFunctionDeclaration *fn, bool isOverloaded)
             {
               SgClassDefinition *clsDef = isSgClassDefinition(fn->get_scope());
               ROSE_ASSERT(clsDef != NULL);
               string clsName = clsDef->get_declaration()->get_name().getString();
               if (fn->get_specialFunctionModifier().isConstructor())
                  {
                    return mkFreshName("new" + clsName.substr(2));
                  }
               else
                  {
                    string hsMethodName = clsName.substr(2);
                    hsMethodName[0] = tolower(hsMethodName[0]);

                    string methodName = fn->get_name();
                    bool capNext = true;
                    for (string::const_iterator i = methodName.begin(); i != methodName.end(); ++i)
                       {
                         if (*i == '_')
                            {
                              capNext = true;
                            }
                         else if (capNext)
                            {
                              hsMethodName += toupper(*i);
                              capNext = false;
                            }
                         else
                            {
                              hsMethodName += *i;
                            }
                       }

                    if (isOverloaded)
                       {
                         SgTypePtrList &args = fn->get_type()->get_arguments();
                         for (SgTypePtrList::const_iterator i = args.begin(); i != args.end(); ++i)
                            {
                              SgType *t = (*i)->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
                              if (SgNamedType *nt = isSgNamedType(t))
                                 {
                                   string typeName;
                                   if (SgTemplateInstantiationDecl *tid = isSgTemplateInstantiationDecl(nt->get_declaration()))
                                      {
                                        typeName = tid->get_templateName().getString();
                                      }
                                   else
                                      {
                                        typeName = nt->get_name().getString();
                                      }
                                   if (typeName.substr(0, 2) == "Sg")
                                      {
                                        typeName = typeName.substr(2);
                                      }
                                   hsMethodName += typeName;
                                 }
                              else
                                 {
                                // cout << "Couldn't resolve a " << t->sage_class_name() << " parameter!";
                                 }
                            }
                       }

                    return mkFreshName(hsMethodName);
                  }
             }

   };

//! This function defines the unique name of the C function in the wrapper which is called by the stub.
string
wrapperFnName(SgMemberFunctionDeclaration *fn)
   {
     return "_haskell_stub_" + fn->get_mangled_name();
   }

//! Convenience function that outputs a list of Haskell exports.  The export list is accumulated during the program.
void
dumpExportList(ostream &stubs, const list<string> &exports)
   {
     if (exports.size() > 0)
        {
          list<string>::const_iterator i = exports.begin();
          stubs << "(\n  " << *i; ++i;
          for(; i != exports.end(); ++i)
             {
               stubs << ",\n  " << *i;
             }
          stubs << "\n)";
        }
   }

//! Generates the header for the main stub file (ROSE.Sage3.Classes)
void
genStubHeader(ostream &stubs, list<string> &exports)
   {
     stubs << "{-# LANGUAGE DeriveDataTypeable #-}\n\n"
              "module ROSE.Sage3.Classes";
     dumpExportList(stubs, exports);
     stubs <<                          " where\n\n"
              "import Foreign.Ptr\n"
              "import Int\n"
              "import Word\n"
              "import CTypes\n"
              "import CString\n"
              "import MarshalAlloc\n"
              "import MarshalArray\n"
              "import Data.Typeable\n"
              "import Data.Map (Map, fromList, (!))\n"
              "import Data.Maybe\n"
              "import Data.IORef\n"
              "import System.IO.Unsafe\n\n"

              "import ROSE.CxxContainers\n\n"

              "data Rose_hash_multimap a = Rose_hash_multimap deriving (Typeable)\n"
              "data AstAttributeMechanism a = AstAttributeMechanism deriving (Typeable)\n"
              "data AttachedPreprocessingInfoType a = AttachedPreprocessingInfoType deriving (Typeable)\n\n"

              "foreign import ccall \"stdlib.h free\" cFree :: Ptr a -> IO ()\n"
              "foreign import ccall \"haskellWrappers.h _haskell_variant\" cVariant :: Ptr (SgNode a) -> IO CInt\n\n";
   }

void
genStubFooter(ostream &stubs) {}

// Generates the header (to be clear- not the header file) for the C wrapper file
void
genWrapperHeader(ostream &wrappers)
   {
     wrappers << "#inc" "lude <rose.h>\n\n"
                 "using namespace std;\n\n"
                 "extern \"C\" {\n\n";
   }

void
genWrapperFooter(ostream &wrappers)
   {
     wrappers << "}\n";
   }

//! Generates the stub code for a given class, not including its methods
void
genClassStub(ostream &stubs, SgGlobal *glob, SgClassDeclaration *klass, list<string> &exports)
   {
     SgClassDefinition *def = klass->get_definition();
     ROSE_ASSERT(def != NULL);
     string cname = klass->get_name().getString();
     list<DoxygenComment *> *commentList = Doxygen::getCommentList(klass);
     if (!commentList->empty())
        {
          stubs << "{-| ";
          DoxygenEntry &entry = commentList->front()->entry;
          if (entry.hasBrief()) stubs << haddockEscape(entry.brief());
          if (entry.hasBrief() && entry.hasDescription()) stubs << "\n\n";
          if (entry.hasDescription()) stubs << haddockEscape(entry.description());
          stubs << " -}\n";
        }
     if (def->get_inheritances().empty())
        {

          stubs << "data Typeable a => " << cname << "T a = " << cname << " (Ptr (" << cname << " a)) deriving (Typeable, Eq)\n"
                   "type " << cname << "N a = a\n"
                   "type " << cname << " a = SgNodeT (" << cname << "N a)\n\n";
          exports.push_back(cname + "T(..)");
          exports.push_back(cname);

          stubs << "{-| Represents a null '" << cname << "'. -}\n";
          stubs << "null" << cname << " :: (Typeable a) => " << cname << " a\n"
                   "null" << cname << " = " << cname << " nullPtr\n\n";
          exports.push_back("null" + cname);
        }
     else
        {
          ROSE_ASSERT(def->get_inheritances().size() == 1);
          SgClassDeclaration *base = def->get_inheritances().front()->get_base_class();
          string bname = base->get_name().getString();
          stubs << "data Typeable a => " << cname << "T a = " << cname << " deriving (Typeable)\n"
                   "type " << cname << "N a = " << bname << "N (" << cname << "T a)\n"
                   "type " << cname << " a = SgNode (" << cname << "N a)\n\n";
          exports.push_back(cname + "T(..)");
          exports.push_back(cname);
        }
   }

//! Generates the header for a methods stub file (one per class, e.g. ROSE.Sage3.Methods.SgNode)
void
genMethodsStubHeader(ostream &stubs, SgClassDeclaration *klass, list<string> &exports, list<string> &methodModuleExports)
   {
     string cname = klass->get_name().getString();
     string mname = "ROSE.Sage3.Methods." + cname;
     methodModuleExports.push_back(mname);
     stubs << "{-# LANGUAGE DeriveDataTypeable #-}\n\n"
              "module " << mname << " ";
     dumpExportList(stubs, exports);
     stubs <<                         " where\n\n"
              "import Foreign.Ptr\n"
              "import Int\n"
              "import Word\n"
              "import CTypes\n"
              "import CString\n"
              "import MarshalAlloc\n"
              "import MarshalArray\n"
              "import Data.Typeable\n\n"

              "import ROSE.CxxContainers\n"
              "import ROSE.Sage3.Classes\n"
              "import " << (cname == "SgNode" ? "{-# SOURCE #-} " : "") << "ROSE.Sage3.Misc\n\n"

              "foreign import ccall \"stdlib.h free\" cFree :: Ptr a -> IO ()\n\n";
   }

void
genMethodsStubFooter(ostream &stubs) {}

void
genClassStubMethods(ostream &stubs, SgClassDeclaration *klass, list<string> &exports)
   {
     string cname = klass->get_name().getString();

     stubs << "{-| Attempts a dynamic cast of the given node to the type '" << cname << "'. -}\n";
     stubs << "is" << cname << " :: Typeable a => SgNode a -> IO (Maybe (" << cname << " ()))\n"
              "is" << cname << " n@(SgNode p) = withVariant n (\\vnt ->\n"
              "\tif V_" << cname << " <= vnt then Just (SgNode (castPtr p) :: " << cname << " ())\n"
              "\t                            else Nothing)\n\n";
     exports.push_back("is" + cname);

     stubs << "{-| Performs an upcast of a '" << cname << "' subtype to a '" << cname << "'. -}\n";
     stubs << "up" << cname << " :: Typeable a => " << cname << " a -> " << cname << " ()\n"
              "up" << cname << " (SgNode p) = SgNode (castPtr p)\n\n";
     exports.push_back("up" + cname);
   }

//! Generates the per-class wrapper code- not currently used.
void
genClassWrapper(ostream &wrappers, SgClassDeclaration *klass) {}

//! True if a string type (char*, STL string, SgName)
bool
isStringType(SgType *t)
   {
     t = t->stripType(SgType::STRIP_MODIFIER_TYPE);
     if (SgTypedefType *tt = isSgTypedefType(t))
        {
          if (isSgTypedefDeclaration(tt->get_declaration())->get_qualified_name() == "::std::string") return true;
        }
     t = t->stripTypedefsAndModifiers();
     if (SgClassType *ct = isSgClassType(t))
        {
          SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
          ROSE_ASSERT(cdec != NULL);

          return (cdec->get_name() == "SgName" || cdec->get_qualified_name() == "::std::string");
        }
     if (SgPointerType *pt = isSgPointerType(t))
        {
          SgType *baseType = pt->get_base_type();
          baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE);
          return (isSgTypeChar(baseType) || isSgTypeUnsignedChar(baseType));
        }
     if (SgReferenceType *rt = isSgReferenceType(t))
        {
          return isStringType(rt->get_base_type());
        }
     return false;
   }

//! Mapping of C type names to those in Haskell
map<string, string> cHsTypeMap;

void
initCHsTypeMap(void)
   {
     cHsTypeMap["ptrdiff_t"] = "CPtrdiff";
     cHsTypeMap["size_t"] = "CSize";
     cHsTypeMap["wchar_t"] = "CWchar";
     cHsTypeMap["sig_atomic_t"] = "CSigAtomic";
     cHsTypeMap["clock_t"] = "CClock";
     cHsTypeMap["time_t"] = "CTime";
     cHsTypeMap["FILE"] = "CFile";
     cHsTypeMap["fpos_t"] = "CFpos";
     cHsTypeMap["jmp_buf"] = "CJmpBuf";
     cHsTypeMap["uint8_t"] = "Word8";
     cHsTypeMap["uint16_t"] = "Word16";
     cHsTypeMap["uint32_t"] = "Word32";
     cHsTypeMap["uint64_t"] = "Word64";
     cHsTypeMap["int8_t"] = "Int8";
     cHsTypeMap["int16_t"] = "Int16";
     cHsTypeMap["int32_t"] = "Int32";
     cHsTypeMap["int64_t"] = "Int64";
     cHsTypeMap["::std::string"] = "CString";
   }

//! If a (recognised) STL container, returns the element type for this container.  Otherwise, returns null.
static SgType *
containerElemType(SgType *t)
   {
     t = t->stripTypedefsAndModifiers();
     if (SgClassType *ct = isSgClassType(t))
        {
          SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
          ROSE_ASSERT(cdec != NULL);

          if (SgTemplateInstantiationDecl *tidec = isSgTemplateInstantiationDecl(cdec))
             {
               string tname = tidec->get_templateDeclaration()->get_qualified_name().getString();
               if (tname == "::std::list" || tname == "::std::vector")
                  {
                    SgTemplateArgumentPtrList &args = tidec->get_templateArguments();
                    ROSE_ASSERT(args.size() >= 1);
                    SgType *elemType = args.front()->get_type();
                    ROSE_ASSERT(elemType != NULL);
                    return elemType;
                  }
             }
        }
     return NULL;
   }

//! The (Haskell) name of the function that converts an enum data type to the corresponding int
string
enumToIntFn(SgEnumDeclaration *ed)
   {
     string enumname = ed->get_name().getString(); enumname[0] = tolower(enumname[0]);
     return enumname + "ToInt";
   }

//! The (Haskell) name of the function that converts an int to an enum data type
string
enumFromIntFn(SgEnumDeclaration *ed)
   {
     string enumname = ed->get_name().getString(); enumname[0] = tolower(enumname[0]);
     return enumname + "FromInt";
   }

//! Return type of this function from the POV of the stubs and wrappers.  The special case is if this is a constructor, which is represented as returning void, in which case we return a pointer to the class.
SgType *
FFIfunctionReturnType(SgFunctionDeclaration *fd)
   {
     SgFunctionType *ft = fd->get_type();
     if (fd->get_specialFunctionModifier().isConstructor())
        {
          SgMemberFunctionType *mft = isSgMemberFunctionType(ft);
          ROSE_ASSERT(mft);
          return SgPointerType::createType(mft->get_class_type());
        }
     return ft->get_return_type();
   }

/* Converts the given type to the Haskell (FFI) representation.
   If this function returns "", this means we were unable to convert
   the type and we should skip the function. */
static string
haskellFFItype(SgDeclarationStatement *ds, SgType *t, char &quantifier)
   {
     t = t->stripType(SgType::STRIP_MODIFIER_TYPE);

     if (SgNamedType *nt = isSgNamedType(t))
        {
          map<string, string>::const_iterator i;
          if ((i = cHsTypeMap.find(nt->get_name().getString())) != cHsTypeMap.end())
             {
               return i->second;
             }
          SgTypedefDeclaration *td = isSgTypedefDeclaration(nt->get_declaration());
          if (td && (i = cHsTypeMap.find(td->get_qualified_name().getString())) != cHsTypeMap.end())
             {
               return i->second;
             }

          t = nt->stripTypedefsAndModifiers();
        }

     if (SgFunctionType *ft = isSgFunctionType(t))
        {
          string ts;
          if (SgMemberFunctionType *mft = isSgMemberFunctionType(ft))
             {
               SgType *ct = mft->get_class_type();
               SgMemberFunctionDeclaration *mfd = isSgMemberFunctionDeclaration(ds);
               if (mfd && !mfd->get_declarationModifier().get_storageModifier().isStatic() && !mfd->get_specialFunctionModifier().isConstructor())
                  {
                    string ctStr = haskellFFItype(isSgClassType(ct->stripTypedefsAndModifiers())->get_declaration(), ct, quantifier);
                    if (ctStr.empty()) return "";
                    ts = "Ptr (" + ctStr + ") -> ";
                  }
             }
          SgTypePtrList &args = ft->get_argument_list()->get_arguments();
          for (SgTypePtrList::iterator i = args.begin(); i != args.end(); ++i)
             {
               string arg = haskellFFItype(0, *i, quantifier);
               if (arg.empty()) return "";
               ts += arg + " -> ";
             }
          char nullQuant = 0;
          string rt = haskellFFItype(0, FFIfunctionReturnType(isSgFunctionDeclaration(ds)), nullQuant);
          if (rt.empty()) return "";
          return ts + "IO (" + rt + ")";
        }

     if (SgPointerType *pt = isSgPointerType(t))
        {
          SgType *baseType = pt->get_base_type();
          baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE);
          if (isSgTypeChar(baseType) || isSgTypeUnsignedChar(baseType)) return "CString";
          string baseFFItype = haskellFFItype(0, baseType, quantifier);
          if (baseFFItype.empty()) return "";
          if (baseFFItype.size() > 8 && baseFFItype.substr(0, 8) == "Ptr (Cxx") return baseFFItype;
          return "Ptr (" + baseFFItype + ")";
        }

     if (SgReferenceType *rt = isSgReferenceType(t))
        {
          SgType *baseType = rt->get_base_type();
          baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE);
          string baseFFItype = haskellFFItype(0, baseType, quantifier);
          if (baseFFItype.empty()) return "";
          if (baseFFItype.size() > 8 && baseFFItype.substr(0, 8) == "Ptr (Cxx") return baseFFItype;
          if (baseFFItype == "CString") return baseFFItype;
          return "Ptr (" + baseFFItype + ")";
        }

     if (SgClassType *ct = isSgClassType(t))
        {
          SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
          ROSE_ASSERT(cdec != NULL);

          if (SgTemplateInstantiationDecl *tidec = isSgTemplateInstantiationDecl(cdec))
             {
               string tname = tidec->get_templateDeclaration()->get_qualified_name().getString();
               bool isList = false, isVector = false;
               if (tname == "::std::list" && (isList = true)
                               || tname == "::std::vector" && (isVector = true))
                  {
                    SgTemplateArgumentPtrList &args = tidec->get_templateArguments();
                    ROSE_ASSERT(args.size() >= 1);
                    SgType *elemType = args.front()->get_type();
                    ROSE_ASSERT(elemType != NULL);

                    string hsCtrName;
                    if (isList) hsCtrName = "CxxList";
                    if (isVector) hsCtrName = "CxxVector";

                    char ctrQuant = 0;
                    string elemFFItype = haskellFFItype(0, elemType, ctrQuant);
                    if (elemFFItype == "")
                       {
                         return "";
                       }
                    return "Ptr (" + hsCtrName + " (" + elemFFItype + "))";
                  }
             }
          if (cdec->get_name() == "SgName" || cdec->get_qualified_name() == "::std::string")
             {
               return "CString";
             }
          else if (globFilter->exportClass(cdec))
             {
               string qspec;
               if (quantifier)
                  {
                    qspec = quantifier++;
                  }
               else
                  {
                    qspec = "()";
                  }
               return cdec->get_name().getString() + " " + qspec;
             }
        }

     if (SgEnumType *et = isSgEnumType(t))
        {
          SgEnumDeclaration *ed = isSgEnumDeclaration(et->get_declaration());
          ROSE_ASSERT(ed != NULL);

          return globFilter->exportEnum(ed) ? "CInt" : "";
        }

     switch (t->variantT())
        {
          case V_SgTypeChar: return "CChar";
          case V_SgTypeSignedChar: return "CSChar";
          case V_SgTypeUnsignedChar: return "CUChar";
          case V_SgTypeShort: return "CShort";
          case V_SgTypeUnsignedShort: return "CUShort";
          case V_SgTypeInt: return "CInt";
          case V_SgTypeUnsignedInt: return "CUInt";
          case V_SgTypeLong: return "CLong";
          case V_SgTypeUnsignedLong: return "CULong";
          case V_SgTypeLongLong: return "CLLong";
          case V_SgTypeUnsignedLongLong: return "CULLong";
          case V_SgTypeFloat: return "CFloat";
          case V_SgTypeDouble: return "CDouble";
          case V_SgTypeLongDouble: return "CLDouble";
          case V_SgTypeVoid: return "()";
          case V_SgTypeBool: return "CInt";
          default: break;
        }

     return "";
   }

//! True if this is a pointer to a class we are exporting (determined using exportClass)
SgClassDeclaration *
isExportPtrType(SgType *t)
   {
     t = t->stripType(SgType::STRIP_MODIFIER_TYPE);

     if (SgPointerType *pt = isSgPointerType(t))
        {
          SgType *baseType = pt->get_base_type();
          baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE);
          if (SgClassType *ct = isSgClassType(baseType))
             {
               SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
               if (globFilter->exportClass(cdec)) return cdec;
             }
        }
     return 0;
   }

//! Returns the pattern that is used to extract a value from the given parameter.  Usually a pattern is not required but if this is an exported pointer we need to extract the actual pointer
string
hsMatchValue(SgType *t, const string &varName)
   {
     if (isExportPtrType(t))
        {
       // here we make assumptions about the root of the class hierarchy - FIXME
          return "(SgNode " + varName + ")";
        }
     return varName;
   }

//! Unmarshalls the value from a Haskell point of view, i.e. converts into a C compatible value which can be passed to the wrapper
string
hsUnmarshallValue(SgType *t, string &toVar, const string &fromVar)
   {
     if (isStringType(t))
        {
          return "\t" + toVar + " <- newCString " + fromVar + "\n";
        }
     SgType *ts = t->stripTypedefsAndModifiers();
     if (SgEnumType *enumt = isSgEnumType(ts))
        {
          SgEnumDeclaration *ed = isSgEnumDeclaration(enumt->get_declaration());
          ROSE_ASSERT(ed != NULL);
          toVar = "(" + enumToIntFn(ed) + " " + fromVar + ")";
          return "";
        }
     if (isSgTypeBool(ts))
        {
          toVar = "(if " + fromVar + " then 1 else 0)";
          return "";
        }
     SgType *elemType;
     if ((elemType = containerElemType(t))
      || isSgPointerType(t) && (elemType = containerElemType(isSgPointerType(t)->get_base_type()))
      || isSgReferenceType(t) && (elemType = containerElemType(isSgReferenceType(t)->get_base_type()))
     )
        {
          string cctr;
          if (isExportPtrType(elemType))
             {
               cctr = "(map (\\(SgNode p) -> p) " + fromVar + ")";
             }
          else
             {
               cctr = fromVar;
             }
          return "\t" + toVar + " <- newContainer " + cctr + "\n";
        }
     toVar = fromVar;
     return "";
   }

//! If we needed to allocate some memory while unmarshalling, free it here (code appears after the wrapper call)
string
hsFreeUnmarshalledValue(SgType *t, const string &var)
   {
     if (isStringType(t))
        {
          return "\tfree " + var + "\n";
        }
     if (containerElemType(t) || isSgReferenceType(t) && containerElemType(isSgReferenceType(t)->get_base_type()))
        {
          return "\tdelete " + var + "\n";
        }
     return "";
   }

//! Marshall the value returned by the C wrapper into a Haskell data type
string
hsMarshallValue(SgType *t, string &toVar, const string &fromVar)
   {
     if (isStringType(t))
        {
          stringstream ss;
          ss << "\t" << toVar << " <- peekCString " << fromVar << "\n";
          if (isSgClassType(t)) // An SgName or std::string
             {
               ss << "\tcFree " << fromVar << "\n";
             }
          return ss.str();
        }
     if (isExportPtrType(t))
        {
          toVar = "(SgNode " + fromVar + ")";
          return "";
        }
     SgType *elemType;
     bool isRef = false;
     if ((elemType = containerElemType(t))
      || isSgPointerType(t) && (elemType = containerElemType(isSgPointerType(t)->get_base_type())) && (isRef = true)
      || isSgReferenceType(t) && (elemType = containerElemType(isSgReferenceType(t)->get_base_type())) && (isRef = true)
     )
        {
          stringstream ss;
          ss << "\t" << toVar << " <- getElems " << fromVar << "\n";
          if (!isRef)
             {
               ss << "\tdelete " << fromVar << "\n";
             }
          if (isExportPtrType(elemType))
             {
               toVar = "(map SgNode " + toVar + ")";
             }
          return ss.str();
        }
     SgType *ts = t->stripTypedefsAndModifiers();
     if (SgEnumType *enumt = isSgEnumType(ts))
        {
          SgEnumDeclaration *ed = isSgEnumDeclaration(enumt->get_declaration());
          ROSE_ASSERT(ed != NULL);
          toVar = "(" + enumFromIntFn(ed) + " " + fromVar + ")";
          return "";
        }
     if (isSgTypeBool(ts))
        {
          toVar = "(" + fromVar + " /= 0)";
          return "";
        }
     toVar = fromVar;
     return "";
   }

//! Returns the type of the given value before unmarshalling/after marshalling.  Not strictly necessary but used by the Haddock. 
string
hsMarshalledType(SgType *t, char &quantifier)
   {
     if (isStringType(t))
        {
          return "String";
        }
     if (SgClassDeclaration *cd = isExportPtrType(t))
        {
          string qspec;
          if (quantifier)
             {
               qspec = quantifier++;
             }
          else
             {
               qspec = "()";
             }
          return cd->get_name().getString() + " " + qspec;
        }
     SgType *elemType;
     bool isRef = false;
     if ((elemType = containerElemType(t))
      || isSgPointerType(t) && (elemType = containerElemType(isSgPointerType(t)->get_base_type())) && (isRef = true)
      || isSgReferenceType(t) && (elemType = containerElemType(isSgReferenceType(t)->get_base_type())) && (isRef = true)
     )
        {
          if (SgClassDeclaration *cd = isExportPtrType(elemType))
             {
               return "[" + cd->get_name().getString() + " ()]";
             }
          return "[" + haskellFFItype(0, elemType, quantifier) + "]";
        }
     SgType *ts = t->stripTypedefsAndModifiers();
     if (SgEnumType *enumt = isSgEnumType(ts))
        {
          SgEnumDeclaration *ed = isSgEnumDeclaration(enumt->get_declaration());
          ROSE_ASSERT(ed != NULL);
          return ed->get_name().getString();
        }
     if (isSgTypeBool(ts))
        {
          return "Bool";
        }
     return haskellFFItype(0, t, quantifier);
   }

//! Outputs all code necessary to support the given argument to the given output streams.
void
hsAddArg(SgType *t, const string &varS, stringstream &matchSS, stringstream &unmarshallSS, stringstream &callargsSS, stringstream &freeUnmarshalledSS, stringstream &typeSS, char &quantifier)
   {
     string varName = "v" + varS;
     matchSS << hsMatchValue(t, varName) << " ";
     string umVarName = "uv" + varS;
     unmarshallSS << hsUnmarshallValue(t, umVarName, varName);
     callargsSS << umVarName << " ";
     freeUnmarshalledSS << hsFreeUnmarshalledValue(t, umVarName);
     typeSS << hsMarshalledType(t, quantifier) << " -> ";
   }

//! Returns the type of "this" in the context of a member function.  Special handling is required for functions marked const or volatile.
SgType *
thisType(SgMemberFunctionDeclaration *fn)
   {
     SgClassDefinition *clsDef = isSgClassDefinition(fn->get_scope());
     ROSE_ASSERT(clsDef != NULL);

     SgType *clsType = clsDef->get_declaration()->get_type();
     SgMemberFunctionType *mfType = isSgMemberFunctionType(fn->get_type());
     if (mfType && (mfType->isConstFunc() || mfType->isVolatileFunc()))
        {
          SgModifierType *modType = new SgModifierType(clsType);
          if (mfType->isConstFunc())    modType->get_typeModifier().get_constVolatileModifier().setConst();
          if (mfType->isVolatileFunc()) modType->get_typeModifier().get_constVolatileModifier().setVolatile();
          clsType = modType;
        }

     return SgPointerType::createType(clsType);
   }

//! Generates stub code for the given method.
void
genMethodStub(ostream &stubs, SgMemberFunctionDeclaration *fn, bool isOverloaded, list<string> &exports)
   {
     SgUnparse_Info upi;
     upi.set_SkipDefinition();
     upi.set_SkipEnumDefinition();

     char quantifier = 'a';
     string wrapperType = haskellFFItype(fn, fn->get_type(), quantifier);

     string wrapperFn = wrapperFnName(fn);
     string stubFn = globFilter->hsMethodName(fn, isOverloaded);
     stubs << "foreign import ccall \"haskellWrappers.h " << wrapperFn << "\" c" << wrapperFn << " :: " << wrapperType << endl;

     // fn->unparseToString() does not work here and returns an empty string (due to declarations being in header files?)
     stubs << "{-| Stub for @" << haddockEscape(fn->get_qualified_name().getString()) << "@ of type @" << haddockEscape(fn->get_type()->unparseToString(&upi)) << "@";
     list<DoxygenComment *> *commentList = Doxygen::getCommentList(fn);
     if (!commentList->empty())
        {
          DoxygenEntry &entry = commentList->front()->entry;
          if (entry.hasBrief() || entry.hasDescription()) stubs << "\n\n";
          if (entry.hasBrief()) stubs << haddockEscape(entry.brief());
          if (entry.hasBrief() && entry.hasDescription()) stubs << "\n\n";
          if (entry.hasDescription()) stubs << haddockEscape(entry.description());
        }
     stubs << " -}" << endl;

     const SgTypePtrList &argTypes = fn->get_type()->get_arguments();

     vector<string> marshalledVarNames, unmarshalledVarNames;
     stringstream matchSS, unmarshallSS, callargsSS, freeUnmarshalledSS, typeSS;

     SgClassDefinition *clsDef = isSgClassDefinition(fn->get_scope());
     ROSE_ASSERT(clsDef != NULL);

     quantifier = 'a';

     bool isStatic = fn->get_specialFunctionModifier().isConstructor() || fn->get_declarationModifier().get_storageModifier().isStatic();
     if (!isStatic) hsAddArg(thisType(fn), "0", matchSS, unmarshallSS, callargsSS, freeUnmarshalledSS, typeSS, quantifier);

     int var = 1;
     for (SgTypePtrList::const_iterator i = argTypes.begin(); i != argTypes.end(); ++i, ++var)
        {
          SgType *t = (*i)->stripType(SgType::STRIP_MODIFIER_TYPE);
          stringstream varSS;
          varSS << var;
          string varS = varSS.str();

          hsAddArg(t, varS, matchSS, unmarshallSS, callargsSS, freeUnmarshalledSS, typeSS, quantifier);
        }

     SgType *retType = FFIfunctionReturnType(fn)->stripType(SgType::STRIP_MODIFIER_TYPE);

     string typeClass;
     if (quantifier > 'a')
        {
          typeClass = "(";
          for (char q = 'a'; q < quantifier; q++)
             {
               if (q != 'a') typeClass += ", ";
               typeClass += "Typeable " + string(1, q);
             }
          typeClass += ") => ";
        }
     quantifier = 0;
     stubs << stubFn << " :: " << typeClass << typeSS.str() << "IO (" << hsMarshalledType(retType, quantifier) << ")" << endl;
     stubs << stubFn << " " << matchSS.str() << "= do\n" << unmarshallSS.str();
     stubs << "\trv <- c" << wrapperFn << " " << callargsSS.str() << "\n";
     stubs << freeUnmarshalledSS.str();
     string marshalledRetName = "mrv";
     stubs << hsMarshallValue(retType, marshalledRetName, "rv");
     stubs << "\treturn " << marshalledRetName << "\n\n";
     exports.push_back(stubFn);
   }

//! Converts the given type to the appropriate unmarshalled type.  For example STL strings are converted to char*
SgType *
cUnmarshalledType(SgType *t)
   {
     if (!isSgPointerType(t) && isStringType(t))
        {
          return SgPointerType::createType(SgTypeChar::createType());
        }
     SgType *ts = t->stripTypedefsAndModifiers();
     if (isSgClassType(ts))
        {
          return SgPointerType::createType(t);
        }
     if (SgReferenceType *rt = isSgReferenceType(t))
        {
          return SgPointerType::createType(rt->get_base_type());
        }
     if (isSgEnumType(ts) || isSgTypeBool(ts))
        {
          return SgTypeInt::createType();
        }
     return t;
   }

//! marshals the given C value to C++ - converts references to pointers, ints to bools etc.
string
cMarshallValue(SgType *t, string &toVar, const string &fromVar)
   {
     if (isSgReferenceType(t) && !isStringType(t) || containerElemType(t))
        {
          toVar = "*" + fromVar;
          return "";
        }
     if (isSgEnumType(t))
        {
          toVar = "((" + t->unparseToString() + ")" + fromVar + ")";
          return "";
        }
     if (isSgTypeBool(t))
        {
          toVar = "(" + fromVar + " != 0)";
          return "";
        }
     toVar = fromVar;
     return "";
   }

//! Unmarshals the given C++ value to C - converts STL strings to char* etc.
string
cUnmarshallValue(SgType *t, string &toVar, const string &fromVar)
   {
     if (SgTypedefType *tt = isSgTypedefType(t->stripType(SgType::STRIP_MODIFIER_TYPE)))
        {
          if (isSgTypedefDeclaration(tt->get_declaration())->get_qualified_name() == "::std::string")
             {
               return "\tchar *" + toVar + " = strdup(" + fromVar + ".c_str());\n";
             }
        }
     SgType *ts = t->stripTypedefsAndModifiers();
     if (SgClassType *ct = isSgClassType(ts))
        {
          SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
          if (cdec->get_name() == "SgName")
             {
               return "\tchar *" + toVar + " = strdup(" + fromVar + ".getString().c_str());\n";
             }
          else if (cdec->get_qualified_name() == "::std::string")
             {
               return "\tchar *" + toVar + " = strdup(" + fromVar + ".c_str());\n";
             }
          else
             {
               string tstr = t->unparseToString();
               return "\t" + tstr + " *" + toVar + " = new " + tstr + "(" + fromVar + ");\n";
             }
        }
     if (isSgTypeBool(ts))
        {
          toVar = "(" + fromVar + " ? 1 : 0)";
          return "";
        }
     if (SgReferenceType *rt = isSgReferenceType(t))
        {
          SgType *bt = rt->get_base_type();
          if (SgTypedefType *tt = isSgTypedefType(bt->stripType(SgType::STRIP_MODIFIER_TYPE)))
             {
               if (isSgTypedefDeclaration(tt->get_declaration())->get_qualified_name() == "::std::string")
                  {
                    return "\tchar *" + toVar + " = strdup(" + fromVar + ".c_str());\n";
                  }
             }
          if (SgClassType *ct = isSgClassType(bt->stripTypedefsAndModifiers()))
             {
               SgClassDeclaration *cdec = isSgClassDeclaration(ct->get_declaration());
               if (cdec->get_name() == "SgName")
                  {
                    return "\tchar *" + toVar + " = strdup(" + fromVar + ".getString().c_str());\n";
                  }
               else if (cdec->get_qualified_name() == "::std::string")
                  {
                    return "\tchar *" + toVar + " = strdup(" + fromVar + ".c_str());\n";
                  }
             }
          toVar = "&" + fromVar;
          return "";
        }
     toVar = fromVar;
     return "";
   }

//! Generates wrapper code for the given method.
void
genMethodWrapper(ostream &wrappers, SgMemberFunctionDeclaration *fn)
   {
     SgUnparse_Info upi;
     upi.set_SkipDefinition();
     upi.set_SkipEnumDefinition();

     string wrapperFn = wrapperFnName(fn);

     const SgTypePtrList &argTypes = fn->get_type()->get_arguments();

     stringstream argsSS, marshallSS, callargsSS;

     bool isStatic = fn->get_specialFunctionModifier().isConstructor() || fn->get_declarationModifier().get_storageModifier().isStatic();

     if (!isStatic)
        {
          SgType *tThis = thisType(fn);
          argsSS << cUnmarshalledType(tThis)->unparseToString(&upi) << " v0";
        }

     int var = 1;
     for (SgTypePtrList::const_iterator i = argTypes.begin(); i != argTypes.end(); ++i, ++var)
        {
          SgType *t = (*i)->stripType(SgType::STRIP_MODIFIER_TYPE);

          stringstream varSS;
          varSS << var;
          string varS = varSS.str();

          string varName = "v" + varS;
          if (!isStatic || i != argTypes.begin()) argsSS << ", ";
          argsSS << cUnmarshalledType(*i)->unparseToString(&upi) << " " << varName;
          string mVarName = "mv" + varS;
          marshallSS << cMarshallValue(t, mVarName, varName);
          if (i != argTypes.begin()) callargsSS << ", ";
          callargsSS << mVarName;
        }

     SgType *retType = FFIfunctionReturnType(fn);

     wrappers << "/* Wrapper for " << fn->get_qualified_name().getString() << " " << fn->get_type()->unparseToString(&upi) << " */" << endl;
     wrappers << cUnmarshalledType(retType)->unparseToString(&upi) << " " << wrapperFn << "(" << argsSS.str() << ") {\n";

     wrappers << marshallSS.str();
     wrappers << "\t";
     if (!isSgTypeVoid(retType))
        {
          wrappers << retType->unparseToString(&upi) << " rv = ";
        }
     if (fn->get_specialFunctionModifier().isConstructor())
        {
          wrappers << "new " << fn->get_name().getString();
        }
     else if (fn->get_declarationModifier().get_storageModifier().isStatic())
        {
          wrappers << fn->get_class_scope()->get_declaration()->get_name().getString()
                   << "::" << fn->get_name().getString();
        }
     else
        {
          wrappers << "v0->" << fn->get_name().getString();
        }
     wrappers << "(" << callargsSS.str() << ");\n";
     if (!isSgTypeVoid(retType))
        {
          string unmarshalledRetName = "urv";
          wrappers << cUnmarshallValue(retType, unmarshalledRetName, "rv");
          wrappers << "\treturn " << unmarshalledRetName << ";\n";
        }
     wrappers << "}\n\n";
   }

//! Generates stub code for the given enum - a data declaration and conversions to/from CInt.
void
genEnumStub(ostream &stubs, SgEnumDeclaration *ed, list<string> &exports)
   {
     stringstream enumData, enumToInt, enumFromInt;
     string enumName = ed->get_name().getString();
     string enumname = enumName; enumname[0] = tolower(enumname[0]);
     enumName[0] = toupper(enumName[0]);
     enumData << "data " << enumName << " = ";
     string enumToIntFnName = enumToIntFn(ed);
     string enumFromIntFnName = enumFromIntFn(ed);
     enumToInt << "{-| Internal use only.  Converts a '" << enumName << "' to its enum representation. -}\n";
     enumToInt << enumToIntFnName << " :: " << enumName << " -> CInt\n";
     enumFromInt << "{-| Internal use only.  Converts the enum representation of a '" << enumName << "' to a '" << enumName << "'. -}\n";
     enumFromInt << enumFromIntFnName << " :: CInt -> " << enumName << "\n";
     SgInitializedNamePtrList &enums = ed->get_enumerators();
     int enumVal = -1;
     for (SgInitializedNamePtrList::iterator i = enums.begin(); i != enums.end(); ++i)
        {
          SgInitializedName *fldName = *i;
          string sFldName = fldName->get_name().getString();
          sFldName[0] = toupper(sFldName[0]);
          SgAssignInitializer *fldInit = isSgAssignInitializer(fldName->get_initializer());
          if (fldInit != NULL)
             {
               SgIntVal *fldVal = isSgIntVal(fldInit->get_operand_i());
               ROSE_ASSERT(fldVal != NULL);
               enumVal = fldVal->get_value();
             }
          else
             {
               enumVal++;
             }
          if (i != enums.begin())
             {
               enumData << " | ";
             }
          enumData << sFldName;
          enumToInt << enumToIntFnName << " " << sFldName << " = " << enumVal << "\n";
          enumFromInt << enumFromIntFnName << " " << enumVal << " = " << sFldName << "\n";
        }
     enumData << " deriving (Show, Read, Eq)\n";
     stubs << enumData.str() << "\n" << enumToInt.str() << "\n" << enumFromInt.str() << "\n";
     exports.push_back(enumName + "(..)");
     exports.push_back(enumToIntFnName);
     exports.push_back(enumFromIntFnName);
   }

typedef vector<SgClassDeclaration *> SgClassDeclarationPtrList;

//! Gets a list of base classes for this class not including the class itself, in descending order of specificity. Won't work correctly for multiple base classes.
SgClassDeclarationPtrList
getInheritanceChain(SgClassDeclaration *cls)
   {
     SgClassDeclarationPtrList chain;
     while (1)
        {
          SgClassDeclaration *dd = isSgClassDeclaration(cls->get_definingDeclaration());
          ROSE_ASSERT(dd != NULL);
          SgClassDefinition *def = dd->get_definition();
          ROSE_ASSERT(def != NULL);
          string cname = cls->get_name().getString();
          if (def->get_inheritances().size() == 1)
             {
               cls = def->get_inheritances().front()->get_base_class();
               chain.push_back(cls);
             }
          else
             {
               break;
             }
        }
     return chain;
   }

/*! Generates an Ord instance for VariantT using the inheritance chain.  Ord instances should be total orders, however VariantT has
    a partially ordered comparison function.  Users should be careful
    to only use the Ord instance for simple comparisons, or else things
    may break! */
void
genClassVariantOrdInstHeader(ostream &inst)
   {
     inst << "-- NOTE: Ord instances should be total orders, however VariantT has\n"
             "-- a partially ordered comparison function.  Users should be careful\n"
             "-- to only use the Ord instance for simple comparisons, or else things\n"
             "-- may break!\n\n";
     inst << "instance Ord VariantT where\n";
     inst << "\tv <= v' | v == v' = True\n";
     inst << "\tV_SgNode <= _ = True\n";
   }

void
genClassVariantOrdInstPart(ostream &inst, SgClassDeclaration *klass)
   {
     string clsName = klass->get_name().getString();
     SgClassDeclarationPtrList clsInhChain = getInheritanceChain(klass);
     for (SgClassDeclarationPtrList::const_iterator i = clsInhChain.begin(); i != clsInhChain.end(); ++i)
        {
          string baseName = (*i)->get_name().getString();
          if (baseName != "SgNode") inst << "\tV_" << baseName << " <= V_" << clsName << " = True\n";
        }
   }

void
genClassVariantOrdInstFooter(ostream &inst)
   {
     inst << "\t_ <= _ = False\n";
   }

/*! Generates implementations for SomeSgNode and sageStaticCast */
void
genStaticCastImplHeader(ostream &dc, list<string> &exports)
   {
     dc << "{-| Existential type containing a particular 'SgNode' -}\n";
     dc << "data SomeSgNode = forall n. Typeable n => SomeSgNode (SgNode n)\n\n";
     exports.push_back("SomeSgNode(..)");

     dc << "{-| Performs a static cast of the given 'SgNode' to a given type.  This\n"
           "    is an unsafe operation which should only be performed if the given\n"
           "    'VariantT' is known to represent a supertype of the given pointer -}\n";
     dc << "sageStaticCast :: Ptr a -> VariantT -> SomeSgNode\n"
           "sageStaticCast p vnt = case vnt of\n";
     exports.push_back("sageStaticCast");
   }

void
genStaticCastImplPart(ostream &dc, SgClassDeclaration *klass)
   {
     string clsName = klass->get_name().getString();
     dc << "\tV_" << clsName << " -> SomeSgNode (SgNode (castPtr p) :: " << clsName << " ())\n";
   }

void
genStaticCastImplFooter(ostream &dc)
   {
     dc << "\tV_SgName -> SomeSgNode (SgNode (castPtr p) :: SgNode ())\n";
     dc << "\t_ -> error (\"Unhandled case in sageStaticCast: \" ++ show vnt)\n\n";
   }

//! Generates implementation for staticVariant
/* This implementation of staticVariant uses a map of type representation keys.  This appears
   to be the fastest approach discovered so far. */
void
genStaticVariantImplHeader(ostream &sv, list<string> &exports)
   {
     sv << "{-# NOINLINE _staticVariantMapRef #-}\n"
           "_staticVariantMapRef :: IORef (Maybe (Map Int VariantT))\n"
           "_staticVariantMapRef = unsafePerformIO (newIORef Nothing)\n\n"

           "{-| Determines the variant of the given argument using its static type. -}\n"
           "staticVariant :: Typeable n => SgNode n -> VariantT\n"
           "staticVariant n = unsafePerformIO (staticVariant' n)\n"
           " where\n"
           "\tstaticVariant' :: Typeable n => SgNode n -> IO VariantT\n"
           "\tstaticVariant' n = do\n"
           "\t\tsvkey <- typeRepKey (typeOf n)\n"
           "\t\tsvmap <- staticVariantMap\n"
           "\t\treturn (svmap ! svkey)\n\n"

           "\tstaticVariantMap :: IO (Map Int VariantT)\n"
           "\tstaticVariantMap = do\n"
           "\t\tmSvmap <- readIORef _staticVariantMapRef\n"
           "\t\tif isJust mSvmap then return (fromJust mSvmap) else do\n"
           "\t\t\tsvmap <- mkStaticVariantMap\n"
           "\t\t\twriteIORef _staticVariantMapRef (Just svmap)\n"
           "\t\t\treturn svmap\n\n"

           "\tmkStaticVariantMap :: IO (Map Int VariantT)\n"
           "\tmkStaticVariantMap = do\n"
           "\t\tlet mapEval = tail [undefined";
     exports.push_back("staticVariant");
   }

void
genStaticVariantImplPart(ostream &sv, SgClassDeclaration *klass)
   {
     string clsName = klass->get_name().getString();
     sv <<   ",\n"
           "\t\t\t(typeOf (undefined :: " << clsName << " ()), V_" << clsName << ")";
   }

void
genStaticVariantImplFooter(ostream &sv)
   {
     sv << "]\n"
           "\t\tmapElems <- mapM (\\(tr, vnt) -> typeRepKey tr >>= \\trk -> return (trk, vnt)) mapEval\n"
           "\t\treturn (fromList mapElems)\n\n";
   }

//! Determines equivalence of two types, modulo some set of stripping parameters which are the same as used by SgType::stripType
bool
equivalentModuloStrip(SgType *t1, SgType *t2, unsigned char stripWhat)
   {
     t1 = t1->stripType(stripWhat);
     t2 = t2->stripType(stripWhat);
     SgFunctionType *ft1, *ft2;
     if ((ft1 = isSgFunctionType(t1)) && (ft2 = isSgFunctionType(t2)))
        {
          if (!equivalentModuloStrip(ft1->get_return_type(), ft2->get_return_type(), stripWhat))
             {
               return false;
             }
          SgTypePtrList &argTypes1 = ft1->get_arguments();
          SgTypePtrList &argTypes2 = ft2->get_arguments();
          if (argTypes1.size() != argTypes2.size()) return false;
          for (SgTypePtrList::const_iterator i1 = argTypes1.begin(), i2 = argTypes2.begin();
                          i1 != argTypes1.end(); ++i1, ++i2)
             {
               if (!equivalentModuloStrip(*i1, *i2, stripWhat))
                  {
                    return false;
                  }
             }
          return true;
        }
     SgPointerType *pt1, *pt2;
     if ((pt1 = isSgPointerType(t1)) && (pt2 = isSgPointerType(t2)))
        {
          return equivalentModuloStrip(pt1->get_base_type(), pt2->get_base_type(), stripWhat);
        }
     SgReferenceType *rt1, *rt2;
     if ((rt1 = isSgReferenceType(t1)) && (rt2 = isSgReferenceType(t2)))
        {
          return equivalentModuloStrip(rt1->get_base_type(), rt2->get_base_type(), stripWhat);
        }
  // There ought to be a better test than this
     return (t1->get_mangled() == t2->get_mangled());
   }

//! Generates a Haskell module file that simply imports all modules in the supplied list 'modules'.
void
genImportModuleFile(string mname, ostream &out, const list<string> &modules)
   {
     out << "module " << mname << " ";
     if (modules.size() > 0)
        {
          list<string>::const_iterator i = modules.begin();
          out << "(\n  module " << *i; ++i;
          for(; i != modules.end(); ++i)
             {
               out << ",\n  module " << *i;
             }
          out << "\n)";
        }
     out << " where\n\n";

     for (list<string>::const_iterator i = modules.begin(); i != modules.end(); ++i)
        {
          out << "import " << *i << "\n";
        }
   }

//! Generates the rose.cabal file by substituting in the list of generated modules
void
genCabalFile(const list<string> &modules)
   {
     ifstream roseCabalIn("rose.cabal.in");
     ofstream roseCabalOut("rose.cabal");

     roseCabalIn.seekg(0, ios::end);
     int length = roseCabalIn.tellg();
     roseCabalIn.seekg(0, ios::beg);

     char *buf = new char[length+1];
     roseCabalIn.read(buf, length);
     buf[length] = 0;

     string bufStr = buf;
     delete[] buf;

     stringstream modSS;
     for (list<string>::const_iterator i = modules.begin(); i != modules.end(); ++i)
        {
          modSS << " " << *i;
        }
     bufStr = StringUtility::copyEdit(bufStr, "@GENERATED_MODULES@", modSS.str());
     roseCabalOut << bufStr;
   }

//! Generates the stubs and wrappers, and outputs them to the given streams.
/* Also generates some auxiliary files which it opens itself.
   This is a mistake and all the files should be opened in one place for
   consistency. */
void
genStubsAndWrappers(SgSourceFile *sage3, ostream &stubs, ostream &wrappers, APIFilter &filter)
   {
     typedef SgDeclarationStatementPtrList DeclPtrList;
     typedef Rose_STL_Container<SgMemberFunctionDeclaration *> FnDeclPtrList;

     globFilter = &filter;

     list<string> stubExports, methodsModuleExports, cabalModules;
     stringstream stubsSS, instSS, dynCastSS, staticVariantSS;

     cabalModules.push_back("ROSE.Sage3.Classes");

     genWrapperHeader(wrappers);
     genClassVariantOrdInstHeader(instSS);
     genStaticCastImplHeader(dynCastSS, stubExports);
     genStaticVariantImplHeader(staticVariantSS, stubExports);

     SgGlobal *globals = sage3->get_globalScope();
     DeclPtrList &decls = globals->getDeclarationList();

     for (DeclPtrList::const_iterator i = decls.begin(); i != decls.end(); ++i)
        {
          SgClassDeclaration *klass = isSgClassDeclaration(*i);
          if (klass && !isSgTemplateInstantiationDecl(klass) && klass == klass->get_definingDeclaration())
             {
               ROSE_ASSERT(klass->get_definition() != NULL);
               if (filter.exportClass(klass))
                  {
                    genClassStub(stubsSS, globals, klass, stubExports);
                    genClassWrapper(wrappers, klass);
                    genClassVariantOrdInstPart(instSS, klass);
                    genStaticCastImplPart(dynCastSS, klass);
                    genStaticVariantImplPart(staticVariantSS, klass);

                    list<string> methodExports;
                    stringstream methodStubsSS;
                    string methodStubsFile = "ROSE/Sage3/Methods/" + klass->get_name().getString() + ".hs";
                    ofstream methodStubs(methodStubsFile.c_str());
                    genClassStubMethods(methodStubsSS, klass, methodExports);

                    DeclPtrList &decls = klass->get_definition()->getDeclarationList();
                    map<string, FnDeclPtrList> overloadMap;
                    for (DeclPtrList::const_iterator i = decls.begin(); i != decls.end(); ++i)
                       {
                         SgMemberFunctionDeclaration *fn = isSgMemberFunctionDeclaration(*i);
                         if (!fn) 
                            {
                              continue;
                            }
                         SgDeclarationModifier &dm = fn->get_declarationModifier();
                         SgSpecialFunctionModifier &sfm = fn->get_specialFunctionModifier();
                      // if ((dm.get_storageModifier().isStatic() && !sfm.isConstructor()) || !dm.get_accessModifier().isPublic() || sfm.isDestructor())
                         if (!dm.get_accessModifier().isPublic() || sfm.isDestructor())
                            {
                              continue;
                            }
                         if (sfm.isConstructor() && klass->get_definition()->get_isAbstract())
                            {
                              continue;
                            }
                         string fnName = fn->get_name().getString();
                         if (fnName.size() > 8 && fnName.substr(0, 8) == "operator")
                            {
                              continue;
                            }
                         if (filter.exportMethod(fn))
                            {
                              FnDeclPtrList &overloadList = overloadMap[fnName];
                              for (FnDeclPtrList::const_iterator i = overloadList.begin(); i != overloadList.end(); ++i)
                                 {
                                   if (equivalentModuloStrip((*i)->get_type(), fn->get_type(), SgType::STRIP_MODIFIER_TYPE))
                                      {
                                        fn = NULL;
                                        break;
                                      }
                                 }
                              if (fn) overloadList.push_back(fn);
                            }
                       }
                    for (map<string, FnDeclPtrList>::const_iterator i = overloadMap.begin(); i != overloadMap.end(); ++i)
                       {
                         const FnDeclPtrList &overloadList = i->second;
                         bool isOverloaded = (overloadList.size() > 1);
                         for (FnDeclPtrList::const_iterator i = overloadList.begin(); i != overloadList.end(); ++i)
                            {
                              genMethodStub(methodStubsSS, *i, isOverloaded, methodExports);
                              genMethodWrapper(wrappers, *i);
                            }
                       }

                    genMethodsStubHeader(methodStubs, klass, methodExports, methodsModuleExports);
                    methodStubs << methodStubsSS.str();
                    genMethodsStubFooter(methodStubs);
                  }
             }
          SgEnumDeclaration *enumDecl = isSgEnumDeclaration(*i);
          if (enumDecl && filter.exportEnum(enumDecl))
             {
               genEnumStub(stubsSS, enumDecl, stubExports);
             }
        }
     cabalModules.insert(cabalModules.end(), methodsModuleExports.begin(), methodsModuleExports.end());

     genClassVariantOrdInstFooter(instSS);
     genStaticCastImplFooter(dynCastSS);
     genStaticVariantImplFooter(staticVariantSS);

     genStubHeader(stubs, stubExports);
     stubs << stubsSS.str();
     stubs << instSS.str();
     stubs << dynCastSS.str();
     stubs << staticVariantSS.str();
     genStubFooter(stubs);
     genWrapperFooter(wrappers);

     cabalModules.push_back("ROSE.Sage3.Methods");
     ofstream methodsModule("ROSE/Sage3/Methods.hs");
     genImportModuleFile("ROSE.Sage3.Methods", methodsModule, methodsModuleExports);

     genCabalFile(cabalModules);
   }

int main( int argc, char * argv[] ) 
   {
     initCHsTypeMap();

  // DQ (5/21/2010): Added macro to turn off the compilation of Wave when tools
  // have to compile ROSE source codes (only effects required header files). This
  // support avoids using the "USE_ROSE" macro which is reserved for ROSE internal 
  // use. This fixes a bug that caused the Haskell support to break the nightly 
  // tests of ROSE compiling ROSE using a slightly modified version of Wave that
  // fixed problems were an issue only for EDG. A newer version of EDG should fix 
  // this.
  // SgProject* project = frontend(argc,argv);
     vector<string> argvList(argv, argv + argc);
     argvList.insert(argvList.begin() + 1, "-DROSE_SKIP_COMPILATION_OF_WAVE");
     SgProject* project = frontend(argvList);

     Doxygen::annotate(project);

     ofstream stubs("ROSE/Sage3/Classes.hs");
     ofstream wrappers("WrapSage3Classes.C");

     SgSourceFile *sage3File = isSgSourceFile((*project)[0]);
     ROSE_ASSERT(sage3File != NULL);

     Sage3Filter s3f;
     genStubsAndWrappers(sage3File, stubs, wrappers, s3f);

     return 0;
   }

