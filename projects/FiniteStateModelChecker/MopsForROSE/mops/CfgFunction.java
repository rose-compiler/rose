package mops;
// $Id: CfgFunction.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

/**
 * Represent a function declaration in CFG
 */
class CfgFunction
{
  /**
   * Entry node
   */
  public Node entry;

  /**
   * Exit node
   */
  public Node exit;

  /**
   * Function name
   */
  public String label;

  /**
   * The root Ast of this function
   */
  public Ast ast;

  /**
   * The parent CFG
   */
  public Cfg cfg;
}

