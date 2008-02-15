package mops;
// $Id: FsaTransitionTrace.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;

/**
 * Extends FsaTransition with backtracking data used in Pda.postStar()
 */
public class FsaTransitionTrace extends FsaTransition
{
  /**
   * Used for backtracking in Pda.post*()
   * Each element of it is an TransitionBridge object.
   */
  public Vector parent;

  /**
   * Used for shortest path
   */
  public int distance;
  public TransitionBridge after;

  public FsaTransitionTrace()
  {
    super();
    parent = new Vector();
    distance = -1;
    after = null;
  }

  public FsaTransitionTrace(int state0, Object input, int state1)
  {
    super(state0, input, state1);
    parent = new Vector();
    distance = -1;
    after = null;
  }

  /*
  public static final boolean addParent(Vector list,
					TransitionBridge transitionBridge)
  {
    int i;
    
    for (i = 0; i < list.size(); i++)
      if (((TransitionBridge)list.get(i)).before.equals(
        transitionBridge.before))
	return false;
    list.add(transitionBridge);
    return true;
  }

  public final boolean addParent(TransitionBridge transitionBridge)
  {
    if (parent == null)
      parent = new Vector();
    return addParent(parent, transitionBridge);
  }

  public final boolean isContainedIn(Vector list)
  {
    int i;

    if (list == null)
      return false;
    
    for (i = 0; i < list.size(); i++)
      if (((TransitionBridge)list.get(i)).before.equals(this))
	return true;

    return false;
  }
  */
}





