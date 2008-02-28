#ifndef PROLOGPARSER_H
#define PROLOGPARSER_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <utility>

namespace PrologLexer {

  enum TokenKind {ATOM, VARIABLE, INTEGER, FLOATING_POINT, STRING, LPAREN,
                  RPAREN, LBRACKET, RBRACKET, VERTICAL_BAR, COMMA, PERIOD,
                  WILDCARD, END_OF_FILE};

  struct PrologToken {
    TokenKind kind;
    std::string stringValue;
    int intValue;
    double floatingValue;
    // For disambiguating prefix operators; see #3 and #4 on 
    // http://www.sics.se/isl/quintus/html/quintus/ref-syn-ops-res.html
    // Only set correctly for atoms; when set, lparen does not appear as a
    // separate token
    bool startOfTerm;

    std::string fileName;
    int line, col;

    PrologToken(TokenKind kind, const std::string& fileName,
                int line, int col,
                const std::string& stringValue = "",
                int intValue = 0, double floatingValue = 0)
      : kind(kind), stringValue(stringValue), intValue(intValue),
        floatingValue(floatingValue), startOfTerm(false),
        fileName(fileName), line(line), col(col) {}

  };

  std::ostream& operator<<(std::ostream& os, const PrologToken& tok);

  std::string readFile(const std::string& fileName);
  std::vector<PrologToken> lexSentence(const std::string& str,
                                       unsigned int& tokenIdx,
                                       const std::string& fileName,
                                       int& line,
                                       int& col);

}

namespace PrologAst {class Node;}

namespace PrologParser {

  // Parse a sentence; return NULL on EOF
  PrologAst::Node* parseSentence(const std::vector<PrologLexer::PrologToken>& tokens);

  enum Associativity {NOT_AN_OPERATOR, XFX, XFY, YFX, YFY, FX, FY, XF, YF};
  enum OperatorTag {PREFIX, INFIX, POSTFIX, NON_OPERATOR};

  inline std::ostream& operator<<(std::ostream& os, OperatorTag t) {
    switch (t) {
      case PREFIX: os << "PREFIX"; break;
      case INFIX: os << "INFIX"; break;
      case POSTFIX: os << "POSTFIX"; break;
      case NON_OPERATOR: os << "NON_OPERATOR"; break;
      default: os << "BAD OperatorTag"; break;
    }
    return os;
  }

  extern std::map<std::string, std::pair<Associativity, int> > infixOperators;
  extern std::map<std::string, std::pair<Associativity, int> > prefixOperators;
  extern std::map<std::string, std::pair<Associativity, int> > postfixOperators;

  void initParser();

}

#endif // PROLOGPARSER_H
