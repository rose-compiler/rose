#include "prologParser.h"
#include "prologAst.h"
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <utility>
#include <stdlib.h>

using namespace std;

using namespace PrologLexer;

namespace PrologParser {

  std::map<std::string, std::pair<Associativity, int> > infixOperators;
  std::map<std::string, std::pair<Associativity, int> > prefixOperators;
  std::map<std::string, std::pair<Associativity, int> > postfixOperators;

  // Precedences and associativities are a subset of those at
  // http://www.sics.se/isl/quintus/html/quintus/ref-syn-ops-bop.html
  // Many hints about syntax and precedence details are from
  // http://www.sics.se/isl/quintus/html/quintus/ref-syn-ops-res.html
  
  void initParser() {
    infixOperators.insert(make_pair(":-", make_pair(XFX, 1200)));
    infixOperators.insert(make_pair(";", make_pair(XFY, 1100)));
    infixOperators.insert(make_pair(",", make_pair(XFY, 1000)));
    infixOperators.insert(make_pair("|", make_pair(XFX, 1000))); // Not really an operator, but treated like one
    infixOperators.insert(make_pair("=", make_pair(XFX, 700)));
    infixOperators.insert(make_pair("is", make_pair(XFX, 700)));
    prefixOperators.insert(make_pair(":-", make_pair(FX, 1200)));
    prefixOperators.insert(make_pair("?-", make_pair(FX, 1200)));
    prefixOperators.insert(make_pair("declare", make_pair(FX, 1150)));
    prefixOperators.insert(make_pair("include", make_pair(FX, 1150)));
    prefixOperators.insert(make_pair("template", make_pair(FX, 1150)));
    prefixOperators.insert(make_pair("\\+", make_pair(FY, 900)));
    prefixOperators.insert(make_pair("ascending", make_pair(FY, 850)));
    prefixOperators.insert(make_pair("descending", make_pair(FY, 850)));
  }

  pair<Associativity, int> getInfixDefinition(const std::string& str) {
    if (infixOperators.find(str) == infixOperators.end()) {
      return make_pair(NOT_AN_OPERATOR, 0);
    }
    return infixOperators[str];
  }

  pair<Associativity, int> getPrefixDefinition(const std::string& str) {
    if (prefixOperators.find(str) == prefixOperators.end()) {
      return make_pair(NOT_AN_OPERATOR, 0);
    }
    return prefixOperators[str];
  }

  pair<Associativity, int> getPostfixDefinition(const std::string& str) {
    if (postfixOperators.find(str) == postfixOperators.end()) {
      return make_pair(NOT_AN_OPERATOR, 0);
    }
    return postfixOperators[str];
  }

  void parseError(const PrologLexer::PrologToken& tok,
                  const std::string& message) {
    cerr << tok.fileName << ":" << tok.line << "." << tok.col <<
            ": " /* << "[" << tok << "] " */ << message << endl;
    exit(1);
  }

  void tagOperators(const std::vector<PrologToken>& tokens, std::vector<OperatorTag>& tags) {
    unsigned int size = tokens.size();
    tags.resize(size);
    PrologToken lastToken = PrologToken(LPAREN, "", 0, 0);
    OperatorTag lastTag = PREFIX;
    int lastPrecedence = 1201;
    int precedenceToPassOn = -1;
    OperatorTag thisTagToPassOn;
    for (unsigned int i = 0; i < size; (lastToken = tokens[i], lastTag = thisTagToPassOn, lastPrecedence = precedenceToPassOn), ++i) {
      PrologToken thisToken = tokens[i];
      PrologToken nextToken = (i + 1 == size) ? PrologToken(RPAREN, "", 0, 0) : tokens[i + 1];
      OperatorTag& thisTag = tags[i];
      thisTagToPassOn = NON_OPERATOR; // This can be different from the current tag (for parentheses, etc.)

      pair<Associativity, int> pPrefix = make_pair(NOT_AN_OPERATOR, 0),
                               pInfix = make_pair(NOT_AN_OPERATOR, 0),
                               pPostfix = make_pair(NOT_AN_OPERATOR, 0);
      if (thisToken.kind == ATOM || thisToken.kind == COMMA || thisToken.kind == VERTICAL_BAR) {
        pPrefix = getPrefixDefinition(thisToken.stringValue);
        pInfix = getInfixDefinition(thisToken.stringValue);
        pPostfix = getPostfixDefinition(thisToken.stringValue);
      }
      if ((thisToken.kind == ATOM || thisToken.kind == COMMA) && thisToken.startOfTerm) {
        // Beginning of term
	thisTag = NON_OPERATOR;
        thisTagToPassOn = PREFIX;
        precedenceToPassOn = 1001; // For just inside lparen of a term
      } else if (thisToken.kind == VARIABLE ||
                 thisToken.kind == WILDCARD ||
                 thisToken.kind == INTEGER ||
                 thisToken.kind == FLOATING_POINT ||
                 thisToken.kind == STRING) {
        thisTag = thisTagToPassOn = NON_OPERATOR;
        precedenceToPassOn = 0;
      } else if (thisToken.kind == VERTICAL_BAR) { // Always acts like an infix operator
        thisTag = thisTagToPassOn = INFIX;
        precedenceToPassOn = 1000;
      } else if (thisToken.kind == LBRACKET) { // Start of a list
	thisTag = NON_OPERATOR;
        thisTagToPassOn = PREFIX;
        precedenceToPassOn = 1001;
      } else if (thisToken.kind == LPAREN) {
	thisTag = NON_OPERATOR;
        thisTagToPassOn = PREFIX;
        precedenceToPassOn = 1201;
      } else if (thisToken.kind == RPAREN || thisToken.kind == RBRACKET) {
        thisTag = thisTagToPassOn = NON_OPERATOR;
        precedenceToPassOn = 0;
      } else if (thisToken.kind == PERIOD) {
        thisTag = thisTagToPassOn = POSTFIX; // Acts like a postfix operator
        precedenceToPassOn = 1201;
      } else if (thisToken.kind == END_OF_FILE) {
        thisTag = thisTagToPassOn = NON_OPERATOR;
        precedenceToPassOn = 1201;
      } else if (thisToken.kind == ATOM || thisToken.kind == COMMA) { // Try for various kinds of operators
        int infixPrecedenceForBefore = pInfix.second - ((pInfix.first == XFX || pInfix.first == XFY) ? 1 : 0);
        int postfixPrecedenceForBefore = pPostfix.second - (pPostfix.first == XF ? 1 : 0);
        int prefixInfixNextPrecedence = ((nextToken.kind == ATOM || nextToken.kind == COMMA) && !nextToken.startOfTerm) ?
                                        ((getPrefixDefinition(nextToken.stringValue).first != NOT_AN_OPERATOR) ?
                                         getPrefixDefinition(nextToken.stringValue).second :
                                         0) :
                                        (nextToken.kind == RPAREN || nextToken.kind == RBRACKET || nextToken.kind == PERIOD) ?
                                        1201 :
                                        0; // Only consider prefix or non-operator cases for after a prefix or infix operator
        int postfixNextPrecedence = ((nextToken.kind == ATOM || nextToken.kind == COMMA || nextToken.kind == VERTICAL_BAR) && !nextToken.startOfTerm) ?
                                    (std::max)(((getPostfixDefinition(nextToken.stringValue).first != NOT_AN_OPERATOR) ?
                                                getPostfixDefinition(nextToken.stringValue).second :
                                                0),
                                               ((getInfixDefinition(nextToken.stringValue).first != NOT_AN_OPERATOR) ?
                                                getInfixDefinition(nextToken.stringValue).second :
                                                0)) :
                                    (nextToken.kind == RPAREN || nextToken.kind == RBRACKET || nextToken.kind == PERIOD) ?
                                    1201 :
                                    0; // Only consider postfix or infix cases for after a postfix operator
        bool nonoperatorPrecedence = (pPrefix.first != NOT_AN_OPERATOR) ? pPrefix.second : 0;
        bool canBePrefix = (pPrefix.first != NOT_AN_OPERATOR &&
                            !thisToken.startOfTerm &&
                            pPrefix.second <= lastPrecedence &&
                            pPrefix.second >= prefixInfixNextPrecedence &&
			    lastTag != POSTFIX && lastTag != NON_OPERATOR);
        bool canBeInfix = (pInfix.first != NOT_AN_OPERATOR &&
                           infixPrecedenceForBefore >= lastPrecedence &&
                           infixPrecedenceForBefore >= prefixInfixNextPrecedence &&
			   lastTag != PREFIX);
        bool canBePostfix = (pPostfix.first != NOT_AN_OPERATOR &&
                             postfixPrecedenceForBefore >= lastPrecedence &&
                             postfixPrecedenceForBefore <= postfixNextPrecedence &&
			     lastTag != PREFIX && lastTag != INFIX);
        bool canBeNonoperator = nonoperatorPrecedence <= lastPrecedence &&
				nonoperatorPrecedence <= postfixNextPrecedence &&
				lastTag != POSTFIX && lastTag != NON_OPERATOR;
        if (canBePrefix && (canBeInfix || canBePostfix || canBeNonoperator) ||
            canBeInfix && (canBePostfix || canBeNonoperator) ||
            canBePostfix && canBeNonoperator) {
#if 0
          std::cerr << "Ambiguous operator definition" << (canBePrefix ? " PREFIX" : "") << (canBeInfix ? " INFIX" : "") << (canBePostfix ? " POSTFIX" : "") << (canBeNonoperator ? " NONOP" : "") << std::endl;
#endif
          parseError(thisToken, "Ambiguous use of operator");
        }
        if (!canBePrefix && !canBeInfix && !canBePostfix && !canBeNonoperator) {
          bool loosePrefix = pPrefix.first != NOT_AN_OPERATOR;
          bool looseInfix = pInfix.first != NOT_AN_OPERATOR;
          bool loosePostfix = pPostfix.first != NOT_AN_OPERATOR;
          bool looseNonoperator = lastTag == INFIX || lastTag == PREFIX;
#if 1
          std::cerr << "Cannot resolve operator definition" << std::endl;
	  std::cerr << "Last tag is " << lastTag << std::endl;
          std::cerr << "For prefix" << (pPrefix.first != NOT_AN_OPERATOR ? "" : " (not defined)") << ": " << lastPrecedence << " " << pPrefix.second << " " << prefixInfixNextPrecedence << std::endl;
          std::cerr << "For infix" << (pInfix.first != NOT_AN_OPERATOR ? "" : " (not defined)") << ": " << lastPrecedence << " " << pInfix.second << " " << prefixInfixNextPrecedence << std::endl;
          std::cerr << "For postfix" << (pPostfix.first != NOT_AN_OPERATOR ? "" : " (not defined)") << ": " << lastPrecedence << " " << pPostfix.second << " " << postfixNextPrecedence << std::endl;
          std::cerr << "For non-op" << (looseNonoperator ? "" : " (not valid here)") << ": " << lastPrecedence << " " << nonoperatorPrecedence << " " << postfixNextPrecedence << std::endl;
#endif
          if (loosePrefix && !looseInfix && !loosePostfix) {
            parseError(thisToken, "Bad precedences near (likely) prefix operator");
          } else if (!loosePrefix && looseInfix && !loosePostfix) {
            parseError(thisToken, "Bad precedences near (likely) infix operator");
          } else if (!loosePrefix && !looseInfix && loosePostfix) {
            parseError(thisToken, "Bad precedences near (likely) postfix operator");
          } else if (looseNonoperator) {
            parseError(thisToken, "Bad precedences near (likely) non-operator");
          } else {
            parseError(thisToken, "Parse error (bad precedences?)");
          }
          goto cantTag;
        }
        if (canBePrefix) {
          thisTag = thisTagToPassOn = PREFIX;
          precedenceToPassOn = pPrefix.second - (pPrefix.first == FX ? 1 : 0);
        } else if (canBeInfix) {
          thisTag = thisTagToPassOn = INFIX;
          precedenceToPassOn = pInfix.second - ((pInfix.first == XFX || pInfix.first == YFX) ? 1 : 0);
        } else if (canBePostfix) {
          thisTag = thisTagToPassOn = POSTFIX;
          precedenceToPassOn = pPostfix.second;
        } else {
          thisTag = thisTagToPassOn = NON_OPERATOR;
          precedenceToPassOn = (pPrefix.first != NOT_AN_OPERATOR) ? pPrefix.second : 0;
        }
      } else {
cantTag:
        std::cerr << "Cannot tag " << lastToken << " " << lastTag << ":" << lastPrecedence << " < " << thisToken << " > " << nextToken << std::endl;
        exit(1);
      }
      // std::cerr << "Tagged " << thisToken << " as " << thisTag << ", passing on tag " << thisTagToPassOn << std::endl;
    }
  }

  struct OperatorStackEntry {
    PrologToken token;
    int precedence;
    int arity;

    OperatorStackEntry(PrologToken token, int precedence, int arity)
      : token(token), precedence(precedence), arity(arity) {}
  };

  ostream& operator<<(ostream& os, const OperatorStackEntry& se) {
    os << se.token;
    if (se.token.kind == ATOM || se.token.kind == COMMA) {
      os << "/" << se.arity;
    }
    return os;
  }

  typedef stack<OperatorStackEntry> OperatorStack;

  void processTokenAtPop(const OperatorStackEntry& se,
                         stack<PrologAst::Node*>& treeBuildStack) {
    // cerr << "Popping " << se.token << " prec=" << se.precedence << endl;
    switch (se.token.kind) {
      case ATOM: case COMMA: {
        vector<PrologAst::Node*> children;
        for (int i = 0; i < se.arity; ++i) {
          if (treeBuildStack.empty()) {parseError(se.token, "Internal error: treeBuildStack empty");}
          // cerr << "Removing " << treeBuildStack.top() << " from tree stack" << endl;
          children.insert(children.begin(), treeBuildStack.top());
          treeBuildStack.pop();
        }
        PrologAst::Node* n = new PrologAst::Term(se.token.stringValue, children);
        // cerr << "Adding " << n << " to tree stack" << endl;
        treeBuildStack.push(n);
      } break;

      default: parseError(se.token, "Bad token kind on stack (bad precedence?)");
    }
  }

  void dumpStack(stack<PrologAst::Node*> st) { // Works on a copy
    cerr << "BEGIN" << endl;
    while (!st.empty()) {cerr << st.top() << endl; st.pop();}
    cerr << "END" << endl;
  }

  PrologAst::Node* commaAndBarToList(PrologAst::Node* n) {
    // Convert a term made up of , and | operators into a dot-list
    PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
    if (t && t->getFunctor() == "|" && t->getArity() == 2) {
      return new PrologAst::Term(".", t->getArgs());
    } else if (t && t->getFunctor() == "," && t->getArity() == 2) {
      vector<PrologAst::Node*> args(2);
      args[0] = t->getArgs()[0];
      args[1] = commaAndBarToList(t->getArgs()[1]);
      return new PrologAst::Term(".", args);
    } else {
      vector<PrologAst::Node*> args(2);
      args[0] = n;
      args[1] = new PrologAst::Term("[]", vector<PrologAst::Node*>());
      return new PrologAst::Term(".", args);
    }
  }

  vector<PrologAst::Node*> parseFile(const std::string& fileName);

  enum GroupingKind {NO_GROUPING, TERM_GROUPING, PAREN_GROUPING, BRACKET_GROUPING, BRACE_GROUPING};
  ostream& operator<<(ostream& os, GroupingKind gk) {
    switch (gk) {
      case NO_GROUPING: os << "NO_GROUPING"; return os;
      case TERM_GROUPING: os << "TERM_GROUPING"; return os;
      case PAREN_GROUPING: os << "PAREN_GROUPING"; return os;
      case BRACKET_GROUPING: os << "BRACKET_GROUPING"; return os;
      case BRACE_GROUPING: os << "BRACE_GROUPING"; return os;
      default: os << "UNKNOWN_GROUPING"; return os;
    }
  }

  PrologAst::Node*
  parse(const vector<PrologToken>& tokens,
        const vector<OperatorTag>& tags,
        unsigned int& index) { // Reads a sentence, returning its AST (and updating index); returns NULL for EOF
    // Assumes tokens.size() == tags.size()
    vector<PrologAst::Node*> output;
    OperatorStack st;
    stack<PrologAst::Node*> treeBuildStack;
    stack<GroupingKind> groupingStack; // Most recent kind of grouping found (for disambiguating comma and vertical bar)
    groupingStack.push(NO_GROUPING); // This stack should never be empty
    bool lastSeenWasOperator = true; // For prefix and infix operators, not non-operators or postfix operators
    for (; index != tokens.size(); ++index) {
      PrologToken tok = tokens[index];
      OperatorTag tag = tags[index];
      // cerr << "Token " << tok << "  tag " << tag << endl;
      switch (tok.kind) {
        case END_OF_FILE: {
          if (groupingStack.top() != NO_GROUPING) {
            parseError(tok, "End of file inside grouping construct");
          }
          return NULL;
        }

        case ATOM: case COMMA: case VERTICAL_BAR: {
          switch (tag) {
            case PREFIX: {
              pair<Associativity, int> pPrefix = getPrefixDefinition(tok.stringValue);
              int rightPrecedence = pPrefix.second;
              if (pPrefix.first == FX) --rightPrecedence;
              st.push(OperatorStackEntry(tok, rightPrecedence, 1));
              lastSeenWasOperator = true;
              break;
            }
            case INFIX: {
              pair<Associativity, int> pInfix = getInfixDefinition(tok.stringValue);
              int leftPrecedence = pInfix.second,
                  rightPrecedence = pInfix.second;
              if (pInfix.first == XFX || pInfix.first == XFY) --leftPrecedence;
              if (pInfix.first == XFX || pInfix.first == YFX) --rightPrecedence;
              while (!st.empty() &&
                     st.top().precedence <= leftPrecedence) {
                processTokenAtPop(st.top(), treeBuildStack);
                st.pop();
              }
              st.push(OperatorStackEntry(tok, rightPrecedence, 2));
              lastSeenWasOperator = true;
              break;
            }
            case POSTFIX: {
              pair<Associativity, int> pPostfix = getPostfixDefinition(tok.stringValue);
              int leftPrecedence = pPostfix.second,
                  rightPrecedence = pPostfix.second;
              if (pPostfix.first == XF) --leftPrecedence;
              while (!st.empty() &&
                     st.top().precedence <= leftPrecedence) {
                processTokenAtPop(st.top(), treeBuildStack);
                st.pop();
              }
              processTokenAtPop(OperatorStackEntry(tok, rightPrecedence, 1), treeBuildStack);
              lastSeenWasOperator = false;
              break;
            }
            case NON_OPERATOR: {
              pair<Associativity, int> pPrefix = getPrefixDefinition(tok.stringValue);
              if (tok.startOfTerm) {
                st.push(OperatorStackEntry(tok, 1001, -1));
                groupingStack.push(TERM_GROUPING);
                lastSeenWasOperator = true;
              } else { // Zero-arity term
                // cerr << "Putting non-op " << tok << " on tree stack" << endl;
                treeBuildStack.push(new PrologAst::Term(tok.stringValue, vector<PrologAst::Node*>()));
                lastSeenWasOperator = false;
              }
              break;
            }
            default: cerr << "Bad tag " << tag << endl; abort();
          }
        } break;

        case PERIOD: {
          if (groupingStack.top() != NO_GROUPING) {
            parseError(tok, "Grouping construct not closed at period");
          }
          while (!st.empty()) {
            processTokenAtPop(st.top(), treeBuildStack);
            st.pop();
          }
          if (treeBuildStack.size() != 1) {
            cerr << "Stack has incorrect size at period" << endl;
            dumpStack(treeBuildStack);
            abort();
          }
          PrologAst::Node* sentence = treeBuildStack.top();
          // cerr << "Parsed sentence " << sentence << endl;
          treeBuildStack.pop();
          ++index; // Skip over period
          return sentence;
        } break;

        case VARIABLE: {
          // cerr << "Pushing special onto tree stack" << endl;
          treeBuildStack.push(new PrologAst::Variable(tok.stringValue));
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before variable");
          lastSeenWasOperator = false;
        } break;

        case WILDCARD: {
          // cerr << "Pushing special onto tree stack" << endl;
          treeBuildStack.push(new PrologAst::Wildcard());
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before wildcard");
          lastSeenWasOperator = false;
        } break;

        case INTEGER: {
          // cerr << "Pushing special onto tree stack" << endl;
          treeBuildStack.push(new PrologAst::Integer(tok.intValue));
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before integer");
          lastSeenWasOperator = false;
        } break;

        case FLOATING_POINT: {
          // cerr << "Pushing special onto tree stack" << endl;
          treeBuildStack.push(new PrologAst::FloatingPoint(tok.floatingValue));
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before floating-point value");
          lastSeenWasOperator = false;
        } break;

        case STRING: {
          // cerr << "Pushing special onto tree stack" << endl;
          treeBuildStack.push(new PrologAst::String(tok.stringValue));
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before string");
          lastSeenWasOperator = false;
        } break;

        case LPAREN: {
          st.push(OperatorStackEntry(tok, 99999, 1));
          groupingStack.push(PAREN_GROUPING);
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before left parenthesis");
          lastSeenWasOperator = true;
        } break;

        case RPAREN: {
          // cerr << "Right paren case" << tok << endl;
          GroupingKind lastGrouping = groupingStack.top();
          if (lastGrouping != PAREN_GROUPING && lastGrouping != TERM_GROUPING) {
            parseError(tok, "Right parenthesis not matched by left parenthesis");
          }
          groupingStack.pop();
          if (lastSeenWasOperator) parseError(tok, "Expected non-operator before right-parenthesis");
          switch (lastGrouping) {
            case PAREN_GROUPING: {
              while (!st.empty() && st.top().token.kind != LPAREN) {
                // cerr << "Popping for rparen: " << st.top().token << " at " << st.top().precedence << endl;
                processTokenAtPop(st.top(), treeBuildStack);
                st.pop();
              }
              if (st.empty()) {
                parseError(tok, "Mismatched parentheses");
              }
              st.pop();
              lastSeenWasOperator = false;
              break;
            }

            case TERM_GROUPING: {
              vector<PrologAst::Node*> result;
              while (!st.empty() && !st.top().token.startOfTerm) {
                // cerr << "Popping for rparen: " << st.top().token << " at " << st.top().precedence << endl;
                // dumpStack(treeBuildStack);
                if (st.top().precedence >= 1000 && st.top().token.kind != COMMA) {
                  parseError(st.top().token, "Bad operator inside term (precedence too high)");
                }
                if (st.top().token.kind == COMMA) {
                  if (treeBuildStack.empty()) parseError(tok, "Internal error: Bad treeBuildStack term");
                  result.insert(result.begin(), treeBuildStack.top());
                  treeBuildStack.pop();
                } else {
                  processTokenAtPop(st.top(), treeBuildStack);
                }
                st.pop();
              }
              if (st.empty() ||
                  (st.top().token.kind != ATOM && st.top().token.kind != COMMA) ||
                  !st.top().token.startOfTerm) {
                parseError(tok, "Mismatched parentheses");
              }
              if (treeBuildStack.empty()) parseError(tok, "Internal error: Bad treeBuildStack term 2");
              result.insert(result.begin(), treeBuildStack.top());
              treeBuildStack.pop();
              PrologToken termStartToken = st.top().token;
              st.pop();
              treeBuildStack.push(new PrologAst::Term(termStartToken.stringValue, result));
              lastSeenWasOperator = false;
              break;
            }

            default: parseError(tok, "Internal error: Bad lastGrouping value");
          }
        } break;

        case LBRACKET: {
          st.push(OperatorStackEntry(tok, 1001, 1));
          groupingStack.push(BRACKET_GROUPING);
          if (!lastSeenWasOperator) parseError(tok, "Expected operator before left bracket");
          lastSeenWasOperator = true;
        } break;

        case RBRACKET: {
          if (groupingStack.top() != BRACKET_GROUPING) {
            parseError(tok, "Right bracket not matched by left bracket");
          }
          groupingStack.pop();
          if (lastSeenWasOperator && !st.empty() && st.top().token.kind == LBRACKET) {
            // cerr << "Pushing empty list" << endl;
            treeBuildStack.push(
                new PrologAst::Term("[]", vector<PrologAst::Node*>()));
            st.pop();
            break;
          }
          if (lastSeenWasOperator || st.empty()) {
            parseError(tok, "Need a non-operator before a right bracket");
          }
          PrologAst::Node* result = new PrologAst::Term("[]", vector<PrologAst::Node*>());
          while (!st.empty() && st.top().token.kind != LBRACKET) {
            // cerr << "Popping for rbracket: " << st.top().token << " at " << st.top().precedence << endl;
            // dumpStack(treeBuildStack);
            if (st.top().precedence >= 1000 && st.top().token.kind != COMMA && st.top().token.kind != VERTICAL_BAR) {
              parseError(st.top().token, "Bad operator inside list (precedence too high)");
            }
            if (st.top().token.kind == COMMA) {
              if (treeBuildStack.empty()) parseError(tok, "Internal error: Bad treeBuildStack list comma");
              vector<PrologAst::Node*> children(2);
              children[0] = treeBuildStack.top();
              treeBuildStack.pop();
              children[1] = result;
              result = new PrologAst::Term(".", children);
            } else if (st.top().token.kind == VERTICAL_BAR) {
              if (treeBuildStack.empty()) parseError(tok, "Internal error: Bad treeBuildStack list bar");
              result = treeBuildStack.top();
              treeBuildStack.pop();
            } else {
              processTokenAtPop(st.top(), treeBuildStack);
            }
            st.pop();
          }
          // cerr << "Done with rbracket loop " << result << endl;
          // dumpStack(treeBuildStack);
          if (st.empty() || st.top().token.kind != LBRACKET) {
            parseError(tok, "Mismatched brackets");
          }
          st.pop();
          if (treeBuildStack.empty()) parseError(tok, "Internal error: Bad treeBuildStack list 2");
          vector<PrologAst::Node*> children(2);
          children[0] = treeBuildStack.top();
          treeBuildStack.pop();
          children[1] = result;
          result = new PrologAst::Term(".", children);
          treeBuildStack.push(result);
          lastSeenWasOperator = false;
        } break;

        default: parseError(tok, "Bad token");
      }
    }
    cerr << "Reached end of loop -- missing period at end of program?" << endl;
    exit (1);
  }

  PrologAst::Node* parseSentence(const std::vector<PrologToken>& tokens) {
    std::vector<OperatorTag> tags;
    tagOperators(tokens, tags);
    unsigned int index = 0;
    return parse(tokens, tags, index);
  }

}
