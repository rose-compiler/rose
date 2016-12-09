#include "test2007_114.h"

#define JS_EXPORT_API(__type) __attribute__((visibility ("default")))  __type


extern "C" {

extern JS_EXPORT_API(JSInt64) JSLL_MaxInt(void);
extern JS_EXPORT_API(JSInt64) JSLL_MinInt(void);
extern JS_EXPORT_API(JSInt64) JSLL_Zero(void);

}

typedef JSIntn intN;
typedef JSUintn uintN;
typedef JSUword jsuword;
typedef JSWord jsword;
typedef float float32;
extern "C" {

/* Scalar typedefs. */
typedef uint16    jschar;
typedef int32     jsint;
typedef uint32    jsuint;
typedef float64   jsdouble;
typedef jsword    jsval;
typedef jsword    jsid;
typedef int32     jsrefcount;   /* PRInt32 if JS_THREADSAFE, see jslock.h */


typedef enum JSVersion {
    JSVERSION_1_0     = 100,
    JSVERSION_1_1     = 110,
    JSVERSION_1_2     = 120,
    JSVERSION_1_3     = 130,
    JSVERSION_1_4     = 140,
    JSVERSION_ECMA_3  = 148,
    JSVERSION_1_5     = 150,
    JSVERSION_1_6     = 160,
    JSVERSION_1_7     = 170,
    JSVERSION_DEFAULT = 0,
    JSVERSION_UNKNOWN = -1
} JSVersion;


/* Result of typeof operator enumeration. */
typedef enum JSType {
    JSTYPE_VOID,                /* undefined */
    JSTYPE_OBJECT,              /* object */
    JSTYPE_FUNCTION,            /* function */
    JSTYPE_STRING,              /* string */
    JSTYPE_NUMBER,              /* number */
    JSTYPE_BOOLEAN,             /* boolean */
    JSTYPE_NULL,                /* null */
    JSTYPE_XML,                 /* xml object */
    JSTYPE_LIMIT
} JSType;

/* Dense index into cached prototypes and class atoms for standard objects. */
typedef enum JSProtoKey {
#define JS_PROTO(name,code,init) JSProto_##name = code,
JS_PROTO(Null,                   0,     js_InitNullClass)
JS_PROTO(Object,                 1,     js_InitFunctionAndObjectClasses)
JS_PROTO(Function,               2,     js_InitFunctionAndObjectClasses)
JS_PROTO(Array,                  3,     js_InitArrayClass)
JS_PROTO(Boolean,                4,     js_InitBooleanClass)
JS_PROTO(Call,                   5,     js_InitCallClass)
JS_PROTO(Date,                   6,     js_InitDateClass)
JS_PROTO(Math,                   7,     js_InitMathClass)
JS_PROTO(Number,                 8,     js_InitNumberClass)
JS_PROTO(String,                 9,     js_InitStringClass)
JS_PROTO(RegExp,                10,     js_InitRegExpClass)
JS_PROTO(Script,                11,     SCRIPT_INIT)
JS_PROTO(XML,                   12,     XML_INIT)
JS_PROTO(Namespace,             13,     NAMESPACE_INIT)
JS_PROTO(QName,                 14,     QNAME_INIT)
JS_PROTO(AnyName,               15,     ANYNAME_INIT)
JS_PROTO(AttributeName,         16,     ATTRIBUTE_INIT)
JS_PROTO(Error,                 17,     js_InitExceptionClasses)
JS_PROTO(InternalError,         18,     js_InitExceptionClasses)
JS_PROTO(EvalError,             19,     js_InitExceptionClasses)
JS_PROTO(RangeError,            20,     js_InitExceptionClasses)
JS_PROTO(ReferenceError,        21,     js_InitExceptionClasses)
JS_PROTO(SyntaxError,           22,     js_InitExceptionClasses)
JS_PROTO(TypeError,             23,     js_InitExceptionClasses)
JS_PROTO(URIError,              24,     js_InitExceptionClasses)
JS_PROTO(Generator,             25,     GENERATOR_INIT)
JS_PROTO(Iterator,              26,     js_InitIteratorClasses)
JS_PROTO(StopIteration,         27,     js_InitIteratorClasses)
JS_PROTO(UnusedProto28,         28,     js_InitNullClass)
JS_PROTO(File,                  29,     FILE_INIT)
JS_PROTO(Block,                 30,     js_InitBlockClass)

#undef JS_PROTO
    JSProto_LIMIT
} JSProtoKey;

/* JSObjectOps.checkAccess mode enumeration. */
typedef enum JSAccessMode {
    JSACC_PROTO  = 0,           /* XXXbe redundant w.r.t. id */
    JSACC_PARENT = 1,           /* XXXbe redundant w.r.t. id */
    JSACC_IMPORT = 2,           /* import foo.bar */
    JSACC_WATCH  = 3,           /* a watchpoint on object foo for id 'bar' */
    JSACC_READ   = 4,           /* a "get" of foo.bar */
    JSACC_WRITE  = 8,           /* a "set" of foo.bar = baz */
    JSACC_LIMIT
} JSAccessMode;

typedef enum JSIterateOp {
    JSENUMERATE_INIT,       /* Create new iterator state */
    JSENUMERATE_NEXT,       /* Iterate once */
    JSENUMERATE_DESTROY     /* Destroy iterator state */
} JSIterateOp;

typedef struct JSClass           JSClass;
typedef struct JSExtendedClass   JSExtendedClass;
typedef struct JSConstDoubleSpec JSConstDoubleSpec;
typedef struct JSContext         JSContext;
typedef struct JSErrorReport     JSErrorReport;
typedef struct JSFunction        JSFunction;
typedef struct JSFunctionSpec    JSFunctionSpec;
typedef struct JSIdArray         JSIdArray;
typedef struct JSProperty        JSProperty;
typedef struct JSPropertySpec    JSPropertySpec;
typedef struct JSObject          JSObject;
typedef struct JSObjectMap       JSObjectMap;
typedef struct JSObjectOps       JSObjectOps;
typedef struct JSXMLObjectOps    JSXMLObjectOps;
typedef struct JSRuntime         JSRuntime;
typedef struct JSRuntime         JSTaskState;   /* XXX deprecated name */
typedef struct JSScript          JSScript;
typedef struct JSString          JSString;
typedef struct JSXDRState        JSXDRState;
typedef struct JSExceptionState  JSExceptionState;
typedef struct JSLocaleCallbacks JSLocaleCallbacks;




typedef JSBool
(*  JSPropertyOp)(JSContext *cx, JSObject *obj, jsval id,
                                 jsval *vp);


typedef JSBool
(*  JSNewEnumerateOp)(JSContext *cx, JSObject *obj,
                                     JSIterateOp enum_op,
                                     jsval *statep, jsid *idp);


typedef JSBool
(*  JSEnumerateOp)(JSContext *cx, JSObject *obj);


typedef JSBool
(*  JSResolveOp)(JSContext *cx, JSObject *obj, jsval id);


typedef JSBool
(*  JSNewResolveOp)(JSContext *cx, JSObject *obj, jsval id,
                                   uintN flags, JSObject **objp);
typedef JSBool
(*  JSConvertOp)(JSContext *cx, JSObject *obj, JSType type,
                                jsval *vp);

typedef void
(*  JSFinalizeOp)(JSContext *cx, JSObject *obj);

typedef void
(*  JSStringFinalizeOp)(JSContext *cx, JSString *str);


typedef JSObjectOps *
(*  JSGetObjectOps)(JSContext *cx, JSClass *clasp);

typedef JSBool
(*  JSCheckAccessOp)(JSContext *cx, JSObject *obj, jsval id,
                                    JSAccessMode mode, jsval *vp);


typedef JSBool
(*  JSXDRObjectOp)(JSXDRState *xdr, JSObject **objp);


typedef JSBool
(*  JSHasInstanceOp)(JSContext *cx, JSObject *obj, jsval v,
                                    JSBool *bp);


typedef uint32
(*  JSMarkOp)(JSContext *cx, JSObject *obj, void *arg);


typedef uint32
(*  JSReserveSlotsOp)(JSContext *cx, JSObject *obj);

typedef JSObjectMap *
(*  JSNewObjectMapOp)(JSContext *cx, jsrefcount nrefs,
                                     JSObjectOps *ops, JSClass *clasp,
                                     JSObject *obj);

typedef void
(*  JSObjectMapOp)(JSContext *cx, JSObjectMap *map);


typedef JSBool
(*  JSLookupPropOp)(JSContext *cx, JSObject *obj, jsid id,
                                   JSObject **objp, JSProperty **propp);


typedef JSBool
(*  JSDefinePropOp)(JSContext *cx, JSObject *obj,
                                   jsid id, jsval value,
                                   JSPropertyOp getter, JSPropertyOp setter,
                                   uintN attrs, JSProperty **propp);


typedef JSBool
(*  JSPropertyIdOp)(JSContext *cx, JSObject *obj, jsid id,
                                   jsval *vp);


typedef JSBool
(*  JSAttributesOp)(JSContext *cx, JSObject *obj, jsid id,
                                   JSProperty *prop, uintN *attrsp);


typedef JSBool
(*  JSCheckAccessIdOp)(JSContext *cx, JSObject *obj, jsid id,
                                      JSAccessMode mode, jsval *vp,
                                      uintN *attrsp);


typedef JSObject *
(*  JSObjectOp)(JSContext *cx, JSObject *obj);

typedef void
(*  JSPropertyRefOp)(JSContext *cx, JSObject *obj,
                                    JSProperty *prop);

typedef JSBool
(*  JSSetObjectSlotOp)(JSContext *cx, JSObject *obj,
                                      uint32 slot, JSObject *pobj);

typedef jsval
(*  JSGetRequiredSlotOp)(JSContext *cx, JSObject *obj,
                                        uint32 slot);

typedef JSBool
(*  JSSetRequiredSlotOp)(JSContext *cx, JSObject *obj,
                                        uint32 slot, jsval v);

typedef JSObject *
(*  JSGetMethodOp)(JSContext *cx, JSObject *obj, jsid id,
                                  jsval *vp);

typedef JSBool
(*  JSSetMethodOp)(JSContext *cx, JSObject *obj, jsid id,
                                  jsval *vp);

typedef JSBool
(*  JSEnumerateValuesOp)(JSContext *cx, JSObject *obj,
                                        JSIterateOp enum_op,
                                        jsval *statep, jsid *idp, jsval *vp);

typedef JSBool
(*  JSEqualityOp)(JSContext *cx, JSObject *obj, jsval v,
                                 JSBool *bp);

typedef JSBool
(*  JSConcatenateOp)(JSContext *cx, JSObject *obj, jsval v,
                                    jsval *vp);

/* Typedef for native functions called by the JS VM. */

typedef JSBool
(*  JSNative)(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);

/* Callbacks and their arguments. */

typedef enum JSContextOp {
    JSCONTEXT_NEW,
    JSCONTEXT_DESTROY
} JSContextOp;

typedef JSBool
(*  JSContextCallback)(JSContext *cx, uintN contextOp);

typedef enum JSGCStatus {
    JSGC_BEGIN,
    JSGC_END,
    JSGC_MARK_END,
    JSGC_FINALIZE_END
} JSGCStatus;

typedef JSBool
(*  JSGCCallback)(JSContext *cx, JSGCStatus status);

typedef JSBool
(*  JSBranchCallback)(JSContext *cx, JSScript *script);

typedef void
(*  JSErrorReporter)(JSContext *cx, const char *message,
                                    JSErrorReport *report);


typedef enum JSExnType {
    JSEXN_NONE = -1,
      JSEXN_ERR,
        JSEXN_INTERNALERR,
        JSEXN_EVALERR,
        JSEXN_RANGEERR,
        JSEXN_REFERENCEERR,
        JSEXN_SYNTAXERR,
        JSEXN_TYPEERR,
        JSEXN_URIERR,
        JSEXN_LIMIT
} JSExnType;

typedef struct JSErrorFormatString {
    /* The error format string (UTF-8 if JS_C_STRINGS_ARE_UTF8 is defined). */
    const char *format;

    /* The number of arguments to expand in the formatted error message. */
    uint16 argCount;

    /* One of the JSExnType constants above. */
    int16 exnType;
} JSErrorFormatString;

typedef const JSErrorFormatString *
(*  JSErrorCallback)(void *userRef, const char *locale,
                                    const uintN errorNumber);

typedef JSBool
(*  JSLocaleCompare)(JSContext *cx,
                                    JSString *src1, JSString *src2,
                                    jsval *rval);

typedef JSBool
(*  JSLocaleToUnicode)(JSContext *cx, char *src, jsval *rval);

/*
 * Security protocol types.
 */
typedef struct JSPrincipals JSPrincipals;


typedef JSBool
(*  JSPrincipalsTranscoder)(JSXDRState *xdr,
                                           JSPrincipals **principalsp);

typedef JSPrincipals *
(*  JSObjectPrincipalsFinder)(JSContext *cx, JSObject *obj);

}

extern "C" {

/* Type tag bitfield length and derived macros. */

extern JS_EXPORT_API(void)
JS_ClearScope(JSContext *cx, JSObject *obj);

}





typedef struct JSFatLock JSFatLock;

struct JSFatLock {
};

typedef struct JSThinLock {
} JSThinLock;

typedef PRLock JSLock;

typedef struct JSFatLockTable {
    JSFatLock   *free;
    JSFatLock   *taken;
} JSFatLockTable;


extern "C" {

typedef uint32 JSHashNumber;
typedef struct JSHashEntry JSHashEntry;
typedef struct JSHashTable JSHashTable;

typedef JSHashNumber (*  JSHashFunction)(const void *key);
typedef intN (*  JSHashComparator)(const void *v1, const void *v2);
typedef intN (*  JSHashEnumerator)(JSHashEntry *he, intN i, void *arg);

typedef struct JSHashAllocOps {
} JSHashAllocOps;

struct JSHashTable {
};

extern JS_EXPORT_API(JSHashTable *)
JS_NewHashTable(uint32 n, JSHashFunction keyHash,
                JSHashComparator keyCompare, JSHashComparator valueCompare,
                JSHashAllocOps *allocOps, void *allocPriv);

extern JS_EXPORT_API(void)
JS_HashTableDestroy(JSHashTable *ht);

extern JS_EXPORT_API(JSHashEntry **)
JS_HashTableRawLookup(JSHashTable *ht, JSHashNumber keyHash, const void *key);

extern JS_EXPORT_API(JSHashEntry *)
JS_HashTableRawAdd(JSHashTable *ht, JSHashEntry **hep, JSHashNumber keyHash,
                   const void *key, void *value);

extern JS_EXPORT_API(void)
JS_HashTableRawRemove(JSHashTable *ht, JSHashEntry **hep, JSHashEntry *he);

extern JS_EXPORT_API(JSHashEntry *)
JS_HashTableAdd(JSHashTable *ht, const void *key, void *value);

extern JS_EXPORT_API(JSBool)
JS_HashTableRemove(JSHashTable *ht, const void *key);

extern JS_EXPORT_API(intN)
JS_HashTableEnumerateEntries(JSHashTable *ht, JSHashEnumerator f, void *arg);

extern JS_EXPORT_API(void *)
JS_HashTableLookup(JSHashTable *ht, const void *key);

extern JS_EXPORT_API(intN)
JS_HashTableDump(JSHashTable *ht, JSHashEnumerator dump, FILE *fp);

extern JS_EXPORT_API(JSHashNumber)
JS_HashString(const void *key);

extern JS_EXPORT_API(intN)
JS_CompareValues(const void *v1, const void *v2);

}

typedef struct JSAtom               JSAtom;
typedef struct JSAtomList           JSAtomList;
typedef struct JSAtomListElement    JSAtomListElement;
typedef struct JSAtomMap            JSAtomMap;
typedef struct JSAtomState          JSAtomState;
typedef struct JSCodeSpec           JSCodeSpec;
typedef struct JSPrinter            JSPrinter;
typedef struct JSRegExp             JSRegExp;
typedef struct JSRegExpStatics      JSRegExpStatics;
typedef struct JSScope              JSScope;
typedef struct JSScopeOps           JSScopeOps;
typedef struct JSScopeProperty      JSScopeProperty;
typedef struct JSStackFrame         JSStackFrame;
typedef struct JSStackHeader        JSStackHeader;
typedef struct JSStringBuffer       JSStringBuffer;
typedef struct JSSubString          JSSubString;
typedef struct JSXML                JSXML;
typedef struct JSXMLNamespace       JSXMLNamespace;
typedef struct JSXMLQName           JSXMLQName;
typedef struct JSXMLArray           JSXMLArray;
typedef struct JSXMLArrayCursor     JSXMLArrayCursor;

extern "C" {

struct JSObjectMap {
};

}

void
Clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
        JS_ClearScope(cx, ((JSObject *)((void *)((argv[0]) & ~(jsval)(1)))));

}


