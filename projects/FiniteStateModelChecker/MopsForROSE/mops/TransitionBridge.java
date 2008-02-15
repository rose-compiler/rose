package mops;
// $Id: TransitionBridge.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;

/**
 * Denotes the parent transition in the P-automaton that triggers this
 * transition to be added and the corresponding Ast
 */
public class TransitionBridge
{
  public TransitionBridge(FsaTransitionTrace before, FsaTransitionTrace after,
			  Ast ast)
  {
    this.before = before;
    this.after = after;
    this.ast = ast;
  }

  public FsaTransitionTrace before, after;
  public Ast ast;
}
