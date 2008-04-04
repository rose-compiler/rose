#include "prologParser.h"
#include <ctype.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdlib.h>

namespace PrologLexer {
  // This is based on the stuff at
  // http://xsb.sourceforge.net/manual1/node50.html
  // and info about strings at
  // http://www.cs.arizona.edu/~collberg/Research/Sicstus/sicstus_41.html

static PrologToken atom(const std::string& str, bool startOfTerm,
                        const std::string& fileName, int line, int col) {
  PrologToken t = PrologToken(ATOM, fileName, line, col, str);
  t.startOfTerm = startOfTerm;
  return t;
}

static PrologToken variable(const std::string& str,
                            const std::string& fileName, int line, int col) {
  return PrologToken(VARIABLE, fileName, line, col, str);
}

static PrologToken integer(int x,
                           const std::string& fileName, int line, int col) {
  return PrologToken(INTEGER, fileName, line, col, "", x);
}

static PrologToken floatingPoint(double x,
                                 const std::string& fileName,
                                 int line, int col) {
  return PrologToken(FLOATING_POINT, fileName, line, col, "", 0, x);
}

static PrologToken stringToken(const std::string& str,
                               const std::string& fileName,
                               int line, int col) {
  return PrologToken(STRING, fileName, line, col, str);
}

static PrologToken leftParen(const std::string& fileName, int line, int col) {
  return PrologToken(LPAREN, fileName, line, col);
}

static PrologToken rightParen(const std::string& fileName, int line, int col) {
  return PrologToken(RPAREN, fileName, line, col);
}

static PrologToken leftBracket(const std::string& fileName,
                               int line, int col) {
  return PrologToken(LBRACKET, fileName, line, col);
}

static PrologToken rightBracket(const std::string& fileName,
                                int line, int col) {
  return PrologToken(RBRACKET, fileName, line, col);
}

static PrologToken verticalBar(const std::string& fileName,
                               int line, int col) {
  return PrologToken(VERTICAL_BAR, fileName, line, col, "|");
}

static PrologToken comma(const std::string& fileName, int line, int col) {
  return PrologToken(COMMA, fileName, line, col, ","); // For use as an operator
}

static PrologToken period(const std::string& fileName, int line, int col) {
  return PrologToken(PERIOD, fileName, line, col, "."); // For use as an operator
}

static PrologToken wildcard(const std::string& fileName, int line, int col) {
  return PrologToken(WILDCARD, fileName, line, col);
}

static PrologToken endOfFile(const std::string& fileName, int line, int col) {
  return PrologToken(END_OF_FILE, fileName, line, col);
}

inline double stringToDouble(const std::string& str,
                             const std::string& fileName, int line, int col) {
  std::istringstream s(str);
  double d;
  s >> d;
  if (!s.eof()) {
    std::cerr << fileName << ":" << line << "." << col << ": Bad floating point constant " << str << std::endl;
    abort();
  }
  return d;
}

inline int stringToInt(const std::string& str, int base) {
  int acc = 0;
  for (unsigned int i = 0; i < str.size(); ++i) {
    acc *= base;
    char c = str[i];
    if (c >= '0' && c <= '9') {
      acc += c - '0';
    } else if (c >= 'a' && c <= 'a' + base - 11) {
      acc += c - 'a' + 10;
    } else if (c >= 'A' && c <= 'A' + base - 11) {
      acc += c - 'A' + 10;
    } else {
      std::cerr << "Bad character in integer having base " << base << ": '" << c << "'" << std::endl;
      abort();
    }
  }
  return acc;
}

PrologToken getToken(const std::string& str, unsigned int& idx,
                     const std::string& fileName, int& line, int& col) {
top:
  const char normalOperatorChars[] = "+-*/\\^<>=`~:.?@#&";
  if (idx == str.size()) { // End of string
    return endOfFile(fileName, line, col);
  }
  int oldLine = line, oldCol = col;
#define STAR_P (str[idx])
#define P_SUB_1 (idx + 1 == str.size() ? 0 : str[idx + 1])
#define INC_INDEX do { \
                    if (STAR_P == '\n') { \
                      ++line; \
                      col = 1; \
                    } else { \
                      ++col; \
                    } \
                    ++idx; \
                  } while (false)
  if (islower(STAR_P)) { // Normal atoms
    unsigned int oldIdx = idx;
    do {INC_INDEX;} while (isalnum(STAR_P) || STAR_P == '_');
    bool startOfTerm = false;
    if (STAR_P == '(') {startOfTerm = true; INC_INDEX;}
    return atom(str.substr(oldIdx, idx - oldIdx - startOfTerm), startOfTerm,
                fileName, oldLine, oldCol);
  } else if (STAR_P == '_' && !isalnum(P_SUB_1)) { // Wildcard
    INC_INDEX;
    return wildcard(fileName, oldLine, oldCol);
  } else if (isupper(STAR_P) || STAR_P == '_' || STAR_P == '$') { // Variables
    unsigned int oldIdx = idx;
    do {INC_INDEX;} while (isalnum(STAR_P) || STAR_P == '_' || STAR_P == '$');
    return variable(str.substr(oldIdx, idx - oldIdx), fileName, oldLine, oldCol);
  } else if (isdigit(STAR_P) || STAR_P == '-' && isdigit(P_SUB_1)) { // Numbers
    int isNegative = 1;
    if (STAR_P == '-') {isNegative = -1; INC_INDEX;}
    unsigned int oldIdx = idx;
    do {INC_INDEX;} while (isdigit(STAR_P));
    std::string tokenString;
    if (STAR_P == '.' && isdigit(P_SUB_1)) { // Floating point
      INC_INDEX;
      while (isdigit(STAR_P)) INC_INDEX;
      if (STAR_P == 'e' || STAR_P == 'E') {
        INC_INDEX;
        if (STAR_P == '-') INC_INDEX;
        while (isdigit(STAR_P)) INC_INDEX;
      }
      return floatingPoint(isNegative *
                           stringToDouble(str.substr(oldIdx, idx - oldIdx),
                                          fileName, oldLine, oldCol),
                           fileName, oldLine, oldCol);
    } else if (STAR_P == '\'') { // Base or character definition
      int base = stringToInt(str.substr(oldIdx, idx - oldIdx), 10);
      if (base == 0 && P_SUB_1) { // Character
        INC_INDEX;
        return integer(isNegative * (STAR_P), fileName, oldLine, oldCol);
      } else if (base >= 2 && base <= 36) {
        INC_INDEX;
        oldIdx = idx;
        while (isalnum(STAR_P)) INC_INDEX;
        return integer(isNegative *
                       stringToInt(str.substr(oldIdx, idx - oldIdx), base),
                       fileName, oldLine, oldCol);
      } else {
        std::cerr << "Invalid number base " << base << std::endl;
        abort();
      }
    } else { // Normal integer
      return integer(isNegative *
                     stringToInt(str.substr(oldIdx, idx - oldIdx), 10),
                     fileName, oldLine, oldCol);
    }
  } else if (STAR_P == '.' && P_SUB_1 && !strchr(normalOperatorChars, P_SUB_1)) {
    INC_INDEX;
    return period(fileName, oldLine, oldCol);
  } else if (strchr(normalOperatorChars, STAR_P)) { // Operators
    unsigned int oldIdx = idx;
    do {INC_INDEX;} while (strchr(normalOperatorChars, STAR_P));
    bool startOfTerm = false;
    if (STAR_P == '(') {startOfTerm = true; INC_INDEX;}
    return atom(str.substr(oldIdx, idx - oldIdx - startOfTerm), startOfTerm,
                fileName, oldLine, oldCol);
  } else if (STAR_P == '!' || STAR_P == ';') { // Single-character operators
    char c = STAR_P;
    INC_INDEX;
    bool startOfTerm = false;
    if (STAR_P == '(') {startOfTerm = true; INC_INDEX;}
    return atom(std::string(1, c), startOfTerm, fileName, oldLine, oldCol);
  } else if (STAR_P == '\'') { // Quoted atoms
    std::string result;
    INC_INDEX;
    unsigned int oldIdx = idx;
get_more_chars_atom:
    while (STAR_P != '\'') INC_INDEX;
    result += str.substr(oldIdx, idx - oldIdx);
    if (P_SUB_1 == '\'') { // Escaped single quote
      result += "'";
      INC_INDEX; INC_INDEX;
      goto get_more_chars_atom;
    } else { // End of atom
      INC_INDEX;
      bool startOfTerm = false;
      if (STAR_P == '(') {startOfTerm = true; INC_INDEX;}
      return atom(result, startOfTerm, fileName, oldLine, oldCol);
    }
  } else if (STAR_P == '"') { // Strings
    std::string result;
    INC_INDEX;
    unsigned int oldIdx = idx;
get_more_chars_string:
    while (STAR_P && STAR_P != '"' && STAR_P != '\\') INC_INDEX;
    result += str.substr(oldIdx, idx - oldIdx);
    if (STAR_P == '"') {
      if (P_SUB_1 == '"') { // Escaped double quote
        result += "\"";
        INC_INDEX; INC_INDEX;
        oldIdx = idx;
        goto get_more_chars_string;
      } else { // End of string
        INC_INDEX;
        return stringToken(result, fileName, oldLine, oldCol);
      }
    } else if (STAR_P == '\\') {
      std::cerr << "Cannot handle backslash escapes in strings yet" << std::endl;
      abort();
    } else if (!STAR_P) {
      std::cerr << "Unterminated string constant" << std::endl;
      abort();
    }
  } else if (STAR_P == '(') {
    INC_INDEX;
    return leftParen(fileName, oldLine, oldCol);
  } else if (STAR_P == ')') {
    INC_INDEX;
    return rightParen(fileName, oldLine, oldCol);
  } else if (STAR_P == '[') {
    if (P_SUB_1 == ']') {
      INC_INDEX; INC_INDEX;
      bool startOfTerm = false;
      if (STAR_P == '(') {startOfTerm = true; INC_INDEX;}
      return atom("[]", startOfTerm, fileName, oldLine, oldCol);
    } else {
      INC_INDEX;
      return leftBracket(fileName, oldLine, oldCol);
    }
  } else if (STAR_P == ']') {
    INC_INDEX;
    return rightBracket(fileName, oldLine, oldCol);
  } else if (STAR_P == '|') {
    INC_INDEX;
    return verticalBar(fileName, oldLine, oldCol);
  } else if (STAR_P == ',') {
    INC_INDEX;
    return comma(fileName, oldLine, oldCol);
  } else if (isspace(STAR_P)) { // White space
    INC_INDEX;
    goto top;
  } else if (STAR_P == '%') { // Comment
    do {INC_INDEX;} while (STAR_P && STAR_P != '\n' && STAR_P != '\r');
    if (!STAR_P) {
      std::cerr << "Unterminated comment" << std::endl;
      abort();
    }
    goto top;
  } else {
    std::cerr << "Bad character '" << STAR_P << "'" << std::endl;
    abort();
  }
#undef STAR_P
#undef P_SUB_1
#undef INC_INDEX
  std::cerr << "Should not get here" << std::endl;
  abort();
  return endOfFile(fileName, line, col); // Should not get here
}

std::ostream& operator<<(std::ostream& os, const PrologToken& tok) {
  switch (tok.kind) {
    case ATOM: os << "<atom " << tok.stringValue << (tok.startOfTerm ? " lp" : "") << ">"; break;
    case VARIABLE: os << "<var " << tok.stringValue << ">"; break;
    case INTEGER: os << "<int " << tok.intValue << ">"; break;
    case FLOATING_POINT: os << "<float " << tok.floatingValue << ">"; break;
    case STRING: os << "<string \"" << tok.stringValue << "\">"; break;
    case LPAREN: os << "<(>"; break;
    case RPAREN: os << "<)>"; break;
    case LBRACKET: os << "<[>"; break;
    case RBRACKET: os << "<]>"; break;
    case VERTICAL_BAR: os << "<|>"; break;
    case COMMA: os << "<,>"; break;
    case PERIOD: os << "<.>"; break;
    case WILDCARD: os << "<_>"; break;
    case END_OF_FILE: os << "<eof>"; break;
    default: os << "<bad>"; break;
  }
  os << "@" << tok.fileName << ":" << tok.line << "." << tok.col;
  return os;
}

std::vector<PrologToken> lexSentence(const std::string& str,
                                     unsigned int& tokenIdx,
                                     const std::string& fileName,
                                     int& line,
                                     int& col) {
  std::vector<PrologLexer::PrologToken> tokens;
  if (tokenIdx == str.size()) {
    tokens.push_back(endOfFile(fileName, line, col));
  } else {
    do {
      tokens.push_back(
          PrologLexer::getToken(str, tokenIdx, fileName, line, col));
    } while (tokens.back().kind != PrologLexer::END_OF_FILE &&
             tokens.back().kind != PrologLexer::PERIOD);
  }
  if (tokens.back().kind == PrologLexer::END_OF_FILE &&
      tokens.size() != 1) {
    std::cerr << tokens.front().fileName << ":" << tokens.front().line << "." << tokens.front().col << ": File does not end with period" << std::endl;
    exit (1);
  }
  return tokens;
}

std::string readFile(const std::string& fileName) {
  std::ifstream f(fileName.c_str());
  if (!f) {
    std::cerr << "Problem reading source file " << fileName << std::endl;
    abort();
  }
  std::string str;
  char buf[4096];
  while (f) {f.read(buf, 4095); buf[f.gcount()] = 0; str += buf;}
  return str;
}

}
