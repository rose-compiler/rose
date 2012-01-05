// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: testcfg.c,v 1.5 2008-09-29 12:33:35 gergo Exp $

/*
 * TestCFG: consistency test for a cfg and its PAG-interface
 * version 0.0.19
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include "testcfg.h"

#undef NDEBUG
#ifdef NDEBUG
# warning: *** TestCFG disabled ** undefine NDEBUG to enable TestCFG ***
int kfg_testit (KFG kfg, int quiet_mode, char *gdl_name)
{
  return 1;
}
#else
static KFG cfg;
static int quiet;
static int *check;
static int error_found;
static int warning_found;


/* Prints a diagnostic message if not in quiet mode */
static void diagnostic (char *fstr, ...)
{
  va_list ap;


  if (0 == quiet)
    {

      va_start (ap, fstr);
      vprintf (fstr, ap);
      va_end (ap);

      fflush (stdout);
    }
}


/* Prints an error message */
static void error (char *fstr, ...)
{
  va_list ap;


  /* newline on stdout */
  if (0 == quiet)
    printf ("\n");

  /* error message to stderr */
  va_start (ap, fstr);
  fprintf (stderr, "TestCFG: ");
  vfprintf (stderr, fstr, ap);
  va_end (ap);

  error_found++;
}


/* Tests wheter a node list contains a certain node. */
static int node_list_contains (KFG_NODE_LIST list, KFG_NODE node)
{
  KFG_NODE n;


  /* look throug all elements of the list */
  while (0 == kfg_node_list_is_empty (list))
    {
      n    = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      if (node == n)
	return 1;
    }

  /* node not found */
  return 0;
}


/* Tests length of a node list. */
static void test_node_list_length (KFG_NODE_LIST list)
{
  int i = 0, length = kfg_node_list_length (list);


  while (0 == kfg_node_list_is_empty (list))
    {
      (void) kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
      i++;
    }

  if (i != length)
    error ("error in length of node lists: counted %d but length is %d\n", i, length);
}


/* Tests for empty control flow graphs. */
static void test_0 (void)
{
  int i, num, l1, l2;
  KFG_NODE node;


  diagnostic (".");

  /* number of nodes in the graph */
  if (0 != (num = kfg_num_nodes (cfg)))
    error ("number of nodes in empty cfg is not zero: %d\n", num);

  /* must be equal to the length of the all-nodes list */
  if (0 != kfg_node_list_length (kfg_all_nodes (cfg)))
    error ("all-nodes list has non zero lengt\n");

  if (0 != kfg_node_list_length (kfg_calls (cfg)))
    error ("call-nodes list has non zero lengt\n");

  if (0 != kfg_node_list_length (kfg_returns (cfg)))
    error ("return-nodes list has non zero lengt\n");

  if (0 != kfg_node_list_length (kfg_entrys (cfg))) 
    error ("entry-nodes list has non zero lengt\n");

  if (0 != kfg_node_list_length (kfg_exits (cfg)))
    error ("exit-nodes list has non zero lengt\n");

  if (0 != kfg_num_procs (cfg))
    error ("routine list is not empty\n");
}


/*
 * - compare length of node lists with total number of nodes
 * - corresponding lists are of equal length
 * - tests length of all node lists
 */
static void test_1 (void)
{
  int i, num, l1, l2;
  KFG_NODE node;
  KFG_NODE_LIST list;


  diagnostic (".");

  /* number of nodes in the graph */
  num = kfg_num_nodes (cfg);

  /* must be equal to the length of the all-nodes list */
  if ((i = kfg_node_list_length (kfg_all_nodes (cfg))) != num)
    error ("length of all-nodes list differs from number of nodes: %d/%d\n", i, num);

  /* not all nodes can be entry-nodes */
  if ((l1 = kfg_node_list_length (kfg_entrys (cfg))) > num/2)
    error ("to many procedure entry nodes: %d/%d\n", l1, num);

  /* not all nodes can be exit-nodes */
  if ((l2 = kfg_node_list_length (kfg_exits (cfg))) > num/2)
    error ("to many procedure exit nodes: %d/%d\n", l2, num);

  /* number of entry and exit nodes must be equal */
  if (l1 != l2)
    error ("number of procedure entry and exit nodes is not equal: %d/%d\n", l1, l2);

  /* not all nodes can be call-nodes */
  if ((l1 = kfg_node_list_length (kfg_calls (cfg))) > num/2)
    error ("to many procedure call nodes: %d/%d\n", l1, num);

  /* not all nodes can be return-nodes */
  if ((l2 = kfg_node_list_length (kfg_returns (cfg))) > num/2)
    error ("to many procedure return nodes: %d/%d\n", l2, num);

  /* number of call and return nodes must be equal */
  if (l1 != l2)
    error ("number of procedure call and return nodes is not equal: %d/%d\n", l1, l2);

  /* test length of node lists */
  test_node_list_length (kfg_entrys (cfg));
  test_node_list_length (kfg_exits (cfg));
  test_node_list_length (kfg_calls (cfg));
  test_node_list_length (kfg_returns (cfg));
  test_node_list_length (list = kfg_all_nodes (cfg));

  while (0 == kfg_node_list_is_empty (list))
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      test_node_list_length (kfg_predecessors (cfg, node));
      test_node_list_length (kfg_successors (cfg, node));
    }
}


/*
 * - mapping (node => id => node) must be correct
 * - node ids should be in range
 * - basic blocks should not be empty
 */
static void test_2 (void)
{
  int i, num, id;
  KFG_NODE node;
  KFG_NODE_LIST list;


  diagnostic (".");

  num  = kfg_num_nodes (cfg);
  list = kfg_all_nodes (cfg);

  /* walk through the list of all nodes */
  for (i = 0; 0 == kfg_node_list_is_empty (list); i++)
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
      id   = kfg_get_id (cfg, node);

      /* check range of the id */
      if (id < 0 || id >= num)
	{
	  error ("node id out of range: %d not wihtin [0..%d]\n", id, num);
	  return;
	}

      /* check mapping */
      if (node != kfg_get_node (cfg, id))
	{
	  error ("error in mapping from node to id to node: id == %d\n", id);
	  return;
	}

      /* empty node */
      if (0 == kfg_get_bbsize (cfg, node))
	error ("empty basic block found: id == %d\n", id);
    }

  /* number of processed nodes must be equal to number of nodes */
  assert (i == num);
}


/*
 * - the nodes in special lists are subsets of the all-nodes list
 * - all special nodes are members of exactly one special node-list
 * - node-lists contain only nodes of a valid type
 */
static void test_3 (void)
{
  int count = 0, type;
  KFG_NODE node;
  KFG_NODE_LIST list, allnodes, entrys, exits, calls, returns;


  diagnostic (".");

  /* get the various node-lists */
  allnodes = kfg_all_nodes (cfg);
  entrys   = kfg_entrys (cfg);
  exits    = kfg_exits (cfg);
  calls    = kfg_calls (cfg);
  returns  = kfg_returns (cfg);

  /* walk through the all-nodes list */
  for (list = allnodes; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      /* look for duplicates */
      if (1 == node_list_contains (list, node))
	{
	  error ("duplicate nodes in all-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      /* special nodes are also in the special node lists */
      switch (type = (int)kfg_node_type (cfg, node))
	{
	case CALL:
	  if (0 == node_list_contains (calls, node))
	    {
	      error ("call node found which is not member of the call-nodes list: id == %d\n", kfg_get_id (cfg, node));
	      return;
	    }
	  break;

	case RETURN:
	  if (0 == node_list_contains (returns, node))
	    {
	      error ("return node found which is not member of the return-nodes list: id == %d\n", kfg_get_id (cfg, node));
	      return;
	    }
	  break;
	
	case START:
	  if (0 == node_list_contains (entrys, node))
	    {
	      error ("entry node found which is not member of the entry-nodes list: id == %d\n", kfg_get_id (cfg, node));
	      return;
	    }
	  break;
	
	case END:
	  if (0 == node_list_contains (exits, node))
	    {
	      error ("exit node found which is not member of the exit-nodes list: id == %d\n", kfg_get_id (cfg, node));
	      return;
	    }
	  break;

	case INNER:
	  /* do nothing */
	  break;
	
	default:
	  if (0 == count)
	    {
	      error ("node of invalid type found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	      count++;
	    }
	  break;
	}
    }

  /* walk through the entry-nodes list */
  for (list = entrys; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      if (START != (type = (int)kfg_node_type (cfg, node)))
	{
	  error ("node of invalid type in entry-nodes list found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	  return;
	}

      if (0 == node_list_contains (allnodes, node))
	{
	  error ("entry-nodes list is no subset of all-nodes list\n");
	  return;
	}

      if (1 == node_list_contains (list, node))
	{
	  error ("duplicate nodes in entry-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (exits, node))
	{
	  error ("entry node in exit-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (calls, node))
	{
	  error ("entry node in call-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (returns, node))
	{
	  error ("entry node in return-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}
    }

  /* walk through the exit-nodes list */
  for (list = exits; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      if (END != (type = (int)kfg_node_type (cfg, node)))
	{
	  error ("node of invalid type in exit-nodes list found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	  return;
	}

      if (0 == node_list_contains (allnodes, node))
	{
	  error ("exit-nodes list is no subset of all-nodes list\n");
	  return;
	}

      if (1 == node_list_contains (list, node))
	{
	  error ("duplicate nodes in exit-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (entrys, node))
	{
	  error ("exit node in entry-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (calls, node))
	{
	  error ("exit node in call-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (returns, node))
	{
	  error ("exit node in return-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}
    }

  /* walk through the call node list */
  for (list = calls; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      if (CALL != (type = (int)kfg_node_type (cfg, node)))
	{
	  error ("node of invalid type in call-nodes list found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	  return;
	}

      if (0 == node_list_contains (allnodes, node))
	{
	  error ("call-nodes list is no subset of all-nodes list\n");
	  return;
	}

      if (1 == node_list_contains (list, node))
	{
	  error ("duplicate nodes in call-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (entrys, node))
	{
	  error ("call node in entry-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (exits, node))
	{
	  error ("call node in exit-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (returns, node))
	{
	  error ("call node in return-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}
    }

  /* walk through the return node list */
  for (list = returns; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
 
      if (RETURN != (type = (int)kfg_node_type (cfg, node)))
	{
	  error ("node of invalid type in return-nodes list found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	  return;
	}

      if (0 == node_list_contains (allnodes, node))
	{
	  error ("return-nodes list is no subset of all-nodes list\n");
	  return;
	}

      if (1 == node_list_contains (list, node))
	{
	  error ("duplicate nodes in return-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (entrys, node))
	{
	  error ("return node in entry-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (exits, node))
	{
	  error ("return node in exit-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}

      if (1 == node_list_contains (calls, node))
	{
	  error ("return node in call-nodes list found: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}
    }
}


/*
 * - entry/exit and call/return nodes are connected via special references
 * - number of predecessors of a return node is equal to the number of successors of a call node
 * - number of predecessors of a entry node is equal to the number of successors of a exit node
 */
static void test_4 (void)
{
  int type, l1, l2, id1, id2;
  KFG_NODE node, node2, node3;
  KFG_NODE_LIST list, entrys, exits, calls, returns;


  diagnostic (".");

  entrys  = kfg_entrys (cfg);
  exits   = kfg_exits (cfg);
  calls   = kfg_calls (cfg);
  returns = kfg_returns (cfg);

  /* test mapping of all entry nodes */
  for (list = entrys; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      node2 = kfg_get_end (cfg, node);

      if (END != (type = (int)kfg_node_type (cfg, node2)))
	{
	  error ("kfg_get_end returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node2), type);
	  return;
	}

      if (0 == node_list_contains (exits, node2))
	{
	  error ("kfg_get_end returns node which is not element of exit-nodes list: id == %d\n", kfg_get_id (cfg, node2));
	  return;
	}

      node3 = kfg_get_start (cfg, node2);

      if (START != (type = (int)kfg_node_type (cfg, node3)))
	{
	  error ("kfg_get_start returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node3), type);
	  return;
	}

      if (0 == node_list_contains (entrys, node3))
	{
	  error ("kfg_get_start returns node which is not element of entry-nodes list: id == %d\n", kfg_get_id (cfg, node3));
	  return;
	}

      if (node != node3)
	{
	  error ("error in mapping from entry to exit to entry node: id1 == %d, id2 == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, node3));
	  return;
	}

      id1 = kfg_get_id (cfg, node3);
      l1 = kfg_node_list_length (kfg_predecessors (cfg, node3));

      id2 = kfg_get_id (cfg, node2);
      l2 = kfg_node_list_length (kfg_successors (cfg, node2));

      if (l1 != l2)
	{
	  error ("number of predecessors/successors (%d/%d) of entry/exit node (id %d/%d) differs\n", l1, l2, id1, id2);
	  return;
	}
    }

  /* test mapping of all exit nodes */
  for (list = exits; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      node2 = kfg_get_start (cfg, node);

      if (START != (type = (int)kfg_node_type (cfg, node2)))
	{
	  error ("kfg_get_start returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node2), type);
	  return;
	}

      if (0 == node_list_contains (entrys, node2))
	{
	  error ("kfg_get_start returns node which is not element of entry-nodes list: id == %d\n", kfg_get_id (cfg, node2));
	  return;
	}

      node3 = kfg_get_end (cfg, node2);

      if (END != (type = (int)kfg_node_type (cfg, node3)))
	{
	  error ("kfg_get_end returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node3), type);
	  return;
	}

      if (0 == node_list_contains (exits, node3))
	{
	  error ("kfg_get_end returns node which is not element of exit-nodes list: id == %d\n", kfg_get_id (cfg, node3));
	  return;
	}

      if (node != node3)
	{
	  error ("error in mapping from exit to entry to exit node: id1 == %d, id2 == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, node3));
	  return;
	}

      id1 = kfg_get_id (cfg, node2);
      l1 = kfg_node_list_length (kfg_predecessors (cfg, node2));

      id2 = kfg_get_id (cfg, node3);
      l2 = kfg_node_list_length (kfg_successors (cfg, node3));

      if (l1 != l2)
	{
	  error ("number of predecessors/successors (%d/%d) of entry/exit node (id %d/%d) differs\n", l1, l2, id1, id2);
	  return;
	}

    }

  /* test mapping of all call nodes */
  for (list = calls; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      node2 = kfg_get_return (cfg, node);

      if (RETURN != (type = (int)kfg_node_type (cfg, node2)))
	{
	  error ("kfg_get_return returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node2), type);
	  return;
	}

      if (0 == node_list_contains (returns, node2))
	{
	  error ("kfg_get_return returns node which is not element of return-nodes list: id == %d\n", kfg_get_id (cfg, node2));
	  return;
	}

      node3 = kfg_get_call (cfg, node2);

      if (CALL != (type = (int)kfg_node_type (cfg, node3)))
	{
	  error ("kfg_get_call returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node3), type);
	  return;
	}

      if (0 == node_list_contains (calls, node3))
	{
	  error ("kfg_get_call returns node which is not element of call-nodes list: id == %d\n", kfg_get_id (cfg, node3));
	  return;
	}

      if (node != node3)
	{
	  error ("error in mapping from call to return to call node: id1 == %d, id2 == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, node3));
	  return;
	}

      id1 = kfg_get_id (cfg, node3);
      l1 = kfg_node_list_length (kfg_successors (cfg, node3));

      id2 = kfg_get_id (cfg, node2);
      l2 = kfg_node_list_length (kfg_predecessors (cfg, node2));

      if (l1 != l2)
	{
	  error ("number of successors/predecessors (%d/%d) of call/return node (id %d/%d) differs\n", l1, l2, id1, id2);
	  return;
	}
    }

  /* test mapping of all return nodes */
  for (list = returns; 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      node2 = kfg_get_call (cfg, node);

      if (CALL != (type = (int)kfg_node_type (cfg, node2)))
	{
	  error ("kfg_get_call returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node2), type);
	  return;
	}

      if (0 == node_list_contains (calls, node2))
	{
	  error ("kfg_get_call returns node which is not element of call-nodes list: id == %d\n", kfg_get_id (cfg, node2));
	  return;
	}

      node3 = kfg_get_return (cfg, node2);

      if (RETURN != (type = (int)kfg_node_type (cfg, node3)))
	{
	  error ("kfg_get_return returns node of invalid type: id == %d, type == %d\n", kfg_get_id (cfg, node3), type);
	  return;
	}

      if (0 == node_list_contains (returns, node3))
	{
	  error ("kfg_get_return returns node which is not element of return-nodes list: id == %d\n", kfg_get_id (cfg, node3));
	  return;
	}

      if (node != node3)
	{
	  error ("error in mapping from return to call to return node: id1 == %d, id2 == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, node3));
	  return;
	}

      id1 = kfg_get_id (cfg, node2);
      l1 = kfg_node_list_length (kfg_successors (cfg, node2));

      id2 = kfg_get_id (cfg, node3);
      l2 = kfg_node_list_length (kfg_predecessors (cfg, node3));

      if (l1 != l2)
	{
	  error ("number of successors/predecessors (%d/%d) of call/return node (id %d/%d) differs\n", l1, l2, id1, id2);
	  return;
	}
    }
}


/*
 * - all nodes are successor of its predecessors and vice versa
 * - all edges except call/return are intraprocedural edges
 * - all types of forward edges are in range
 */
static void test_5 (void)
{
  int t;
  KFG_NODE node, n;
  KFG_NODE_LIST list, plist, slist;
  
  
  diagnostic (".");

  /* walk through all-nodes list */
  for (list = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
      
      /* walk through the predecessor list of the current node */
      for (plist = kfg_predecessors (cfg, node); 0 == kfg_node_list_is_empty (plist); )
	{
	  n     = kfg_node_list_head (plist);
	  plist = kfg_node_list_tail (plist);
	  
	  /* get list of successors of the predecessor */
	  slist = kfg_successors (cfg, n);
	  
	  if (0 == node_list_contains (slist, node))
	    {
            error ("node which is not a successor of its predecessor found: id == %d, pred-id == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, n));
            return;
	  }
	  
	  /* get type of edge from pre- to node */
	  t = (int)kfg_edge_type (n, node);
	  
	  if (t < 0 || t >= (int)kfg_edge_type_max (cfg) || t > 31)
	    {
	      error ("type of edge out of range: type == %d\n", t);
	      return;
	    }

	  /* look for interprocedural non-call/non-return edges */
	  if (! (CALL == kfg_node_type (cfg, n) && START  == kfg_node_type (cfg, node)) &&
	      ! (END  == kfg_node_type (cfg, n) && RETURN == kfg_node_type (cfg, node)))
	    {
	      if (kfg_procnumnode (cfg, n) != kfg_procnumnode (cfg, node))
		{
		  error ("interprocedural edge found!\n");
		  error_found++;
		  return;
		  
		}
	    }
	}
      
      /* walk through the successor list of the current node */
      for (slist = kfg_successors (cfg, node); 0 == kfg_node_list_is_empty (slist); )
	{
	  n = kfg_node_list_head (slist);
	  slist = kfg_node_list_tail (slist);
	  
	  /* get list of predecessors of the successor */
	  plist = kfg_predecessors (cfg, n);
	  
	  if (0 == node_list_contains (plist, node))
	    {
	      error ("node which is not a predecessor of its successor found: id == %d, succ-id == %d\n", kfg_get_id (cfg, node), kfg_get_id (cfg, n));
	      return;
            }

	  /* get type of edge from node to successor */
	  t = (int)kfg_edge_type (node, n);
	  
	  if (t < 0 || t >= (int)kfg_edge_type_max (cfg) || t > 31)
	    {
	      error ("type of edge out of range: type == %d\n", t);
	      return;
            }

	  /* look for interprocedural non-call/non-return edges */
	  if (! (CALL == kfg_node_type (cfg, node) && START  == kfg_node_type (cfg, n)) &&
	      ! (END  == kfg_node_type (cfg, node) && RETURN == kfg_node_type (cfg, n)))
	    {
	      if (kfg_procnumnode (cfg, n) != kfg_procnumnode (cfg, node))
		{
		  error ("intrerrocedural edge found!\n");
		  error_found++;
		  return;
		}
	    }
	}
    }
}
    

/*
 * recursive travel over the cfg (needed by test_6 ())
 * 
 * - range check for node ids
 * - all nodes found in the graph are contained in the all-nodes list
 */
static void test_6_visit (KFG_NODE node, int direction)
{
  int id;
  KFG_NODE n;
  KFG_NODE_LIST list;

 
  /* get id of current node */
  id = kfg_get_id (cfg, node);

  if (id < 0 || id >= kfg_num_nodes (cfg))
    {
      error ("node id out of range: id == %d\n", id);
      return;
    }

  /* process only not yet visited nodes */
  if (0 == check[id])
    {
      check[id] = 1;

      /* node must be in all-nodes list */ 
      if (0 == node_list_contains (kfg_all_nodes (cfg), node))
	{
	  error ("node in cfg found which is not element of all-nodes list: id == %d\n", kfg_get_id (cfg, node));
	  return;
	}
      
      /* get list of neighbours... */
      if (0 == direction)
	list = kfg_successors (cfg, node);
      else
	list = kfg_predecessors (cfg, node);
      
      /* ...and visit them all */
      while (0 == kfg_node_list_is_empty (list))
	{
	  n    = kfg_node_list_head (list);
	  list = kfg_node_list_tail (list);
	  test_6_visit (n, direction);
	}
    }
}


/*
 * - all nodes are reachable from entry/exit node of the graph
 */
static void test_6 (void)
{
  int i, num, count=0;
  KFG_NODE_LIST list;
  KFG_NODE node;


  diagnostic (".");

  /* create a marker array */
  num   = kfg_num_nodes (cfg);
  check = malloc (sizeof (int) * num);
  assert (NULL != check);

  /* forward direction */
  for (i = 0; i < num; i++)
    check[i] = 0;
  
  for (list = kfg_entrys (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
	  
      test_6_visit (node, 0);
    }

  /* all nodes must be visited */
  for (i = 0; i < num; i++)
    if (0 == check[i] && 0 == count)
      {
	error ("unreachable node (forward direction) in cfg found: id == %d (in procedure %d)\n", i, kfg_procnumnode (cfg, kfg_get_node(cfg, i)));
	count++;
      }

  /* backward direction */
  for (i = 0; i < num; i++)
    check[i] = 0;

  for (list = kfg_exits (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
	  
      test_6_visit (node, 1);
    }

  /* all nodes must be visited */
  for (i = 0; i < num; i++)
    if (0 == check[i] && 0 == count)
      {
	error ("unreachable node (backward direction) in cfg found: id == %d (in procedure %d)\n", i, kfg_procnumnode (cfg, kfg_get_node (cfg, i)));
	count++;
      }

  /* delete marker array */
  free (check);
}


/*
 * - range check for procedure numbers at every node
 * - mapping of procedure i to correct entry node
 * - a entry node for each procedure does exist
 */
static void test_7 (void)
{
  int num, max, i;
  KFG_NODE node;
  KFG_NODE_LIST list;


  diagnostic (".");

  /* create a marker arrey */
  max   = kfg_num_procs (cfg);
  check = malloc (sizeof (int)*max);
  assert (NULL != check);

  for (i = 0; i < max; i++)
    check[i] = 0;

  /* range check procedure numbers */ 
  for (list = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      num = kfg_procnumnode (cfg, node);

      if (num < 0 || num >= max)
	{
	  error ("procedure number of node out of range: procedure-number == %d\n", num);
	  return;
	}
    }

  /* test mapping of procedure i => node */
  for (i = 0, list = kfg_entrys (cfg); 0 == kfg_node_list_is_empty (list); i++)
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
      num  = kfg_procnumnode (cfg, node);

      /* duplicate procedure numbers */
      if (1 == check[num])
	{
	  error ("multiple entry nodes with equal procedure number found: procedure-number == %d\n", num);
	  return;
	}

      check[num] = 1;

      /* mapping procnum -> entry node */
      if (node != kfg_numproc (cfg, num))
	{
	  error ("error in mapping from procedure number to entry node found: procedure-number == %d\n", num);
	  return;
	}
    }

  /* all procedures must exist */
  for (i = 0; i < max; i++)
    if (0 == check[i])
      {
	error ("no entry-node found for procedure: procedure-number == %d\n", i);
	return;
      }
  
  /* free marker array */
  free (check);
}


/*
 * - test for multiple edges from node a to b and b to a
 */
static void test_8 (void)
{
  KFG_NODE n1, n2;
  KFG_NODE_LIST l1, l2;


  diagnostic (".");

  /* walk through all nodes-list */
  for (l1 = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* walk through list of successors */
      for (l2 = kfg_successors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  if (1 == node_list_contains (l2, n2))
	    {
	      error ("multiple forward edge to successor found: id1 == %d, id2 == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2));
	      return;
	    }
	}

      for (l2 = kfg_predecessors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);
	  
	  if (1 == node_list_contains (l2, n2))
	    {
	      error ("multiple backward edge to predecessor found: id1 == %d, id2 == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2));
	      return;
	    }
	}
    }
}


/*
 * - test interprocedural consistency
 */
static void test_9 (void)
{
  int type;
  KFG_NODE n1, n2, n3;
  KFG_NODE_LIST l1, l2, l3;


  diagnostic (".");

  /* walk through list of call-nodes */
  for (l1 = kfg_calls (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* successors have to be return or entry nodes */
      for (l2 = kfg_successors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  switch ((type = (int)kfg_node_type (cfg, n2)))
	    {
	    case RETURN:
	      if (n2 != kfg_get_return (cfg, n1))
		{
		  error ("call node with more than one successor of type 'return' found: id == %d\n", kfg_get_id (cfg, n1));
		  return;
		}

	      if (0 != (type = (int)kfg_edge_type (n1, n2)))
		{
		  error ("local edge with invalid type found: type == %d\n", type);
		  return;
		}
	      break;

	    case START:
	      break;

	    default:
	      error ("invalid call-node successor: call-id == %d, succ-id == %d, type == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2), type);
	      return;
	      break;
	    }
	}
    }

  /* walk through list of return nodes */
  for (l1 = kfg_returns (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* predecessors have to be call or exit nodes */
      for (l2 = kfg_predecessors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  switch ((type = (int)kfg_node_type (cfg, n2)))
	    {
	    case CALL:
	      assert (n2 == kfg_get_call (cfg, n1));
	      assert (0 == kfg_edge_type (n2, n1));

	      if (n2 != kfg_get_call (cfg, n1))
		{
		  error ("return node with more than one predecessor of type 'call' found: id == %d\n", kfg_get_id (cfg, n1));
		  return;
		}

	      if (0 != (type = (int)kfg_edge_type (n2, n1)))
		{
		  error ("local edge with invalid type found: type == %d\n", type);
		  return;
		}
	      break;

	    case END:
	      break;

	    default:
	      error ("invalid return-node predecessor: return-id == %d, pred-id == %d, type == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2), type);
	      error_found++;
	      return;
	      break;
	    }
	}
    }

  /* walk through list of entry nodes */
  for (l1 = kfg_entrys (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* predecessors have to be call nodes */
      for (l2 = kfg_predecessors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  if (CALL != (type = (int)kfg_node_type (cfg, n2)))
	    {
	      error ("invalid entry node predecessor: entry-id == %d, pred-id == %d, type == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2), type);
	      return;
	    }
	}
    }

  /* walk through list of exit nodes */
  for (l1 = kfg_exits (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* successors have to be return nodes */
      for (l2 = kfg_successors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  if (RETURN != (type = (int)kfg_node_type (cfg, n2)))
	    {
	      error ("invalid exit node successor: exit-id == %d, succ-id == %d, type == %d\n", kfg_get_id (cfg, n1), kfg_get_id (cfg, n2), type);
	      return;
	    }
	}
    }

  /*
   *  test chain call -> start -> end -> return
   */

  /* walk through list of call-nodes */
  for (l1 = kfg_calls (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* walk through list if call successors */
      for (l2 = kfg_successors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  if (START == kfg_node_type (cfg, n2))
	    {
	      n3 = kfg_get_end (cfg, n2);
	      l3 = kfg_successors (cfg, n3);

	      if (0 == node_list_contains (l3, kfg_get_return (cfg, n1)))
		{
		  error ("interprocedural chain error: call (%d) => start (%d) => end (%d) => return (%d)\n",
			 kfg_get_id (cfg, n1),
			 kfg_get_id (cfg, n2),
			 kfg_get_id (cfg, n3),
			 kfg_get_id (cfg, kfg_get_return (cfg, n1)));
		  return;
		}
	    }
	}
    }

  /*
   *  test chain return -> end -> start -> call
   */

  /* walk through list of return-nodes */
  for (l1 = kfg_returns (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* walk through list if return predecessors */
      for (l2 = kfg_predecessors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2 = kfg_node_list_head (l2);
	  l2 = kfg_node_list_tail (l2);

	  if (END == kfg_node_type (cfg, n2))
	    {
	      n3 = kfg_get_start (cfg, n2);
	      l3 = kfg_predecessors (cfg, n3);

	      if (0 == node_list_contains (l3, kfg_get_call (cfg, n1)))
		{
		  error ("interprocedural chain error: return (%d) => end (%d) => start (%d) => call (%d)\n",
			 kfg_get_id (cfg, n1),
			 kfg_get_id (cfg, n2),
			 kfg_get_id (cfg, n3),
			 kfg_get_id (cfg, kfg_get_return (cfg, n1)));
		  return;
		}
	    }
	}
    }
}


/*
 * - test node types
 */
static void test_10 (void)
{
  KFG_NODE node;
  KFG_NODE_LIST list;
  int count = 0;
  int type;


  diagnostic (".");

  /* walk through list of all nodes */
  for (list = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);

      /* test node type */
      switch ((type = (int)kfg_node_type (cfg, node)))
	{
	case START:
	case END:
	case CALL:
	case RETURN:
	case INNER:
	  break;

	default:
	  if (count==0)
	    {
	      error ("node of invalid type found: id == %d, type == %d\n", kfg_get_id (cfg, node), type);
	      warning_found++;
	      count++;
	    }
	  break;
	}
    }
}


/*
 * recursive travel over the cfg (needed by test_11 ())
 * - all nodes in the graph are reachable from the entry/exit node of the corresponding procedure
 */
static int test_11_visit (KFG_NODE current, KFG_NODE node, int direction)
{
  int id;
  KFG_NODE n;
  KFG_NODE_LIST list;


  /* node found ? */
  if (current == node)
    return 1;

  /* get id of current node */
  id = kfg_get_id (cfg, current);

  /* process only not yet visited nodes */
  if (0 == check[id])
    {
      /* set marker */
      check[id] = 1;

      /* get list of neighbours... */
      if (0 == direction)
	list = kfg_successors (cfg, current);
      else
	list = kfg_predecessors (cfg, current);

      /* ...and visit them all */
      while (0 == kfg_node_list_is_empty (list))
	{
	  n    = kfg_node_list_head (list);
	  list = kfg_node_list_tail (list);

	  if (1 == test_11_visit (n, node, direction))
	    return 1;
	}
    }

  /* not reached */
  return 0;
}


/*
 * - all nodes of a procedure are reachable from entry/exit node of the corresponding procedure
 */
static void test_11 (void)
{
  int i, procnum, num, count = 0;
  KFG_NODE_LIST list;
  KFG_NODE node;


  diagnostic (".");

  /* create a marker array */
  num   = kfg_num_nodes (cfg);
  check = malloc (sizeof (int) * num);
  assert (NULL != check);

  /* walk through all-nodes list */
  for (list = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (list); )
    {
      node    = kfg_node_list_head (list);
      list    = kfg_node_list_tail (list);
      procnum = kfg_procnumnode (cfg, node);

      for (i = 0; i < num; i++)
	check[i] = (procnum != kfg_procnum (cfg, i));

      /* node must be reachable (forward direction) */
      if (0 == test_11_visit (kfg_numproc (cfg, procnum), node, 0))
	if (count == 0)
	  {
	    error ("node which is unreachable from its procedure entry node found: id == %d, procedure-number == %d\n", kfg_get_id (cfg, node), procnum);
	    count++;
	  }

      for (i = 0; i < num; i++)
	check[i] = (procnum != kfg_procnum (cfg, i));

      /* node must be reachable (backward direction) */
      if (0 == test_11_visit (kfg_get_end (cfg, kfg_numproc (cfg, procnum)), node, 1))
	if (count == 0)
	  {
	    error ("node which is unreachable from its procedure exit node found: id == %d, procedure-number == %d\n", kfg_get_id (cfg, node), procnum);
	    count++;
	  }
    }

  /* delete marker array */
  free (check);
}


/*
 * - test in/out edge-bitmask
 */
static void test_12 (void)
{
  int i, mask, type;
  KFG_NODE_LIST l1, l2;
  KFG_NODE n1, n2;


  diagnostic (".");

  /* walk through all-nodes list */
  for (l1 = kfg_all_nodes (cfg); 0 == kfg_node_list_is_empty (l1); )
    {
      n1 = kfg_node_list_head (l1);
      l1 = kfg_node_list_tail (l1);

      /* compute input-edge-mask */
      for (mask = 0, l2 = kfg_predecessors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2   = kfg_node_list_head (l2);
	  l2   = kfg_node_list_tail (l2);
	  type = (int)kfg_edge_type (n2, n1);

	  /* edge type must be valid */
	  if (type < 0 || type > 31 || type >= (int)kfg_edge_type_max (cfg))
	    {
	      error ("edge with invalid type found: type == %d\n", type);
	      return;
	    }

	  /* set bit in mask */
	  mask |= 1 << type;
	}

      /* compare mask with computed mask */
      if (mask != kfg_which_in_edges (n1))
	{
	  error ("node with wrong input edge-mask found: id == %d, mask == 0x%X/0x%X\n", kfg_get_id (cfg, n1), mask, kfg_which_in_edges (n1));
	  return;
	}

      /* compute output-edge-mask */
      for (mask = 0, l2 = kfg_successors (cfg, n1); 0 == kfg_node_list_is_empty (l2); )
	{
	  n2   = kfg_node_list_head (l2);
	  l2   = kfg_node_list_tail (l2);
	  type = (int)kfg_edge_type (n1, n2);

	  /* edge type must be valid */
	  if (type < 0 || type > 31 || type >= (int)kfg_edge_type_max (cfg))
	    {
	      error ("edge with invalid type found: type == %d\n", type);
	      return;
	    }

	  /* set bit in mask */
	  mask |= 1 << type;
	}

      /* compare mask with computed mask */
      if (mask != kfg_which_out_edges (n1))
	{
	  error ("node with wrong output edge-mask found: id == %d, mask == 0x%X/0x%X\n", kfg_get_id (cfg, n1), mask, kfg_which_out_edges (n1));
	  return;
	}
    }
}


/*
 * - look for unprintable characters
 */
static int test_13_printable (FILE *file)
{
  int bslashed = 0;


  while (1)
    {
      /* read char and handle them */
      switch (fgetc (file))
	{  
	case EOF:
	  /* end of file */
	  if (feof (file))
	    return 1;
	  else
	    {
	      error ("error reading temporary file\n");
	      return -1;
	    }
	  break;
 
	case '\\':
	  /* new backslash character found */
	  if (bslashed == 0)
	    bslashed = 1;

	  /* previous backslash now consumed */
	  else
	    bslashed = 0;
	  break;

	case '"':
	  /* not bslashed => error */
	  if (bslashed == 0)
	    {
	      error ("double quote in label found\n");
	      return 0;
	    }

	  /* backslash now consumed */
	  bslashed = 0;
	  break;

	default:
	  /* previous backslash now consumed */
	  bslashed = 0;
	  break;
	}
    }

  /* never readched */
  assert (0);
}


/*
 * - test kfg_proc_name and kfg_node_infolabel_print_fp
 */
static void test_13 (void)
{
  int i;
  char *name;
  KFG_NODE_LIST list;
  KFG_NODE node;
  FILE *file;



  /* open temporary file */
  if (NULL == (file = tmpfile ()))
    {
      error ("cannot open temporary file\n");
      return;
    }
  
  /* walk through all-nodes list */
  for (list = kfg_all_nodes (cfg); error_found == 0 && 0 == kfg_node_list_is_empty (list); )
    {
      node = kfg_node_list_head (list);
      list = kfg_node_list_tail (list);
      
      rewind (file);
      ftruncate (fileno (file), 0);
      
      for (i = 0; (i == 0) || i < kfg_get_bbsize (cfg, node); i++)
	{
	  /* print infolabel */
	  kfg_node_infolabel_print_fp (file, cfg, node, i);
	  
	  rewind (file);
	  
	  /* test label */
	  if (0 == test_13_printable (file))
	    return;
	  
	  rewind (file);
	  ftruncate (fileno (file), 0);	  
	}
    }

  /* for all procedures */
  for (i = 0; error_found == 0 && error_found == 0 && i < kfg_num_procs (cfg); i++)
    {
      /* is the name null */
      if (NULL == (name = kfg_proc_name (cfg, i)))
	{
	  error ("empty procedure name found: procedure-number == %d\n", i);
	  return;
	}

      /* seek to beginning and truncate file */
      rewind (file);
      ftruncate (fileno (file), 0);

      /* test printability */
      fputs (name, file); 

      /* seek to beginning and test infolabel */
      rewind (file);

      if (0 ==  test_13_printable (file))
	return;
    }

  /* close file */
  fclose (file);
}


/*
 * write a gdl-file with the control flow graph
 */
static void writegdl (const char *name)
{
  KFG_NODE node, n2;
  KFG_NODE_LIST list;
  FILE *file;
  int i, j, k;


  /* filename defined ? */
  if (NULL == name)
    return;

  /* open gdl-file */
  if (NULL == (file = fopen (name, "wt")))
    {
      error ("cannot write file '%s'\n", name);
      return;
    }

  /* write header */
  (void)fprintf (file, "graph: {\n"               \
		 "  display_edge_labels: yes\n"   \
		 "  manhattan_edges: yes\n"       \
		 "  layout_algorithm:maxdepth\n"  \
		 "  straight_phase:yes\n"         \
		 "  fstraight_phase:yes\n"        \
		 "  priority_phase:yes\n"         \
		 "  linear_segments:yes\n"        \
		 "  port_sharing:no\n"            \
		 "  subgraph_labels:no\n\n");

  /* write all procedures */
  for (i = 0; i < kfg_num_procs (cfg); i++)
    {
      /* begin subgraph */
      (void)fprintf (file, "  graph: {\n"       \
		     "    status: clustered\n"  \
		     "    color: lightyellow\n" \
		     "    title: \"%s\"\n\n", kfg_proc_name (cfg, i));
      
      /* write all nodes of the procedure */
      for (j = 0; j < kfg_num_nodes (cfg); j++) 
	{
	  node = kfg_get_node (cfg, (KFG_NODE_ID)j);

	  /* is the node in this procedure */
	  if (i == kfg_procnumnode (cfg, node))
	    {
	      (void)fprintf (file, "    node: {\n"   \
			     "      title: \"%d\"\n" \
			     "      label: \"%d %d\n", j, j, i);


	      for (k = 0; (k == 0) || k < kfg_get_bbsize (cfg, node); k++)
		{
		  kfg_node_infolabel_print_fp (file, cfg, node, k);
		  (void)fprintf (file, "\n");
		}

	      (void)fprintf (file, " \"\n"                \
			     "      bordercolor: black\n" \
			     "      }\n");

	      /* write all edges */
	      for (list = kfg_successors (cfg, node); 0 == kfg_node_list_is_empty (list); )
		{
		  n2 = kfg_node_list_head (list);
		  list = kfg_node_list_tail (list);

		  /* write edge */
		  (void)fprintf (file, "    edge: {\n"      \
				 "      sourcename: \"%d\"" \
				 "      targetname: \"%d\"" \
				 "      color: black"       \
				 "      }", j, kfg_get_id (cfg, n2));
		}

	      /* write all back-edges */
	      for (list = kfg_predecessors (cfg, node); 0 == kfg_node_list_is_empty (list); )
		{
		  n2 = kfg_node_list_head (list);
		  list = kfg_node_list_tail (list);

		  /* write edge */
		  (void)fprintf (file, "    edge: {\n"       \
				 "      sourcename: \"%d\""  \
				 "      targetname: \"%d\""  \
				 "      linestyle: dotted\n" \
				 "      color: red"          \
				 "      }", j, kfg_get_id (cfg, n2));
		}
	    }
	}

      /* close subgraph */
      (void)fprintf (file, "  }\n");
    }

  /* write footer */
  (void)fprintf (file, "}\n");

  /* close the file */
  fclose(file);
} 

/*
 * MS 2011
 * write a dot-file with the control flow graph
 */
static void writedot (const char *name)
{
  KFG_NODE node, n2;
  KFG_NODE_LIST list;
  FILE *file;
  int i, j, k;
  
  /* filename defined ? */
  if (NULL == name) {
    error ("writedot: no file name provided.\n");
    return;
  }
  /* open gdl-file */
  if (NULL == (file = fopen (name, "wt"))) {
    error ("writedot: cannot write file '%s'.\n", name);
    return;
  }
  
  /* write header */
  (void)fprintf (file, "digraph Program {\n");
  
  // BLOCK1: write all clusters and their respective nodes
  /* write all procedures */
  for (i = 0; i < kfg_num_procs (cfg); i++) {
    /* begin subgraph */
    char* function_name=kfg_proc_name(cfg, i);
    (void)fprintf (file, " subgraph cluster_%s {\n",function_name); 
    (void)fprintf (file, " style=filled;\n color=lightgrey;\n");
    (void)fprintf (file, " label=\"%s\";\n",function_name); 
    (void)fprintf (file, " node [style=filled, color=green];\n");
    
    /* write all nodes of the procedure */
    for (j = 0; j < kfg_num_nodes (cfg); j++) {
      node = kfg_get_node (cfg, (KFG_NODE_ID)j);
	  
      /* is the node in this procedure */
      if (i == kfg_procnumnode (cfg, node)) {
		/* print node text */
		(void)fprintf (file, "L%d [label=\"",j);
		(void)fprintf (file, "%d: ",j); 
		kfg_node_infolabel_print_fp(file, cfg, node, 0);
		(void)fprintf (file, "\"];\n");	  
		
		//(void)fprintf (file, "    node: {\n"		\
		//	       "      title: \"%d\"\n"			\
		//	       "      label: \"%d %d\n", j, j, i);
		
		//  for (k = 0; (k == 0) || k < kfg_get_bbsize (cfg, node); k++)
		//	{
		//	  kfg_node_infolabel_print_fp (file, cfg, node, k);
		//	  (void)fprintf (file, "\n");
		//	}
      }
    }
	
    /* close subgraph */
    (void)fprintf (file, "  }\n");
  }

  // BLOCK2: write all edges (inside clusters and between clusters)
  /* write all procedures */
  /* iterate over all nodes */
  for (j = 0; j < kfg_num_nodes (cfg); j++) {
	node = kfg_get_node (cfg, (KFG_NODE_ID)j);
	
	/* write all outgoing edges */
	for (list = kfg_successors (cfg, node); 0 == kfg_node_list_is_empty (list); ) {
	  n2 = kfg_node_list_head (list);
	  list = kfg_node_list_tail (list);
	  (void)fprintf (file, "L%d -> L%d [color=black];\n",j,kfg_get_id (cfg, n2));
	}
	
#if 0
	// dot not write back-edges (no option yet)
	/* write all back-edges */
	for (list = kfg_predecessors (cfg, node); 0 == kfg_node_list_is_empty (list); )  	{
	  n2 = kfg_node_list_head (list);
	  list = kfg_node_list_tail (list);
	  (void)fprintf (file, "L%d -> L%d [color=red];\n",j,kfg_get_id (cfg, n2));	    
	}
#endif
  }
  
  
  /* write footer */
  (void)fprintf (file, "}\n");
  
  /* close the file */
  fclose(file);
} 

/* The main testfunction. Returns 0 if any error is found in the cfg */
int kfg_testit (KFG kfg, int quiet_mode)
{
  cfg           = kfg;
  quiet         = (quiet_mode == 1);
  error_found   = 0;

  diagnostic ("Consistency test of cfg and PAG-interface ");

  /* do the tests */
  if (0 != kfg_num_nodes (cfg))
    {
      if (error_found < 100) test_1 ();
      if (error_found < 100) test_2 ();
      if (error_found < 100) test_3 ();
      if (error_found < 100) test_4 ();
      if (error_found < 100) test_5 ();
      if (error_found < 100) test_6 ();
      if (error_found < 100) test_7 ();
      if (error_found < 100) test_8 ();
      if (error_found < 100) test_9 ();
      if (error_found < 100) test_10 ();
      if (error_found < 100) test_11 ();
      if (error_found < 100) test_12 ();
      if (error_found < 100) test_13 ();
    }

  /* tests for empty cfg's */
  else test_0 ();

  if (error_found == 0)
    diagnostic (" excellent!\n");
  else
    diagnostic (" ** errors found **\n");

  return (error_found == 0);
}

void outputGdlIcfg(KFG kfg, const char *gdl_name) {
  cfg=kfg;
  writegdl (gdl_name);
}

void outputDotIcfg(KFG kfg, const char *dot_name) {
  cfg=kfg;
  writedot (dot_name);
}

#endif /* ifdef NDEBUG */
