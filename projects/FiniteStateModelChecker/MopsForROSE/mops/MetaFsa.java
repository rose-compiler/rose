package mops;
// $Id: MetaFsa.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.util.*;
import java.io.*;

/**
 * Represents a variable "{var x}" in pattern AST
 * The scope of a variable name is a .fsa file.  I.e. The same varaible name
 * in different .fsa files are independent.
 */
class Variable
{
  /**
   * All Asts in the Cfg that are instances of this variable
   */
  public Vector instances;

  /**
   * All places in the model Fsa where this variable appears
   */
  public Vector anchors;

  public Variable()
  {
    instances = new Vector();
    anchors = new Vector();
  }
}

/**
 * Represents a MetaFsa.  Read in MetaFsa and resolve variables if any
 */
public class MetaFsa
{
  public MetaFsa()
  {
    fsas = new Vector();
    initialStates = new Vector();
    finalStates = new Vector();
    variables = new Vector();
  }

  /**
   * Get an absolute filename derived from "absolute" and "relative"
   */
  private static String getAbsoluteFilename(String absolute, String relative)
  {
    int index;
    
    // Does absolute contain path?
    index = absolute.lastIndexOf('/');
    if (index == -1)
      return relative;

    // Is relative absolute?
    if (relative.charAt(0) == '/')
      return relative;

    return absolute.substring(0, index + 1) + relative;
  }

  /**
   * Read from a .mfsa file
   * All relative filenames in .mfsa are relative to the .mfsa file
   */
  public void read(String filename) throws IOException
  {
    Vector stateHashs;
    Hashtable stateHash;
    Fsa fsa;
    String label, name, str;
    BufferedReader bufferedReader;
    LineReader reader;
    Integer stateInt;
    int i, state, size;
    char ch;
    int[] states;

    fsas.clear();
    initialStates.clear();
    finalStates.clear();
    variables.clear();
    
    bufferedReader = new BufferedReader(new FileReader(filename));
    reader = new LineReader(bufferedReader);
    stateHashs = new Vector();

    for (name = reader.nextToken(); name != null; name = reader.nextToken())
    {
      stateHash = new Hashtable();
      stateHashs.add(stateHash);
      fsa = new Fsa();
      fsa.read(getAbsoluteFilename(filename, name), stateHash);
      fsas.add(fsa);
    }
    if (fsas.size() == 0)
      Util.die("this meta-FSA contains no FSA", filename, reader.lineno());
    
    for (label = reader.nextToken();
	 reader.getTtype() != StreamTokenizer.TT_EOF;
	 label = reader.nextToken())
    {
      // skip empty line
      if (label == null)
	continue;

      switch((ch = label.charAt(0)))
      {
	// initial and final state
	case 'q':
	case 'f':
	  states = new int[fsas.size()];
	  for (i = 0; i < fsas.size(); i++)
	  {
	    if ((str = reader.nextToken()) == null)
	      Util.die("expect " + fsas.size() + " states", filename,
		       reader.lineno());
	    if ((stateInt = (Integer)((Hashtable)(stateHashs.get(i))).get(str))
		== null)
	      Util.die("unknown state " + str, filename, reader.lineno());
	    else
	      states[i] = stateInt.intValue();
	  }
	  if (ch == 'q')  // initial state
	    initialStates.add(states);
	  else  // final state
	    finalStates.add(states);

          // read eol
	  reader.nextToken();
	  break;
	  
	default:
	  Util.die("unknown syntax " + label, filename, reader.lineno());
      }
      if (reader.getTtype() != StreamTokenizer.TT_EOL &&
	  reader.getTtype() != StreamTokenizer.TT_EOF)
      {
	Util.warn(Util.WARNING, "ignore extra words at the end of the line", filename, reader.lineno());
	reader.skipToEol();
      }
    }

    if (initialStates.size() == 0)
      Util.warn(Util.WARNING, "no initial state");
    if (finalStates.size() == 0)
      Util.warn(Util.WARNING, "no final state");
    
    for (i = 0; i < stateHashs.size(); i++)
      ((Hashtable)stateHashs.get(i)).clear();
    stateHashs.clear();
    bufferedReader.close();
  }

  private static class FsaTuple
  {
    /**
     * All Asts that contain variables
     */
    public Vector asts;

    /**
     * All variables.
     * Key: variable name
     * Value: Variable object
     */
    public Hashtable variables;

    public FsaTuple()
    {
      asts = new Vector();
      variables = new Hashtable();
    }
  }

  /**
   * Resolve variables in model Fsa against a Cfg
   */
  public void resolveVariable(Cfg cfg)
  {
    Fsa fsa;
    FsaTuple[] fsaTuples;
    FsaTuple tuple;
    Iterator iter, iter2;
    FsaTransition transition;
    Hashtable variables, variables2;
    Ast ast, ast2;
    Vector nodeQueue, inEdges, outEdges, list, functions, asts;
    HashSet nodeSet;
    Node node;
    Variable variable;
    Map.Entry entry;
    int i, j, k, l, m, n;

    // find out all Asts containing variables from each Fsa
    fsaTuples = new FsaTuple[fsas.size()];
    variables = new Hashtable();
    for (i = 0; i < fsaTuples.length; i++)
    {
      fsa = (Fsa)fsas.get(i);
      fsaTuples[i] = new FsaTuple();
      variables2 = fsaTuples[i].variables;
      iter = fsa.iterator();
      while (iter.hasNext())
      {
	transition = (FsaTransition)iter.next();
	ast = (Ast)transition.input;
	if (ast != null)
	{
	  ast.findVariable(variables);
	  if (variables.entrySet().size() > 0)
	  {
	    fsaTuples[i].asts.add(ast);
	    iter2 = variables.entrySet().iterator();
	    while (iter2.hasNext())
	    {
	      entry = (Map.Entry)iter2.next();
	      if ((variable = (Variable)variables2.get(entry.getKey())) == null)
	      {
		variable = new Variable();
		variables2.put(entry.getKey(), variable);
	      }
	      list = (Vector)entry.getValue();
	      for (j = 0; j < list.size(); j++)
		variable.anchors.add(list.get(j));
	    }
	    variables.clear();
	  }
	}
      }
    }

    // scan cfg and instantiate each variable
    nodeQueue = new Vector();
    nodeSet = new HashSet();
    functions = cfg.getFunctions();
    for (i = 0; i < functions.size(); i++)
    {
      node = ((CfgFunction)functions.get(i)).entry;
      nodeQueue.add(node);
      nodeSet.add(node);
      while (nodeQueue.size() > 0)
      {
	node = (Node)nodeQueue.remove(0);
	outEdges = node.getOutEdges();
	for (j = 0; j < outEdges.size(); j++)
	{
	  ast = ((Edge)outEdges.get(j)).getLabel();
	  if (ast != null)
	  {
	    for (k = 0; k < fsaTuples.length; k++)
	    {
	      tuple = fsaTuples[k];
	      for (l = 0; l < tuple.asts.size(); l++)
	      {
		if (ast.match((Ast)tuple.asts.get(l), variables))
		{
		  iter = variables.entrySet().iterator();
		  while (iter.hasNext())
		  {
		    entry = (Map.Entry)iter.next();
		    if ((variable = (Variable)tuple.variables.get(
		       entry.getKey())) == null)
		      Util.die("the variable " + entry.getKey() +
			" is matched in the CFG, but is not found in the " + k + "th FSA");
		    else
		    {
		      // add entry.getValue() to variable.instances if it's
		      // not already there
		      asts = (Vector)entry.getValue();
		      for (m = 0; m < asts.size(); m++)
		      {
			ast2 = (Ast)asts.get(m);
			for (n = 0; n < variable.instances.size(); n++)
			{
			  if (ast2.match((Ast)variable.instances.get(n), null))
			    break;
			}
			if (n >= variable.instances.size())
			{
			  variable.instances.add(ast2);
			}
		      }
		    }
		  }
		}
		variables.clear();
	      }
	    }
	  }
	  // traverse graph
	  node = (Node)((Edge)outEdges.get(j)).getDstNode();
	  if (!nodeSet.contains(node))
	  {
	    nodeQueue.add(node);
	    nodeSet.add(node);
	  }
	}
      }
      nodeSet.clear();
    }

    // fill in this.variable.  Remove all Variable which is not instantiated
    this.variables.clear();
    for (i = 0; i < fsaTuples.length; i++)
    {
      iter = fsaTuples[i].variables.values().iterator();
      while (iter.hasNext())
      {
	variable = (Variable)iter.next();
	// Add Variable only if it has instances
	if (variable.instances.size() > 0)
  	  this.variables.add(variable);
      }
    }

    // clean up
    for (i = 0; i < fsaTuples.length; i++)
      fsaTuples[i] = null;

    //System.err.println("variables.size()=" + this.variables.size());
  }

  /**
   * Iterates through all instances of variables.
   * Each iteration gives a MetaFsa where with one instantiation of variables
   */
  private class MetaFsaIterator implements Iterator
  {
    public MetaFsaIterator(MetaFsa metaFsa)
    {
      int i, j;
      Vector asts;
      Ast ast;
      
      this.metaFsa = metaFsa;
      counter = new int[variables.size()];
      oldCounter = new int[variables.size()];
      for (i = 0; i < counter.length; i++)
      {
	counter[i] = 0;
	/*
	asts = ((Variable)variables.get(i)).anchors;
	for (j = 0; j < asts.size(); j++)
	{
	  ast = (Ast)asts.get(j);
	  ast.kind = Ast.kind_identifier;
	  ast.children.setSize(2);
	}
	*/
      }
      if (counter.length > 0)
        counter[0] = -1;
      else
	used = false;
    }

    /**
     * Determine if there is another instantiation of MetaFsa
     */
    public boolean hasNext()
    {
      int i, size;

      for (i = 0; i < variables.size(); i++)
      {
	size = ((Variable)variables.get(i)).instances.size();
	if (counter[i] < (size - 1))
	  return true;
      }

      if (variables.size() == 0)
	return !used;
      else
        return false;
    }

    /**
     * Give the next instantiation of MetaFsa
     * In fact, this method always returns the same MetaFsa object that is
     * passed into the constructor, but the internal Asts are modified.
     */
    public Object next()
    {
      int i, j, size;
      Variable variable;
      Vector asts;
      Ast ast, instance;
      
      if (!hasNext())
	return null;

      // If no variable is found/matched, return the original MetaFsa only once
      if (counter.length == 0)
      {
	used = true;
	return metaFsa;
      }

      // record old counter value
      if (counter[0] == -1)
      {
	// Make sure the first counter value is fresh
	for (i = 0; i < oldCounter.length; i++)
	  oldCounter[i] = -1;
      }
      else
      {
        for (i = 0; i < counter.length; i++)
	  oldCounter[i] = counter[i];
      }
      // compute new counter value
      for (i = 0; i < counter.length; i++)
      {
	size = ((Variable)variables.get(i)).instances.size();
	if ((counter[i] + 1) < size)
	{
	  counter[i]++;
	  break;
	}
	else
	  counter[i] = 0;
      }
      if (i >= counter.length)
	Util.die("counter overflow");

      // update all asts
      for (i = 0; i < counter.length; i++)
	if (counter[i] != oldCounter[i])
	{
	  variable = (Variable)variables.get(i);
	  asts = variable.anchors;
	  instance = (Ast)variable.instances.get(counter[i]);
	  for (j = 0; j < asts.size(); j++)
	  {
	    ast = (Ast)asts.get(j);
	    // shallow copy instance to ast
	    ast.kind = instance.kind;
	    // Warning: ast.children (i.e. variable name) is lost.
	    ast.children = instance.children;
	  }
	}

      /*
      for (i = 0; i < counter.length; i++)
        Util.print(counter[i] + " ");
      Util.println("");
      */
      
      return metaFsa;
    }

    public void remove()
    {
    }

    private int[] counter, oldCounter;
    /**
     * I don't know how to refer to the enclosing class, so I have to
     * save its reference
     */
    private MetaFsa metaFsa;
    /**
     * Hack for cases where no variable is found/matched
     */
    private boolean used;
  }
  
  public Iterator iterator()
  {
    return new MetaFsaIterator(this);
  }

  /**
   * Each element is a Fsa
   */
  public Vector getFsas()
  {
    return fsas;
  }

  /**
   * Each element is a int[]
   */
  public Vector getInitialStates()
  {
    return initialStates;
  }

  /**
   * Each element is a int[]
   */
  public Vector getFinalStates()
  {
    return finalStates;
  }

  /**
   * Each element is a Variable.
   * You shouldn't need this method unless for debugging purposes
   */
  public Vector getVariables()
  {
    return variables;
  }

  private Vector fsas, initialStates, finalStates;

  /**
   * Each element is a Variable object
   */
  private Vector variables;
}
  
