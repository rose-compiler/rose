package mops;
// $Id: Cfg.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;
import java.util.*;

public class Cfg
{
  public Cfg()
  {
    functions = new Vector();
    dataDecls = new Vector();
    functionDefns = new Vector();
    variableDefns = new Vector();
    variableDecls = new Vector();
    moduleName = "";
  }

  public Vector getFunctions()
  {
    return functions;
  }

  public Vector getDataDecls()
  {
    return dataDecls;
  }
  
  public String getModuleName()
  {
    return moduleName;
  }

  public void clear()
  {
    functions.clear();
    dataDecls.clear();
    moduleName = "";
  }

  /** 
   * Read CFG 
   */
  public void read(String filename) throws IOException
  {
    BufferedReader bufferedReader;
    LineReader reader;
    String str, label;
    int ttype, address;
    Hashtable nodeHash, astTable, globalAstTable;
    Node node, node2;
    Edge edge;
    CfgFunction function;
    Ast ast = null;
    Iterator iter;
    Map.Entry entry;
    Vector list = null;

    clear();

    nodeHash = new Hashtable();
    astTable = new Hashtable();
    globalAstTable = new Hashtable();
    bufferedReader = new BufferedReader(new FileReader(filename));
    reader = new LineReader(bufferedReader);

    moduleName = filename;
    function = null;

    for (str = reader.nextToken();
	 reader.getTtype() != StreamTokenizer.TT_EOF;
	 str = reader.nextToken())
    {
      // skip empty line
      if (str == null)
	continue;
      
      switch(str.charAt(0))
      {
	// function_decl
	case 'f': 
	  if (str.equals("fb"))
	  {
	    if ((label = reader.nextToken()) == null)
	      Util.die("expect a function name", filename, reader.lineno());
	    function = new CfgFunction();
	    function.label = label;
	    function.entry = function.exit = null;
	    function.cfg = this;
	    reader.skipToEol();
	    // Add entries in globalAstTable into astTable (i.e. add global
	    // variable names into local namespace)
	    iter = globalAstTable.entrySet().iterator();
	    while (iter.hasNext())
	    {
	      entry = (Map.Entry)iter.next();
	      astTable.put(entry.getKey(), entry.getValue());
	    }
	    function.ast = Ast.read(reader, astTable);
	  }
	  else if (str.equals("fe"))
	  {
	    if (function.entry == null || function.exit == null)
	    {
	      Util.warn(Util.ALERT, "missing "
			+ (function.entry == null ? "entry " : "")
			+ (function.exit == null ? "exit " : "")
			+ "point in the function " +  function.label, filename,
			reader.lineno());
	    }	      
            functions.add(function);
	    function = null;
	    astTable.clear();
	    nodeHash.clear();
          }
	  else
	    Util.die("unknown syntax " + str, filename, reader.lineno());
	  break;

	// node
	case 'n':
	  if (function == null)
	    Util.die("nodes must be defined within a function", filename,
		     reader.lineno());
  	  node = readNode(reader, nodeHash);
	  if (node == null)
	    Util.die("expect a node address", filename, reader.lineno());
	  label = reader.nextToken();
	  if (label == null)
	    Util.die("expect a node label", filename, reader.lineno());
	  node.setLabel(label);
	  if (str.length() > 1)
	    switch(str.charAt(1))
	    {
	      case 'n':
		if (function.entry == null)
		  function.entry = node;
		else
		  Util.warn(Util.WARNING, "ignore duplicate entry node",
			    filename, reader.lineno());
		break;
		
	      case 'x':
		if (function.exit == null)
		  function.exit = node;
		else
		  Util.warn(Util.WARNING, "ignore duplicate exit node",
			    filename, reader.lineno());
		break;

	      default:
		Util.die("unknown syntax " + str, filename, reader.lineno());
	    }
	  break;

	// edge
	case 'e':
	  if (function == null)
	    Util.die("edges must be defined within a function", filename,
		     reader.lineno());
	  node = readNode(reader, nodeHash);
	  node2 = readNode(reader, nodeHash);
	  address = reader.nextInt();
	  if (node == null || node2 == null)
	    Util.die("expect a node address", filename, reader.lineno());

	  if (address == 0)
	  {
	    Util.die("the address of an AST is zero", filename, reader.lineno());
	  }
	  else if ((ast = (Ast)astTable.get(new Integer(address))) == null)
	  {
	    ast = new Ast();
	    ast.setAddress(address);
	    ast.setKind(Ast.kind_dummy);
	    astTable.put(new Integer(address), ast);
	    //Util.warn("Unresolved AST node address " + address, filename,
	    //      reader.lineno());
	    //System.err.print("U ");
	  }
	  edge = new Edge();
	  edge.setLabel(ast);
	  edge.setNodes(node, node2);
	  break;

        // global declaration
	case 'd':
	  reader.skipToEol();
	  ast = Ast.read(reader, globalAstTable);
	  if (ast == null)
	    Util.warn(Util.ALERT, "expect a global declaration", filename,
		      reader.lineno());
	  else
	    dataDecls.add(ast);
	  break;

	  /*
	// module name
	case 'm':
	  if (moduleName != null)
	    Util.warn("Denied duplicate module name", filename,
		      reader.lineno());
	  else
	  {
            moduleName = reader.nextToken();
	    if (moduleName == null)
	      Util.die("Module name expected", filename, reader.lineno());
	  }
	  break;
	  */
	  
	// function definition
	case 'l':
	  switch(str.charAt(1))
	  {
	    case 'f':
	      list = functionDefns;
	      break;
	      
	    case 'c':
	      list = variableDefns;
	      break;
	      
	    case 'u':
	      list = variableDecls;
	      break;

	    default:
	      Util.die("unknown syntax: " + str, filename, reader.lineno());
	  }
	  
          for (label = reader.nextToken();
	       reader.getTtype() != StreamTokenizer.TT_EOL &&
	       reader.getTtype() != StreamTokenizer.TT_EOF;
	       label = reader.nextToken())
	  {
	    list.add(label);
	  }
	  break;
	  
	default:
	  Util.die("unknown syntax: " + str, filename, reader.lineno());
      }
      // read eol
      if (reader.getTtype() != StreamTokenizer.TT_EOL &&
	  reader.getTtype() != StreamTokenizer.TT_EOF)
      {
        reader.nextToken();
        if (reader.getTtype() != StreamTokenizer.TT_EOL &&
	    reader.getTtype() != StreamTokenizer.TT_EOF)
        {
	  Util.warn(Util.WARNING, "ignore extra words near the end of the line",
		    filename, reader.lineno());
	  reader.skipToEol();
        }
      }
    }

    if (function != null)
      Util.die("expect fe", filename, reader.lineno());

    globalAstTable.clear();
    bufferedReader.close();
  }

  private Node readNode(LineReader reader, Hashtable nodeHash)
    throws IOException
  {
    Node node;
    Integer address;

    address = new Integer(reader.nextInt());
    if (address.intValue() == 0)
      Util.warn(Util.ALERT, "node address is 0");
        
    if ((node = (Node)nodeHash.get(address)) == null)
    {
      node = new Node();
      node.address = address.intValue();
      nodeHash.put(address, node);
    }
    return node;
  }

  private void writeLinkageNames(PrintWriter writer, Vector list, String label)
  {
    int i;
    
    if (list.size() > 0)
    {
      writer.print(label);
      writer.print(' ');
      for (i = 0; i < list.size(); i++)
        Ast.writeEscape(writer, (String)list.get(i));
      writer.print('\n');
    }
  }
    
  /**
   * Write to file
   * Note: Different Ast node within different functions may have the same
   * address as a result of CfgMerge.
   */
  public void write(String filename) throws IOException
  {
    CfgFunction function;
    Vector nodeQueue;
    Node node, node2;
    Edge edge;
    Vector outEdges, inEdges;
    int i, j;
    PrintWriter writer;
    HashSet nodeHash, astHash, globalAstHash;
    Ast ast;
    Iterator iter;

    nodeHash = new HashSet();
    astHash = new HashSet();
    globalAstHash = new HashSet();
    writer = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
    nodeQueue = new Vector();

    /*
    writer.print("m ");
    Ast.writeEscape(writer, getModuleName());
    writer.println();
    */
    
    for (i = 0; i < dataDecls.size(); i++)
    {
      writer.println("d");
      ((Ast)dataDecls.get(i)).write(writer, true, globalAstHash);
      writer.println();
    }
    
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.elementAt(i);
      writer.print("fb ");
      Ast.writeEscape(writer, function.label);
      writer.println();
      // Add entries in globalAstTable into astTable (i.e. add global
      // variable names into local namespace)
      iter = globalAstHash.iterator();
      while (iter.hasNext())
	astHash.add(iter.next());
      function.ast.write(writer, true, astHash);
      astHash.clear();
      writer.println();
      nodeQueue.add(function.entry);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	if (node == function.entry)
	  writer.print("nn");
	else if (node == function.exit)
	  writer.print("nx");
	else
	  writer.print("n");
	writer.print(" ");
	writer.print(node.getAddress());
	writer.print(" ");
	Ast.writeEscape(writer, node.getLabel());
	writer.println();
        outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);
	  node2 = (Node)edge.getDstNode();
	  writer.println("e " + node.getAddress() + " " + node2.getAddress()
			 + " " +
			 ((ast = edge.getLabel()) == null ?
			  0 : ast.getAddress()));
	  if (!nodeHash.contains(node2))
	  {
	    nodeQueue.add(node2);
	    nodeHash.add(node2);
	  }
	}
      }
      writer.println("fe");
      nodeHash.clear();
    }

    writeLinkageNames(writer, functionDefns, "lf");
    writeLinkageNames(writer, variableDefns, "lc");
    writeLinkageNames(writer, variableDecls, "lu");
    
    writer.close();
    nodeHash.clear();
    globalAstHash.clear();
  }

  /**
   * Write to .dot file
   */
  public void writeToDot(String filename, boolean isWriteAst)
    throws IOException
  {
    CfgFunction function;
    Vector nodeQueue;
    Node node, node2;
    Edge edge;
    Vector outEdges, inEdges;
    int i, j;
    PrintWriter writer;
    HashSet nodeHash;
    String label;
    final int maxLabelLength = 24;

    nodeHash = new HashSet();
    nodeQueue = new Vector();
    writer = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
    
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.elementAt(i);
      if (isWriteAst)
        function.ast.writeToDot(writer);

      writer.println("digraph \"" + function.label + "\" {\nsize=\"8,11\";");
      nodeQueue.add(function.entry);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	label = node.getLabel();
	if (label.length() > maxLabelLength)
	  label = "..." + label.substring(label.length() - maxLabelLength + 3);
	writer.print(node.getAddress() + " [label=");
	Ast.writeEscape(writer, label);
	if (node == function.entry)
	  writer.print(", shape=box");
	else if (node == function.exit)
	  writer.print(", shape=trapezium");
	writer.println("];");
        outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);
	  node2 = (Node)edge.getDstNode();
	  if (edge.getLabel() != null)
	    label = edge.getLabel().getKindLabel();
	  else
	    label = "(unmatched)";
	  writer.print(node.getAddress() +  " -> " + node2.getAddress() +
	    " [label=");
	  Ast.writeEscape(writer, label);
	  writer.println("];");
	  if (!nodeHash.contains(node2))
	  {
	    nodeQueue.add(node2);
	    nodeHash.add(node2);
	  }

	}
      }
      writer.println("}");
      nodeHash.clear();
    }

    writer.close();
    nodeQueue.clear();
  }

  // This part deals with compacting a CFG
  /**
   * Implements the phi function in DefUse.  Note: index 0 referes to bottom.
   */
  private static class Phi
  {
    public Phi()
    {
      bitSets = new Vector();
      // index 0 should not be used
      bitSets.add(null);
      indices = new Hashtable();
    }

    public void clear()
    {
      // Util.warn("Phi.clear(): bitSets.size()=" + bitSets.size());
      bitSets.clear();
      indices.clear();
    }

    /**
     * Make a new bit
     */
    public int newValue()
    {
      int index;
      BitSet bitSet;

      index = bitSets.size();
      bitSet = new BitSet(index + 1);
      bitSet.set(index);
      bitSets.add(bitSet);
      indices.put(bitSet, new Integer(index));
      return index;
    }

    /**
     * Join a list of BitSet
     */
    public int join(Vector list)
    {
      BitSet sum, element;
      int i, index;
      Integer indexInt;

      sum = new BitSet();
      for (i = 0; i < list.size(); i++)
      {
	index = ((Integer)list.get(i)).intValue();
	if (index == 0)
	  continue;
	else if (index >= bitSets.size())
	  Util.die("index=" + index + " but bitSets.size()=" + bitSets.size());
	element = (BitSet)bitSets.get(index);
	sum.or(element);
      }
      if (sum.length() == 0)
      {
	Util.warn(Util.ALERT, "join " + list.size() + " bottoms");
	return 0;
      }
      if ((indexInt = (Integer)indices.get(sum)) != null)
	return indexInt.intValue();
      else
      {
	bitSets.add(sum);
	index = bitSets.size() - 1;
	indices.put(sum, new Integer(index));
	return index;
      }
    }

    /**
     * Each element is a BitSet where each bit corresponds to a specific def.
     */
    private Vector bitSets;

    /**
     * Lookup table for elements in bitSets.  Key: an element in bitSet.
     * Value: its index in bitSet
     */
    private Hashtable indices;
  }

  /**
   * Compute DefUse value for each node in a CFG.
   * WARNING: Rely on the assumption that all node.getSN() == 0 initially
   */
  private void defUse(CfgFunction function, Vector asts,
		      Hashtable nameToFunction, CfgFunctionExt[] functionsExt,
		      boolean handleOther)
  {
    Vector nodeQueue, inEdges, outEdges, list;
    NodeExt node;
    Edge edge;
    int i, value, otherValue = 0;
    boolean isModified;
    Phi phi;

    nodeQueue = new Vector();
    node = (NodeExt)function.entry;
    nodeQueue.add(node);
    phi = new Phi();
    list = new Vector();
    // defuse value for the special transition "other"
    if (handleOther)
    {
      otherValue = phi.newValue();
    }
    
    while (nodeQueue.size() > 0)
    {
      node = (NodeExt)nodeQueue.remove(0);
      //System.err.print(node.label);
      inEdges = node.getInEdges();
      for (i = 0; i < inEdges.size(); i++)
      {
	edge = (Edge)inEdges.get(i);
	if (hasUsefulAst(edge, asts, nameToFunction, functionsExt))
	{
	  list.clear();
	  break;
	}
	else if (edge.getSrcNode().getSN() != 0)
	{
	  list.add(new Integer(edge.getSrcNode().getSN()));
	}
      }

      if (
	  // This node has an interesting incoming edge
	  i < inEdges.size() ||
	  // This node is the entry node
	  list.size() == 0)
      {
	if (node.getSN() == 0)
	// First time to visit this node.  Give it a new DefUse value
	{
	  node.setSN(phi.newValue());
	  isModified = true;
	}
	else
	{
	  // Already visited this node.
	  isModified = false;
	}
      }
      else
      {
	if (handleOther)
	{
	  list.add(new Integer(otherValue));
	}
	value = phi.join(list);
	if (value != node.getSN())
	{
	  node.setSN(value);
	  isModified = true;
	}
	else
	{
	  isModified = false;
	}
        list.clear();
      }

      if (isModified)
      // Place all child nodes into work queue
      {
	// System.err.println("  new value: " + node.getSN());
	outEdges = node.getOutEdges();
	for (i = 0; i < outEdges.size(); i++)
	  nodeQueue.add(((Edge)outEdges.get(i)).getDstNode());
      }
      /*
      else
	System.err.println();
      */
    }

    phi.clear();
  }

  /**
   * Extends Node to record a set of numbers as the label.
   * Used in merging nodes
   */
  private static class NodeExt extends Node
  {
    public NodeExt(Node node, String functionLabel)
    {
      // shallow copy from node
      super(node.getInEdges(), node.getOutEdges());
      Integer nodeInt;
      address = node.address;
      nodeSet = new HashSet();
      nodeInt = new Integer(getAddress());
      nodeSet.add(nodeInt);
      this.functionLabel = functionLabel;
      functionSet = null;
      // for debugging
      label = node.label;
    }

    public String getLabel()
    {
      StringBuffer strBuf;
      Iterator iter;
      Integer nodeInt;

      strBuf = new StringBuffer();
      strBuf.append(functionLabel);
      strBuf.append(delimiter);
      iter = nodeSet.iterator();
      while (iter.hasNext())
      {
	nodeInt = (Integer)iter.next();
	//if (nodeInt == entry)
	//strBuf.append('n');
	strBuf.append(nodeInt);
	strBuf.append(delimiter);
      }
      if (functionSet != null)
      {
	iter = functionSet.iterator();
	while (iter.hasNext())
	{
	  strBuf.append('(');
	  strBuf.append(iter.next());
	  strBuf.append(')');
	  strBuf.append(delimiter);
	}
      }
      return strBuf.substring(0, strBuf.length() - 1);
    }

    /**
     * Add the label of another node to this node
     * The function entry node, if any, within the other node is ignored
     */
    public void addLabel(NodeExt node)
    {
      Iterator iter;

      if (node == null)
	return;
      iter = node.nodeSet.iterator();
      while (iter.hasNext())
      {
	nodeSet.add(iter.next());
      }
      if (node.functionSet != null)
      {
	if (functionSet == null)
	  functionSet = new HashSet();
	iter = node.functionSet.iterator();
	while (iter.hasNext())
	{
	  functionSet.add(iter.next());
	}
      }
    }

    public void addFunctionLabel(String label)
    {
      if (functionSet == null)
	functionSet = new HashSet();
      functionSet.add(label);
    }

    // functionSet contains the name of useless functions that were removed.
    private HashSet nodeSet, functionSet;

    private String functionLabel;

    private static char delimiter = ';';
  }

  /**
   * Provides additional information for each CfgFunction
   */
  private class CfgFunctionExt
  {
    /**
     * Whether this function is useful.  A function is useful if it contains
     * useful Asts or if it calls a useful function directly or indirectly.
     */
    public boolean isUseful;

    /**
     * All functions that call this function directly
     */
    public HashSet caller;

    /**
     * All functions that this function calls directly or indirectly
     * (transitive closure)
     */
    public HashSet callee;

    public CfgFunctionExt()
    {
      isUseful = false;
      caller = new HashSet();
      callee = new HashSet();
    }
  }

  /**
   * Insert ast into asts, sorted by ast.kind
   *
   * @return false if ast is already in asts, true otherwise
   */
  private boolean insertAst(Vector asts, Ast ast)
  {
    int i;
    Ast ast2;

    // Ideally, should use binary search
    for (i = 0; i < asts.size(); i++)
      if (((Ast)asts.get(i)).kind >= ast.kind)
	break;

    for (; i < asts.size(); i++)
    {
      ast2 = (Ast)asts.get(i);
      if (ast2.kind > ast.kind)
	break;
      if (ast2.patternMatch(ast))
	return false;
    }

    asts.add(i, ast);

    return true;
  }

  /**
   * Determine if edge.getLabel() is in asts.  asts is sorted by Ast.kind
   */
  private boolean hasUsefulAst(Edge edge, Vector asts, Hashtable nameToFunction,
			       CfgFunctionExt[] functionsExt)
  {
    int i, kind, kind2, start, end, index = -1;
    Ast ast;
    String str;
    Integer indexInt;

    if (asts.size() == 0 || (ast = edge.getLabel()) == null)
      return false;

    if (nameToFunction != null &&
	(str = ast.getFunctionCallName()) != null &&
	(indexInt = (Integer)nameToFunction.get(str)) != null &&
	functionsExt != null && 
	functionsExt[indexInt.intValue()].isUseful)
    // If ast is a call to a useful function, return true
    {
      return true;
    }
    
    // Binary search for matching Ast.kind in asts
    kind = edge.getLabel().kind;
    start = 0;
    end = asts.size() - 1;
    while (start <= end)
    {
      index = (start + end) / 2;
      kind2 = ((Ast)asts.get(index)).kind;
      if (kind < kind2)
	end = index - 1;
      else if (kind > kind2)
	start = index + 1;
      else
	break;
    }

    if (start <= end)
    // Found ast.kind in asts
    {
      // Check all ast in asts whose Ast.kind matches ast.kind
      start = end = index;
      for (start--; start >= 0; start--)
	if (((Ast)asts.get(start)).kind != kind)
	  break;
      start++;
      for (end++; end < asts.size(); end++)
	if (((Ast)asts.get(end)).kind != kind)
	  break;
      end--;

      for (i = start; i <= end; i++)
      {
	ast = (Ast)asts.get(i);
	// This handles pattern variable correctly, because a pattern variable
	// matches anything
	if (edge.getLabel().match(ast, null))
	  break;
      }
      if (i <= end)
	return true;
    }
    
    // Match ast against all pattern Asts.  Be sure to double check with all
    // possible pattern Asts in Ast.java
    for (i = asts.size() - 1; i >= 0; i--)
      if (((Ast)asts.get(i)).kind < Ast.kind_min_pattern)
	break;
    for (i++; i < asts.size(); i++)
      if (edge.getLabel().match((Ast)asts.get(i), null))
	return true;
    
    return false;
  }

  /**
   * If allowed, merge mergedEdge.dstNode into mergedEdge.srcNode and
   * discard the former.
   */
  private boolean mergeNode(Edge mergedEdge, Vector asts, CfgFunction function,
			Hashtable nameToFunction, CfgFunctionExt[] functionsExt)
  {
    Vector inEdges1, inEdges2, outEdges1, outEdges2, deletedEdges;
    //HashSet labels;
    Node node1, node2;
    Edge edge;
    Ast ast;
    int i, j, index;
    String str = null;
    Iterator iter;
    Integer indexInt;
    boolean done;

    node1 = mergedEdge.getSrcNode();
    node2 = mergedEdge.getDstNode();

    // Merge two adjacent nodes only if they have the same DefUse value
    // SN records DefUse value, which was computed by defUse()
    if (node1.getSN() != node2.getSN())
      return false;
    
    // Deny the merge of entry and exit nodes
    if ((node1 == function.entry && node2 == function.exit) ||
	(node1 == function.exit && node2 == function.entry))
      return false;

    // Deny the merge of exit node, because Pda.read() ignores exit node
    if (node1 == function.exit || node2 == function.exit)
      return false;

    //System.err.println("Merging node " + node2.getAddress() + " into node "
    //	       + node1.getAddress() + "ast=" + mergedEdge.getLabel());
    inEdges1 = node1.getInEdges();
    inEdges2 = node2.getInEdges();
    outEdges1 = node1.getOutEdges();
    outEdges2 = node2.getOutEdges();

    /* This is not necessary any more, since the DefUse of node1 and node2
       wouldn't be the same
    for (i = 0; i < inEdges2.size(); i++)
    {
      edge = (Edge)inEdges2.get(i);
      if (edge == mergedEdge)
	continue;
      if (edge.getSrcNode() == node1 &&
	  hasUsefulAst(edge, asts, nameToFunction, functionsExt))
	break;
    }
    if (i < inEdges2.size())
    {
      return false;
    }

    for (i = 0; i < outEdges2.size(); i++)
    {
      edge = (Edge)outEdges2.get(i);
      if (edge.getDstNode() == node1 &&
	  hasUsefulAst(edge, asts, nameToFunction, functionsExt))
	break;
    }
    if (i < outEdges2.size())
    {
      //if (node1.getLabel().equals("main.c:284"))
      //Util.warn("failed because of cycle");
      return false;
    }
    */

    deletedEdges = new Vector();
    
    // Modify incoming edges to node2
    for (i = 0; i < inEdges2.size(); i++)
    {
      edge = (Edge)inEdges2.get(i);
      /*
      if (edge == mergedEdge)
	continue;
      */
      if (edge.getSrcNode() == node1)
      {
	deletedEdges.add(edge);
	edge.setSrcNode(null);
	edge.setDstNode(null);
	done = false;
	for (j = 0; j < outEdges1.size(); j++)
	  if ((Edge)outEdges1.get(j) == edge)
	  {
	    outEdges1.remove(j);
	    done = true;
	    break;
	  }
	if (!done)
	  Util.die("cannot find the edge from SrcNode");
      }
      else
      {
        edge.setDstNode(node1);
        inEdges1.add(edge);
      }
    }

    // Modify outgoing edges from node2
    for (i = 0; i < outEdges2.size(); i++)
    {
      edge = (Edge)outEdges2.get(i);
      if (edge.getDstNode() == node1)
      {
	deletedEdges.add(edge);
	edge.setSrcNode(null);
	edge.setDstNode(null);
	done = false;
	for (j = 0; j < inEdges1.size(); j++)
	  if ((Edge)inEdges1.get(j) == edge)
	  {
	    inEdges1.remove(j);
	    done = true;
	    break;
	  }
	if (!done)
	  Util.die("cannot find the edge from DstNode");
      }
      else
      {
        edge.setSrcNode(node1);
        outEdges1.add(edge);
      }
    }

    // Add the label of node2 to the label of node1
    ((NodeExt)node1).addLabel((NodeExt)node2);

    
    /*
      If any of the deleted edge contains a function call, add the label
      of this function and all of its callees to node1, because call
      to this function and all of its callees between node1 and node2
      are removed from the CFG.  The label of a function is
      represented by the label of its entry node, which happens to be
      the line number of the first line of the function definition.
    */
    for (i = 0; i < deletedEdges.size(); i++)
      if (nameToFunction != null &&
	  (ast = ((Edge)(deletedEdges.get(i))).getLabel()) != null &&
	  (str = ast.getFunctionCallName()) != null &&
	  (indexInt = (Integer)nameToFunction.get(str)) != null)
      {
	//System.err.println("i=" + i + " callee=" + str);
	((NodeExt)node1).addFunctionLabel
	  (((CfgFunction)functions.get(indexInt.intValue())).label);
	iter = functionsExt[indexInt.intValue()].callee.iterator();
	while (iter.hasNext())
	{
	  index = ((Integer)iter.next()).intValue();
	  //System.err.print(((CfgFunction)functions.get(index)).label + ":");
	  ((NodeExt)node1).addFunctionLabel
	    (((CfgFunction)functions.get(index)).label);
	}
	//System.err.println();
      }

    // Remove node2 and mergedEdge
    node2.getInEdges().clear();
    node2.getOutEdges().clear();
    deletedEdges.clear();
    /*
    for (i = outEdges1.size() - 1; i >= 0; i--)
      if ((Edge)outEdges1.get(i) == mergedEdge)
      {
	outEdges1.remove(i);
	// should I break?
	// break;
      }
    mergedEdge.setSrcNode(null);
    mergedEdge.setDstNode(null);

    // Check if exit node is merged.  Entry node cannot be merged
    if (node2 == function.exit)
    {
      function.exit = node1;
    }
    else if (node2 == function.entry)
    {
      function.entry = node1;
    }
    */

    return true;
  }

  /**
   * Compact CFG.
   * <ul>
   * <li>Merge adjacent nodes if they have the same DefUse value</li>
   * <li>Remove useless function definitions.  A useless function is a function
   *     that do not have any useful Asts and do not call any interesting
   *     functions directly or indirectly.  Note that
   *     <code>entryFunction</code> is considered useful</li>
   * <ul>
   */
  public void compact(MetaFsa metaFsa, String entryFunctionName)
  {
    Vector asts, fsas, nodeQueue, inEdges, outEdges;
    CfgFunction function;
    Node node, node2;
    Edge edge;
    Hashtable nameToFunction;
    HashSet nodeHash, callee;
    Fsa fsa;
    Iterator iter, iter2;
    String name, str;
    Ast ast;
    int i, j, k, ret;
    Integer index, index2, index3;
    CfgFunctionExt[] functionsExt;
    CfgFunctionExt functionExt;
    boolean isUseful, isModified;
    NodeExt nodeExt;
    boolean handleOther = false;

    /*
      Get all useful Asts.  Note: variables are NOT instantiated here, but it
      does not matter, since a variable matches anything
    */
    fsas = metaFsa.getFsas();
    asts = new Vector();
    for (i = 0; i < fsas.size(); i++)
    {
      fsa = (Fsa)fsas.get(i);
      switch (ret = fsa.checkTransitions())
      {
	case 0:
	  break;

	case 1:
	  handleOther = true;
	  break;

	case 2:
	  Util.warn(Util.WARNING, "an FSA has consecutive non-self-loop { other } transitions, which will break CfgCompact");
	  handleOther = true;
	  break;

	case 3:
	  Util.warn(Util.WARNING, "an FSA has an { any } transition, which will break CfgCompact");
	  break;

	default:
	  Util.die("unknown return value from Fsa.checkTransitions(): " +
		   ret);
	  
      }
      // System.err.println("ret=" + ret);
      iter = fsa.iterator();
      while (iter.hasNext())
      {
	ast = (Ast)((FsaTransition)iter.next()).input;
	// Warning: assume kind_other is insignificant
	if (ast.kind != Ast.kind_other)
	  insertAst(asts, ast);
      }
    }

    // Index each function name.  functionsExt[] will be used to hold extra
    // information about each function later.
    nameToFunction = new Hashtable();
    functionsExt = new CfgFunctionExt[functions.size()];
    for (i = 0; i < functionsExt.length; i++)
    {
      nameToFunction.put(((CfgFunction)functions.get(i)).label, new Integer(i));
      functionsExt[i] = new CfgFunctionExt();
    }

    // Change every Node into NodeExt while keeping the graph structure of CFG
    // This is a hack!
    nodeHash = new HashSet();
    nodeQueue = new Vector();
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      nodeQueue.add(function.entry);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	nodeExt = new NodeExt(node, function.label);
	inEdges = node.getInEdges();
	for (j = 0; j < inEdges.size(); j++)
	{
	  edge = (Edge)inEdges.get(j);
	  edge.setDstNode(nodeExt);
	}
	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);
	  edge.setSrcNode(nodeExt);
	  node2 = (Node)(edge.getDstNode());
	  if (!(node2 instanceof NodeExt) && !nodeHash.contains(node2))
          {
	    nodeQueue.add(node2);
	    nodeHash.add(node2);
	  }
	}
	if (node == function.exit)
	  function.exit = nodeExt;
	else if (node == function.entry)
	  function.entry = nodeExt;
      }
      if (!(function.entry instanceof NodeExt &&
	    function.exit instanceof NodeExt))
	Util.warn(Util.ALERT,
		  (function.entry instanceof NodeExt ? "" : "entry ") +
		  (function.exit instanceof NodeExt ? "" : "exit ") +
		  "node is not changed into nodeExt in function " +
		  function.label);

      nodeHash.clear();
    }

    // find out
    // 1. Which function has useful Ast
    // 2. The callers and callees of each function
    nodeHash.clear();
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      nodeQueue.add(function.entry);
      nodeHash.add(function.entry);
      // the entry function is always useful.  Never remove it.
      isUseful = Pda.matchFunctionName(function.label, entryFunctionName);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);
	  if (edge.getLabel() != null)
	  {
	    if (!isUseful && hasUsefulAst(edge, asts, null, null))
	      isUseful = true;
	    if ((str = edge.getLabel().getFunctionCallName()) != null &&
		(index = (Integer)nameToFunction.get(str)) != null)
	    {
	      functionsExt[index.intValue()].caller.add(new Integer(i));
	      functionsExt[i].callee.add(index);
	    }
	  }
	  node2 = edge.getDstNode();
	  if (!nodeHash.contains(node2))
	  {
	    nodeQueue.add(node2);
	    nodeHash.add(node2);
	  }
	}
      }
      
      functionsExt[i].isUseful = isUseful;
      nodeHash.clear();
    }

    // Compute the transitive closure of useful functions.  A function is
    // useful if it calls a useful function directly or indirectly
    for (i = 0; i < functionsExt.length; i++)
    {
      if (functionsExt[i].isUseful)
	nodeQueue.add(new Integer(i));
    }
    while (nodeQueue.size() > 0)
    {
      index = (Integer)nodeQueue.remove(0);
      functionExt = (CfgFunctionExt)functionsExt[index.intValue()];
      iter = functionExt.caller.iterator();
      while (iter.hasNext())
      {
	index = (Integer)iter.next();
	if (!functionsExt[index.intValue()].isUseful)
	{
	  functionsExt[index.intValue()].isUseful = true;
	  nodeQueue.add(index);
	}
      }
    }

    // Computer transitive closure of callee
    nodeQueue.clear();
    nodeHash.clear();
    for (i = 0; i < functionsExt.length; i++)
      if (functionsExt[i].callee.size() > 0)
      {
	index = new Integer(i);
	nodeQueue.add(index);
	nodeHash.add(index);
      }
    
    while (nodeQueue.size() > 0)
    {
      // this function
      index = (Integer)nodeQueue.remove(0);
      nodeHash.remove(index);
      iter = functionsExt[index.intValue()].caller.iterator();
      while (iter.hasNext())
      {
	// caller to this function
	index2 = (Integer)iter.next();
	// caller's callee list
	callee = functionsExt[index2.intValue()].callee;
	iter2 = functionsExt[index.intValue()].callee.iterator();
	isModified = false;
	while (iter2.hasNext())
	{
	  // One callee of this function
	  index3 = (Integer)iter2.next();
	  if (!callee.contains(index3))
	  {
	    callee.add(index3);
	    isModified = true;
	  }
	}
	if (isModified && !nodeHash.contains(index2))
	{
	  nodeQueue.add(index2);
	  nodeHash.add(index2);
	}
      }
    }

    /*
    nodeHash.clear();
    nodeQueue.clear();

    newNode = new HashSet();
    while (nodeQueue.size() > 0)
    {
      index = (Integer)nodeQueue.remove(0);
      nodeHash.remove(index);
      callee = functionsExt[index.intValue()].callee;
      iter = callee.iterator();
      while (iter.hasNext())
      {
	iter2 = functionsExt[((Integer)iter.next()).intValue()].callee.iterator();
	while (iter2.hasNext())
	{
	  index2 = (Integer)iter2.next();
	  if (!callee.contains(index2))
	  {
	    newNode.add(index2);
	  }
	}
      }
      if (newNode.size() > 0)
      {
	iter = newNode.iterator();
	while (iter.hasNext())
	  callee.add(iter.next());
	if (!nodeHash.contains(index))
	{
	  nodeHash.add(index);
	  nodeQueue.add(index);
	}
	newNode.clear();
      }
    }
    */
    
    // Compact CFG and remove useless functions
    nodeHash.clear();
    for (i = 0; i < functions.size(); i++)
    {
      // Don't compact useless CfgFunction because it will be thrown away
      if (!functionsExt[i].isUseful)
	continue;

      function = (CfgFunction)functions.get(i);
      defUse(function, asts, nameToFunction, functionsExt, handleOther);
      //System.err.println("compacting " + function.label);

      nodeQueue.add(function.entry);
      nodeHash.add(function.entry);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
        outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  edge = (Edge)outEdges.get(j);

	  /*
	  if (edge.getLabel() != null &&
	      (name = edge.getLabel().getFunctionCallName()) != null &&
	      deadFunctions.contains(name))
	  // Delete dead function calls.  Dead function calls never return
	  {
	    outEdges.remove(j);
	    // Because the next edge fills in the position of the removed one.
	    j--;
	    inEdges = edge.getDstNode().getInEdges();
	    isModified = false;
	    for (k = 0; k < inEdges.size(); k++)
	      if (((Edge)inEdges.get(k)) == edge)
	      {
		inEdges.remove(k);
		isModified = true;
		break;
	      }
	    if (!isModified)
	      Util.die("Cannot find edge from DstNode");
	    edge.setSrcNode(null);
	    edge.setDstNode(null);
	  }
	  else
	  */
	  {
	    if (mergeNode(edge, asts, function, nameToFunction, functionsExt))
	    // Because the next edge fills in the position of the removed one.
	      j--;
	    else
	    {
	      node2 = edge.getDstNode();
	      if (!nodeHash.contains(node2))
	      {
		nodeQueue.add(node2);
		nodeHash.add(node2);
	      }
	    }
	  }
	}
      }
      nodeHash.clear();
    }

    // remove useless functions from this.functions
    for (i = functions.size() - 1; i >= 0; i--)
      if (!functionsExt[i].isUseful)
	functions.remove(i);

    nameToFunction.clear();
  }

  /**
   * Find external (function and variable) name reference
   * information from CFGs
   */
  private static void findExternalDefinitions
    (Cfg[] cfgs, Hashtable functionDefns, Hashtable variableDefns)
  {
    int i, j, k;
    String label;
    HashSet hashSet;
    CfgFunction function;
    Cfg cfg;
    Ast dataDecl, variableDecl, ast;

    hashSet = new HashSet();
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      // functions
      for (j = 0; j < cfg.functionDefns.size(); j++)
      {
	hashSet.add(cfg.functionDefns.get(j));
      }
      for (j = 0; j < cfg.functions.size(); j++)
      {
	function = (CfgFunction)cfg.functions.get(j);
	label = function.label;
	if (hashSet.contains(label))
	{
	  if (functionDefns.containsKey(label))
	    Util.warn(Util.ALERT, "ignore duplicate function definition "
		      + label + " in module " + cfg.getModuleName());
	  else
	    functionDefns.put(label, function);
	  hashSet.remove(label);
	}
      }
      if (hashSet.size() > 0)
      {
	Util.warn(Util.ALERT, "cannot find the definitions of some external functions in module " + cfg.getModuleName());
	hashSet.clear();
      }

      // variables
      for (j = 0; j < cfg.variableDefns.size(); j++)
      {
	hashSet.add(cfg.variableDefns.get(j));
      }
      for (j = 0; j < cfg.dataDecls.size(); j++)
      {
	dataDecl = (Ast)cfg.dataDecls.get(j);
	for (k = 0; k < dataDecl.getChildren().size(); k++)
	{
	  variableDecl = (Ast)dataDecl.getChildren().get(k);
	  // sanity check
	  if (variableDecl.getChildren().size() < 2 ||
	      (ast = (Ast)variableDecl.getChildren().get(1)).getKind()
	      != Ast.kind_declarator ||
	      ast.getChildren().size() < 1 ||
	      !(ast.getChildren().get(0) instanceof String))
	    Util.warn(Util.ALERT,
		      "ignored invalid variable declaration: " + variableDecl);
	  else
	  {
	    label = (String)ast.getChildren().get(0);
	    if (hashSet.contains(label))
	    {
	      if (variableDefns.containsKey(label))
		Util.warn(Util.ALERT,
			  "ignored duplicate variable declaration " + label +
			  " in module " + cfg.getModuleName());
	      else
		variableDefns.put(label, variableDecl.getChildren().get(0));
	      hashSet.remove(label);
	    }
	  }
	}
      }
      if (hashSet.size() > 0)
      {
	Util.warn(Util.ALERT, "cannot find the definitions of some external variables in module " + cfg.getModuleName());
	hashSet.clear();
      }
    }
  }
  
  /**
   * Resolve references to global names (variables and functions) in the AST
   * of a CFG.
   */
  private static void linkExternals(Ast rootAst, Hashtable localFunctionDefns,
				Hashtable externFunctionDefns)
  {
    Vector nodeQueue;
    HashSet nodeHash;
    Ast ast, identifier;
    Object object;
    String name;
    CfgFunction function;
    int i;

    nodeQueue = new Vector();
    nodeHash = new HashSet();
    nodeQueue.add(rootAst);
    nodeHash.add(rootAst);
    while (nodeQueue.size() > 0)
    {
      ast = (Ast)nodeQueue.remove(0);
      switch (ast.getKind())
      {
	case Ast.kind_identifier:
	  // Resolve an identifier pointing to a global variable.
	  if (ast.getChildren().size() >= 2 &&
	      (object = ast.getChildren().get(1)) instanceof Ast &&
	      ((Ast)object).getKind() == -1)
	  // object.kind == -1 indicates that object is in the AST of a
	  // global variable that is defined in another module
	  // (object.kind was set to -1 in merge())
	  {
	    ast.getChildren().set(1, ((Ast)object).getChildren().get(0));
	  }
	  break;

	case Ast.kind_function_call:
	  // Resolve calls to external functions
	  identifier = (Ast)ast.children.get(0);
	  if (identifier.kind == Ast.kind_identifier)
          {
            name = (String)identifier.children.get(0);
	    if ((function = (CfgFunction)localFunctionDefns.get(name)) == null
		&&
		(function = (CfgFunction)externFunctionDefns.get(name)) == null)
	    {
	      // Util.warn("Cannot resolve function " + name);
	    }
	    else
	    {
	      // At this moment, function.label has been manged in merge()
	      identifier.children.set(0, function.label);
	    }
	  }
	  // fall through to examine all parameters

	default:
	  for (i = 0; i < ast.children.size(); i++)
	    if ((object = ast.children.get(i)) instanceof Ast)
	      if (!nodeHash.contains(object))
	      {
		nodeQueue.add(object);
		nodeHash.add(object);
	      }
      }
    }

    nodeHash.clear();
  }

  private static int getUniqueAddress
    (HashSet allAddresses, HashSet localAddresses, int address,
     Random random)
  {
    Integer addressInt;
    int count, value;

    addressInt = new Integer(address);
    if (allAddresses.contains(addressInt))
    {
      count = 0;
      do
      {
	value = random.nextInt();
	if (value < 0)
	  value = -value;
	else if (value == 0)
	  value = 1;
        addressInt = new Integer(value);
	count++;
      }
      while (allAddresses.contains(addressInt) ||
	     localAddresses.contains(addressInt));
      //System.err.println("Random: " + address + " -> " + addressInt
      //		 + "  count=" + count);
      return addressInt.intValue();
    }
    else
    {
      if (!localAddresses.contains(addressInt))
	localAddresses.add(addressInt);
      return address;
    }
  }

  /*
   * Change colliding addresses among Ast objects and Node objects
   * from different Cfgs.  Within a Cfg, no two Ast objects have the
   * same address.  Within a function, no two Node objects have the
   * same address.  (An Ast object and a Node object may have the same
   * address, but this is ok since they are in separate address
   * spaces.)  When we merge several Cfgs, we must ensure that in the
   * merged Cfg no two Ast objects have the same address, and no two
   * Node objects have the same address.
   */
  private static void uniquefyAddresses(Cfg[] cfgs)
  {
    Ast ast;
    Vector nodeQueue, outEdges, functions, dataDecls;
    HashSet nodeHash, allAddresses, localAddresses;
    Object object;
    int i, j, k, address;
    Random random;
    Node node, node2;
    Cfg cfg;
    CfgFunction function;
    Iterator iter;

    random = new Random();
    nodeQueue = new Vector();
    nodeHash = new HashSet();
    allAddresses = new HashSet();
    localAddresses = new HashSet();

    // Uniquefy Ast objects
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      functions = cfg.getFunctions();
      dataDecls = cfg.getDataDecls();
      for (j = 0; j < functions.size() + dataDecls.size(); j++)
      {
	if (j < functions.size())
	{
	  ast = ((CfgFunction)functions.get(j)).ast;
	}
	else
	{
	  ast = (Ast)dataDecls.get(j - functions.size());
	}
	nodeQueue.add(ast);
	nodeHash.add(ast);
	while (nodeQueue.size() > 0)
	{
	  ast = (Ast)nodeQueue.remove(0);
	  address = getUniqueAddress(allAddresses, localAddresses,
				     ast.getAddress(), random);
	  if (address != ast.getAddress())
	    ast.setAddress(address);
	  if (ast.children != null)
	    for (k = 0; k < ast.children.size(); k++)
	    {
	      object = ast.children.get(k);
	      if (object instanceof Ast && !nodeHash.contains(object))
	      {
	        nodeQueue.add(object);
   	        nodeHash.add(object);
	      }
	    }
	}
	nodeHash.clear();
      }
      // merge localAddresses into allAddresses
      iter = localAddresses.iterator();
      while (iter.hasNext())
      {
	allAddresses.add(iter.next());
      }
      localAddresses.clear();
    }
    allAddresses.clear();

    // uniquefy Node objects
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      functions = cfg.getFunctions();
      for (j = 0; j < functions.size(); j++)
      {
	function = (CfgFunction)functions.get(j);
	node = function.entry;
	nodeQueue.add(node);
	nodeHash.add(node);
	while (nodeQueue.size() > 0)
	{
	  node = (Node)nodeQueue.remove(0);
	  address = getUniqueAddress(allAddresses, localAddresses,
				     node.getAddress(), random);
	  if (address != node.getAddress())
	    node.setAddress(address);

	  outEdges = node.getOutEdges();
	  for (k = 0; k < outEdges.size(); k++)
	  {
	    node2 = ((Edge)outEdges.get(k)).getDstNode();
	    if (!nodeHash.contains(node2))
	    {
	      nodeQueue.add(node2);
	      nodeHash.add(node2);
	    }
	  }
	}
	nodeHash.clear();
        iter = localAddresses.iterator();
        while (iter.hasNext())
        {
	  allAddresses.add(iter.next());
        }
        localAddresses.clear();
      }
    }
    allAddresses.clear();
  }

  /**
   * Merge CFGs.  Resolve cross references of global variables and functions
   *
   * @param crefFileName Cross reference file generated by ld
   */
  public static Cfg merge(String[] cfgFileNames)
    throws IOException
  {
    Cfg[] cfgs;
    BufferedReader reader;
    Cfg cfg, newCfg;
    CfgFunction function;
    Ast dataDecl, variableDecl, dataDeclaration, ast;
    String str, symbolName, fileName, name = null;
    Map.Entry entry;
    Object object;
    int i, j, k, l;
    Hashtable externFunctionDefns, externVariableDefns;
    HashSet externVariableDecls;
    Hashtable[] localFunctionDefns;

    // read all cfgs
    cfgs = new Cfg[cfgFileNames.length];
    for (i = 0; i < cfgs.length; i++)
    {
      cfgs[i] = new Cfg();
      cfgs[i].read(cfgFileNames[i]);
    }

    // Scan Cfgs.  Resolve external names
    externFunctionDefns = new Hashtable();
    externVariableDefns = new Hashtable();
    externVariableDecls = new HashSet();
    localFunctionDefns = new Hashtable[cfgs.length];
    for (i = 0; i < localFunctionDefns.length; i++)
      localFunctionDefns[i] = new Hashtable();

    findExternalDefinitions(cfgs, externFunctionDefns, externVariableDefns);
    // Collect all locally defined functions.
    // Note: this must be done after findExternalDefinitions()
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      for (j = 0; j < cfg.functions.size(); j++)
      {
	function = (CfgFunction)cfg.functions.get(j);
	localFunctionDefns[i].put(function.label, function);
	//mangle function names to make each of them unique
	function.label = i + ":" + function.label;
      }
    }
    
    // Link external variable references 
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      for (j = 0; j < cfg.variableDecls.size(); j++)
	externVariableDecls.add(cfg.variableDecls.get(j));
      
      // resolve global variables.  Remove variable declarations
      for (j = cfg.dataDecls.size() - 1; j >= 0; j--)
      {
	// data_decl contains variable_decl contains data_declaration
	dataDecl = (Ast)cfg.dataDecls.get(j);
        for (k = dataDecl.getChildren().size() - 1; k >= 0; k--)
	  if ((object = dataDecl.getChildren().get(k)) instanceof Ast &&
	      ((Ast)object).getKind() == Ast.kind_variable_decl)
	  {
	    variableDecl = (Ast)object;
	    name = null;
	    dataDeclaration = null;
            for (l = 0; l < variableDecl.getChildren().size(); l++)
	      if ((object = variableDecl.getChildren().get(l)) instanceof Ast)
	      {
		ast = (Ast)object;
		switch(ast.getKind())
		{
		  case Ast.kind_data_declaration:
		    dataDeclaration = ast;
		    break;

		  case Ast.kind_declarator:
		    name = (String)ast.children.get(0);
		    break;

		  case -1:
		    dataDecl.children.remove(k);
		    break;

		    /*
		  case Ast.kind_lexical_cst:
		    break;
		    
		  default:
		    Util.warn("Unexpected kind " + ast.getKind() + ": "
			     + variableDecl);
		    */
		}
	      }
	    
	    if (name != null && dataDeclaration != null
	      /*
	    {
	      Util.warn((name == null ? "declarator " : "") +
			(dataDeclaration == null ? "data_declaration " : "") +
	                " not found inside variable_decl in module "
			+ cfg.getModuleName() + ": " + variableDecl);
	    }
	      */
		&& externVariableDecls.contains(name))
	    // This is an external variable declaration.  It should be removed
	    {
	      if ((ast = (Ast)externVariableDefns.get(name)) == null)
		Util.warn(Util.ALERT,
			  "cannot find definition for external variable "
			  + name + " in module " + cfg.getModuleName());
	      else
	      {
		// remove this variable_decl from parent data_decl
		dataDecl.children.remove(k);
		// mark the kind of data_declaration as -1 to indicate that
		// it should be resolved later.  Cannot resolve it now
		// because we may not have found its definition yet.  Record
		// its entry in crossRefs.
		dataDeclaration.kind = -1;
		dataDeclaration.children.clear();
		dataDeclaration.children.add(ast);
	      }
	    }
	  }
	// If all variable_decl inside data_decl have been removed
	if (dataDecl.children.size() == 0)
	  cfg.dataDecls.remove(j);
      }

      // modify cfgs
      for (j = 0; j < cfg.functions.size(); j++)
      {
	function = (CfgFunction)cfg.functions.get(j);
	linkExternals(function.ast, externFunctionDefns, localFunctionDefns[i]);
      }

      // modify dataDecls
      for (j = 0; j < cfg.dataDecls.size(); j++)
      {
	dataDecl = (Ast)cfg.dataDecls.get(j);
	linkExternals(dataDecl, externFunctionDefns, localFunctionDefns[i]);
      }

      externVariableDecls.clear();
    }

    for (i = 0; i < localFunctionDefns.length; i++)
      localFunctionDefns[i].clear();
    externFunctionDefns.clear();

    // merge Cfgs
    uniquefyAddresses(cfgs);
    newCfg = new Cfg();
    for (i = 0; i < cfgs.length; i++)
    {
      cfg = cfgs[i];
      for (j = 0; j < cfg.functions.size(); j++)
	newCfg.functions.add(cfg.functions.get(j));
      for (j = 0; j < cfg.dataDecls.size(); j++)
	newCfg.dataDecls.add(cfg.dataDecls.get(j));
    }

    for (i = 0; i < cfgs.length; i++)
    {
      cfgs[i].clear();
      cfgs[i] = null;
    }
    return newCfg;
  }

  // This part deals with backtrack node
  private class PathInfo
  {
    public int distance;
    public Node before;
    public PathInfo()
    {
      distance = -1;
      before = null;
    }
  }
  
  /**
   * Find a shortest path starting with entryNode.  All nodes on the
   * path must come from nodeSet (however not all nodes in nodeSet
   * have to be on the path) and the last node must have an outgoing
   * edge whose ast is astInt.  A node cannot appear more than once on
   * the path.
   *
   * @return the next entry node (the dstNode of ast)
   */
  private Node backtrackNode(HashSet nodeSet, Node entryNode, int astInt,
			     Vector path)
  {
    Vector nodeQueue, outEdges;
    HashSet nodeHash, touchedNodes;
    Node node, node2, exitNode, nextEntryNode;
    PathInfo pathInfo, pathInfo2;
    Edge edge;
    Iterator iter;
    int i;

    touchedNodes = new HashSet();
    nodeQueue = new Vector();
    nodeHash = new HashSet();
    node = entryNode;
    if (node.data == null)
    {
      pathInfo = new PathInfo();
      node.data = pathInfo;
    }
    else
      pathInfo = (PathInfo)node.data;
    touchedNodes.add(pathInfo);
    pathInfo.distance = 0;
    pathInfo.before = null;
    nodeQueue.add(node);
    nodeHash.add(node);
    exitNode = null;
    nextEntryNode = null;
    
    while (nodeQueue.size() > 0)
    {
      node = (Node)nodeQueue.remove(0);
      nodeHash.remove(node);
      outEdges = node.getOutEdges();
      pathInfo = (PathInfo)node.data;
      for (i = 0; i < outEdges.size(); i++)
      {
	edge = (Edge)outEdges.get(i);
	node2 = edge.getDstNode();
	if (edge.label.address == astInt &&
	    !nodeSet.contains(new Integer(node2.address)))
	  // fix me: should also confirm that node2 belongs to the next nodeset
	{
	  if (exitNode == null)
	  {
	    exitNode = node;
	    nextEntryNode = node2;
	  }
	  else if (exitNode != node || nextEntryNode != node2)
	    Util.warn(Util.ALERT, "ignore duplicate exitNode or nextEntryNode.  Already have exitNode=" + exitNode.getAddress() + " nextEntryNode=" + nextEntryNode.getAddress() + ".  New: exitNode=" + node.getAddress() + " nextEntryNode=" + node2.getAddress() + " ast=" + astInt);
	}
	if (nodeSet.contains(new Integer(node2.address)))
	{
	  if (node2.data == null)
	  {
	    node2.data = new PathInfo();
	  }
	  pathInfo2 = (PathInfo)node2.data;
	  if (pathInfo2.distance == -1 ||
	      pathInfo2.distance > (pathInfo.distance + 1))
	  {
	    pathInfo2.distance = pathInfo.distance + 1;
	    pathInfo2.before = node;
	    if (!nodeHash.contains(node2))
	    {
	      nodeQueue.add(node2);
	      nodeHash.add(node2);
	    }
	    touchedNodes.add(pathInfo2);
	  }
	}
      }
    }

    nodeHash.clear();
    if (exitNode != null)
    {
      node = exitNode;
      while (node != null)
      {
	path.add(0, node);
	pathInfo = (PathInfo)node.data;
	node = pathInfo.before;
      }
    }

    iter = touchedNodes.iterator();
    while (iter.hasNext())
    {
      pathInfo = (PathInfo)iter.next();
      pathInfo.distance = -1;
      pathInfo.before = null;
    }
    return nextEntryNode;
  }
  
  private void skipToEol(LineReader reader, String inputFilename)
    throws IOException
  {
    reader.nextToken();
    if (reader.getTtype() != StreamTokenizer.TT_EOL &&
	reader.getTtype() != StreamTokenizer.TT_EOF)
    {
      Util.warn(Util.WARNING, "ignore extra words on line ", inputFilename, 
		reader.lineno());
      reader.skipToEol();
    }
  }
  
  /**
   * Input: a trace from the compacted CFG
   * Output: the corresponding trace in the original CFG in the format
   * of emacs compilation mode (can use "goto-next-error" to go to next line)
   */
  public void transformPath(String inputFilename, String outputFilename)
    throws IOException
  {
    CfgFunction function;
    StringTokenizer st;
    Hashtable entries;
    HashSet nodeSet;
    Vector callStack, path;
    Node entryNode, nextEntryNode;
    Vector outEdges;
    String str, str2, state, functionLabel;
    Iterator iter;
    int i, ast, depth, index;
    BufferedReader bufferedReader;
    LineReader reader;
    PrintWriter writer;
    Integer addressInt, nodeInt;
    char ch;

    writer = new PrintWriter(new BufferedWriter(new FileWriter(outputFilename)));

    // hash all function labels
    entries = new Hashtable();
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      if (entries.containsKey(function.label))
      {
	Util.warn(Util.ALERT, "ignore duplicate function label " + function.label);
      }
      else
        entries.put(function.label, function.entry);
    }

    nodeSet = new HashSet();
    callStack = new Vector();
    path = new Vector();
    entryNode = null;
    depth = 0;
    
    writer.println("-*- compilation -*-\nTrace from " + inputFilename);
    bufferedReader = new BufferedReader(new FileReader(inputFilename));
    reader = new LineReader(bufferedReader);

    str = reader.nextToken();
    while (reader.getTtype() != StreamTokenizer.TT_EOF)
    {
      ch = str.charAt(0);
      if (ch == '(' || ch == ')')
      {
	switch (ch)
	{
	  case '(':
	    callStack.add(entryNode);
	    entryNode = null;
	    depth++;
	    break;

	  case ')':
	    if (callStack.size() <= 0)
	      Util.die("the call stack is empty when returning from a function call",
		       inputFilename, reader.lineno());
	    entryNode = (Node)callStack.remove(callStack.size() - 1);
	    depth--;
	    break;
	}
	skipToEol(reader, inputFilename);
	str = reader.nextToken();
	continue;
      }
	  
      if (str.charAt(0) != 't')
	Util.die("expect a transition", inputFilename, reader.lineno());

      state = reader.nextToken();
      str = reader.nextToken();
      skipToEol(reader, inputFilename);
      //System.err.println(str);
      // find function label
      index = str.indexOf(';');
      if (index == -1)
	Util.die("cannot find a function label", inputFilename, reader.lineno());
      functionLabel = str.substring(0, index);
      str = str.substring(index + 1);
      
      // break node addresses separated by ';'
      st = new StringTokenizer(str, ";");
      while (st.hasMoreTokens())
      {
	str2 = st.nextToken();
	// ignore function names in brackets because they are useless functions
	// We don't backtrack useless functions
	if (str2.charAt(0) != '(')
	{
	//if (str2.charAt(0) == 'n')
	//{
	// str2 = str2.substring(1);
	//nodeInt = new Integer(str2);
	//if (entryNodeInt == null)
	//  entryNodeInt = nodeInt;
	//else
	//  Util.warn("Duplicate entry node " + nodeInt + " in " + str);
	//}
	//else
	//{
	  nodeInt = new Integer(str2);
	//}
	  nodeSet.add(nodeInt);
	}
      }
      
      if (entryNode == null)
      // enter a function call
      {
	if ((entryNode = (Node)entries.get(functionLabel)) == null)
	  Util.die("cannot find the real entry node inside a composite function entry node",
		   inputFilename, reader.lineno());
      }

      // Read AST
      if ((str = reader.nextToken()) == null || str.charAt(0) == ')')
	// last transition in current function
      {
	writer.println(entryNode.getLabel() + ": <" + state + ">  " + depth);
	continue;
      }
      
      if (!(str.charAt(0) == 'a'))
	Util.die("Expect an AST", inputFilename, reader.lineno());
      ast = reader.nextInt();
      skipToEol(reader, inputFilename);

      if ((nextEntryNode =
	   backtrackNode(nodeSet, entryNode, ast, path)) == null)
      {
	Util.die("backtrackNode() failed: entryNode=" + entryNode.getAddress()
		 + " ast=" + ast);
      }
      
      for (i = 0; i < path.size(); i++)
	writer.println(((Node)path.get(i)).getLabel() + ": <" + state + ">  "
		       + depth);

      /*
      outEdges = ((Node)path.get(path.size() - 1)).getOutEdges();
      for (i = 0; i < outEdges.size(); i++)
      {
	if (((Ast)((Edge)outEdges.get(i)).getLabel()).address == ast)
	  break;
      }
      if (i >= outEdges.size())
	Util.die("Cannot find entry node of a composite node");
      entryNode = ((Edge)outEdges.get(i)).getDstNode();
      */
      entryNode = nextEntryNode;
      // debug
      // writer.flush();
      
      path.clear();
      nodeSet.clear();
      str = reader.nextToken();
    }

    entries.clear();
    bufferedReader.close();
    writer.close();
  }

  /**
   * "Show me all statements that may be executed in certain state"
   * Input: output from Fsa.writeInitialTransitions() on compacted CFG
   * Output: transformed into filename:lineno in this (uncompacted) CFG
   */
  public void transformTransitions(String inputFilename, String outputFilename)
    throws IOException
  {
    BufferedReader reader;
    PrintWriter writer;
    String str, moduleName, label;
    Node node, node2;
    CfgFunction function;
    StringTokenizer st;
    HashSet nodeSet, nodeHash, hashSet, positions;
    Vector nodeQueue, outEdges, list;
    HashMap positionsPerFunc;
    Hashtable positionsPerModule;
    Integer nodeInt, lineNumber;
    Iterator iter, iter2;
    Map.Entry entry;
    int i, j, index;

    // Sort all transitions by function name
    positionsPerFunc = new HashMap();
    reader = new BufferedReader(new FileReader(inputFilename));
    while ((str = reader.readLine()) != null)
    {
      index = str.indexOf(';');
      label = str.substring(0, index);
      if (positionsPerFunc.containsKey(label))
      {
	if ((positions = (HashSet)positionsPerFunc.get(label)) == null)
	{
	  Util.warn(Util.ALERT,
		    "ignore the following transition because the function"
		    + label + " is useless and has been removed: " + str);
	  continue;
	}
      }
      else
      {
	positions = new HashSet();
	positionsPerFunc.put(label, positions);
      }
      st = new StringTokenizer(str.substring(index + 1), ";");
      while (st.hasMoreTokens())
      {
	str = st.nextToken();
	if (str.charAt(0) == '(')
	{
	  label = str.substring(1, str.length() - 1);
	  if (positionsPerFunc.containsKey(label))
	  {
	    if (positionsPerFunc.get(label) != null)
	      Util.warn(Util.ALERT, "ignore the useless function " + label +
			" because I have seen its transitions");
	  }
	  else
	    positionsPerFunc.put(label, null);
	}
	else
	  positions.add(new Integer(str));
      }
    }
    reader.close();

    // turn each transition into module name and line numbers
    positionsPerModule = new Hashtable();
    nodeQueue = new Vector();
    nodeHash = new HashSet();
    // traverse CFG
    for (i = 0; i < functions.size(); i++)
    {
      function = (CfgFunction)functions.get(i);
      if (!positionsPerFunc.containsKey(function.label))
	continue;
      positions = (HashSet)positionsPerFunc.get(function.label);
      node = function.entry;
      nodeQueue.add(node);
      nodeHash.add(node);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	nodeInt = new Integer(node.getAddress());
        if (positions == null || positions.contains(nodeInt))
	{
	  st = new StringTokenizer(node.getLabel(), ":");
	  moduleName = st.nextToken();
	  lineNumber = new Integer(st.nextToken());
	  if ((hashSet = (HashSet)positionsPerModule.get(moduleName)) == null)
	  {
	    hashSet = new HashSet();
	    positionsPerModule.put(moduleName, hashSet);
	  }
	  hashSet.add(lineNumber);
	  if (positions != null)
	    positions.remove(nodeInt);
	}
	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  node2 = ((Edge)outEdges.get(j)).getDstNode();
	  if (!nodeHash.contains(node2))
	  {
	    nodeHash.add(node2);
	    nodeQueue.add(node2);
	  }
	}
      }
      nodeHash.clear();
      if (positions != null && positions.size() > 0)
      {
	StringBuffer sb = new StringBuffer();
	iter = positions.iterator();
	while (iter.hasNext())
	{
	  sb.append((Integer)iter.next());
	  sb.append(' ');
	}
	Util.warn(Util.ALERT, "cannot dereference these node addresses: "
		  + sb.toString());
      }
      positionsPerFunc.remove(function.label);
    }

    if (positionsPerFunc.keySet().size() > 0)
    {
      StringBuffer sb = new StringBuffer();
      iter = positionsPerFunc.keySet().iterator();
      while (iter.hasNext())
      {
	sb.append(iter.next());
	sb.append(' ');
      }
      Util.warn(Util.ALERT,
	"cannot resolve these functions in the source code: " + sb.toString());
    }
    
    writer = new PrintWriter(new BufferedWriter(new FileWriter(outputFilename)));
    iter = positionsPerModule.entrySet().iterator();
    list = new Vector();
    while (iter.hasNext())
    {
      entry = (Map.Entry)iter.next();
      moduleName = (String)entry.getKey();
      iter2 = ((HashSet)entry.getValue()).iterator();
      while (iter2.hasNext())
	list.add(iter2.next());
      Collections.sort(list);
      for (i = 0; i < list.size(); i++)
	writer.println(moduleName + ":" + list.get(i));
      list.clear();
    }
    writer.close();
    positionsPerModule.clear();
    positionsPerFunc.clear();
  }

  /**
   * Function declarations
   */
  protected Vector functions;

  /**
   * Global variable declarations
   */
  protected Vector dataDecls;

  /**
   * Name of this module
   */
  protected String moduleName;

  /**
   * Linkage information
   */
  protected Vector functionDefns, variableDefns, variableDecls;
}

