package mops;
// $Id: PdaTransition.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;

/**
 * Represents a transition in PDA
 */
public class PdaTransition extends FsaTransition
{
  public int stack0, stack1, stack2;

  public PdaTransition(int state0, int stack0, Object input, int state1,
		       int stack1, int stack2)
  {
    super(state0, input, state1);
    this.stack0 = stack0;
    this.stack1 = stack1;
    this.stack2 = stack2;
  }

  public PdaTransition()
  {
    super();
  }

  /**
   * Overrides FsaTransition.equals()
   */
  public boolean equals(FsaTransition transition)
  {
    if (!(transition instanceof PdaTransition))
    {
      Util.warn(Util.ALERT, "not an instance of PdaTransition");
      return false;
    }
    
    PdaTransition t = (PdaTransition)transition;
    return super.equals(t) 
      && stack0 == t.stack0 && stack1 == t.stack1 && stack2 == t.stack2;
  }

  /**
   * Overrides FsaTransition.expand()
   */
  public FsaTransition expand(int newState0, int newState1, int dim)
  {
    return new PdaTransition(newState0 * dim + state0, stack0, input,
			     newState1 * dim + state1, stack1, stack2);
  }
}
