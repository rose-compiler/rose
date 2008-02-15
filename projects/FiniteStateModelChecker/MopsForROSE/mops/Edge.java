package mops;
// $Id: Edge.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;
import java.util.*;

/**
 * Represent an Edge in the CFG
 */
public class Edge
{
  public Edge()
  {
    label = null;
    srcNode = dstNode = null;
  }

  public final void setLabel(Ast label)
  {
    this.label = label;
  }

  public final Ast getLabel()
  {
    return label;
  }

  public final void setNodes(Node srcNode, Node dstNode)
  {
    this.srcNode = srcNode;
    this.dstNode = dstNode;
    srcNode.addOutEdge(this);
    dstNode.addInEdge(this);
  }

  public final void setSrcNode(Node srcNode)
  {
    this.srcNode = srcNode;
  }

  public final void setDstNode(Node dstNode)
  {
    this.dstNode = dstNode;
  }
  
  public final Node getSrcNode()
  {
    return srcNode;
  }

  public final Node getDstNode()
  {
    return dstNode;
  }

  /**
   * The AST on this edge
   */
  protected Ast label;

  /**
   * Source node
   */
  protected Node srcNode;

  /**
   * Destination Node
   */
  protected Node dstNode;
}
