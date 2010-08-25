// $Id: CFG.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
// -*-C++-*-
// * BeginRiceCopyright *****************************************************
// 
// Copyright ((c)) 2002, Rice University 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of Rice University (RICE) nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// This software is provided by RICE and contributors "as is" and any
// express or implied warranties, including, but not limited to, the
// implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall RICE or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or
// consequential damages (including, but not limited to, procurement of
// substitute goods or services; loss of use, data, or profits; or
// business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence
// or otherwise) arising in any way out of the use of this software, even
// if advised of the possibility of such damage. 
// 
// ******************************************************* EndRiceCopyright *

// Best seen in 120-column wide window (or print in landscape mode).
//--------------------------------------------------------------------------------------------------------------------
// This file is part of Mint.
// Arun Chauhan (achauhan@cs.rice.edu), Dept of Computer Science, Rice University, 2001.
//--------------------------------------------------------------------------------------------------------------------

// standard headers

#ifdef NO_STD_CHEADERS
# include <stdlib.h>
# include <string.h>
# include <assert.h>
#else
# include <cstdlib>
# include <cstring>
# include <cassert>
using namespace std; // For compatibility with non-std C headers
#endif

#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;

// Mint headers
#include "CFG.h"

unsigned int CFG::Node::label_count = 0; // First node will be numbered 1;

//--------------------------------------------------------------------------------------------------------------------
CFG::CFG (IRInterface &_ir, IRStmtIterator *stmt_iterptr, SymHandle _name, bool return_statements_allowed) 
  : ir(_ir)
{
  name = _name;

  // Create the unique entry node.
  entry = new CFG::Node();
  add(entry);
  // Create the actual start node where the first statements wil be added.
  CFG::Node* r = new CFG::Node();
  add(r);
  connect (entry, r, FALLTHROUGH_EDGE);

  CFG::NodeLabelList exit_nodes;
  CFG::NodeLabelList return_nodes;
  if (return_statements_allowed)
    build_block(r, stmt_iterptr, exit_nodes, 0, &return_nodes, 0 /* continue nodes */);
  else
    build_block(r, stmt_iterptr, exit_nodes, 0, 0, 0 /* continue nodes */);
  CFG::Node* final = new CFG::Node();  // final node is the exit node of the sub-program
  add(final);
  exit = final;
  connect(exit_nodes, final); // connect the exit nodes to the final node
  connect(return_nodes, final); // return nodes also lead to the exit node of the sub-program

  // finalize control flow for unstructured constructs
#if 1
  HandleDelayedBranches();
#endif
}

CFG::~CFG()
{
  non_locals.clear();
  def_blocks_set.clear(); 
  entry = NULL;
  exit = NULL;
  label_to_node_map.clear();
  the_worklist = NULL; 
  fallThrough.clear();
}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
CFG::Edge::Edge (CFG::Node* n1, CFG::Node* n2, EdgeType _type, ExprHandle _expr) 
  : DGraph::Edge(n1, n2)
{
  type = _type;
  expr = _expr;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
static const char *edgeTypeToString[] = {
  "TRUE_EDGE", "FALLTHROUGH_EDGE", "FALSE_EDGE",
  "BACK_EDGE", "MULTIWAY_EDGE", "BREAK_EDGE",
  "CONTINUE_EDGE", "RETURN_EDGE" 
};

void CFG::Edge::dump (ostream& os)
{
  os << edgeTypeToString[type];
}
//--------------------------------------------------------------------------------------------------------------------


IRStmtType
CFG::build_block (CFG::Node* prev_node, IRStmtIterator *si_ptr, CFG::NodeLabelList& exit_nodes,
                  CFG::NodeLabelList* break_nodes, CFG::NodeLabelList* return_nodes,
                  CFG::NodeLabelList* continue_nodes)
{
  NodeLabelList x_nodes;
  IRStmtType prev_statement = SIMPLE;
  while (si_ptr->IsValid()) {
    StmtHandle stmt = si_ptr->Current();
    if (ir.GetLabel(stmt)) {
      // FIXME: Unstructured handling (labels), partially tested.
      // If this statement has a label, it begins a new block (if prev_node
      // is empty, we don't need to create another new block).  The new block
      // will have already been created if the label was referenced by a
      // previously encountered statement.
      StmtLabel lab = ir.GetLabel(stmt);
      CFG::Node *new_node; 
      if (label_to_node_map.find(lab) != label_to_node_map.end()) {
        new_node = label_to_node_map[lab];
      }
      else if (! prev_node->empty()) {
        new_node = new CFG::Node();
        add(new_node);
        label_to_node_map[lab] = new_node; 
      }
      else {
        label_to_node_map[lab] = prev_node; 
        new_node = prev_node;
      }
        
      if (prev_statement != SIMPLE && new_node != prev_node)
        connect(x_nodes, new_node);
      else if (prev_node && new_node != prev_node)
        connect(prev_node, new_node, FALLTHROUGH_EDGE);
      x_nodes.clear();
      prev_node = new_node;
    } else
    if (prev_statement != SIMPLE) {
      // now we need to create a new CFG node
      prev_node = new CFG::Node();
      add(prev_node);
      connect(x_nodes, prev_node);
      x_nodes.clear();
    }
    prev_statement = build_stmt(prev_node, stmt, x_nodes, break_nodes, return_nodes, continue_nodes);
    IRStmtIterator &tmp = *si_ptr;
    ++tmp;
//    ++(*si_ptr);
  }

  if (prev_statement == SIMPLE)
    exit_nodes.push_back(NodeLabel(prev_node, FALLTHROUGH_EDGE));
  else {
    NodeLabelListIterator x_nodes_iter(&x_nodes);
    while ((bool)x_nodes_iter) {
      exit_nodes.push_back((NodeLabel)x_nodes_iter);
      ++x_nodes_iter;
    }
  }

  return NONE;
}


//--------------------------------------------------------------------------------------------------------------------
/** Build a CFG for the IR rooted at the given node.  The CFG builder depends upon the IR providing a minimal
    interface.  In particular, the CFG needs for the IR nodes to be able to categorize themselves as simple
    statements, statement lists, loops, two-way branches, and multi-way branches.

    The routine uses the given node prev_node as the entry node and updates the exit_nodes for the CFG that it builds.
    Notice that for every CFG component built by this call, there is exactly one entry node, but there may be multiple
    exit nodes.
*/
IRStmtType
CFG::build_stmt (CFG::Node* prev_node, StmtHandle stmt, CFG::NodeLabelList& exit_nodes, CFG::NodeLabelList* break_nodes,
		CFG::NodeLabelList* return_nodes, CFG::NodeLabelList* continue_nodes)
  throw (CFG::Unexpected_Break, CFG::Unexpected_Return, CFG::Unexpected_Continue)
{
    // handle different types of nodes. 
    switch (ir.GetStmtType(stmt)) {
    case SIMPLE:
      prev_node->add(stmt);
      return SIMPLE;
    case COMPOUND:
      build_block(prev_node, ir.GetFirstInCompound(stmt), exit_nodes, break_nodes, return_nodes, continue_nodes);
      return COMPOUND;
    case BREAK:
      prev_node->add(stmt);
      if (break_nodes == 0)
	throw CFG::Unexpected_Break();
      else
	break_nodes->push_back(NodeLabel(prev_node, BREAK_EDGE));
      return BREAK;
    case RETURN:
      prev_node->add(stmt);
      if (return_nodes == 0)
	throw CFG::Unexpected_Return();
      else
	return_nodes->push_back(NodeLabel(prev_node, RETURN_EDGE));
      return RETURN;
    case LOOP:
      return build_CFG_loop (prev_node, stmt, exit_nodes, return_nodes);
    case STRUCT_TWOWAY_CONDITIONAL:
      return build_CFG_twoway_branch (prev_node, stmt, exit_nodes, break_nodes,
				      return_nodes, continue_nodes);
    case STRUCT_MULTIWAY_CONDITIONAL:
      if (ir.IsBreakImplied(stmt))
        return build_CFG_multiway_branch (prev_node, stmt, exit_nodes, break_nodes,
					  return_nodes, continue_nodes);
      else
        return build_CFG_multiway_branch_with_fallthrough (prev_node, stmt, exit_nodes,
					  return_nodes, continue_nodes);
    case END_TESTED_LOOP:
      return build_CFG_end_tested_loop (prev_node, stmt, exit_nodes, return_nodes);
    case LOOP_CONTINUE:
      prev_node->add(stmt);
      if (continue_nodes == 0)
        throw CFG::Unexpected_Continue();
      else
        continue_nodes->push_back(NodeLabel(prev_node, CONTINUE_EDGE));
      return LOOP_CONTINUE;
    //
    // These unstructured constructs have the possibility of being
    // delayed branches (depending on the underlying machine). Those
    // without delay slots are handled now.  Those with delay slots
    // are just added as simple statements.  A post-processing step
    // will finalize control flow for them.
    //
    case UNCONDITIONAL_JUMP:
      if (ir.NumberOfDelaySlots(stmt) == 0) {
        return build_CFG_unconditional_jump (prev_node, stmt);
      } else { 
        prev_node->add(stmt);
        return SIMPLE;
      }
    case USTRUCT_TWOWAY_CONDITIONAL_T:
    case USTRUCT_TWOWAY_CONDITIONAL_F:
      if (ir.NumberOfDelaySlots(stmt) == 0) {
        return build_CFG_ustruct_twoway_branch (prev_node, stmt, exit_nodes);
      } else {
        prev_node->add(stmt);
        return SIMPLE;
      }
    case UNCONDITIONAL_JUMP_I:
      if (ir.NumberOfDelaySlots(stmt) == 0) {
        return build_CFG_unconditional_jump_i (prev_node, stmt);
      } else {
        prev_node->add(stmt);
        return SIMPLE;
      }
    case USTRUCT_MULTIWAY_CONDITIONAL:
      // Currently assume multiways don't have delay slots.
      assert(ir.NumberOfDelaySlots(stmt) == 0);
      return build_CFG_ustruct_multiway_branch (prev_node, stmt);

    // FIXME: Unimplemented.
    case ALTERNATE_PROC_ENTRY:
      cout << "FIXME: ALTERNATE_PROC_ENTRY not yet implemented" << endl;
      assert (0);
      return ALTERNATE_PROC_ENTRY;
    default:
      assert (0);
    }
    assert (0); // eraxxon: eliminate compiler warnings about missing return 
    return (NONE);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** The general structure of a top-tested loop CFG (e.g., FORTRAN DO statement, C for statement,
    while statements, etc.) is as below:
    <PRE>
              |
              V
          header (optional)
              |      _______
              |     /       \  
              V    V         |
           condition         | 
     ________/|              |
    /         |              |
    |         V              |
    |       body             |
    |         |              |
    |         |              |
    |         V              |
    |   increment (optional) |
    |         \_____________/
    |
     \________
              |
              V
    </PRE>
*/
IRStmtType
CFG::build_CFG_loop (CFG::Node* prev_node, StmtHandle th, CFG::NodeLabelList& exit_nodes,
                     CFG::NodeLabelList* return_nodes)
{
  // If there is a header, (as in a "for" loop), it is assimilated in the previous (prev_node) CFG node.  The
  // condition needs a separate CFG node.  So, unless prev_node is empty (in which case that can become the condition
  // node) a new CFG node must be allocated for the loop condition.
  CFG::Node* c;

  StmtHandle header = ir.LoopHeader(th);
  if (header) {
    prev_node->add(header);
  }

  // To handle C and Fortran loops in the simplest way we take the following
  // measures:
  //
  // If the number of loop iterations is defined
  // at loop entry (i.e. Fortran semantics), we add the loop statement
  // representative to the header node so that definitions from inside
  // the loop don't reach the condition and increment specifications
  // in the loop statement.
  //
  // On the other hand, if the number of iterations is not defined at
  // entry (i.e. C semantics), we add the loop statement to a node that
  // is inside the loop in the CFG so definitions inside the loop will 
  // reach uses in the conditional test. for C style semantics, the 
  // increment itself may be a separate statement. if so, it will appear
  // explicitly at the bottom of the loop. 
  bool definedAtEntry =  ir.LoopIterationsDefinedAtEntry(th);
  if (definedAtEntry) {
    prev_node->add(th); 
  }
  if (prev_node->empty()) {
    prev_node->add(th); 
    c = prev_node;
  }
  else {
    c = new CFG::Node(th);
    connect(prev_node, c, FALLTHROUGH_EDGE);
  }
  exit_nodes.push_back(NodeLabel(c, FALSE_EDGE));

  // allocate a new CFG node for the loop body and recur
  CFG::NodeLabelList x_nodes;
  IRStmtIterator *body = ir.LoopBody(th);
  if (body->IsValid() && body->Current()) {
    Node* b = new CFG::Node();
    add(b);
    NodeLabelList break_statements;
    NodeLabelList continue_statements;
    if (build_block(b, body, x_nodes, &break_statements, return_nodes, &continue_statements) == SIMPLE)
      x_nodes.push_back(NodeLabel(b, FALLTHROUGH_EDGE));

    connect(c, b, TRUE_EDGE);
    NodeLabelListIterator break_iter(&break_statements);
    while ((bool)break_iter) {
      exit_nodes.push_back((NodeLabel)break_iter);
      ++break_iter;
    }

    NodeLabelListIterator continue_iter(&continue_statements);
    while ((bool)continue_iter) {
      x_nodes.push_back((NodeLabel)continue_iter);
      ++continue_iter;
    }
  }
  else // empty body
    x_nodes.push_back(NodeLabel(c, TRUE_EDGE));

  // allocate a new CFG node for the increment part, if needed
  StmtHandle incr = ir.GetLoopIncrement(th);
  if (incr) {
    CFG::Node* li = new CFG::Node(incr);
    connect(x_nodes, li);
    connect(li, c, FALLTHROUGH_EDGE);
  }
  else
    connect(x_nodes, c);

  return LOOP;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** The general structure of an end-tested loop (e.g., do-while in C) CFG is as below:
    <PRE>
              |
              |   ______
              |  /      \  
              V V        |
             body        | 
              |          |
              |          |
              V          |
          condition      |
              |\________/
              | 
              | 
              |
              V

    The statement representing the do-while loop will be used to represent the condition block
    in the CFG.
    </PRE>
*/
IRStmtType
CFG::build_CFG_end_tested_loop (CFG::Node* prev_node, StmtHandle th, CFG::NodeLabelList& exit_nodes,
		              CFG::NodeLabelList* return_nodes)
{
  // FIXME: New construct, partially tested.
  // New CFG nodes must be allocated for both the body and loop condition (we can re-use
  // prev_node for the body if it is empty).
  Node* b = NULL;
  Node* c = NULL;

  // Process the loop body.
  CFG::NodeLabelList x_nodes;
  IRStmtIterator *body = ir.LoopBody(th);
  if (body->IsValid() && body->Current()) {
    if (prev_node->empty()) {
      b = prev_node;
    } else {
      b = new CFG::Node();
      add(b);
      connect(prev_node, b, FALLTHROUGH_EDGE);
    }
    NodeLabelList break_statements;
    NodeLabelList continue_statements;
    if (build_block(b, body, x_nodes, &break_statements, return_nodes, &continue_statements) == SIMPLE)
      x_nodes.push_back(NodeLabel(b, FALLTHROUGH_EDGE));

    NodeLabelListIterator break_iter(&break_statements);
    while ((bool)break_iter) {
      exit_nodes.push_back((NodeLabel)break_iter);
      ++break_iter;
    }

    NodeLabelListIterator continue_iter(&continue_statements);
    while ((bool)continue_iter) {
      x_nodes.push_back((NodeLabel)continue_iter);
      ++continue_iter;
    }
  }
  else // empty body
    // FIXME: c is NULL at this point...
    x_nodes.push_back(NodeLabel(c, TRUE_EDGE));

  // Allocate a node for the condition and make the proper connections.
  c = new CFG::Node(th);
  exit_nodes.push_back(NodeLabel(c, FALSE_EDGE));
  connect(c, b, TRUE_EDGE);
  connect(x_nodes, c);

  return END_TESTED_LOOP;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** A two-way branch consists of a condition along with a true branch and a false branch.  Note that "elseif"
    statements should be translated to this form while parsing, since, they are semantically equivalent to a chain of
    nested "else if" statements.  Indeed, "elseif" is just a short form for "else if".
    The general structure of the CFG for a two-way branch is as follows:

    <PRE>
        |
        V
    condition
      |      \ 
      V       V
   true_body false_body
      |          |
       \________/
           |
           V
    </PRE>
*/
IRStmtType
CFG::build_CFG_twoway_branch (CFG::Node* prev_node, StmtHandle th, CFG::NodeLabelList& exit_nodes,
			      CFG::NodeLabelList* break_nodes, CFG::NodeLabelList* return_nodes,
                              CFG::NodeLabelList* continue_nodes)
{
  // add the if statement itself to the previous block. it represents 
  // the conditional branch terminating the block.
  prev_node->add(th); 

  // construct the CFG for the true block
  CFG::Node* b;
  IRStmtIterator *true_body = ir.TrueBody(th);
  if (true_body->IsValid() && true_body->Current()) {
    b = new CFG::Node();
    add(b);
    if (build_block(b, true_body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(NodeLabel(b, FALLTHROUGH_EDGE));

    connect(prev_node, b, TRUE_EDGE);
  }
  else {
    // the body is empty
    b = 0;
    exit_nodes.push_back(NodeLabel(prev_node, TRUE_EDGE));
  }

  // handle the false block
  IRStmtIterator *false_body = ir.ElseBody(th);
  if (false_body->IsValid() && false_body->Current()) {
    CFG::Node* c_body = new CFG::Node();
    add(c_body);
    if (build_block(c_body, false_body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(NodeLabel(c_body, FALLTHROUGH_EDGE));

    connect(prev_node, c_body, FALSE_EDGE);
  }
  else if (b != 0) // if b is 0 then the "condition" node has already been added to exit_nodes
    exit_nodes.push_back(NodeLabel(prev_node, FALSE_EDGE));

  return STRUCT_TWOWAY_CONDITIONAL;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** A multi-way branch consists of a multiway condition expression that branches off into a number of cases, and an
    optional catchall (default) body.  The structure of a multi-way branch is as follows:

    <PRE>
                |
                |
                V
         ___condition______
        /  /  |  \ ...\    \ 
       /  /   |   \    \    \ 
      V  V    V    V    V    V
     B1  B2   B3  B4    Bk  catchall
      |  |    |    |    |    |
      \__|____|____|____|___/
                |
                |
                V
    </PRE>
*/
IRStmtType
CFG::build_CFG_multiway_branch (CFG::Node* prev_node, StmtHandle th, CFG::NodeLabelList& exit_nodes,
				CFG::NodeLabelList* break_nodes, CFG::NodeLabelList* return_nodes,
                                CFG::NodeLabelList* continue_nodes)
{
  // add the switch statement to the previous block. the switch statement
  // represents the multi-way conditional branch terminating the block.
  prev_node->add(th); 

  // iterate over the multi-branches
  bool direct_drop = false;   // should there be a direct edge from the condition to exit?

  for (int bridx = 0; bridx < ir.NumMultiCases(th); bridx++) {
    ExprHandle multiCond = ir.GetSMultiCondition(th, bridx);
    IRStmtIterator *body = ir.MultiBody(th, bridx);
    if (body->IsValid() && body->Current()) {
      CFG::Node* c = new CFG::Node();
      add(c);
      if (build_block(c, body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
        exit_nodes.push_back(NodeLabel(c, FALLTHROUGH_EDGE));
      connect(prev_node, c, MULTIWAY_EDGE, multiCond);
    } // if branch != 0
    else
      direct_drop = true;
  } /* for */

  // check for catch all case
  IRStmtIterator *catchall = ir.GetMultiCatchall(th);
  if (catchall->IsValid() && catchall->Current()) {
    CFG::Node* c = new CFG::Node();
    add(c);
    if (build_block(c, catchall, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(NodeLabel(c, FALLTHROUGH_EDGE));
    connect(prev_node, c, MULTIWAY_EDGE); // originally "default" ???
  }
  else
    direct_drop = true;

  // direct drop is true iff there is not catchall or at least one multi-branch body is null
  if (direct_drop)
    exit_nodes.push_back(NodeLabel(prev_node, FALLTHROUGH_EDGE));

  return STRUCT_MULTIWAY_CONDITIONAL;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** This is similar to build_CFG_multiway_branch, but it handles switch statements that have
    fall-through semantics for each case (that is, there are no implied breaks).  
    Each case can fall-through to the following case if an explicit control flow statement
    (e.g., break in C) is not present.  Also, unlike build_CFG_multiway_branch, the catchall/default
    case is not necessarily the last.  The functions are different enough that is seems cleaner
    to keep them separate.
    FIXME: The default case is still assumed to be last. All of the underlying infrastructure of
    mint assumes this and has to be fixed.
*/
IRStmtType
CFG::build_CFG_multiway_branch_with_fallthrough (CFG::Node* prev_node, StmtHandle th, CFG::NodeLabelList& exit_nodes,
				CFG::NodeLabelList* return_nodes, CFG::NodeLabelList* continue_nodes)
{
  // add the switch statement to the previous block. the switch statement
  // represents the multi-way conditional branch terminating the block.
  prev_node->add(th); 

  bool direct_drop = false;   // should there be a direct edge from the condition to exit?

  // We must create a new list of break statements since any breaks seen
  // in subtrees of this statement will exit this switch, not some other
  // enclosing construct.
  NodeLabelList break_switch_statements;

  // Track the last case body processed so that fall-through connections can be made
  // This will be non-null whenever the previous case falls through (i.e., did not end in a
  // break statement).
  CFG::Node* prev_case_node = 0;

  // iterate over the multi-branches
  for (int bridx = 0; bridx < ir.NumMultiCases(th); bridx++) {
    ExprHandle multiCond = ir.GetSMultiCondition(th, bridx);
    IRStmtIterator *body = ir.MultiBody(th, bridx);
    if (body->IsValid() && body->Current()) {
      CFG::Node* c = new CFG::Node();
      add(c);
      break_switch_statements.clear();
      // If a previous case falls-through, connect it to the current case node.
      if (prev_case_node) {
        connect(prev_case_node, c, FALLTHROUGH_EDGE);
        prev_case_node = 0;          
      }

      // A dummy placeholder for exit nodes.  We don't want the case body to be an exit from the
      // contruct unless there is a break.
      NodeLabelList x_nodes;
      if (build_block(c, body, x_nodes, &break_switch_statements, return_nodes, continue_nodes) == SIMPLE
          || break_switch_statements.size() == 0) {
        // We didn't see a break.
        prev_case_node = c;
      }
      // There should only be one break, but iterate anyway.
      NodeLabelListIterator break_stmt_iter(&break_switch_statements);
      while ((bool)break_stmt_iter) {
        exit_nodes.push_back((NodeLabel)break_stmt_iter);
        ++break_stmt_iter;
      }
      connect(prev_node, c, MULTIWAY_EDGE, multiCond);
    } // if branch != 0
    else
      direct_drop = true;
  } /* for */

  // check for catch all case
  IRStmtIterator *catchall = ir.GetMultiCatchall(th);
  if (catchall->IsValid() && catchall->Current()) {
    CFG::Node* c = new CFG::Node();
    add(c);
    break_switch_statements.clear();
    // If the previous case falls-through, connect it to the default node.
    if (prev_case_node) {
      connect(prev_case_node, c, FALLTHROUGH_EDGE);
      prev_case_node = 0;          
    }
    if (build_block(c, catchall, exit_nodes, &break_switch_statements, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(NodeLabel(c, FALLTHROUGH_EDGE));
    connect(prev_node, c, MULTIWAY_EDGE); // originally "default" ???
  }
  else
    direct_drop = true;

  // direct drop is true iff there is not catchall or at least one multi-branch body is null
  if (direct_drop)
    exit_nodes.push_back(NodeLabel(prev_node, FALLTHROUGH_EDGE));

  return STRUCT_MULTIWAY_CONDITIONAL;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Handle unconditional direct jumps.
    
    Create:
    goto statement ---> target block
    
*/
IRStmtType
CFG::build_CFG_unconditional_jump (CFG::Node* prev_node, StmtHandle stmt)
{
  // FIXME: New construct, partially tested.
  StmtLabel lab = ir.GetTargetLabel(stmt, 0);
  prev_node->add(stmt);
  connect(prev_node, node_from_label(lab), FALLTHROUGH_EDGE);
  return UNCONDITIONAL_JUMP;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Handle unconditional indirect jumps.

    Create:
    <PRE>
      goto statement
       |   ...   |   
       |         |
       V         V
    potential   potential
    target 0    target n
    </PRE> 

    Currently, any label in the program unit is considered to
    be a potential target of the indirect branch. This could be very
    conservative for some constructs, such as Fortran computed gotos
    (since we could more intelligently examine only those labels that
    are used in the Fortran ASSIGN statement -- i.e., labels that have
    their address taken and stored). It may be worthwhile to revisit 
    this if unacceptably large graphs result from a combination of many
    labels and many indirect branches.
*/
IRStmtType
CFG::build_CFG_unconditional_jump_i (CFG::Node* prev_node, StmtHandle stmt)
{
  // Every label must be seen before all the edges can be added.
  // Just add this to the list of indirect jumps, which will have their
  // edges added later.
  cout << "FIXME: UNCONDITIONAL_JUMP_I not yet implemented" << endl;
  assert(0);
  return UNCONDITIONAL_JUMP_I;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Handle unstructured twoway branches.
    The structure of a branch-on-true is as follows:

    <PRE>
        |
        V
    condition_t
      |        \ (true edge)
      V          \
    fall-through   \
     block          V
                  true block
    </PRE>

    A similar subgraph is created for branch-on-false, except that the
    fall-through is on true and the branching edge is a false edge.
*/
IRStmtType
CFG::build_CFG_ustruct_twoway_branch (CFG::Node* prev_node, StmtHandle stmt, CFG::NodeLabelList& exit_nodes)
{
  // FIXME: New construct, partially untested.
  StmtLabel lab = ir.GetTargetLabel(stmt, 0);
  bool branch_on_true = (ir.GetStmtType(stmt) == USTRUCT_TWOWAY_CONDITIONAL_T);
  prev_node->add(stmt);
  connect(prev_node, node_from_label(lab), (branch_on_true ? TRUE_EDGE : FALSE_EDGE));

  // This is so that the branch block will get connected to the fall-through block.
  exit_nodes.push_back(NodeLabel(prev_node, FALLTHROUGH_EDGE));  /// ??? Use FALSE_EDGE?

  return ir.GetStmtType(stmt);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Handle unstructured multiway branches.

    An unstructured multiway is essentially any non-structured branch
    with multiple targets. An optional default/catchall target is also
    allowed. Examples of this construct include Fortran's computed goto
    statement or a simple jump/dispatch table in a low-level intermediate
    representation. A vanilla jump table will not have a default target,
    but a Fortran computed goto (for example) may if it isn't known that
    one of the targets is always taken.
    <PRE>
                |
                |
                V
         ____branch________
        /  /  |  \ ...\    \ 
       /  /   |   \    \    \ 
      V  V    V    V    V    V
     B1  B2   B3  B4    Bk  catchall (optional)

    </PRE>

    Each edge will have an associated condition. For case-like statements,
    this is the case value for the particular target. For jump/dispatch
    tables or computed gotos, which have no conditions, the index of the
    target is used (i.e., 0..number_of_targets).
*/
IRStmtType
CFG::build_CFG_ustruct_multiway_branch (CFG::Node* prev_node, StmtHandle stmt)
{
  // FIXME: New construct, partially tested.
  // Add the multi-way statement to the previous block. It represents
  // the multi-way branch terminating the block (which holds the selector
  // expression).
  prev_node->add(stmt);

  // Connect the branch to each of its targets.
  for (int br = 0; br < ir.NumUMultiTargets(stmt); br++) {
    StmtLabel target_lab = ir.GetUMultiTargetLabel(stmt, br);
    connect(prev_node, node_from_label(target_lab), MULTIWAY_EDGE /* , multiCond */);
  }

  // If there is a default target, make the proper connections.
  StmtLabel default_lab = ir.GetUMultiCatchallLabel(stmt);
  if (default_lab) {
    connect(prev_node, node_from_label(default_lab), MULTIWAY_EDGE /* , multiCond */);
  }
   
  return USTRUCT_MULTIWAY_CONDITIONAL;
}


//--------------------------------------------------------------------------------------------------------------------
// Connect the src to each of the nodes in dst_list.
void
CFG::connect (CFG::Node* src, CFG::NodeLabelList& dst_list)
{
  NodeLabelListIterator dst_list_iter(&dst_list);
  while ((bool)dst_list_iter) {
    NodeLabel nl = (NodeLabel)dst_list_iter;
    connect(src, nl.n, nl.et, nl.eh);
    ++dst_list_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
// Connect each node in src_list to dst.
void
CFG::connect (CFG::NodeLabelList& src_list, CFG::Node* dst)
{
  NodeLabelListIterator src_list_iter(&src_list);
  while ((bool)src_list_iter) {
    NodeLabel nl = (NodeLabel)src_list_iter;
    connect(nl.n, dst, nl.et, nl.eh);
    ++src_list_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
// 
// Given a label, determine whether it has been encountered already.
// If so, return its associated block (obtained from the label_to_node_map).
// If not, create a new block and update the label_to_node_map.
// 
CFG::Node*
CFG::node_from_label (StmtLabel lab)
{
  CFG::Node* node = 0;
  if (label_to_node_map.find(lab) != label_to_node_map.end()) {
    // This label has been encountered already, thus it already has an associated block.
    node = label_to_node_map[lab];
  }
  else {
    // This label has not been encountered yet, so create a new block.
    node = new CFG::Node();
    add(node);
    label_to_node_map[lab] = node; 
  }
  return node;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Compute "uses" set for each basic block (one CFG node corresponds to one basic block).  The set of uses is
    computed by traversing the statements in each node and eliminating the uses that get killed.  In other words, only
    upwardly exposed uses are computed.  Simultaneously, for each variable, the set of blocks that defines that
    variable, is also built (def_blocks_set).
*/
void
CFG::compute_uses_sets ()
{
  std::set<SymHandle /*, Name::CompStruct */> kill_set;
  NodesIterator nodes_iter(*this);
  while ((bool)nodes_iter) {
    kill_set.clear();
    Node* n = dynamic_cast<Node*>((BaseGraph::Node*)nodes_iter);
    NodeStatementsIterator s_iter(n);
    while ((bool)s_iter) {
      IRUseDefIterator *u_iterptr = ir.GetUses((StmtHandle)s_iter);
      while (u_iterptr->IsValid()) {
	LeafHandle s = u_iterptr->Current();
	if (kill_set.find(ir.GetSymHandle(s)) == kill_set.end()) // the variable does not exist in the kill set
	  if (non_locals.find(s) == non_locals.end())
	    // insert only if not already in the set, otherwise, it can get duplicated
	    non_locals.insert(s);
	++(*u_iterptr);
      }

      IRUseDefIterator *d_iterptr = ir.GetDefs((StmtHandle)s_iter);
      while (d_iterptr->IsValid()) {
	LeafHandle s = d_iterptr->Current();
        SymHandle s_name = ir.GetSymHandle(s);
	kill_set.insert(s_name);
	if (def_blocks_set[s_name].find(n) == def_blocks_set[s_name].end())
	  def_blocks_set[s_name].insert(n);
	++(*d_iterptr);
      }
      ++s_iter;
    }
    ++nodes_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
// Transfer all statements beginning at (and including) splitPoint to
// the new basic block, 'newBlock'
void
CFG::Node::split(StmtHandle splitPoint, Node* newBlock)
{
  std::list<StmtHandle>::iterator splitPointIter;
  
  for (splitPointIter = stmt_list.begin();
       (splitPointIter != stmt_list.end() && *splitPointIter != splitPoint);
       ++splitPointIter) {
  }
  // 'splitPointIter' now points to 'splitPoint'

  newBlock->stmt_list.splice(newBlock->stmt_list.end(),
                             stmt_list, splitPointIter, stmt_list.end());
}


// erase: Remove 'h' from the statement list; return the removed
// handle or 0 if not found.
StmtHandle
CFG::Node::erase(StmtHandle h)
{
  std::list<StmtHandle>::iterator it;
  for (it = stmt_list.begin(); (it != stmt_list.end()); ++it) {
    StmtHandle stmt = *it;
    if (stmt == h) {
      stmt_list.erase(it);
      return h;
    }
  }
  return 0;
}


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
CFG::dump (ostream& os)
{
  os << "===== CFG dump =====" << endl << "    " << ir.GetSymNameFromSymHandle(name) << endl << "--------------------" << endl;
  // print the contents of all the nodes
  NodesIterator nodes_iter(*this);
  while ((bool)nodes_iter) {
    (dynamic_cast<CFG::Node*>((DGraph::Node*)nodes_iter))->longdump(this, os);
    os << endl;
    ++nodes_iter;
  }

  // print the set of non-local variables (upwardly exposed uses)
  os << "--------------------" << endl << "Non Locals: {";
  NonLocalsIterator nli(*this);
  if ((bool)nli) {
    LeafHandle leafnode = (LeafHandle)nli;
    ir.PrintLeaf(leafnode, os);
    ++nli;
    while ((bool)nli) {
      leafnode = (LeafHandle)nli;
      os << ", ";
      ir.PrintLeaf(leafnode, os);
      ++nli;
    }
  }
  os << "}" << endl << "--------------------" << endl;

  // finally, print the mapping from variables to their definition blocks
  os << "--------------------" << endl << "Variable definition blocks" << endl;
  std::map<SymHandle, std::set<Node*> /*, Name::CompStruct */>::iterator def_blks_iter = def_blocks_set.begin();
  while (def_blks_iter != def_blocks_set.end()) {
    SymHandle nm = (*def_blks_iter).first;
    os << ir.GetSymNameFromSymHandle(nm) << " --> {";
    std::set<Node*>::iterator nodes_iter = (*def_blks_iter).second.begin();
    Node* n;
    if (nodes_iter != (*def_blks_iter).second.end()) {
      n = *nodes_iter;
      os << n->label;
      ++nodes_iter;
      while (nodes_iter != (*def_blks_iter).second.end()) {
	os << ", ";
	n = *nodes_iter;
	os << n->label;
	++nodes_iter;
      }
    }
    os << "}" << endl;
    ++def_blks_iter;
  }
  os << "====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
CFG::Node::longdump (CFG *currcfg, ostream& os)
{
  // print the node ID
  os << "CFG Node: " << label;
  if (num_incoming() == 0)
    os << " (root)";
  if (currcfg->Entry() == this) {
    os << " [ENTRY]";
  } else if (currcfg->Exit() == this) {
    os << " [EXIT]"; 
  }

  // print the source(s)
  SourceNodesIterator src(this);
  if ((bool)src) {
    os << " <-- (" << (dynamic_cast<CFG::Node*>((DGraph::Node*)src))->label;
    ++src;
    while ((bool)src) {
      os << ", " << (dynamic_cast<CFG::Node*>((DGraph::Node*)src))->label;
      ++src;
    }
    os << ")";
  };
  os << endl;

  // print the node contents
  CFG::NodeStatementsIterator s_iter(this);
  while ((bool)s_iter) {
    os << "    ";
    StmtHandle s = (StmtHandle)s_iter;
    IRInterface &rir = currcfg->GetIRInterface();
    rir.Dump(s, os);
    os << endl;
    os << "      DEFS: {";
    IRUseDefIterator *defs_iterptr = rir.GetDefs(s);
    if (defs_iterptr->IsValid()) {
        rir.PrintLeaf(defs_iterptr->Current(), os);
	++(*defs_iterptr);
	while (defs_iterptr->IsValid()) {
	  os << ", ";
          rir.PrintLeaf(defs_iterptr->Current(), os);
	  ++(*defs_iterptr);
	}
    }
    os << "}, Uses: {";
    IRUseDefIterator *uses_iterptr = rir.GetUses(s);
    if (uses_iterptr->IsValid()) {
        rir.PrintLeaf(uses_iterptr->Current(), os);
	++(*uses_iterptr);
	while (uses_iterptr->IsValid()) {
	  os << ", ";
          rir.PrintLeaf(uses_iterptr->Current(), os);
	  ++(*uses_iterptr);
	}
    }
    os << "}" << endl;
    ++s_iter;
  }

  // print the sink(s)
  OutgoingEdgesIterator out_iter(this);
  if ((bool)out_iter) {
    DGraph::Edge* e = (DGraph::Edge*)out_iter;
    os << " --> (" << (dynamic_cast<Node*>(e->sink()))->label;
    os << " [";
    e->dump(os);
    os << "]";
    ++out_iter;
    while ((bool)out_iter) {
      e = (DGraph::Edge*)out_iter;
      os << ", " << (dynamic_cast<Node*>(e->sink()))->label;
      os << " [";
      e->dump(os);
      os << "]";
      ++out_iter;
    }
    os << ")" << endl;
  }
}
//--------------------------------------------------------------------------------------------------------------------



#if 1
//
// FIXME: The code for delay slot handling is untested.  This C++ implementation was translated directly
//        from a known-working C version in a different compilation system.
//
//
// Pre: h is a branch
// Post: Returns true if the label is in label_to_block_map
//
bool
CFG::isInternalBranch(StmtHandle h)
{
  StmtLabel lab = ir.GetTargetLabel(h, 0);

  if (label_to_node_map.find(lab) != label_to_node_map.end()) {
    return true;
  } else {
    return false;
  }
}


CFG::Node*
CFG::getLabelBlock (StmtLabel lab)
{
  if (label_to_node_map.find(lab) != label_to_node_map.end()) {
    return label_to_node_map[lab];
  } else {
    return 0;
  }
}


//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Definitions of auxilliary data structures to support construction of CFGs
// with delay slots
//*****************************************************************************

class Branch_Counter {
public:
  Branch_Counter(): branchInstruction(0 /* NULL */), count(-1) {}
  Branch_Counter(StmtHandle a, int c): branchInstruction(a), count(c) {}

public:
  StmtHandle branchInstruction;
  int count;
};

#include <list>
#include <set>
#include <map>


struct compareBranchCounters {

  bool operator() (const Branch_Counter* b1, const Branch_Counter* b2) const {
    return ((b1->branchInstruction != b2->branchInstruction) && 
	    (b1->count == b2->count));
  }
};


typedef std::set<Branch_Counter *, compareBranchCounters>::iterator CounterlistIterator;


class Counterlist {
public:
  Counterlist();
  Counterlist(CounterlistIterator, CounterlistIterator);


  void decrementCounters();
  void addCounter(StmtHandle, int);
  void addCounters(Counterlist* );

  CounterlistIterator getFirstNonZeroCounter();
  
  void addCounters(std::list<Branch_Counter*>);
  
  int size();
  void reset();

  Branch_Counter* getLeastCounter();
  
  CounterlistIterator begin();
  CounterlistIterator end();

private:
  std::set<Branch_Counter*, compareBranchCounters> counters;
};


  
class Worklist {
public:
  void copyCountersToWorklist(CFG::Node*, Counterlist*);
  
  void addCounterToWorklist(CFG::Node*, StmtHandle, int);
  
  Counterlist* getCounterlist(CFG::Node*);
  
  void eraseCounterlist(CFG::Node*);
  bool isEmpty();

private:
  std::map<CFG::Node*, Counterlist*> allElements;
};


//*****************************************************************************

Counterlist::Counterlist ()
{
}


Counterlist::Counterlist (CounterlistIterator begin, CounterlistIterator end)
{
  counters.insert(begin, end);
}


//
// Subtracts 1 from each counter in the Worklist element
//
void
Counterlist::decrementCounters ()
{ 
  for (std::set<Branch_Counter*, compareBranchCounters>::iterator it = counters.begin();
      it != counters.end(); ++it) {
    
    if ((*it)->count == 0) {
      cerr << "Trying to decrement counter beyond zero. Could be an uninitialized counter." << endl;
      exit(-1);
    }

    ((*it)->count)--;
  }
}


void
Counterlist::addCounter (StmtHandle stmt, int counterValue)
{
  counters.insert(new Branch_Counter(stmt, counterValue) );
}


std::set<Branch_Counter*, compareBranchCounters>::iterator Counterlist::getFirstNonZeroCounter ()
{
  for (std::set<Branch_Counter*, compareBranchCounters>::iterator iter = counters.begin();
      iter != counters.end(); ++iter) {

    if ((*iter)->count > 0) {
      return iter;
    }

  }
  
  return counters.end();
}


void
Counterlist::addCounters (Counterlist* newCounters)
{
  // Called by copyCountersToWorklist
  
  for (CounterlistIterator it = newCounters->begin();
       it != newCounters->end();
       ++it) {
    
    counters.insert(*it);
  }

}



int
Counterlist::size ()
{
  return counters.size();
}


void
Counterlist::reset ()
{
  counters.clear();
}


Branch_Counter*
Counterlist::getLeastCounter ()
{
  if (size() > 0) {
    return *(counters.begin());
  }
  else {
    return NULL;
  }
}


CounterlistIterator
Counterlist::begin ()
{
  return counters.begin();
}


CounterlistIterator
Counterlist::end ()
{
  return counters.end();
}


void
Worklist::copyCountersToWorklist (CFG::Node* newBlock, Counterlist* blockCounterlist)
{
  //  Counterlist blockCounters = allElements[newBlock];
 
  allElements[newBlock]->addCounters(blockCounterlist);
}


void
Worklist::addCounterToWorklist (CFG::Node* block, StmtHandle stmt, int count)
{
  allElements[block]->addCounter(stmt, count);
}



void
Worklist::eraseCounterlist (CFG::Node* block)
{
  allElements.erase(block);
}


bool
Worklist::isEmpty ()
{
  return allElements.empty();
}


Counterlist*
Worklist::getCounterlist (CFG::Node* block)
{
  return allElements[block];
}

//*****************************************************************************


//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Refine the CFG to split blocks as necessary to handle delay slots, along
// with branches and labels in delay slots
//*****************************************************************************
void
CFG::HandleDelayedBranches ()
{
  //
  // Preconditions:
  //
  // On input to this routine, all nodes and edges resulting from
  // structured control flow are already present.  In addition, all
  // nodes and edges induced by labels are present. To be finalized
  // here are the unstructured control flow contructs.
  //

  //
  // Create the "basic" CFG.  This adds control flow for the first
  // branch in a block (after its 'k' delay slots).
  //
  createBasicCFG();

#if 0 // FIXME: Disabled until its fixed. 
  //
  // Finally, perform Waterman's worklist algorithm to finalize
  // control flow for the cases where branches are contained in
  // the delay slots of other branches.
  // 

  // Place the start block of the procedure on the worklist
  Counterlist* emptyCounterList = new Counterlist;
  the_worklist->copyCountersToWorklist(Entry(), emptyCounterList);

  while (! the_worklist->isEmpty()) {
    for (NodesIterator nodes_iter(*this);
         (bool)nodes_iter && !the_worklist->isEmpty(); ++nodes_iter) {
      CFG::Node *node = dynamic_cast<CFG::Node*>((DGraph::Node*)nodes_iter);
      processBlock(node);
    }    
  }
#endif
} 


void
CFG::createBasicCFG()
{
  // Create and populate the block_list.
  std::set<Node*> block_list;
  for (NodesIterator nodes_iter(*this); (bool)nodes_iter; ++nodes_iter) {
    Node *node = dynamic_cast<Node*>((DGraph::Node*)nodes_iter);
    block_list.insert(node);
  }

  // Process the blocks. 
  while (block_list.size() > 0) {
    bool branch_found = false;
    int countdown = 0;
    StmtHandle stmt = 0;
    Node* node = *block_list.begin();
    assert(block_list.erase(node) > 0);

    // Find first unstructured branch statement in this block.
    NodeStatementsIterator si(node);
    for ( ; (bool)si; ++si) {
      stmt = (StmtHandle)si;
      IRStmtType ty = ir.GetStmtType(stmt);
      assert(ty != UNCONDITIONAL_JUMP_I);  // FIXME: Add support.
      if (ty == USTRUCT_TWOWAY_CONDITIONAL_T
          || ty == USTRUCT_TWOWAY_CONDITIONAL_F 
          || ty == UNCONDITIONAL_JUMP) {
        branch_found = true;
        countdown = ir.NumberOfDelaySlots(stmt);
        break;
      } // if ty == branch
    } // For statements.

    // Only do this if countdown > 0, non-delayed branches are already
    // done in the main build routine.
    if (branch_found == true && countdown > 0) {
      // Move to end of this branch's delay slots.
      for ( ; (bool)si && countdown > 0; ++si) {
        // FIXME: Need to check ParallelWithSuccessor, etc.
        --countdown;
      }

      if (countdown == 0) {
        //
        // Split the block just past its final delay slot.
        // Add the new block (the second piece of the original block) to
        // the block_list.
        // Add edges from the current block to the target(s) of the branch.
        //
        // FIXME: But before trying to do the split, check if the split point is
        // already at the end of the block.  This could happen if there
        // is a label just after the branch's final delay slot.
        //
        ++si;
        if (1 /* (bool)si == true */) {
	  StmtHandle val = ((bool)si) ? (StmtHandle)si : 0;
          Node *newnode = splitBlock (node, val);
          block_list.insert(newnode);
          IRStmtType ty = ir.GetStmtType(stmt);
          if (ty == USTRUCT_TWOWAY_CONDITIONAL_T
              || ty == USTRUCT_TWOWAY_CONDITIONAL_F) {
            connect(node, getLabelBlock(ir.GetTargetLabel(stmt, 0)),
                    ty == USTRUCT_TWOWAY_CONDITIONAL_T ? TRUE_EDGE : FALSE_EDGE);
            connect(node, newnode, FALLTHROUGH_EDGE);
          } else if (ty == UNCONDITIONAL_JUMP) {
	    StmtLabel label = ir.GetTargetLabel(stmt, 0);
	    Node* nodeTmp = getLabelBlock(label);
            connect(node, nodeTmp, FALLTHROUGH_EDGE);
          } else {
            assert(0);
          }
        } // if (si)
      } // if (countdown == 0)
    } // if branch_found.

    if (branch_found == false || countdown > 0) {
      // ??? Add fall-thru edge from b (fixme, should already be there?).
      // Note, countdown > 0 can only happen if there was a label
      // in a delay slot.
    }
  } // While block_list.
 
}

// Split the basic block 'block' at the given split point and return
// the new basic block.  'splitPoint' is the first statement in the
// new basic block.
CFG::Node *
CFG::splitBlock (Node* block, StmtHandle splitPoint)
{
  // TODO: if a NOP is in the middle of being processed split into 
  // 2 NOPs so they can be split. 

  Node* newBlock = new Node;
  add(newBlock);

  // Move all instructions beginning at 'splitPoint' from 'block' to
  // the new block
  block->split((StmtHandle)splitPoint, newBlock);

  // Change Edge information
  std::list<Edge *> del_list;

  for (OutgoingEdgesIterator ei(block); (bool)ei; ++ei) {
    // Add successor edges for new block
    Edge *cfge = dynamic_cast<Edge *>((DGraph::Edge *)ei);
    connect(newBlock, dynamic_cast<Node *>(cfge->sink()), cfge->getType());

    // Update fallthrough 
    fallThrough[block] = newBlock;

    del_list.push_back(cfge);
  }

  std::list<Edge *>::iterator dli = del_list.begin();
  for ( ; dli != del_list.end(); dli++) {
    CFG::disconnect(*dli);
    delete (*dli); 
  }

#if 0 // FIXME: Splitting won't change any label's block??
  // Update label->block maps.
  for (NodeStatementsIterator si(newBlock); (StmtHandle)si; ++si) {
    StmtHandle stmt = (StmtHandle)si;
    if (ir.GetLabel(stmt) != 0) {
      label_to_node_map.erase(ir.GetLabel(stmt));
      label_to_node_map[ir.GetLabel(stmt)] = newBlock;
    }
  }
#endif

  return newBlock;
}


//
// Add edges for all branches (including those in delay slots)
//
void
CFG::processBlock (CFG::Node* block)
{
  //
  // Mark block visited 
  //
  
  //  CFG::Node* block = (*currentWorklistElem)->block;

  Counterlist* blockCounterlist = the_worklist->getCounterlist(block); 

  //CounterlistIterator blockCountersIter = blockCounterlist->begin();

  // Run loop until first element of counter_list hits zero or the end of 
  // the block is reached.  If a counter reaches zero divide the block
  // and add new edges as necessary 
  CFG::NodeStatementsIterator statementsIter(block);

  while ((bool)statementsIter) {
    bool isTheSamePacket = true;
    // this loop emulates single_cycle()
    while (isTheSamePacket)  {
      //
      // Iterate through all instructions in a packet and  add an 
      // instruction to counter_list if it is a branch
      //
      if ((ir.GetStmtType((StmtHandle)statementsIter) == USTRUCT_TWOWAY_CONDITIONAL_T
           || ir.GetStmtType((StmtHandle)statementsIter) == USTRUCT_TWOWAY_CONDITIONAL_F)
          && isInternalBranch((StmtHandle)statementsIter)) {
	
	the_worklist->addCounterToWorklist(block, (StmtHandle)statementsIter, 
					  ir.NumberOfDelaySlots((StmtHandle)statementsIter) );
      }


      ++statementsIter;

      // If the next instruction's parallel flag is clear, then a new packet
      // has begun
      isTheSamePacket = ! ir.ParallelWithSuccessor((StmtHandle)statementsIter);

    }

    /* 
     * Don't decrement if end of block is reached and first instruction 
     * in the next block is in parallel 
     */	
    
    // Get the first statement in the fallthrough node
    StmtHandle firstInstructionInNextBlock = 0 /* NULL */;
    for (CFG::NodeStatementsIterator nextBlockIter(fallThrough[block]);
         (bool)nextBlockIter;
	 ++nextBlockIter) {
      firstInstructionInNextBlock = (StmtHandle)nextBlockIter;
    }			

    if (! (!(bool)statementsIter && 
           ir.ParallelWithSuccessor(firstInstructionInNextBlock))) {
      
      blockCounterlist->decrementCounters();
    }

    //
    // If the first counter reaches zero, exit the loop
    //
    if (blockCounterlist->size() != 0 && 
	blockCounterlist->getLeastCounter() == 0) {
      break;
    }

  }


  //
  // If a counter reached zero before the block ended:
  //
  if ((StmtHandle)statementsIter)  {

    //
    // Splice off remainder of stmt_list and put in new block
    // connect these two blocks via a fallthrough
    //
    
    //
    // splitBlock adds an edge between block and the 
    // newly created fallthrough block
    //
    splitBlock(block, (StmtHandle)statementsIter);

    // Add predecessor edge for new block
    connect(block, fallThrough[block], FALLTHROUGH_EDGE);

  }
    
  blockCounterlist = the_worklist->getCounterlist(block);

  CounterlistIterator firstNonZeroCounter = 
    blockCounterlist->getFirstNonZeroCounter();
  
  // newCounterlist consists of all counter lists which will be 
  // passed on to the successor blocks
  Counterlist* newCounterList = 
    new Counterlist(blockCounterlist->begin(), firstNonZeroCounter);
  
  // Iterate through all the zero counters and add control flow
  for (CounterlistIterator counterListIter = blockCounterlist->begin();
       counterListIter != firstNonZeroCounter;
       ++counterListIter) {
   
    StmtHandle currentStatement = (*counterListIter)->branchInstruction;

    if (ir.GetStmtType(currentStatement) == USTRUCT_TWOWAY_CONDITIONAL_T
        || ir.GetStmtType(currentStatement) == USTRUCT_TWOWAY_CONDITIONAL_F) {
      
      connect(block, fallThrough[block], FALLTHROUGH_EDGE);      
      // Add the fallthrough block to the worklist
      the_worklist->copyCountersToWorklist(fallThrough[block], newCounterList);
      
    }

    if (isInternalBranch(currentStatement)) {
      CFG::Node* targetBlock = 
	getLabelBlock(ir.GetTargetLabel(currentStatement, 0));
      
      connect(block, targetBlock, TRUE_EDGE );
      // Add the target block to the worklist
      the_worklist->copyCountersToWorklist( targetBlock, newCounterList);
    }
  } // for


  // If no counter reached zero, it implies that the blocks 
  // were split not due to a branch but due to something like a label    
  if ((firstNonZeroCounter == blockCounterlist->begin()) 
       && (fallThrough.find(block) != fallThrough.end())) {
    the_worklist->copyCountersToWorklist( fallThrough[block], newCounterList);
  }

  // Erase the Counter list associated with the block
  the_worklist->eraseCounterlist(block);
}
#endif
