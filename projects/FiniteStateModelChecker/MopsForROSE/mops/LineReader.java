package mops;
// $Id: LineReader.java,v 1.1 2006/09/08 11:39:23 dquinlan Exp $

import java.io.*;

/**
 * Read in tokens
 * LineReader relies on the mercy of StreamTokenizer whose implementation is:
        wordChars('a', 'z');
        wordChars('A', 'Z');
        wordChars(128 + 32, 255);
        whitespaceChars(0, ' ');
        commentChar('/');
        quoteChar('"');
        quoteChar('\'');
        parseNumbers();
*/
class LineReader
{
  public LineReader(Reader reader)
  {
    st = new StreamTokenizer(reader);
    st.eolIsSignificant(true);
    // consider '_' and '*' as regular characters
    st.wordChars('_', '_');
    st.wordChars('*', '*');
    //st.wordChars(':', ':');
    // Override the default, which is to consider '/' a comment character
    //st.wordChars('/', '/');
  }

  public String nextToken() throws IOException
  {
    st.nextToken();

    switch(st.ttype)
    {
      case StreamTokenizer.TT_WORD:
      case '\"':
	return st.sval;

      case StreamTokenizer.TT_NUMBER:
	return Integer.toString((int)st.nval);

      case StreamTokenizer.TT_EOL:
      case StreamTokenizer.TT_EOF:
	return null;
      default:
	return new String(new byte[] {(byte)st.ttype});
    }
  }

  public int nextInt() throws IOException
  {
    st.nextToken();
    if (st.ttype != StreamTokenizer.TT_NUMBER)
      Util.die("Number expected", null, lineno());
    return (int)st.nval;
  }
  
  public void skipToEol() throws IOException
  {
    while (st.ttype != StreamTokenizer.TT_EOL &&
	   st.ttype != StreamTokenizer.TT_EOF)
      st.nextToken();
  }

  public int getTtype()
  {
    return st.ttype;
  }

  public int lineno()
  {
    return st.lineno();
  }
  
  private StreamTokenizer st;
}

