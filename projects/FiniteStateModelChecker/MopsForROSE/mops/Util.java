package mops;
// $Id: Util.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;

/**
 * Common tools
 */
public class Util
{
  /**
   * Print a warning message with the location of the call site of this
   * function.  However, if <code>type == ALERT && !isDebug()</code>,
   * print nothing.
   */
  public static void warn(int type, String message)
  {
    warn0(new Throwable(), type, message);
  }

  public static void warn(int type, String message, String filename, int lineno)
  {
    warn0(new Throwable(), type, formatMessage(message, filename, lineno));
  }

  private static void warn0(Throwable throwable, int type, String message)
  {
    switch(type)
    {
      case WARNING:
        System.err.print("MOPS warning ");
	break;

      case ALERT:
	if (!getDebug())
	  return;
	else
	  System.err.print("MOPS alert ");
	break;

      default:
	die("Unknown type " + type);
    }
    System.err.println("at " + getLocation(throwable) + ": " + message + ".");
  }
  
  /**
   * Exit with an error message with the location of the call site of this
   * function.
   */
  public static void die(String message, String filename, int lineno)
  {
    die0(new Throwable(), formatMessage(message, filename, lineno));
  }

  public static void die(String message)
  {
    die0(new Throwable(), message);
  }

  private static void die0(Throwable throwable, String message)
  {
    System.err.println("MOPS error at " + getLocation(throwable) + ": "
		       + message + ".");
    System.exit(1);
  }
			   
  /**
   * Extract the second to top program location from a stack dump
   */
  private static String getLocation(Throwable throwable)
  {
    StringBuffer sb;
    int i, begin, length;
    boolean tab;

    // Get class and method names at the call site by parsing
    // Throwable messages
    throwable.printStackTrace(pw);
    sb = sw.getBuffer();
    return sb.toString();
    /*
    length = sb.length();
    tab = false;
    for (i = 0; i < length; i++)
      if (sb.charAt(i) == '\t')
      {
	if (tab)
	  break;
	else
	  tab = true;
      }

    begin = i + 4;
    for (i = begin; i < length; i++)
      if (sb.charAt(i) == '\n')
	break;
    if (i >= length)
      throw new RuntimeException("Unexpected Throwable format");

    return sb.substring(begin, i);
    */
  }

  private static String formatMessage(String message, String filename, int lineno)
  {
    StringBuffer str;

    str = new StringBuffer(message);
    if (filename != null)
      str.append(" in the file " + filename);
    if (lineno >= 0)
      str.append(" on Line " + lineno);

    return str.toString();
  }
  
  public static boolean getDebug()
  {
    return isDebug;
  }

  public static void setDebug(boolean isDebug)
  {
    Util.isDebug = isDebug;
  }

  public static void printVersion()
  {
    System.out.println(version);
  }

  static StringWriter sw;
  static PrintWriter pw;
  static boolean isDebug;
  static String version;

  static
  {
    sw = new StringWriter();
    pw = new PrintWriter(sw);
    isDebug = false;
    version =
      "MOPS version 0.9.1: MOdelchecking Programs for Security violations";
  }

  public static final int WARNING = 1, ALERT = 2;
}
