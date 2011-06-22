#include <rose.h>
#include <dlfcn.h>
#include <map>
#include <typeinfo>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>

#include "typeLayoutStore.h"
#include <interp_extcall.h>

#include <ffi.h>

using namespace std;
using namespace Interp;
using namespace boost::bimaps;

namespace Interp {
namespace extcall {

class RoseFFI
   {

     vector<ffi_type *> allocatedTypes;
     vector<ffi_type **> allocatedTypeArrays;

     ffi_type **argTypes, *retType;
     ffi_cif cif;

     ffi_type *getFFIClassType(SgClassType *ct)
        {
          const StructLayoutInfo &sli = typeLayout(ct);
          ffi_type **fieldTypes = new ffi_type*[sli.fields.size()];
          allocatedTypeArrays.push_back(fieldTypes);
          for (size_t i = 0; i < sli.fields.size(); i++)
             {
               SgNode *decl = sli.fields[i].decl;
               if (SgInitializedName *in = isSgInitializedName(decl))
                  {
                    fieldTypes[i] = getFFIType(in->get_type());
                  }
               else if (SgBaseClass *bc = isSgBaseClass(decl))
                  {
                    fieldTypes[i] = getFFIType(bc->get_base_class()->get_type());
                  }
               else
                  {
                    throw InterpError("Encountered unsupported field decl: " + decl->class_name());
                  }
             }

          ffi_type *ctType = new ffi_type;
          allocatedTypes.push_back(ctType);
          ctType->elements = fieldTypes;
          return ctType;
        }

     ffi_type *getFFIType(SgType *t)
        {
          t = t->stripTypedefsAndModifiers();
          switch (t->variantT())
             {
               case V_SgTypeBool: return &ffi_type_uchar;
               case V_SgTypeChar: return &ffi_type_schar;
               case V_SgTypeDouble: return &ffi_type_double;
               case V_SgTypeFloat: return &ffi_type_float;
               case V_SgTypeInt: return &ffi_type_sint;
               case V_SgTypeLongDouble: return &ffi_type_longdouble;
               case V_SgTypeLong: return &ffi_type_slong;
               case V_SgTypeLongLong: return &ffi_type_sint64; /* NOTE: inaccurate */
               case V_SgTypeShort: return &ffi_type_sshort;
               case V_SgTypeUnsignedChar: return &ffi_type_uchar;
               case V_SgTypeUnsignedInt: return &ffi_type_uint;
               case V_SgTypeUnsignedLongLong: return &ffi_type_uint64; /* NOTE: inaccurate */
               case V_SgTypeUnsignedLong: return &ffi_type_ulong;
               case V_SgTypeUnsignedShort: return &ffi_type_ushort;
               case V_SgTypeVoid: return &ffi_type_void;
               case V_SgClassType: return getFFIClassType(isSgClassType(t));
               case V_SgArrayType: return &ffi_type_pointer;
               case V_SgPointerType: return &ffi_type_pointer;
               default:
                 throw InterpError("Encountered unsupported type: " + t->class_name());
             }
        }

     public:
     bool prepare(SgFunctionType *fnType)
        {
          // TODO: get the calling convention from the SgTypeModifier
          SgTypePtrList &args = fnType->get_argument_list()->get_arguments();
          argTypes = new ffi_type*[args.size()];
          allocatedTypeArrays.push_back(argTypes);
          for (size_t i = 0; i < args.size(); ++i)
             {
               argTypes[i] = getFFIType(args[i]);
             }
          retType = getFFIType(fnType->get_return_type());
          return (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, args.size(), retType, argTypes) == FFI_OK);
        }

     typedef bimap<set_of<ValueP>, set_of<void *> > ptrMap_t;
     
     ptrMap_t ptrMap;

     void *getPtr(ValueP val)
        {
          Value *valP = val.get();
          if (OffsetValue *ofsVal = dynamic_cast<OffsetValue *>(valP))
             {
               void *basePtr = getPtr(ofsVal->getBase());
               return static_cast<void *>(static_cast<char *>(basePtr) + ofsVal->getOffset());
             }
          if (ptrMap.left.count(val))
             {
               return ptrMap.left.at(val);
             }
          void *ptr = malloc(val->forwardValidity());
          ptrMap.insert(ptrMap_t::value_type(val, ptr));
          serialize(ptr, val);
          return ptr;
        }

     void serialize(void *data, ValueP val)
        {
          Value *valP = val.get();
          if (PointerValue *ptrVal = dynamic_cast<PointerValue *>(valP))
             {
               void **pdata = static_cast<void **>(data);
               ValueP target = ptrVal->getTarget();
               if (target.get() == NULL)
                  {
                    *pdata = NULL;
                  }
               else
                  {
                    *pdata = getPtr(target);
                  }
             }
          else if (CharValue *charVal = dynamic_cast<CharValue *>(valP))
             {
               char *pdata = static_cast<char *>(data);
               *pdata = charVal->getConcreteValueChar();
             }
          else if (IntValue *intVal = dynamic_cast<IntValue *>(valP))
             {
               char *pdata = static_cast<char *>(data);
               *pdata = intVal->getConcreteValueInt();
             }
          // TODO: other primtypes
          else if (BaseCompoundValue *compVal = dynamic_cast<BaseCompoundValue *>(valP))
             {
               struct SerializeField : FieldVisitor
                  {

                    RoseFFI *ffi;
                    void *data;

                    SerializeField(RoseFFI *ffi, void *data) : ffi(ffi), data(data) {}

                    void operator()(long offset, ValueP prim)
                       {
                         if (offset >= 0)
                            {
                              void *dataOfs = static_cast<void *>(static_cast<char *>(data) + offset);
                              ffi->serialize(dataOfs, prim);
                            }
                       }

                  };
               SerializeField sf(this, data);
               compVal->forEachPrim(sf);
             }
          else
             {
               throw InterpError(string("Unable to serialize a ") + typeid(*valP).name());
             }
        }

     void call(void (*fnPtr)(), const vector<ValueP> &args)
        {
          void **argPtrs = new void*[args.size()];
          for (size_t i = 0; i < args.size(); ++i)
             {
               argPtrs[i] = getPtr(args[i]);
             }
          char dummy[256]; // TODO: get return value (from here?)
          ffi_call(&cif, fnPtr, dummy, argPtrs);
          delete[] argPtrs;
        }

     ~RoseFFI()
        {
          for (size_t i = 0; i < allocatedTypes.size(); i++)
               delete allocatedTypes[i];
          for (size_t i = 0; i < allocatedTypeArrays.size(); i++)
               delete[] allocatedTypeArrays[i];
          // TODO: delete ptrMap
        }

   };

string ExternalCallingFunctionValue::functionName() const
   {
     stringstream ss;
     ss << fnName << "@" << fnPtr;
     return ss.str();
   }

ValueP ExternalCallingFunctionValue::call(SgFunctionType *fnType, const vector<ValueP> &args) const
   {
     if (fnType->get_argument_list()->get_arguments().size() != args.size())
        {
          throw InterpError("ExternalCalling: function's formal parameter list length differs from actual parameter list length");
        }
     RoseFFI ffi;
     if (!ffi.prepare(fnType))
        {
          throw InterpError("Could not prepare the function type " + fnType->unparseToString());
        }
     ffi.call(fnPtr, args);
     return ValueP(); // TODO: get return value
   }

StackFrameP ExternalCallingStackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new ExternalCallingStackFrame(sharedLibraries, interp(), funSym, thisBinding));
   }

ValueP ExternalCallingStackFrame::externEvalFunctionRefExp(SgFunctionSymbol *sym)
   {
     string symName = sym->get_name().getString();
     for (vector<void *>::const_iterator sharedLib = sharedLibraries.begin(); sharedLib != sharedLibraries.end(); ++sharedLib)
        {
          void (*fnPtr)() = reinterpret_cast<void(*)()>(dlsym(*sharedLib, symName.c_str()));
          if (fnPtr != NULL)
             {
               return ValueP(new ExternalCallingFunctionValue(symName, fnPtr, PTemp, shared_from_this()));
             }
        }
     throw InterpError("Could not find the function symbol " + symName + " in the current list of shared libraries");
   }

ValueP ExternalCallingStackFrame::evalFunctionRefExp(SgFunctionSymbol *sym)
   {
     SgFunctionDeclaration *decl = sym->get_declaration();
     SgFunctionDeclaration *defDecl = isSgFunctionDeclaration(decl->get_definingDeclaration());
     if (defDecl == NULL)
        {
          return externEvalFunctionRefExp(sym);
        }
     SgFunctionDefinition *def = defDecl->get_definition();
     if (def == NULL)
        {
          return externEvalFunctionRefExp(sym);
        }

     return StackFrame::evalFunctionRefExp(sym);
   }

#if 0
class DL
   {
     void *dl;

     public:
     DL(const char *filename, int flag) : dl(dlopen(filename, flag))
        {
          if (dl == NULL)
             {
               throw string(dlerror());
             }
        }

     operator void*() { return dl; }

     void *get() { return dl; }

   };
#endif

vector<void *> buildLibraryList(SgProject *prj)
   {
     vector<void *> libs;
     void *libc;
#ifdef __APPLE__
     libc = dlopen("libc.dylib", RTLD_NOW | RTLD_GLOBAL);
#else
     libc = dlopen("libc.so.6", RTLD_NOW | RTLD_GLOBAL);
#endif
     char *err = dlerror();
     if (err != NULL)
        {
          throw InterpError(string("Couldn't load libc: ") + err);
        }
     libs.push_back(libc);
     // libs.push_back(RTLD_DEFAULT);
     return libs;
   }

/*
int main(int argc, char **argv)
   {
     SgProject *prj = frontend(argc, argv);
     SgSourceFile *file = isSgSourceFile((*prj)[0]);
     ROSE_ASSERT(file != NULL);
     SgGlobal *global = file->get_globalScope();
     SgFunctionSymbol *testSym = global->lookup_function_symbol("main");

     Interpretation interp;
     StackFrameP head(new ExternalCallingStackFrame(buildLibraryList(prj), &interp, testSym));
     ValueP argcVal (new IntValue(0, PTemp, head));
     ValueP argvVal (new PointerValue(ValueP(), PTemp, head));
     vector<ValueP> params;
     params.push_back(argcVal);
     params.push_back(argvVal);
     ValueP rv = head->interpFunction(params);
     cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
   }
*/

}
}
