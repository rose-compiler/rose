// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: pag_support.c,v 1.2 2007-03-08 15:36:48 markus Exp $

#include <iostream>

#include "config.h"
#include "rose.h"

#include "syntree.h"
#include "pignodelist.h"
#include "pag_support.h"

int syntaxtype;
int e_syntaxtype;

#if OLD_GET_VALUE
#define get_value_macro(T) char *T##_get_value(void *s) \
    { if (s == NULL) return "<null>"; return (char *) s; }
#else
#define get_value_macro(T) char *T##_get_value(T x) \
{ std::stringstream s; s << x; return strdup(s.str().c_str()); }
#endif

PIG_EXTERN_C get_value_macro(astint)
PIG_EXTERN_C get_value_macro(aststring)
PIG_EXTERN_C get_value_macro(astshort)
PIG_EXTERN_C get_value_macro(astchar)
PIG_EXTERN_C get_value_macro(astuchar)
PIG_EXTERN_C get_value_macro(astushort)
PIG_EXTERN_C get_value_macro(astuint)
PIG_EXTERN_C get_value_macro(astlong)
PIG_EXTERN_C get_value_macro(astllong)
PIG_EXTERN_C get_value_macro(astullong)
PIG_EXTERN_C get_value_macro(astulong)
PIG_EXTERN_C get_value_macro(astfloat)
PIG_EXTERN_C get_value_macro(astdouble)
PIG_EXTERN_C get_value_macro(astldouble)

PIG_EXTERN_C
const char *Expression_print(void *e)
{
    std::cout << expr_to_string(isSgExpression((SgNode *) e));
    return "";
}

PIG_EXTERN_C
const char *c_str_print(const void *s)
{
    std::cout << (const char *) s;
    return (const char *) s;
}

PIG_EXTERN_C
int LIST_VariableSymbolNT_empty(void *l)
{
    return ((PigNodeList *) l)->empty();
}

PIG_EXTERN_C
void *LIST_VariableSymbolNT_hd(void *l)
{
    return ((PigNodeList *) l)->head();
}

PIG_EXTERN_C
void *LIST_VariableSymbolNT_tl(void *l)
{
    return ((PigNodeList *) l)->tail();
}

PIG_EXTERN_C
char *basic_type_name(const void *p)
{
    const SgNode *node = (SgNode *) p;

    switch (node->variantT())
    {
    case V_SgTypeBool: return "bool"; break;
    case V_SgTypeChar: return "char"; break;
    case V_SgTypeDouble: return "double"; break;
    case V_SgTypeFloat: return "float"; break;
    case V_SgTypeInt: return "int"; break;
    case V_SgTypeLong: return "long"; break;
    case V_SgTypeLongDouble: return "long double"; break;
    case V_SgTypeLongLong: return "long long"; break;
    case V_SgTypeShort: return "short"; break;
    case V_SgTypeSignedChar: return "signed char"; break;
    case V_SgTypeSignedInt: return "signed int"; break;
    case V_SgTypeSignedLong: return "signed long"; break;
    case V_SgTypeSignedShort: return "signed short"; break;
    case V_SgTypeString: return "string"; break;
    case V_SgTypeUnsignedChar: return "unsigned char"; break;
    case V_SgTypeUnsignedInt: return "unsinged int"; break;
    case V_SgTypeUnsignedLong: return "unsigned long"; break;
    case V_SgTypeUnsignedLongLong: return "unsigned long long"; break;
    case V_SgTypeUnsignedShort: return "unsigned short"; break;
    case V_SgTypeVoid: return "void"; break;
    case V_SgTypeWchar: return "wchar"; break;
    default: return NULL;
    }
}

#include "genkfg.h"

KFG_ATTR_DEF empty = { 0 };
KFG_ATTR_DEF *addr = &empty;
int zero = 0;

char const *kfg_get_attribute_symbol(char *)
{
    return NULL;
}

int kfg_get_global_attribute_information(KFG, KFG_ATTR_DEF **arr,
int *len)
{
    arr = &addr;
    len = &zero;
    return 1;
}

int kfg_get_routine_attribute_information(KFG, KFG_ATTR_DEF **arr,
int *len)
{
    arr = &addr;
    len = &zero;
    return 1;
}

int kfg_get_bblock_attribute_information(KFG, KFG_ATTR_DEF **arr,
int *len)
{
    arr = &addr;
    len = &zero;
    return 1;
}

int kfg_get_instruction_attribute_information(KFG, KFG_ATTR_DEF
**arr, int *len)
{
    arr = &addr;
    len = &zero;
    return 1;
}

int kfg_remap_global_attributes(KFG, KFG_ATTR_DEF **, int)
{
    return 1;
}

int kfg_remap_routine_attributes(KFG, KFG_ATTR_DEF **, int)
{
    return 1;
}

int kfg_remap_bblock_attributes(KFG, KFG_ATTR_DEF **, int)
{
    return 1;
}

int kfg_remap_instruction_attributes(KFG, KFG_ATTR_DEF **, int)
{
    return 1;
}

char const *kfg_get_global_attribute(KFG, int)
{
    return "";
}

char const *kfg_get_routine_attribute(KFG, int, int)
{
    return "";
}

char const *kfg_get_bblock_attribute(KFG, KFG_NODE, int)
{
    return "";
}

char const *kfg_get_instruction_attribute(KFG, KFG_NODE, int, int)
{
    return "";
}

char const *kfg_get_global_attribute_by_name(KFG, char *)
{
    return "";
}

char const *kfg_get_routine_attribute_by_name(KFG, int, char*)
{
    return "";
}

char const *kfg_get_bblock_attribute_by_name(KFG, KFG_NODE, char *)
{
    return "";
}

char const *kfg_get_instruction_attribute_by_name(KFG, KFG_NODE,
int, char *)
{
    return "";
}

/* static attributes */
//#define HAVE_MEMMOVE
#include "pagheader.h"
#include "snum.h"
/* numbers of types and expressions */
extern "C" snum kfg_get_global_attribute__numtypes(KFG cfg)
{
    return int_to_snum(((CFG *) cfg)->types_numbers.size());
}
extern "C" snum kfg_get_global_attribute__numexprs(KFG cfg)
{
    return int_to_snum(((CFG *) cfg)->exprs_numbers.size());
}
/* statement labels */
extern "C" snum kfg_get_bblock_attribute__label(KFG cfg, KFG_NODE bb)
{
    return int_to_snum(kfg_get_id(cfg, bb));
}

/* external support functions */
#include "bool.h"
extern "C" bool o_is_unary(void *expr)
{
    return isSgUnaryOp((SgNode *) expr);
}

extern "C" bool o_is_binary(void *expr)
{
    return isSgBinaryOp((SgNode *) expr);
}

extern "C" bool o_is_value(void *expr)
{
    return isSgValueExp((SgNode *) expr);
}

#include "assert.h"

extern "C" void *o_unary_get_child(void *expr)
{
    assert(is_unary(expr));
    return isSgUnaryOp((SgNode *) expr)->get_operand();
}

extern "C" void *o_binary_get_left_child(void *expr)
{
    assert(is_binary(expr));
    return isSgBinaryOp((SgNode *) expr)->get_lhs_operand();
}

extern "C" void *o_binary_get_right_child(void *expr)
{
    assert(is_binary(expr));
    return isSgBinaryOp((SgNode *) expr)->get_rhs_operand();
}

CFG *global_cfg = NULL;

static CFG *get_global_cfg()
{
    return global_cfg;
}

extern "C" void *o_typenum_to_type(snum n)
{
    return get_global_cfg()->numbers_types[snum_to_int(n)];
}

extern "C" snum o_type_to_typenum(void *type)
{
    return get_global_cfg()->types_numbers[isSgType((SgNode *) type)];
}

#include "str.h"

extern "C" str o_typenum_to_str(snum n)
{
    return strdup(isSgType((SgNode *) o_typenum_to_type(n))
        ->unparseToString().c_str());
}

extern "C" void *o_exprnum_to_expr(snum n)
{
    return get_global_cfg()->numbers_exprs[snum_to_int(n)];
}

extern "C" snum o_expr_to_exprnum(void *expr)
{
    return get_global_cfg()->exprs_numbers[isSgExpression((SgNode *) expr)];
}

extern "C" str o_exprnum_to_str(snum n)
{
    return expr_to_string((SgExpression *) o_exprnum_to_expr(n));
}

extern "C" void *o_expr_type(void *expr)
{
    return isSgExpression((SgNode *) expr)->get_type();
}

extern "C" snum o_exprnum_typenum(snum n)
{
    return o_type_to_typenum(o_expr_type(o_exprnum_to_expr(n)));
}

extern "C" bool o_is_subtype_of(void *a, void *b)
{
    SgClassType *at = isSgClassType((SgType *) a),
                *bt = isSgClassType((SgType *) b);
    /* a is subtype of b if a inherits from b: look at a's
     * inheritances (base classes) */
    if (at == NULL) return false;
    if (bt == NULL) return false;
    const std::list<SgBaseClass *> &base_classes = 
        isSgClassDeclaration(at->get_declaration())->get_definition()
            ->get_inheritances();
    std::list<SgBaseClass *>::const_iterator i;
    for (i = base_classes.begin(); i != base_classes.end(); ++i)
    {
        SgClassDeclaration *base = (*i)->get_base_class();
        SgClassDeclaration *bdecl
            = isSgClassDeclaration(bt->get_declaration());
        if (base == bdecl || (base->get_type() != NULL && bdecl != NULL
                    && o_is_subtype_of(base->get_type(), bdecl->get_type())))
            return true;
    }
    return false;
}

extern "C" bool o_is_subtypenum_of(snum a, snum b)
{
    return o_is_subtype_of(o_typenum_to_type(a), o_typenum_to_type(b));
}

extern "C" str o_exp_root_str(void *exp)
{
    /* WARNING: This is not clean at all, but the Value Flow Graph
     * analysis relies on the fact that the strings representing
     * operators returned by this function begin with "op " (yes,
     * including the space character; that ensures it is not a
     * variable name beginning with "op").
     * Please do not change that without due cause, thanks. */
    SgExpression *expr = isSgExpression((SgNode *) exp);
    if (expr == NULL)
        return "'not an expression'";
    else switch (expr->variantT())
    {
    case V_SgAddOp:             return "op +";
    case V_SgSubtractOp:        return "op -";
    case V_SgMultiplyOp:        return "op *";
    case V_SgDivideOp:          return "op / (floating)";
    case V_SgIntegerDivideOp:   return "op / (integer)";
    case V_SgModOp:             return "op %";
    case V_SgAndOp:             return "op &&";
    case V_SgOrOp:              return "op ||";
    case V_SgBitXorOp:          return "op ^";
    case V_SgBitAndOp:          return "op &";
    case V_SgBitOrOp:           return "op |";
    case V_SgLshiftOp:          return "op <<";
    case V_SgRshiftOp:          return "op >>";
    case V_SgMinusOp:           return "op - (unary)";
    case V_SgPointerDerefExp:   return "op * (dereference)";
    case V_SgAddressOfOp :      return "op & (address of)";
    case V_SgVarRefExp:
        {
            SgVarRefExp *v = isSgVarRefExp(expr);
            std::string name = v->get_symbol()->get_name().str();
            return strdup(name.c_str());
        }
    case V_SgIntVal:
        {
            std::stringstream value;
            value << isSgIntVal((SgNode *) expr)->get_value();
            return strdup(value.str().c_str());
        }
    default:
        {
            SgValueExp *ve = isSgValueExp(expr);
            if (ve != NULL)
                return expr_to_string(ve);
            std::string class_name = expr->class_name();
            return strdup(class_name.c_str());
        }
    }
}

extern "C" bool o_is_operatorname(str s)
{
    return strncmp(s, "op ", 3) == 0;
}

#include <string.h>
#include "gc_mem.h"

unsigned int synttype_hash(void *x)
{
    return (unsigned int) x;
}

int syntax_eq(void *x, void *y)
{
    return (strcmp((const char *) x, (const char *) y) != 0);
}

void syntax_mcopy(void *x, void *y)
{
    strcpy((char *) y, (const char *) x);
}

void syntaxdummy(void *)
{
}

#ifdef __cplusplus
extern "C"
#endif
void syntax_init(void)
{
    syntaxtype = GC_registertype(48, syntaxdummy, syntax_mcopy,
        syntax_eq, synttype_hash, 0);
    e_syntaxtype = GC_registertype(1024, syntaxdummy, syntax_mcopy,
        syntax_eq, synttype_hash, 0);
}
