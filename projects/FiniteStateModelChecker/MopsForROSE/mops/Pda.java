package mops;
// $Id: Pda.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;
import java.util.*;

/**
 * Pushdown Automaton
 */
public class Pda extends Fsa
{ 
  public Pda()
  {
    super();
    stackLabels = new Hashtable();
    initialStackSymbols = new Vector();
  }

  public void clear()
  {
    super.clear();
    stackLabels.clear();
    initialStackSymbols.clear();
  }
  
  public void addStackLabel(Integer symbol, String label)
  {
    if (symbol.intValue() == PdaTransition.EPSILON)
      return;
    if (stackLabels.get(symbol) == null)
      stackLabels.put(symbol, label);
  }
 
  public void addStackLabel(int symbol, String label)
  {
    addStackLabel(new Integer(symbol), label);
  }

  public Hashtable getStackLabels()
  {
    return stackLabels;
  }

  protected String getStackLabel(Integer symbol)
  {
    String label;

    if ((label = (String)stackLabels.get(symbol)) == null)
      return "(" + symbol.toString() + ")";
    else
      return label;
  }

  protected String getStackLabel(int symbol)
  {
    return getStackLabel(new Integer(symbol));
  }
  
  public void addInitialStackSymbol(Integer symbols)
  {
    initialStackSymbols.add(symbols);
  }

  public Vector getInitialStackSymbols()
  {
    return initialStackSymbols;
  }

  /**
   * Compose this with an FSA
   *
   * @param fsa2 Must be a FSA
   * @param newFsa The composed PDA.  It is declared as Fsa for polymorphism
   */
  public void compose(Fsa fsa2, Fsa newFsa)
  {
    Pda newPda;
    
    if (!(newFsa instanceof Pda))
      Util.die("newFsa must be a Pda");
    newPda = (Pda)newFsa;
    super.compose(fsa2, newPda);

    // Note: this and newPda share stackLabels and initialStackSymbols
    newPda.stackLabels = stackLabels;
    newPda.initialStackSymbols = initialStackSymbols;
  }

  /**
   * compose this with all FSAs in a MetaFsa
   */
  public Pda compose(MetaFsa metaFsa) throws IOException
  {
    Vector fsas, states;
    Pda oldPda, newPda;
    int i;
    
    fsas = metaFsa.getFsas();
    oldPda = this;
    newPda = null;
    for (i = 0; i < fsas.size(); i++)
    {
      newPda = new Pda();
      oldPda.compose((Fsa)fsas.get(i), newPda);
      // Do NOT clear oldPda because newPda and oldPda share
      // stackLabels and initialStackSymbols.
      oldPda = newPda;
    }
    states = metaFsa.getInitialStates();
    for (i = 0; i < states.size(); i++)
      newPda.addInitialState(newPda.packState((int[])states.get(i)));
    states = metaFsa.getFinalStates();
    for (i = 0; i < states.size(); i++)
      newPda.addFinalState(newPda.packState((int[])states.get(i)));

    return newPda;
  }

  /**
   * Create a P-automaton to describe the stack configuration of the PDA
   * States of P-automaton are states of PDA
   * Inputs of P-automaton are stack symbols of PDA
   * See paper for details
   * Note: postStar() works only if:
   * - initialStackSymbols contains only one element which is also the entry
   *   point of a function
   * - Each function entry point has no incoming edges
   */
  public Fsa post()
  {
    // P-automaton
    Fsa fsa = new Fsa();
    Iterator iter;
    int state0, state1, finalState, i;
    Integer state;
    String label;
    StateLabel stateLabel;
    FsaTransition fsaTransition;

    if (initialStackSymbols.size() <= 0)
      Util.die("initialStackSymbols.size() <= 0");

    // Shallow copy stateLabels
    for (i = 0; i < stateLabels.size(); i++)
    {
      stateLabel = (StateLabel)stateLabels.get(i);
      fsa.stateLabels.add(new StateLabel(stateLabel.size, stateLabel.labels));
    }
    
    // Create initial states
    // The set of initial states of P-automaton is the set of final states of
    // PDA
    iter = getFinalStates().iterator();
    while (iter.hasNext())
    {
      state = (Integer)iter.next();
      fsa.addInitialState(state);
    }

    // Create one final state
    finalState = fsa.getNewState();
    fsa.addFinalState(finalState);

    // Create a path from each initial state to the final state with all
    // initial stack symbols on.  The first stack symbol is adjacent to the
    // initial states.  The last stack symbol is adjacent to the final state.
    iter = getInitialStates().iterator();
    while (iter.hasNext())
    {
      state = (Integer)iter.next();
      fsa.addState(state);

      state0 = state.intValue();
      for (i = 0; i < initialStackSymbols.size() - 1; i++)
      {
	state1 = fsa.getNewState();
	fsa.addState(state1);
	fsa.addTransition(new FsaTransitionTrace(state0,
			  (Integer)initialStackSymbols.get(i),
    		          state1));
	state0 = state1;
      }
      fsa.addTransition(new FsaTransitionTrace(state0,
			(Integer)initialStackSymbols.get(i),
			finalState));
    }

    // poststar.  See paper for details
    postStar(fsa, this);

    // Transforms input symbols of P-automaton from integers to strings
    // Remember input symbols of P-automaton are stack symbols of PDA
    iter = fsa.iterator();
    while (iter.hasNext())
    {
      fsaTransition = (FsaTransition)iter.next();
      fsaTransition.input = getStackLabel((Integer)fsaTransition.input);
    }
    
    return fsa;
  }

  /**
   * Add an FsaTransitionTrace and its parent to rel or trans if they
   * do not already exist.  See paper for explanation of rel and trans
   *
   * @param isToTrans If true, add new FsaTransitionTrace to trans.
   *   If false, add it to rel
   */
  private static void addTransParent(Fsa rel, Fsa trans, int state0,
    Object input, int state1, FsaTransitionTrace before, Ast ast,
    boolean isToTrans)
  {
    FsaTransitionTrace t, ttemp;
    TransitionBridge transitionBridge;
    int i;

    ttemp = new FsaTransitionTrace(state0, input, state1);
    if ((t = (FsaTransitionTrace)rel.queryTransition(ttemp)) == null &&
	(t = (FsaTransitionTrace)trans.queryTransition(ttemp)) == null)
    {
      t = ttemp;
      if (isToTrans)
      {
	trans.addTransition(t);
      }
      else
      {
	rel.addTransition(t);
      }
      // System.err.println("Pda added " + t.input + " " + t.state0 + " " + t.state1);
    }

    if (before != null)
    {
      transitionBridge = new TransitionBridge(before, t, ast);
      t.parent.add(transitionBridge);
    }
  }

  /**
   * PostStar() operation.  See paper for details
   */
  protected static void postStar(Fsa rel, Pda pda)
  {
    Vector perState;
    FsaTransitionTrace fsaTransition, fsaTransition2, t;
    PdaTransition pdaTransition;
    Hashtable states, newStates, eps;
    Iterator iter, iter2;
    Integer state, state2;
    int i;
    Fsa trans;
    Map.Entry entry;
    Ast ast;

    // rel is fsa
    trans = new Fsa();
    // make FsaTransition happy
    trans.addInitialDimension();

    iter = rel.transitions.entrySet().iterator();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      state = (Integer)entry.getKey();
      if (pda.containsState(state))
      {
	perState = (Vector)entry.getValue();
	if (perState != null)
	{
	  for (i = 0; i < perState.size(); i++)
	  {
	    fsaTransition = (FsaTransitionTrace)perState.get(i);
	    trans.addTransition(fsaTransition);
	  }
	  perState.clear();
	}
      }
    }

    eps = new Hashtable();
    newStates = new Hashtable();
    iter = pda.iterator();
    while (iter.hasNext())
    {
      pdaTransition = (PdaTransition)iter.next();
      if (pdaTransition.stack2 != PdaTransition.EPSILON)
      {
	state = new Integer(rel.getNewState());
	rel.addState(state);
	newStates.put(pdaTransition, state);
	eps.put(state, new Hashtable());
      }
    }

    while ((t = (FsaTransitionTrace)trans.popTransition()) != null)
    {
      if (rel.containsTransition(t))
      {
	Util.die("rel & trans intersect: " + t.state0 + " " + t.input + " " + t.state1);
      }
      rel.addTransition(t);
      iter = pda.iterator(t.state0);
      while (iter.hasNext())
      {
	pdaTransition = (PdaTransition)iter.next();
	if (pdaTransition.stack0 != ((Integer)t.input).intValue())
	  continue;

	if (pdaTransition.stack1 == PdaTransition.EPSILON)
	{
	  state = new Integer(t.state1);
	  states = (Hashtable)eps.get(state);
	  if (states == null)
	  {
	    if (!rel.getFinalStates().contains(state))
	      Util.die("transition into a non-final and non-intermediate state.");
	    continue;
	  }
	  state2 = new Integer(pdaTransition.state1);
	  if (!states.containsKey(state2))
	  {
	    states.put(state2, t);
	    if (rel.getFinalStates().contains(new Integer(t.state1)))
	      rel.addFinalState(pdaTransition.state1);
	    iter2 = rel.iterator(t.state1);
	    while (iter2.hasNext())
	    {
	      fsaTransition = (FsaTransitionTrace)iter2.next();
	      addTransParent(rel, trans, pdaTransition.state1,
			     fsaTransition.input, fsaTransition.state1, t,
			  ((TransitionBridge)fsaTransition.parent.get(0)).ast,
			     true);
	    }
	  }
	  //else
	  //{
	  // Util.warn("Multiple exit nodes?");
	  //}
	}
	else if (pdaTransition.stack2 == PdaTransition.EPSILON)
	{
	  addTransParent(rel, trans, pdaTransition.state1,
			 new Integer(pdaTransition.stack1), t.state1, t,
			 (Ast)pdaTransition.input, true);
	}
	else
	{
	  state = (Integer)newStates.get(pdaTransition);
	  if (state.intValue() != t.state1)
	  {
	    addTransParent(rel, trans, pdaTransition.state1,
		           new Integer(pdaTransition.stack1),
		           state.intValue(), null, null, true);
	    addTransParent(rel, trans, state.intValue(),
			   new Integer(pdaTransition.stack2), t.state1,
			   t, (Ast)pdaTransition.input, false);
	    states = (Hashtable)eps.get(state);
	    iter2 = states.entrySet().iterator();
	    while (iter2.hasNext())
	    {
	      entry = (Map.Entry)iter2.next();
	      addTransParent(rel, trans, ((Integer)entry.getKey()).intValue(),
			     new Integer(pdaTransition.stack2), t.state1,
			     (FsaTransitionTrace)entry.getValue(),
			     (Ast)pdaTransition.input, true);
	    }
	  }
	  else
	  {
	    Util.warn(Util.ALERT, 
              "Recursive function call ignored: " +
	      pda.getStackLabel(pdaTransition.stack0) + " -> " +
	      pda.getStackLabel(pdaTransition.stack1) + " " +
	      pda.getStackLabel(pdaTransition.stack2));
	  }
	}
      }
    }

    // clean up
    trans.clear();
    newStates.clear();
    eps.clear();
  }

  /*
  protected static void postStar(Fsa rel, Pda pda)
  {
    Vector perState, transitions;
    FsaTransitionTrace fsaTransition, fsaTransition2, t;
    PdaTransition pdaTransition;
    Hashtable newStates, eps, states;
    Iterator iter, iter2;
    Integer state;
    int i;
    Fsa trans;
    Map.Entry entry;
    TransitionBridge transitionBridge;
    Ast ast;

    // rel is fsa
    trans = new Fsa();
    // make FsaTransition happy
    trans.addInitialDimension();

    iter = rel.transitions.entrySet().iterator();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      state = (Integer)entry.getKey();
      if (pda.containsState(state))
      {
	perState = (Vector)entry.getValue();
	if (perState != null)
	{
	  for (i = 0; i < perState.size(); i++)
	  {
	    fsaTransition = (FsaTransitionTrace)perState.get(i);
	    trans.addTransition(fsaTransition);
	  }
	  perState.clear();
	}
      }
    }

    eps = new Hashtable();
    newStates = new Hashtable();
    iter = pda.iterator();
    while (iter.hasNext())
    {
      pdaTransition = (PdaTransition)iter.next();
      if (pdaTransition.stack2 != PdaTransition.EPSILON)
      {
	state = new Integer(rel.getNewState());
	rel.addState(state);
	newStates.put(pdaTransition, state);
	addTransParent(rel, trans, pdaTransition.state1,
		       new Integer(pdaTransition.stack1),
		       state.intValue(), null, null, true);
	eps.put(state, new Hashtable());
      }
    }

    while ((t = (FsaTransitionTrace)trans.popTransition()) != null)
    {
      if (rel.containsTransition(t))
      {
	Util.die("Rel & trans intersect: should not happen: " + t.state0 + " " + t.input + " " + t.state1);
      }
      rel.addTransition(t);
      iter = pda.iterator(t.state0);
      while (iter.hasNext())
      {
	pdaTransition = (PdaTransition)iter.next();
	if (pdaTransition.stack0 != ((Integer)t.input).intValue())
	  continue;

	if (pdaTransition.stack1 == PdaTransition.EPSILON)
	{
	  state = new Integer(t.state1);
	  states = (Hashtable)eps.get(state);
	  if (states == null)
	  {
	    if (!rel.getFinalStates().contains(state))
	      Util.die("Transition into a non-final and non-intermediate state.");
	    continue;
	  }
	  state2 = new Integer(pdaTransition.state1);
	  if (!states.containsKey(state2))
	  {
	    states.put(state2, new Vector());
	    if (rel.getFinalStates().contains(state))
	      rel.addFinalState(pdaTransition.state1);
	    iter2 = rel.iterator(t.state1);
	    while (iter2.hasNext())
	    {
	      fsaTransition = (FsaTransitionTrace)iter2.next();
	      addTransParent(rel, trans, pdaTransition.state1,
			     fsaTransition.input, fsaTransition.state1,
			     fsaTransition.parent, true);
	    }
	  }
	}
	else if (pdaTransition.stack2 == PdaTransition.EPSILON)
	{
          transitionBridge = new TransitionBridge(t, null,
						  (Ast)pdaTransition.input);
	  addTransParent(rel, trans, pdaTransition.state1,
	    new Integer(pdaTransition.stack1), t.state1, transitionBridge,
			true);
	}
	else
	{
	  state = (Integer)newStates.get(pdaTransition);
	  if (state.intValue() != t.state1)
	  {
	    transitionBridge = new TransitionBridge(t, null,
					    (ast = (Ast)pdaTransition.input));
	    addTransParent(rel, trans, state.intValue(),
			   new Integer(pdaTransition.stack2), t.state1,
			   transitionBridge, false);
	    states = (HashSet)eps.get(state);
	    iter2 = states.iterator();
	    while (iter2.hasNext())
	    {
	      state2 = (Integer)iter2.next();
	      addTransParent(rel, trans, state2.intValue(),
			     new Integer(pdaTransition.stack2), t.state1,
			     t.parent, true);
	    }
	  }
	  else
	  {
	    Util.warn(
              "Recursive function call ignored: " +
	      pda.getStackLabel(pdaTransition.stack0) + " -> " +
	      pda.getStackLabel(pdaTransition.stack1) + " " +
	      pda.getStackLabel(pdaTransition.stack2));
	  }
	}
      }
    }

    // clean up
    trans.clear();
    newStates.clear();
    eps.clear();
  }
  */

  public static boolean matchFunctionName(String fullName, String partialName)
  {
    int index;
    
    if (fullName.equals(partialName))
      return true;
    else
    // try to truncate the leading module name
    {
      if ((index = fullName.indexOf(':')) > -1 &&
	  fullName.substring(index + 1).equals(partialName))
      return true;
    }
    return false;
  }
  
  /**
   * Read in a CFG, from which construct a PDA
   */
  public void read(Cfg cfg, String entryFunctionName)
  {
    Hashtable functionHash;
    HashSet nodeHash;
    int counter, symbolNumber, i, j, index;
    Vector nodeQueue, functions, outEdges;
    CfgFunction function, function2, entryFunction;
    Node node, node2;
    Edge edge;
    String label;
    Ast ast, unmatchedAst;
    //boolean isEntry;

    clear();

    // Number cfg nodes
    nodeQueue = new Vector();
    nodeHash = new HashSet();
    functionHash = new Hashtable();
    functions = cfg.getFunctions();
    counter = 0;
    entryFunction = null;
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      functionHash.put(function.label, function);
      /*
      // look for entry function
      isEntry = false;
      if (function.label.equals(entryFunctionName))
	isEntry = true;
      else
      // try to truncate the leading module name
      { if ((index = function.label.indexOf(':')) > -1 &&
 	    function.label.substring(index + 1).equals(entryFunctionName))
	  isEntry = true;
      }
      */

      if (matchFunctionName(function.label, entryFunctionName))
      {
	if (entryFunction == null)
	  entryFunction = function;
        else
	  Util.warn(Util.ALERT, "ignore duplicate entry function "
		    + function.label + " (was " + entryFunction.label + ")");
      }

      if (function.entry == null)
	Util.warn(Util.ALERT, "No entry node in the function " + function.label);
      if (function.exit == null)
	Util.warn(Util.ALERT, "No exit node in the function " + function.label);
      // number nodes
      nodeQueue.add(function.entry);
      //function.entry.setVisited(true);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
        node.setSN(++counter);
  	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  node2 = ((Edge)outEdges.get(j)).getDstNode();
	  //if (!node2.getVisited())
	  if (!nodeHash.contains(node2))
	  {
	    nodeQueue.add(node2);
	    //node2.setVisited(true);
	    nodeHash.add(node2);
	  }
	}
      }
      nodeHash.clear();
    }

    unmatchedAst = new Ast();
    unmatchedAst.setKind(Ast.kind_unmatched);
    // create Pda transitions from CFG
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      nodeQueue.add(function.entry);
      //function.entry.setVisited(false);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	addStackLabel(new Integer(node.getSN()), node.getLabel());
	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);
	  node2 = edge.getDstNode();
	  ast = edge.getLabel();
	  //if (node2.getVisited())
	  if (!nodeHash.contains(node2))
	  {
	    //node2.setVisited(false);
	    nodeQueue.add(node2);
	    nodeHash.add(node2);
	  }

	  // Warning: whether to trace into function call is solely based
	  // on whether the function is defined in the module.  This implies
	  // that it is wrong to mention module-defined functions in model
	  // FSA
          if (ast == null || ast.getKind() != Ast.kind_function_call ||
	     (function2 = (CfgFunction)functionHash.get
	      (ast.getFunctionCallName())) == null)
	  // Do not trace into a function call
	  {
	    // Note the delicacy here.  If the type of an AST node is
	    // unmatched, it will match the "other" symbol in compose().
	    if (ast == null)
	    {
	      ast = unmatchedAst;
	    }
	    addTransition(new PdaTransition(PdaTransition.EPSILON,
 	      node.getSN(), ast, PdaTransition.EPSILON, node2.getSN(),
	      PdaTransition.EPSILON));
	  }
	  else
	  // Trace into function call
	  {
	    // use ast instead of EPSILON to be able to backtrack
	    // in function call
	      addTransition(new PdaTransition(PdaTransition.EPSILON,
	        node.getSN(), ast, PdaTransition.EPSILON,
	        function2.entry.getSN(), node2.getSN()));
	      //}
	  }
	  // if node2 is exit node
	  if (node2 == function.exit)
	    addTransition(new PdaTransition(PdaTransition.EPSILON,
	      node2.getSN(), unmatchedAst, PdaTransition.EPSILON,
	      PdaTransition.EPSILON, PdaTransition.EPSILON));
	}
      }
      nodeHash.clear();
    }

    // add initialStackSymbols
    if (entryFunction == null)
      Util.die("cannot find the entry function " + entryFunctionName);
    else
      initialStackSymbols.add(new Integer(entryFunction.entry.getSN()));
    
    functionHash.clear();
  }

  /*
  public void print()
  {
    Iterator iter; 
    String label;
    Integer key;
    
    super.print();

    // print stack symbols
    iter = getStackLabels().keySet().iterator();
    System.out.println("Stack Symbols");
    while (iter.hasNext())
      System.out.print(getStackLabel((Integer)iter.next()) + "  ");
    System.out.println();
  }
  */

  public void writeTransitions(PrintWriter writer)
  {
    Iterator iter = iterator();
    PdaTransition transition;

    while (iter.hasNext())
    {
      transition = (PdaTransition)iter.next();
      writer.println("t " + getStateLabelString(transition.state0) + " " +
		     getStackLabel(transition.stack0) + " " +
		     getStateLabelString(transition.state1) + " " +
		     getStackLabel(transition.stack1) + " " +
		     getStackLabel(transition.stack2) + " " +
  		     transition.input);
    }
  }

  protected Hashtable stackLabels;

  /**
   * Each element is an Integer, an index to stackLabels
   */
  protected Vector initialStackSymbols;
}

