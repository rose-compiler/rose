// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file C-API.C
 *  \brief Implements utility functions for describing a C-based API.
 */

#include <cassert>
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <string>
#include "C-API.h"

//=========================================================================
// String helper functions.
//=========================================================================

//! Strip whitespace from left and right sides of a string.
static
void
lrtrim (std::string& s, const std::string& delim = std::string (" \t"))
{
  using namespace std;
  string::size_type pos = s.find_last_not_of (delim);
  if(pos != string::npos)
    {
      s.erase (pos + 1); // Trim tail
      pos = s.find_first_not_of (delim);
      if (pos != string::npos) // Trim head
        s.erase(0, pos);
    }
  else // 's' contains only spaces.
    s.erase (s.begin (), s.end ());
}

//=========================================================================

//! Split a string into sequence of tokens.
template <typename FwdOutIter_>
static
void
split (const std::string& s, FwdOutIter_ out,
       const std::string& delim = std::string (" \t"))
{
  using namespace std;
  string::size_type tok_begin = 0; // Begin of next token.
  do {
    // Find end of token.
    string::size_type tok_end = s.find_first_of (delim, tok_begin);

    // Copy current token to output.
    string::size_type n = tok_end ==
      string::npos ? string::npos : (tok_end - tok_begin);
    *out++ = s.substr (tok_begin, n);

    // Next token.
    tok_begin = s.find_first_not_of (delim, tok_end);
  } while (tok_begin != string::npos); // Tokens remain
}

//=========================================================================

using namespace std;

namespace C_API
{
  //! Stores a sequence of tokens.
  typedef vector<string> Tokens_t;

  //=========================================================================
  //! \name Parses a sequence of tokens, returning a signature or NULL on error.
  //@{
  template <typename FwdInIter_T>
  static
  ConstSig *
  parseConst (FwdInIter_T begin, FwdInIter_T end)
  {
    if (begin != end)
      {
        string name = *begin++;
        if (!name.empty () && begin == end)
          return new ConstSig (name);
      }
    // Some error occurred; balk.
    return 0;
  } // parseConst ()

  template <typename FwdInIter_T>
  static
  TypeSig *
  parseType (FwdInIter_T begin, FwdInIter_T end)
  {
    if (begin != end)
      {
        string name = *begin++;
        if (!name.empty () && begin == end)
          return new TypeSig (name);
      }
    // Some error occurred; balk.
    return 0;
  } // parseType ()

  //! Removes the leading '&' (if it exists) from the input string.
  static
  string
  parseArg (const string& arg)
  {
    if (!arg.empty () && arg[0] == '&')
      return arg.substr (1);
    return arg;
  } // parseArg ()

  template <typename FwdInIter_T>
  static
  FuncSig *
  parseFunc (FwdInIter_T begin, FwdInIter_T end)
  {
    if (begin != end)
      {
        FwdInIter_T cur = begin;
        string name = *cur++;
        if (!name.empty ())
          {
            FuncSig::Args_t args;
	    // JJW 10-17-2007 Don't know why the next line causes a link error
            // transform (cur, end, back_inserter (args), parseArg);
	    for (; cur != end; ++cur) {
	      args.push_back(parseArg(*cur));
	    }
            return new FuncSig (name, args);
          }
      }
    // Some error occurred; balk.
    return 0;
  } // parseFunc ()

  template <typename FwdInIter_T>
  static
  SigBase *
  parse (FwdInIter_T begin, FwdInIter_T end)
  {
    SigBase* new_sig = 0;
    if (begin != end)
      {
        FwdInIter_T cur = begin;
        string tag (*cur++);
        if (tag == "<CONST>")
          new_sig = parseConst (cur, end);
        else if (tag == "<TYPE>")
          new_sig = parseType (cur, end);
        else if (tag == "<SIG>")
          new_sig = parseFunc (cur, end);
      }
    return new_sig;
  } // parse ()
  //@}

  //=========================================================================
  bool
  readSigs (istream& infile, SigMap_t* sigs)
  {
    assert (sigs);
    size_t line_count = 0;
    string s;
    while (getline (infile, s))
      {
        ++line_count;
        lrtrim (s);

        // If line is not empty and not a comment...
        if (!s.empty () && s[0] != '#')
          {
            vector<string> tokens;
            split (s, back_inserter (tokens));
            assert (!tokens.empty ());
            SigBase* sig = parse (tokens.begin (), tokens.end ());
            if (!sig)
              {
                cerr << "*** ERROR: Can't parse signature at line "
                     << line_count
                     << " ***"
                     << endl;
                return false;
              }

            // 'sig' is valid; insert.
            (*sigs)[sig->getName ()] = sig;
          }
      } // while
    return true;
  } // readSigs ()

} // namespace C_API

//=========================================================================
// eof
