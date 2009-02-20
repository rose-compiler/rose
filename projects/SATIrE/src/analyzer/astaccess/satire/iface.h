// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: iface.h,v 1.6 2008-09-23 07:50:38 gergo Exp $

#ifndef H_IFACE
#define H_IFACE

#include <stdio.h>

#define ANIM

#if 0
enum KFG_NODE_TYPE;
typedef enum
{
    CALL,
    RETURN,
    START,
    END,
    INNER
} KFG_NODE_TYPE;
typedef int KFG_NODE_ID;
#endif

#include "kfg_types.h"

#if HAVE_PAG
#include "genkfg.h"
#endif
#if 0
#define genkfg2_Il1l_I_
typedef struct{char const*attr_def_name;int attr_def_id;}KFG_ATTR_DEF;
#endif

#ifdef __cplusplus
extern "C"
#endif
KFG kfg_create(KFG);
#ifdef __cplusplus
extern "C"
#endif
int kfg_num_nodes(KFG);
#ifdef __cplusplus
extern "C"
#endif
/*enum*/ KFG_NODE_TYPE kfg_node_type(KFG, KFG_NODE); // removed "enum" MS: 20.07.2007
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_ID kfg_get_id(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_get_node(KFG, KFG_NODE_ID);
#ifdef __cplusplus
extern "C"
#endif
int kfg_get_bbsize(KFG, KFG_NODE);
/* PIrStatement kfg_get_bbelem(KFG, KFG_NODE, int); */
#ifdef __cplusplus
extern "C"
#endif
void *kfg_get_bbelem(KFG, KFG_NODE, int);
#ifdef __cplusplus
extern "C"
#endif
void kfg_node_infolabel_print_fp(FILE *, KFG, KFG_NODE, int);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_predecessors(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_successors(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_get_call(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_get_return(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_get_start(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_get_end(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
const int *kfg_get_beginnings(KFG);
#ifdef __cplusplus
extern "C"
#endif
int kfg_replace_beginnings(KFG, const int *);

#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_node_list_head(KFG_NODE_LIST);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_node_list_tail(KFG_NODE_LIST);
#ifdef __cplusplus
extern "C"
#endif
int kfg_node_list_is_empty(KFG_NODE_LIST);
#ifdef __cplusplus
extern "C"
#endif
int kfg_node_list_length(KFG_NODE_LIST);

#ifdef __cplusplus
extern "C"
#endif
unsigned int kfg_edge_type_max(KFG);
#ifdef __cplusplus
extern "C"
#endif
KFG_EDGE_TYPE kfg_edge_type(KFG_NODE, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
int kfg_which_in_edges(KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
int kfg_which_out_edges(KFG_NODE);

#ifdef __cplusplus
extern "C"
#endif
int kfg_num_procs(KFG);
#ifdef __cplusplus
extern "C"
#endif
char *kfg_proc_name(KFG, int);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE kfg_numproc(KFG, int);
#ifdef __cplusplus
extern "C"
#endif
int kfg_procnumnode(KFG, KFG_NODE);
#ifdef __cplusplus
extern "C"
#endif
int kfg_procnum(KFG, KFG_NODE_ID);

#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_all_nodes(KFG);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_entrys(KFG);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_calls(KFG);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_returns(KFG);
#ifdef __cplusplus
extern "C"
#endif
KFG_NODE_LIST kfg_exits(KFG);

// Headers for support types defined by SATIrE.
#include "o_ExpressionId.h"
#include "o_TypeId.h"
#include "o_VariableId.h"

#endif
