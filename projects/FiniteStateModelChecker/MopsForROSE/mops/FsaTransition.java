package mops;
// $Id: FsaTransition.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;

/**
 * Represents a transition in FSA
 */
public class FsaTransition
{
  public int state0, state1;
  public Object input;

  public FsaTransition()
  {
    state0 = 0;
    input = null;
    state1 = 0;
  }

  public FsaTransition(int state0, Object input, int state1)
  {
    this.state0 = state0;
    this.input = input;
    this.state1 = state1;
  }

  /**
   * Two FsaTransition a and b are equal if all of the followings are true:
   * <ul>
   * <li> (a.input == null && b.input == null)
   *   || (a.input != null && b.input != null && a.input.equals(b.input) </li>
   * <li> a.state0 == b.state0 </li>
   * <li> a.state1 == b.state1 </li>
   * </ul>
   */
  public boolean equals(FsaTransition t)
  {
    if (t.input == null)
    {
      if (this.input != null)
	return false;
    }
    else
    {
      if (this.input == null)
	return false;
      else if (!t.input.equals(this.input))
	return false;
    }
    return t.state0 == this.state0 && t.state1 == this.state1;
  }

  /**
   * Creates a new FsaTransition with an additional dimension
   */
  public FsaTransition expand(int newState0, int newState1, int dim)
  {
    return new FsaTransition(newState0 * dim + state0, input,
			     newState1 * dim + state1);
  }

  /* EPSILON is used only for stack symbols.  Epsilon input is null
     No epsilon state is allowed
  */
  public static final int EPSILON = 0;
}
