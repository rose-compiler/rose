package mops;
// $Id: StateLabel.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;

/**
 * Contains labels in a dimension in the FSA/PDA state space
 */
public class StateLabel
{
  /**
   * size of this dimension
   */
  public int size;
  /**
   * labels in this dimension.  Each element is a String
   */
  public HashMap labels;

  public StateLabel()
  {
    size = 0;
    labels = null;
  }

  public StateLabel(int size, HashMap labels)
  {
    this.size = size;
    this.labels = labels;
  }
}

