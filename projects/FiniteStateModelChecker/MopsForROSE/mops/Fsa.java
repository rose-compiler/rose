package mops;
//$Id: Fsa.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;
import java.util.*;

/**
 * Finite State Automaton
 */
public class Fsa
{
  public Fsa()
  {
    initialStates = new HashSet();
    finalStates = new HashSet();
    transitions = new HashMap();
    stateLabels = new Vector();
  }

  public void clear()
  {
    int i;
    
    initialStates.clear();
    finalStates.clear();
    transitions.clear();
    for (i = 0; i < stateLabels.size(); i++)
      ((StateLabel)stateLabels.get(i)).labels = null;
    stateLabels.clear();
  }

  /**
   * list contains a sorted list of FsaTransitions.  Currently, it is indexed
   * on state1.  Different transitions with the same state1 are placed adjacent
   * to each other.
   * Note: addTransition() relies on this method for insertion point.
   * So changing indexing field in this methods affects in addTransition() too.
   *
   * @return
   * <ul>
   * <li> If transition is null, return -1. </li>
   * <li> If transition exists in list, return its position. </li>
   * <li> Otherwise, return -(index + 2) where index is the position where
   *      transition should be inserted. </li>
   * </ul>
   */
  protected static int getTransitionIndex(Vector list, FsaTransition transition)
  {
    FsaTransition t;
    int begin, end, index, i;

    if (transition == null)
      return -1;

    begin = 0;
    end = list.size() - 1;
    while (begin <= end)
    {
      index = (begin + end) / 2;
      t = (FsaTransition)list.get(index);
      if (t.equals(transition))
	return index;
      if (transition.state1 < t.state1)
	end = index - 1;
      else if (transition.state1 > t.state1)
	begin = index + 1;
      else
      {
	for (i = index - 1; i >= 0; i--)
	{
	  t = (FsaTransition)list.get(i);
	  if (transition.state1 != t.state1)
	    break;
	  if (t.equals(transition))
	    return i;
	}
	for (i = index + 1; i < list.size(); i++)
	{
	  t = (FsaTransition)list.get(i);
	  if (transition.state1 != t.state1)
	    break;
	  if (t.equals(transition))
	    return i;
	}
	return - (i + 2);
      }
    }
    return - (begin + 2);
  }

  public boolean addTransition(FsaTransition transition)
  {
    Vector perState;
    Integer state0Int = new Integer(transition.state0);
    int index;
 
    if ((perState = (Vector)transitions.get(state0Int)) == null)
    {
      perState = new Vector();
      transitions.put(state0Int, perState);
      if (transition.state0 >= getStateSize())
	setStateSize(transition.state0 + 1);
    }

    // insert into ordered list
    index = getTransitionIndex(perState, transition);
    if (index >= 0)
      return false;
    else if (index == -1)
      return false;
    else
      perState.add(-index - 2, transition);
    
    // add state1 if necessary
    addState(transition.state1);
    return true;
  }

  public HashMap getTransitions() { return transitions; }
  
  public boolean containsTransition(FsaTransition t)
  {
    return queryTransition(t) != null;
  }

  public FsaTransition queryTransition(FsaTransition t)
  {
    Vector perState;
    int index;

    if ((perState = (Vector)getTransitions().get(new Integer(t.state0)))
	== null)
      return null;

    if ((index = getTransitionIndex(perState, t)) >= 0)
      return (FsaTransition)perState.get(index);
    else
      return null;
  }

  public static final boolean containsTransition(Vector list,
						 FsaTransition transition)
  {
    return getTransitionIndex(list, transition) >= 0;
  }

  /**
   * Remove a transition and return it
   */
  public FsaTransition popTransition()
  {
    Vector perState;
    Iterator iter;

    iter = getTransitions().entrySet().iterator();
    while (iter.hasNext())
    {
      perState = (Vector)((Map.Entry)iter.next()).getValue();
      if (perState != null && perState.size() > 0)
      {
	return (FsaTransition)perState.remove(0);
      }
    }

    return null;
  }
  
  public void addState(Integer state)
  {
    if (!transitions.containsKey(state))
    {
      transitions.put(state, null);
      if (state.intValue() >= getStateSize())
      	setStateSize(state.intValue() + 1);
    }
  }

  public void addState(int state)
  {
    addState(new Integer(state));
  }

  public Set getStates() { return transitions.keySet(); }
  
  public void addInitialState(Integer state)
  {
    addState(state);
    if (initialStates.contains(state))
      Util.warn(Util.ALERT, "ignore duplicate initial states");
    else
      initialStates.add(state);
  }

  public void addInitialState(int state)
  {
    addInitialState(new Integer(state));
  }
  
  public HashSet getInitialStates() { return initialStates; }

  public void addFinalState(Integer state)
  {
    addState(state);
    if (finalStates.contains(state))
      Util.warn(Util.ALERT, "ignore duplicate final states");
    else
      finalStates.add(state);
  }

  public void addFinalState(int state)
  {
    addFinalState(new Integer(state));
  }
  
  public HashSet getFinalStates() { return finalStates; }

  /**
   * Compute a single state value from its components in all dimensions
   */
  public int packState(int[] states)
  {
    int i, state;

    if (states.length != stateLabels.size())
      Util.die("the dimensions of states mismatch: stateLabels().size()=" +
	       stateLabels.size() + " but states.length=" + states.length);

    state = states[0];
    for (i = 1; i < stateLabels.size(); i++)
      state += states[i] * ((StateLabel)stateLabels.get(i - 1)).size;

    return state;
  }
  
  public void addStateLabel(Integer state, String label)
  {
    if (stateLabels.size() != 1)
      Util.die("cannot add a state label unless statelabels.size() == 1");
    
    HashMap labels = (HashMap)((StateLabel)stateLabels.get(0)).labels;
    if (labels.get(state) == null)
      labels.put(state, label);
    addState(state);
  }

  protected String[] getStateLabel(int state)
  {
    String[] labels = new String[stateLabels.size()];
    int i, size, index, origState;

    if (labels.length == 0)
      return labels;

    if (state >= ((StateLabel)stateLabels.get(stateLabels.size() - 1)).size)
    {
      Util.warn(Util.ALERT, "state > maxsize");
      return labels;
    }

    origState = state;
    for (i = labels.length - 1; i > 0; i--)
    {
      size = ((StateLabel)stateLabels.get(i - 1)).size;
      index = state / size;
      state %=  size;
      labels[i] = (String)((StateLabel)stateLabels.get(i)).labels.get(
  	          new Integer(index));
      if (labels[i] == null)
      {
	return new String[] {"(" + origState + ")"};
	//labels[i] = "(" + index + ")";
      }
    }
    labels[0] = (String)((StateLabel)stateLabels.get(0)).labels.get(
           	new Integer(state));
    if (labels[0] == null)
      return new String[] {"(" + state + ")"};
      //labels[0] = "(" + state + ")";
    
    return labels;
  }

  public String getStateLabelString(int state)
  {
    String[] labels = getStateLabel(state);
    StringBuffer label = new StringBuffer();
    int i;

    if (labels[0] != null)
      label.append(labels[0]);
    for (i = 1; i < labels.length; i++)
    {
      label.append(",");
      if (labels[i] != null)
	label.append(labels[i]);
    }
    return label.toString();
  }

  /**
   * Add the first dimension
   */
  public void addInitialDimension()
  {
    stateLabels.add(new StateLabel(0, new HashMap()));
  }
  
  public void setStateSize(int size)
  {
    StateLabel stateLabel;
    if (stateLabels.size() == 0)
      Util.die("cannot set the state size when stateLabels.size() == 0");

    stateLabel = (StateLabel)stateLabels.get(stateLabels.size() - 1);
    if (stateLabel.size > size)
      Util.warn(Util.ALERT, "shrink state size");
    stateLabel.size = size;
  }

  public int getStateSize()
  {
    if (stateLabels.size() == 0)
      // hack for pda where there is no state.  Be aware of all implications
      return 1;
    else
      return ((StateLabel)stateLabels.get(stateLabels.size() - 1)).size;
  }

  /**
   * Creates and returns a new state
   */
  public int getNewState()
  {
    if (stateLabels.size() == 0)
      Util.die("Cannot get new state when stateLabels.size() == 0");
    return ((StateLabel)stateLabels.get(stateLabels.size() - 1)).size++;
  }

  public boolean containsState(Integer state)
  {
    return transitions.containsKey(state);
  }

  public Iterator iterator(int requestedState)
  {
    return new FsaIterator(requestedState);
  }

  public Iterator iterator()
  {
    return iterator(-1);
  }

  /**
   * Convenient for iterating over all FsaTransitions
   */
  private class FsaIterator implements Iterator
  {
    public FsaIterator(int requestedState)
    {
      this.requestedState = (requestedState < 0) ?
	                     null : new Integer(requestedState);
      stateIter = transitions.keySet().iterator();
      index = -1;
      state = null;
    }
    
    public boolean hasNext()
    {
      if (index != -1)
	if (index < perState.size())
	  return true;
        else
          index = -1;
      
      if (stateIter == null)
	return false;

      // client requested a specific state
      if (requestedState != null)
	if (state != null)
	{
	  stateIter = null;
	  return false;
	}
        else
	  if ((perState = (Vector)transitions.get(requestedState)) == null)
	  {
	    stateIter = null;
	    return false;
	  }
	  else
	  {
	    if (perState.size() <= 0)
	    {
	      stateIter = null;
	      return false;
	    }
	    state = requestedState;
	    index = 0;
	    return true;
	  }

      // client requested no specific state
      while (stateIter.hasNext())
      {
	state = (Integer)stateIter.next();
	perState = (Vector)transitions.get(state);
	if (perState != null && perState.size() > 0)
	{
	  index = 0;
	  return true;
	}
      }

      stateIter = null;
      return false;
    }

    public Object next()
    {
      if (!hasNext())
	return null;

      return (FsaTransition)perState.get(index++);
    }

    public void remove()
    {
    }

    private Integer state, requestedState;
    private Iterator stateIter;
    private Vector perState;
    private int index;
  }

  /**
   * Compose two FSAs or one PDA with one FSA.
   * this may be a PDA, but fsa2 must be a FSA.
   * this is a regular fsa(from CFG), but fsa2 may be a pattern fsa
   * epsilon transitions are NOT treated specially
   * initial and final states are NOT set.  You must set them manually
   *
   * @param newFsa Holds the product.  Must be an empty Fsa or Pda
   */
  public void compose(Fsa fsa2, Fsa newFsa)
  {
    Fsa fsa1 = this;
    Iterator iter, iter2;
    Integer state;
    Vector perState, inputs, map;
    FsaTransition fsaTransition, fsaTransition2;
    StateLabel stateLabel, stateLabel2;
    Ast ast;
    int i, index, size;

    // make sure it's clean
    newFsa.clear();

    // set up state labels.  shallow copy
    for (i = 0; i < fsa1.stateLabels.size(); i++)
    {
      stateLabel = new StateLabel();
      stateLabel2 = (StateLabel)fsa1.stateLabels.get(i);
      stateLabel.size = stateLabel2.size;
      stateLabel.labels = stateLabel2.labels;
      newFsa.stateLabels.add(stateLabel);
    }
    size = fsa1.getStateSize();
    for (i = 0; i < fsa2.stateLabels.size(); i++)
    {
      stateLabel = new StateLabel();
      stateLabel2 = (StateLabel)fsa2.stateLabels.get(i);
      stateLabel.size = stateLabel2.size * size;
      stateLabel.labels = stateLabel2.labels;
      newFsa.stateLabels.add(stateLabel);
    }

    // set up transitions
    inputs = new Vector();
    map = new Vector();
    iter = fsa1.iterator();
    while (iter.hasNext())
    {
      fsaTransition = (FsaTransition)iter.next();
      if (fsaTransition.input == null)
	Util.die("fsaTransition.input == null");
      iter2 = fsa2.transitions.keySet().iterator();
      while (iter2.hasNext())
      {
	state = (Integer)iter2.next();
	/* do not deal with EPSILON transition, because they are handled
	   by the default "other" transitions
	if (((Ast)fsaTransition.input).kind == Ast.kind_epsilon)
	{
	  newFsa.addTransition(fsaTransition.expand(state.intValue(),
	    state.intValue(), fsa1.getStateSize()));
	}
	else
	*/
	perState = (Vector)fsa2.transitions.get(state);
	if (perState == null || perState.size() == 0)
	  continue;
	// fill in "inputs" with all transitions in perState.  The order of
	// the transitions are recorded in Ast.address as a hack.
	inputs.setSize(perState.size());
	map.setSize(perState.size());
	for (i = 0; i < perState.size(); i++)
	{
	  fsaTransition2 = (FsaTransition)perState.get(i);
	  ast = (Ast)fsaTransition2.input;
	  inputs.set(ast.address, ast);
	  map.set(ast.address, fsaTransition2);
	}
	// sanity check
	for (i = 0; i < perState.size(); i++)
	  if (inputs.get(i) == null)
	    Util.die("sanity check failed");
	
	if ((index = ((Ast)fsaTransition.input).match(inputs)) >= 0)
	{
	  // fsaTransition2 = (FsaTransition)perState.get(index);
	  fsaTransition2 = (FsaTransition)map.get(index);
	  newFsa.addTransition(fsaTransition.expand(fsaTransition2.state0,
	    fsaTransition2.state1, fsa1.getStateSize()));
	  
	  // Sanity check: whether to trace into function call is solely based
	  // on whether the function is defined in the module.  This implies
	  // that it is wrong to mention user-defined functions in model FSA
	  /*
	  if (fsaTransition instanceof PdaTransition)
	  {
	    if (((PdaTransition)fsaTransition).stack2 != FsaTransition.EPSILON
		&& fsaTransition2.state0 != fsaTransition2.state1)
	      Util.die(
		"In model FSA, it is illegal to mention user defined function "
		       + ((Ast)fsaTransition.input).getFunctionCallName()
		+ fsa2.getStateLabelString(fsaTransition2.state0) + " " +
		fsa2.getStateLabelString(fsaTransition2.state1));
	  }
	  */
	}
	inputs.clear();
	map.clear();
      }
    }
  }

  /**
   * Find all transitions that originate from initial states in P-automaton
   * Note: initial states in P-automaton are final states in the model FSA
   */
  public Vector findInitialTransitions()
  {
    FsaTransitionTrace transition;
    Vector output, perState;
    Iterator iter;
    Integer initialState;
    int i;

    output = new Vector();
    iter = getInitialStates().iterator();
    while (iter.hasNext())
    {
      initialState = (Integer)iter.next();
      perState = (Vector)getTransitions().get(initialState);
      if (perState == null)
	continue;
      for (i = 0; i < perState.size(); i++)
      {
	transition = (FsaTransitionTrace)perState.get(i);
	output.add(transition);
      }
    }

    return output;
  }

  /**
   * Print out initial transitions.  This could answer questions like
   * "show me all statements that are executed in privileged mode"
   */
  public void writeInitialTransitions(Vector initialTransitions, String filename)
    throws IOException
  {
    int i;
    PrintWriter writer;
    FsaTransitionTrace transition;

    writer = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
    for (i = 0; i < initialTransitions.size(); i++)
    {
      transition = (FsaTransitionTrace)initialTransitions.get(i);
      writer.println(transition.input);
    }
    writer.close();
  }

  /**
   * Get the previous transition (skipping function calls) during
   * shortest path computation.
   */
  private FsaTransitionTrace getPrevTransition(TransitionBridge
					       transitionBridge)
  {
    FsaTransitionTrace transition, transition2, functionEntry;
    Vector perState, parents;
    int i;
    
    transition = transitionBridge.after;
    transition2 = transitionBridge.before;
    if (transition2.state1 != transition.state1)
    {
      perState = (Vector)getTransitions().get(new Integer(transition2.state1));
      functionEntry = null;
      for (i = 0; i < perState.size(); i++)
      {
	transition2 = (FsaTransitionTrace)perState.get(i);
	if (transition2.state1 == transition.state1)
	{
	  if (functionEntry == null)
	    functionEntry = transition2;
	  else
	    Util.warn(Util.ALERT, "ignore duplicate functionEntry");
	}
      }
      if (functionEntry == null)
	Util.die("cannot find functionEntry");
      parents = functionEntry.parent;
      if (parents.size() != 1)
	Util.die("parens.size() != 1");
      transition2 = ((TransitionBridge)parents.get(0)).before;
      if (transition2.state1 != transition.state1)
	Util.die("transition2.state1 != transition.state1");
    }

    return transition2;
  }

  /*
   * Backtrack the shortest path from the transition stored in
   * <code>path</code> to the entry point of the same function and
   * place the path in <code>path</code>.  For each function call on
   * the path, create a Vector object, store the exit node of the
   * function call in the Vector object, store the Vector object into
   * the path, and place the Vector object into the functionList.
   *
   * The grammar of a path is:
   * Vector = FsaTransitionTrace (Ast [Vector] FsaTransitionTrace)
   *
   * @param functionList A list of function exit nodes that are to be backtracked
   * @param functionHash Mapping between an exit node and its backtracked path
   */
  private void backtrackOneFunction(Vector path, Vector functionList,
				    Hashtable functionHash)
  {
    HashSet touchedTransitions, workHash;
    Vector workQueue, list, parents;
    FsaTransitionTrace functionEntry, transition, transition2;
    TransitionBridge transitionBridge;
    Iterator iter;
    int i;

    // The starting point is stored in path.  Retrieve and remove it.
    if (path.size() > 1)
      Util.die("path.size() > 1");

    // shortest path computation
    workQueue = new Vector();
    workHash = new HashSet();
    touchedTransitions = new HashSet();
    transition = (FsaTransitionTrace)path.remove(0);
    transition.distance = 0;
    transition.after = null;
    workQueue.add(transition);
    workHash.add(transition);
    touchedTransitions.add(transition);
    functionEntry = null;
    while (workQueue.size() > 0)
    {
      transition = (FsaTransitionTrace)workQueue.remove(0);
      workHash.remove(transition);
      parents = transition.parent;
      if (parents == null || parents.size() == 0)
      {
	functionEntry = transition;
      }
      else
      {
	for (i = 0; i < parents.size(); i++)
	{
	  transitionBridge = (TransitionBridge)parents.get(i);
	  transition2 = getPrevTransition(transitionBridge);
	  {
	    if (transition2.distance == -1 ||
		transition2.distance > (transition.distance + 1))
	    {
	      transition2.distance = transition.distance + 1;
	      transition2.after = transitionBridge;
              touchedTransitions.add(transition2);
	      if (!workHash.contains(transition2))
	      {
		workHash.add(transition2);
		workQueue.add(transition2);
	      }
	    }
	  }
	}
      }
    }

    // shortest path retrieval
    if (functionEntry == null)
      Util.die("cannot find functionEntry");
    transition = functionEntry;
    path.add(transition);
    while (transition.after != null)
    {
      transitionBridge = transition.after;
      path.add(transitionBridge.ast);
      if (transitionBridge.before != transition)
      {
	// add the exit node of the callee
	if ((list = (Vector)functionHash.get(transitionBridge.before)) == null)
	{
	  list = new Vector();
	  list.add(transitionBridge.before);
    	  functionList.add(list);
	  functionHash.put(transitionBridge.before, list);
	}
	//else
	//{
	// System.err.println("Reuse path.  good!");
	//}
        path.add(list);
      }
      path.add(transitionBridge.after);
      transition = transitionBridge.after;
    }

    // clear touched transitions
    iter = touchedTransitions.iterator();
    while (iter.hasNext())
    {
      transition = (FsaTransitionTrace)iter.next();
      transition.distance = -1;
      transition.after = null;
    }

    touchedTransitions.clear();
    workHash.clear();
  }

  /**
   * Records the distance of each state from the final state.
   */
  private class Distance
  {
    /**
     * distance of this state from the final state
     */
    public int distance;

    /**
     * the transitions that come into this state
     */
    public Vector transitions;

    public Distance()
    {
      distance = -1;
      transitions = new Vector();
    }
  }

  /**
   * Compute the shortest distance of each state from the final state
   */
  private void shortestDistanceFromFinalState(Hashtable distances)
  {
    Distance perState, perState2;
    Vector transitions, workQueue;
    HashSet workHash;
    Iterator iter;
    Map.Entry entry;
    int i;
    Integer state0, state1;
    FsaTransitionTrace transition;

    workQueue = new Vector();
    workHash = new HashSet();
    // construct a table indexed by state1
    iter = getTransitions().entrySet().iterator();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      transitions = (Vector)entry.getValue();
      if (transitions != null)
      {
	for (i = 0; i < transitions.size(); i++)
	{
	  transition = (FsaTransitionTrace)transitions.get(i);
	  state1 = new Integer(transition.state1);
	  if ((perState = (Distance)distances.get(state1)) == null)
	  {
	    perState = new Distance();
	    if (getFinalStates().contains(state1))
	    {
	      perState.distance = 0;
	      if (!workHash.contains(state1))
	      {
	        workQueue.add(state1);
	        workHash.add(state1);
	      }
	    }
	    else
	    {
	      perState.distance = -1;
	    }
	    distances.put(state1, perState);
	  }
	  perState.transitions.add(transition);
	}
      }
    }

    // compute shortest path of each node
    while (workQueue.size() > 0)
    {
      state1 = (Integer)workQueue.remove(0);
      workHash.remove(state1);
      perState = (Distance)distances.get(state1);
      transitions = perState.transitions;
      for (i = 0; i < transitions.size(); i++)
      {
	transition = (FsaTransitionTrace)transitions.get(i);
	state0 = new Integer(transition.state0);
	perState2 = (Distance)distances.get(state0);
	if (perState2 != null && 
	    (perState2.distance == -1 ||
	     perState2.distance > (perState.distance + 1)))
	{
	  perState2.distance = perState.distance + 1;
	  if (!workHash.contains(state0))
	  {
	    workQueue.add(state0);
	    workHash.add(state0);
	  }
	}
      }
    }
    workHash.clear();

    /*
    iter = distances.entrySet().iterator();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      state0 = (Integer)entry.getKey();
      perState = (Distance)entry.getValue();
      System.err.println(state0 + ": " + perState.distance);
    }
    */
  }

  private class VectorPosition
  {
    public Vector list;
    public int position;
    public VectorPosition(Vector list, int position)
    {
      this.list = list;
      this.position = position;
    }
  }
  
  /**
   * Backtrack a shortest path
   */
  public void writeBacktrackPath(Vector initialTransitions, String filename)
    throws IOException
  {
    Vector perState, path, path2, stack, functionList;
    Hashtable  distances, functionHash;
    FsaTransitionTrace transition, transition2, functionEntry;
    RandomAccessFile writer;
    Object object;
    Ast ast;
    int index, minDistance, distance, i, lastState, position;
    long length;
    //BufferedReader reader;
    VectorPosition vectorPosition;
    TransitionBridge transitionBridge;
    
    // compute shortest distance of each state from the final state
    distances = new Hashtable();
    shortestDistanceFromFinalState(distances);
    
    functionList = new Vector();
    functionHash = new Hashtable();
    path = null;
    ast = null;
    /*
    for (i = 0; i < initialTransitions.size(); i++)
    {
      transition = (FsaTransitionTrace)initialTransitions.get(i);
      System.err.println(i + ": " + transition.input + " " + transition.state0
      		 + " " + transition.state1);
    }
    System.err.print("> ");
    reader = new BufferedReader(new InputStreamReader(System.in));
    index = Integer.parseInt(reader.readLine());
    */

    // pick a random initial transition
    index = new Random().nextInt(initialTransitions.size());
    transition = (FsaTransitionTrace)initialTransitions.get(index);
    while (true)
    {
      // backtrack in one stack frame
      path2 = path;
      path = new Vector();
      path.add(transition);
      //System.err.println(transition.input);
      backtrackOneFunction(path, functionList, functionHash);
      if (path2 != null)
      {
	path.add(ast);
	path.add(path2);
      }
      // go one level upper in the stack frame
      functionEntry = (FsaTransitionTrace)path.get(0);
      perState = (Vector)getTransitions().get(new Integer(functionEntry.state1));
      if (perState == null || perState.size() == 0)
	// entry of main()
	break;

      // get the transition that is closest to the final node
      transition = (FsaTransitionTrace)perState.get(0);
      minDistance = ((Distance)distances.get(new Integer(transition.state1)))
	          .distance;
      for (i = 1; i < perState.size(); i++)
      {
	transition2 = (FsaTransitionTrace)perState.get(i);
	distance = ((Distance)distances.get(new Integer(transition2.state1)))
	          .distance;
	if (distance < minDistance)
	{
	  transition = transition2;
	  minDistance = distance;
	}
      }
      /*
      if (perState.size() > 1)
      {
	System.err.println("More than one upper frames.  Choose first one? " +
			   (transition == (FsaTransitionTrace)perState.get(0) ? "T" : "F"));
      }
      */
      if (transition.parent.size() != 1)
	Util.die("transition.parent.size() != 1");
      transitionBridge = (TransitionBridge)transition.parent.get(0);
      transition = transitionBridge.before;
      ast = transitionBridge.ast;
    }
    //System.err.println("Finished going upper frames.  Now going deeper");
    distances.clear();

    // backtrack in deeper stack frames
    while (functionList.size() > 0)
    {
      path2 = (Vector)functionList.remove(0);
      //transition = (FsaTransitionTrace)path2.get(0);
      //System.err.println(path2 + " " +  transition.input + " "
      //		 + transition.state0 + " " + transition.state1);
      backtrackOneFunction(path2, functionList, functionHash);
    }
    functionHash.clear();

    // write backtrack path.  Remove trailing redundant nodes
    stack = new Vector();
    writer = new RandomAccessFile(filename, "rw");
    //initialState = ((FsaTransitionTrace)initialTransitions.get(0)).state0;
    length = -1;
    lastState = -1;
    position = 0;
    writer.writeBytes("(\n");
    while (position < path.size())
    {
      object = path.get(position++);
      if (object instanceof Vector)
      {
	stack.add(new VectorPosition(path, position));
	path = (Vector)object;
	position = 0;
	writer.writeBytes("(\n");
      }
      else
      {
	transition = (FsaTransitionTrace)object;
	writer.writeBytes("t \"" + getStateLabelString(transition.state0)
			  + "\" \"" + transition.input + "\"\n");
	if (lastState != -1 && transition.state0 != lastState)
	{
	  length = writer.getFilePointer();
	}
	lastState = transition.state0;
	if (position < path.size())
	{
	  ast = (Ast)path.get(position++);
	  writer.writeBytes("a " + ast.getAddress() + "\n");
	}
	else
	{
	  if (stack.size() > 0)
	  {
	    writer.writeBytes(")\n");
	    vectorPosition = (VectorPosition)stack.remove(stack.size() - 1);
	    path = vectorPosition.list;
	    position = vectorPosition.position;
	  }
	}
      }
    }
    writer.writeBytes(")\n");
    //System.err.println("file length = " + writer.getFilePointer() +
    //	       "  setLength = " + length);
    //if (length > -1)
    //  writer.setLength(length);
    writer.close();
    stack.clear();
  }
  
  /**
   * Read from .fsa file.
   * Note: initialStates and finalStates in .fsa files are inconsequential
   * initialStates and finalStates in .mfsa overrides them.
   */
  public final void read(String filename, Hashtable stateHash)
    throws IOException
  {
    BufferedReader bufferedReader;
    LineReader reader;
    String type;
    Integer state, state0, state1, input;
    Ast ast;
    Vector list;
      
    clear();
    addInitialDimension();
    
    bufferedReader = new BufferedReader(new FileReader(filename));
    reader = new LineReader(bufferedReader);
    
    for (type = reader.nextToken();
	 reader.getTtype() != StreamTokenizer.TT_EOF;
	 type = reader.nextToken())
    {
      // skip empty line
      if (type == null)
	continue;

      switch(type.charAt(0))
      {
        // initial states
	case 'q':
	  state = readState(reader, stateHash);
	  if (state == null)
	    Util.die("unknown state", filename,  reader.lineno());
	  addInitialState(state);
	  // read eol
	  reader.nextToken();
	  break;

        // final states
	case 'f':
	  state = readState(reader, stateHash);
	  if (state == null)
	    Util.die("unknown state", filename,  reader.lineno());
	  addFinalState(state);
	  // read eol
	  reader.nextToken();
	  break;

	// transitions
	case 't':
	  state0 = readState(reader, stateHash);
	  state1 = readState(reader, stateHash);
	  if (state0 == null || state1 == null)
	    Util.die("unknown state ", filename,  reader.lineno());
 	  ast = Ast.read(reader, null);
	  if (ast == null)
	    Util.die("NULL AST is forbidden", filename, reader.lineno());

	  // hack to record the order of transitions out of a given state
	  // in the FSA
	  list = (Vector)getTransitions().get(state0);
	  //System.err.println("state " + state0 + " order " +
	  //             (list == null ? 0 : list.size()));
	  ast.setAddress(list == null ? 0 : list.size());
	  
          addTransition(new FsaTransition(state0.intValue(), ast,
					  state1.intValue()));
	  // read eol
	  reader.nextToken();
	  break;

	default:
	  Util.die("unknown syntax " + type, filename, reader.lineno());
      }
      
      if (reader.getTtype() != StreamTokenizer.TT_EOL &&
	  reader.getTtype() != StreamTokenizer.TT_EOF)
      {
	Util.warn(Util.WARNING, "ignore extra words at the end of the line",
		  filename, reader.lineno());
	reader.skipToEol();
      }
    }

    bufferedReader.close();
  }

  public void read(String filename) throws IOException
  {
    Hashtable stateHash = new Hashtable();
    read(filename, stateHash);
    stateHash.clear();
  }
  
  protected Integer readState(LineReader reader, Hashtable stateHash)
            throws IOException
  {
    String name;
    Integer number;
    
    name = reader.nextToken();
    if (name == null)
      return null;

    if ((number = (Integer)stateHash.get(name)) == null)
    {
      number = new Integer(getNewState());
      addState(number);
      addStateLabel(number, name);
      stateHash.put(name, number);
    }
    return number;
  }

  /**
   * Write FSA
   */

  public final void write(String filename) throws IOException
  {
    PrintWriter writer;
    HashSet states;

    writer = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
    writeStates(writer, getInitialStates().iterator(), "q ");
    writeStates(writer, getFinalStates().iterator(), "f ");
    writeTransitions(writer);
    writer.close();
  }
  
  /**
   * Write FSA to .dot file
   */
  public final void writeToDot(String filename, boolean writeLabel)
    throws IOException
  {
    PrintWriter writer;
    Iterator iter;
    FsaTransition transition;
    Integer state;
    Vector perState;
    StringBuffer strBuf;
    String str;
    char ch;
    int i, j;

    strBuf = new StringBuffer();
    writer = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
    writer.println("digraph FSA {\nsize=\"8,11\";");

    iter = transitions.keySet().iterator();
    while (iter.hasNext())
    {
      state = (Integer)iter.next();
      writer.print(state + " [label=\""
		   + getStateLabelString(state.intValue()) + "\"");
      if (initialStates.contains(state))
	writer.print(", shape=box");
      else if (finalStates.contains(state))
	writer.print(", shape=trapezium");
      writer.println("];");
      
      perState = (Vector)transitions.get(state);
      if (perState == null)
	continue;
      for (i = 0; i < perState.size(); i++)
      {
	transition = (FsaTransition)perState.get(i);
	str = transition.input.toString();
	// Sanitize str to make dot happy
	for (j = 0; j < str.length(); j++)
	  if ((ch = str.charAt(j)) != '"' && ch != '\\')
	    strBuf.append(ch);
        writer.print(transition.state0 + " -> " + transition.state1);
	if (writeLabel)
	  writer.print(" [label=\"" + strBuf + "\"];");
	writer.println();
	strBuf.setLength(0);
      }
    }
    writer.println("}");
    writer.close();
  }

  /*
  public void print()
  {
    Iterator iter;
    
    System.out.println("Transitions:");
    printTransitions();
    System.out.println("States:");
    printStates(getStates().iterator());
    System.out.println("Initial states:");
    printStates(getInitialStates().iterator());
    System.out.println("Final states:");
    printStates(getFinalStates().iterator());
  }
  */
  
  protected void writeTransitions(PrintWriter writer)
  {
    Iterator iter;
    FsaTransition transition;
    Vector map, perState;
    int i, index;

    // Hack: the order of the transitions out of a state is recorded in
    // ast.address
    map = new Vector();
    iter = getTransitions().entrySet().iterator();
    while (iter.hasNext())
    {
      perState = (Vector)((Map.Entry)iter.next()).getValue();
      if (perState == null)
	continue;
      map.setSize(perState.size());
      for (i = 0; i < map.size(); i++)
	map.set(i, null);
      
      for (i = 0; i < perState.size(); i++)
      {
	transition = (FsaTransition)perState.get(i);
	index = ((Ast)transition.input).getAddress();
        //System.err.println("i=" + i + "  index=" + index);
	if (map.get(index) == null)
	{
	  map.set(index, transition);
	}
	else
	  Util.die("duplicate ordering in Fsa");
      }

      for (i = 0; i < map.size(); i++)
      {
	transition = (FsaTransition)map.get(i);
        writer.println("t \"" + getStateLabelString(transition.state0)
		       + "\" \"" + getStateLabelString(transition.state1)
		       + "\" " + transition.input);
      }
      map.clear();
    }
  }

  protected void writeStates(PrintWriter writer, Iterator iter, String prefix)
  {
    while (iter.hasNext())
      writer.println(prefix + 
		     getStateLabelString(((Integer)iter.next()).intValue()));
  }

  /**
   * Decide if the FSA contains non-self-loop "other" transitions or
   * consecutive such transitions.
   *
   * @return
   * <li>
   * <ul>0: has no non-self-loop "other" transition </ul>
   * <ul>1: has no consecutive non-self-loop "other" transitions </ul>
   * <ul>2: has consecutive non-self-loop "other" transitions </ul>
   * <ul>3: has "any" transition
   */
  public int checkTransitions()
  {
    HashSet states;
    Integer state;
    Iterator iter;
    Vector perState;
    int i, kind;
    FsaTransition transition;
    Map.Entry entry;

    states = new HashSet();
    iter = transitions.entrySet().iterator();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      state = (Integer)entry.getKey();
      perState = (Vector)entry.getValue();
      if (perState != null)
      {
	for (i = 0; i < perState.size(); i++)
	{
	  transition = (FsaTransition)perState.get(i);
	  kind = ((Ast)transition.input).kind;
	  if (kind == Ast.kind_other &&
	      transition.state1 != transition.state0)
	  {
	    states.add(state);
	    break;
	  }
	  else if (kind == Ast.kind_any)
	  {
	    states.clear();
	    return 3;
	  }
	}
      }
    }
    if (states.isEmpty())
    {
      return 0;
    }

    iter = states.iterator();
    while (iter.hasNext())
    {
      perState = (Vector)transitions.get(iter.next());
      for (i = 0; i < perState.size(); i++)
      {
	transition = (FsaTransition)perState.get(i);
	if (transition.state0 != transition.state1 &&
	    states.contains(new Integer(transition.state1)))
	{
	  states.clear();
	  return 2;
	}
      }
    }

    states.clear();
    return 1;
  }
  
  protected HashSet
    initialStates, // initial states
    finalStates; // final states

  /**
   * Indexed by states and contains all states in stateLabels, initialStates
   * and finalStates.
   */
  protected HashMap transitions;  

  /**
   * Records size and labels of each dimension of the state space
   * 0th element in stateLabels corresponds to LSB of state value
   * Note: state added by newState() cannot have labels
   */
  protected Vector stateLabels;
}
