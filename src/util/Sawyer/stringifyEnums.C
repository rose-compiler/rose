// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




static const char *purpose = "generate enum-to-string functions";
static const char *description =
    "Scans the specified source files and generates functions that convert enum values into enum name strings.";

#include <Sawyer/Clexer.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Graph.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Map.h>
#include <boost/algorithm/string/predicate.hpp>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <string>
#include <vector>

using namespace Sawyer::Language::Clexer;
using namespace Sawyer::Message::Common;

static Sawyer::Message::Facility mlog;
static bool emitImplementations = false;
static std::string outerNamespace = "stringify";
static bool emitCompatible = false;
static bool deprecateCompatible = true;
std::string relativeBase;

static std::vector<boost::filesystem::path>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser.errorStream(mlog[FATAL]);
    parser.purpose(purpose);
    parser.doc("Description", description);

    parser.with(Switch("help", 'h')
                .action(showHelpAndExit(0))
                .doc("Show this documentation."));

    parser.with(Switch("log")
                .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                .argument("config")
                .whichValue(SAVE_ALL)
                .doc("Configures diagnostics."));

    parser.with(Switch("namespace")
                .argument("name", anyParser(outerNamespace))
                .doc("Namespace in which to enclose everything. Defaults to \"" + outerNamespace + "\"."));

    parser.with(Switch("implementations")
                .longName("implementation")
                .intrinsicValue(true, emitImplementations)
                .doc("Generate function implementations.\n"));
    parser.with(Switch("declarations")
                .longName("declaration")
                .intrinsicValue(false, emitImplementations)
                .doc("Generate function declarations.\n"));

    parser.with(Switch("compatible")
                .intrinsicValue(true, emitCompatible)
                .doc("Also emit code that's compatible with the old ROSE stringification system."));
    parser.with(Switch("deprecate")
                .intrinsicValue(true, deprecateCompatible)
                .doc("When generating ROSE compatibility functions, mark them as deprecated. The "
                     "@s{no-deprecate} switch causes them to not be marked, although they will still "
                     "not be documented either.  The defautl is to " +
                     std::string(deprecateCompatible ? "" : "not ") + "mark as deprecated."));
    parser.with(Switch("no-deprecate")
                .intrinsicValue(false, deprecateCompatible)
                .hidden(true));

    parser.with(Switch("root")
                .argument("directory", anyParser(relativeBase))
                .doc("Strip @v{directory} from file names in the documentation."));
    
    std::vector<boost::filesystem::path> retval;
    BOOST_FOREACH (const boost::filesystem::path &path, parser.parse(argc, argv).apply().unreachedArgs())
        retval.push_back(path);
    return retval;
}

typedef std::pair<size_t /*line*/, size_t /*col*/> Location;

struct NameAndLocation {
    std::string fileName;
    Location location;

    NameAndLocation() {}
    NameAndLocation(const std::string &fileName, const Location &location)
        : fileName(fileName), location(location) {}
};

typedef Sawyer::Container::Map<std::string, NameAndLocation> NamedLocationMap;

struct Scope {
    std::string name;
    Location location;

    Scope() {}
    Scope(const std::string &name, const Location &location)
        : name(name), location(location) {}
    explicit Scope(const Location &location)
        : location(location) {}
};

typedef std::vector<Scope> Scopes;

std::ostream&
operator<<(std::ostream &out, const Scopes &scopes) {
    BOOST_FOREACH (const Scope &scope, scopes) {
        if (!scope.name.empty())
            out <<"::" <<scope.name;
    }
    return out;
}

typedef Sawyer::Container::Map<std::string, int64_t> SymbolValues;
typedef SymbolValues EnumMembers;

static SymbolValues globalSymbols;
static EnumMembers currentEnumMembers;
static NamedLocationMap emittedEnums;

typedef Sawyer::Container::Graph<std::string> NamespaceLattice;
typedef Sawyer::Container::Map<std::string, NamespaceLattice::VertexIterator> NamespaceLatticeRoots;
static NamespaceLattice namespaceLattice;
static NamespaceLatticeRoots namespaceLatticeRoots;

static std::string
where(const boost::filesystem::path &filename, const std::pair<size_t, size_t> &location) {
    std::ostringstream ss;
    ss <<filename.string() <<":" <<(location.first+1) <<"." <<(location.second+1);
    return ss.str();
}

static bool
isAnonymous(const Scopes &scopes) {
    BOOST_FOREACH (const Scope &scope, scopes) {
        if (boost::starts_with(scope.name, "<anonymous"))
            return true;
    }
    return false;
}

static void
parseExtern(TokenStream &tokens, Scopes &scopes) {
    ASSERT_require(tokens.matches(tokens[0], "extern"));

    if (tokens[1].type() == TOK_STRING && tokens.matches(tokens[2], "{")) {
        scopes.push_back(Scope("<extern " + tokens.lexeme(tokens[1]) + ">", tokens.location(tokens[0])));
        tokens.consume(3);
    } else {
        tokens.consume();
    }
}

static void
parseNamespace(TokenStream &tokens, Scopes &scopes) {
    ASSERT_require(tokens.matches(tokens[0], "namespace"));
    tokens.consume();

    std::string name;
    if (tokens.matches(tokens[0], "{")) {
        scopes.push_back(Scope("<anonymous-namespace>", tokens.location(tokens[0])));
        SAWYER_MESG(mlog[DEBUG]) <<where(tokens.fileName(), tokens.location(tokens[0])) <<": enter namespace " <<scopes <<"\n";
        tokens.consume();
    } else if (tokens[0].type() == TOK_WORD && tokens.matches(tokens[1], "{")) {
        scopes.push_back(Scope(tokens.lexeme(tokens[0]), tokens.location(tokens[0])));
        SAWYER_MESG(mlog[DEBUG]) <<where(tokens.fileName(), tokens.location(tokens[0])) <<": enter namespace " <<scopes <<"\n";
        tokens.consume(2);
    } else {
        // not a namespace declaration
    }
}

static void
parseClass(TokenStream &tokens, Scopes &scopes) {
    ASSERT_require(tokens.matches(tokens[0], "class") || tokens.matches(tokens[0], "struct"));
    Location loc = tokens.location(tokens[0]);
    tokens.consume();

    // Class name is the last word after class:
    //   class DLL_EXPORT Foo
    std::string name = "<anonymous-class>";
    while (tokens[0].type() == TOK_WORD) {
        name = tokens.lexeme(tokens[0]);
        loc = tokens.location(tokens[0]);
        tokens.consume();
    }

    // If the class name is followed by a ":" but not "::" then we're defining a class and can scan up to the opening brace.
    if (tokens.matches(tokens[0], ":")) {
        tokens.consume();
        while (tokens[0].type() != TOK_EOF && !tokens.matches(tokens[0], "{"))
            tokens.consume();
    }

    // If we're not at an opening brace then this isn't a class definition.
    if (!tokens.matches(tokens[0], "{"))
        return;
    
    scopes.push_back(Scope(name, loc));
    SAWYER_MESG(mlog[DEBUG]) <<where(tokens.fileName(), loc) <<": enter class " <<scopes <<"\n";
    tokens.consume();                                   // the opening brace
}

static int64_t
parseIntegralConstant(TokenStream &tokens) {
    int64_t retval = 0;
    std::string s = tokens.lexeme(tokens[0]);
    if (boost::starts_with(s, "0b") && s.size() > 2) {
        for (size_t i = 2; i < s.size(); ++i) {
            if (s[i] == '0') {
                retval = retval << 1;
            } else if (s[i] == '1') {
                retval = (retval << 1) | 1;
            } else {
                std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: invalid integer literal\n";
                break;
            }
        }
    } else {
        char *rest = NULL;
        errno = 0;
        retval = (int64_t) strtoul(s.c_str(), &rest, 0);
        if (errno != 0 || *rest)
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: invalid integer literal\n";
    }
    tokens.consume();
    return retval;
}

static int64_t parseCommaExpr(TokenStream&);
static int64_t parseConstExpr(TokenStream&);

static int64_t
parseTermExpr(TokenStream &tokens) {
    int64_t retval = 0;
    if (tokens[0].type() == TOK_NUMBER) {
        retval = parseIntegralConstant(tokens);
    } else if (currentEnumMembers.getOptional(tokens.lexeme(tokens[0])).assignTo(retval)) {
        tokens.consume();
    } else if (globalSymbols.getOptional(tokens.lexeme(tokens[0])).assignTo(retval)) {
        tokens.consume();
    } else if (tokens[0].type() == TOK_WORD) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: symbol or variable \"" <<tokens.lexeme(tokens[0]) <<"\" not supported\n";
        tokens.consume();
    } else if (tokens[0].type() == TOK_CHAR && tokens.lexeme(tokens[0]).size()==3) {
        retval = tokens.lexeme(tokens[0])[1];
        tokens.consume();
    } else {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: number or name expected\n";
        tokens.consume();
    }
    return retval;
}

static int64_t
parseParenExpr(TokenStream &tokens) {
    if (tokens.matches(tokens[0], "(")) {
        tokens.consume();
        int64_t retval = parseCommaExpr(tokens);
        if (tokens.matches(tokens[0], ")")) {
            tokens.consume();
        } else {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                      <<": error: \")\" expected\n";
        }
        return retval;
    } else {
        return parseTermExpr(tokens);
    }
}

static int64_t
parseScopeExpr(TokenStream &tokens) {
    int64_t retval = parseParenExpr(tokens);
    while (tokens.matches(tokens[0], "::")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: \"::\" is not supported\n";
        tokens.consume();
        (void) parseParenExpr(tokens);
    }
    return retval;
}

static int64_t
parseSuffixExpr(TokenStream &tokens) {
    int64_t retval = parseScopeExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "++") || tokens.matches(tokens[0], "--")) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                      <<": error: \"" <<tokens.lexeme(tokens[0]) <<"\" not supported\n";
            tokens.consume();
            (void) parseScopeExpr(tokens);
        } else if (tokens.matches(tokens[0], "(")) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                      <<": error: function calls and type casts not supported\n";
            tokens.consume();
            (void) parseCommaExpr(tokens);
            if (tokens.matches(tokens[0], ")")) {
                tokens.consume();
            } else {
                std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                          <<": error: expected \")\"\n";
            }
        } else if (tokens.matches(tokens[0], "[")) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                      <<": error: an array reference cannot appear in a constant-expression\n";
            tokens.consume();
            (void) parseConstExpr(tokens);
            if (tokens.matches(tokens[0], "]")) {
                tokens.consume();
            } else {
                std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                          <<": error: expected \"]\"\n";
            }
        } else if (tokens.matches(tokens[0], ".") || tokens.matches(tokens[0], "->")) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                      <<": error: \"" <<tokens.lexeme(tokens[0]) <<"\" not supported\n";
            tokens.consume();
            (void) parseScopeExpr(tokens);
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parsePrefixExpr(TokenStream &tokens) {
    if ((tokens.matches(tokens[0], "delete") || tokens.matches(tokens[0], "new")) &&
        tokens.matches(tokens[1], "[") && tokens.matches(tokens[2], "]")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: \"" <<tokens.lexeme(tokens[0]) <<"[]\" cannot appear in a constant-expression\n";
        tokens.consume(3);
        return parsePrefixExpr(tokens);
    } else if (tokens.matches(tokens[0], "delete") || tokens.matches(tokens[0], "new") || tokens.matches(tokens[0], "sizeof") ||
               tokens.matches(tokens[0], "&") || tokens.matches(tokens[0], "*") || false/*C-style-type-case*/ ||
               tokens.matches(tokens[0], "++") || tokens.matches(tokens[0], "--")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: \"" <<tokens.lexeme(tokens[0]) <<"\" cannot appear in a constant-expression\n";
        tokens.consume();
        return parsePrefixExpr(tokens);
    } else if (tokens.matches(tokens[0], "+")) {
        tokens.consume();
        return parsePrefixExpr(tokens);
    } else if (tokens.matches(tokens[0], "-")) {
        tokens.consume();
        return -parsePrefixExpr(tokens);
    } else if (tokens.matches(tokens[0], "!")) {
        tokens.consume();
        return !parsePrefixExpr(tokens);
    } else if (tokens.matches(tokens[0], "~")) {
        tokens.consume();
        return ~parsePrefixExpr(tokens);
    } else {
        return parseSuffixExpr(tokens);
    }
}

static int64_t
parsePointerExpr(TokenStream &tokens) {
    int64_t retval = parsePrefixExpr(tokens);
    while (tokens.matches(tokens[0], ".*") || tokens.matches(tokens[0], "->*")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: \"" <<tokens.lexeme(tokens[0]) <<"\" not supported\n";
        tokens.consume();
        (void) parsePrefixExpr(tokens);
    }
    return retval;
}

static int64_t
parseMultiplicativeExpr(TokenStream &tokens) {
    int64_t retval = parsePointerExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "*")) {
            tokens.consume();
            int64_t b = parsePointerExpr(tokens);
            retval *= b;
        } else if (tokens.matches(tokens[0], "/")) {
            tokens.consume();
            int64_t b = parsePointerExpr(tokens);
            retval /= b;
        } else if (tokens.matches(tokens[0], "%")) {
            tokens.consume();
            int64_t b = parsePointerExpr(tokens);
            retval %= b;
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parseAdditiveExpr(TokenStream &tokens) {
    int64_t retval = parseMultiplicativeExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "+")) {
            tokens.consume();
            int64_t b = parseMultiplicativeExpr(tokens);
            retval += b;
        } else if (tokens.matches(tokens[0], "-")) {
            tokens.consume();
            int64_t b = parseMultiplicativeExpr(tokens);
            retval -= b;
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parseShiftExpr(TokenStream &tokens) {
    int64_t retval = parseAdditiveExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "<<")) {
            tokens.consume();
            int64_t b = parseAdditiveExpr(tokens);
            retval = retval << b;
        } else if (tokens.matches(tokens[0], ">>")) {
            tokens.consume();
            int64_t b = parseAdditiveExpr(tokens);
            retval = retval >> b;
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parseRelationalExpr(TokenStream &tokens) {
    int64_t retval = parseShiftExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "<")) {
            tokens.consume();
            int64_t b = parseShiftExpr(tokens);
            retval = retval < b;
        } else if (tokens.matches(tokens[0], "<=")) {
            tokens.consume();
            int64_t b = parseShiftExpr(tokens);
            retval = retval <= b;
        } else if (tokens.matches(tokens[0], ">")) {
            tokens.consume();
            int64_t b = parseShiftExpr(tokens);
            retval = retval > b;
        } else if (tokens.matches(tokens[0], ">=")) {
            tokens.consume();
            int64_t b = parseShiftExpr(tokens);
            retval = retval >= b;
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parseEqualityExpr(TokenStream &tokens) {
    int64_t retval = parseRelationalExpr(tokens);
    while (1) {
        if (tokens.matches(tokens[0], "==")) {
            tokens.consume();
            int64_t b = parseRelationalExpr(tokens);
            retval = retval == b;
        } else if (tokens.matches(tokens[0], "!=")) {
            tokens.consume();
            int64_t b = parseRelationalExpr(tokens);
            retval = retval != b;
        } else {
            break;
        }
    }
    return retval;
}

static int64_t
parseBitwiseAndExpr(TokenStream &tokens) {
    int64_t retval = parseEqualityExpr(tokens);
    while (tokens.matches(tokens[0], "&")) {
        tokens.consume();
        int64_t b = parseEqualityExpr(tokens);
        retval &= b;
    }
    return retval;
}

static int64_t
parseBitwiseXorExpr(TokenStream &tokens) {
    int64_t retval = parseBitwiseAndExpr(tokens);
    while (tokens.matches(tokens[0], "^")) {
        tokens.consume();
        int64_t b = parseBitwiseAndExpr(tokens);
        retval ^= b;
    }
    return retval;
}

static int64_t
parseBitwiseOrExpr(TokenStream &tokens) {
    int64_t retval = parseBitwiseXorExpr(tokens);
    while (tokens.matches(tokens[0], "|")) {
        tokens.consume();
        int64_t b = parseBitwiseXorExpr(tokens);
        retval |= b;
    }
    return retval;
}

static int64_t
parseLogicalAndExpr(TokenStream &tokens) {
    int64_t retval = parseBitwiseOrExpr(tokens);
    while (tokens.matches(tokens[0], "&&")) {
        tokens.consume();
        int64_t b = parseBitwiseOrExpr(tokens);
        retval = retval && b;
    }
    return retval;
}

static int64_t
parseLogicalOrExpr(TokenStream &tokens) {
    int64_t retval = parseLogicalAndExpr(tokens);
    while (tokens.matches(tokens[0], "||")) {
        tokens.consume();
        int64_t b = parseLogicalAndExpr(tokens);
        retval = retval || b;
    }
    return retval;
}

static int64_t
parseAssignExpr(TokenStream &tokens) {
    int64_t retval = parseLogicalOrExpr(tokens);
    if (tokens.matches(tokens[0], "=") || tokens.matches(tokens[0], "+=") || tokens.matches(tokens[0], "-=") ||
        tokens.matches(tokens[0], "*=") || tokens.matches(tokens[0], "/=") || tokens.matches(tokens[0], "%=") ||
        tokens.matches(tokens[0], "<<=") || tokens.matches(tokens[0], ">>=") || tokens.matches(tokens[0], "&=") ||
        tokens.matches(tokens[0], "^=") || tokens.matches(tokens[0], "|=")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: \"" <<tokens.lexeme(tokens[0]) <<"\" cannot appear in a constant-expression\n";
        tokens.consume();
        retval = parseAssignExpr(tokens);
    } else if (tokens.matches(tokens[0], "?")) {
        tokens.consume();
        int64_t a = parseLogicalOrExpr(tokens);
        if (tokens.matches(tokens[0], ":")) {
            tokens.consume();
            int64_t b = parseAssignExpr(tokens);
            retval = retval ? a : b;
        } else {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: \":\" expected\n";
        }
    }
    return retval;
}

static int64_t
parseConstExpr(TokenStream &tokens) {
    return parseAssignExpr(tokens);
}

static int64_t
parseCommaExpr(TokenStream &tokens) {
    int64_t retval = parseConstExpr(tokens);
    while (tokens.matches(tokens[0], ",")) {
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0]))
                  <<": error: comma operator cannot appear in a constant-expression\n";
        tokens.consume();
        retval = parseConstExpr(tokens);
    }
    return retval;
}

static std::string
fullyQualifiedEnumName(const Scopes &scopes) {
    std::string retval;
    BOOST_FOREACH (const Scope &scope, scopes) {
        if (!scope.name.empty() && !boost::starts_with(scope.name, "<")) {
            if (!retval.empty())
                retval += "::";
            retval += scope.name;
        }
    }
    return retval;
}

// Emit something like "namespace A { namespace B { ..." and also update the namespaceLattice and namespaceLatticeRoots so they
// have records of the namespaces.
static size_t
generateNamespaces(const std::string initial, const Scopes &scopes, size_t nOmit) {
    size_t nns = 0;
    NamespaceLattice::VertexIterator parent = namespaceLattice.vertices().end();
    
    if (!initial.empty()) {
        std::cout <<"namespace " <<initial <<" {";
        if (!namespaceLatticeRoots.getOptional(initial).assignTo(parent)) {
            // Create a new namespace root in the lattice
            parent = namespaceLattice.insertVertex(initial);
            namespaceLatticeRoots.insert(initial, parent);
        }
        ++nns;
    }

    for (size_t i = 0; i+nOmit < scopes.size(); ++i) {
        if (!scopes[i].name.empty() && !boost::starts_with(scopes[i].name, "<")) {
            if (parent == namespaceLattice.vertices().end()) {
                if (!namespaceLatticeRoots.getOptional(scopes[i].name).assignTo(parent)) {
                    // Create a new namespace root in the lattice
                    parent = namespaceLattice.insertVertex(scopes[i].name);
                    namespaceLatticeRoots.insert(scopes[i].name, parent);
                }
            } else {
                // Does this namespace exist in the lattice?
                NamespaceLattice::VertexIterator me = namespaceLattice.vertices().end();
                BOOST_FOREACH (NamespaceLattice::Edge &edge, parent->outEdges()) {
                    if (edge.target()->value() == scopes[i].name) {
                        me = edge.target();
                        break;
                    }
                }
                if (me == namespaceLattice.vertices().end()) {
                    // Doesn't exist yet, so create it
                    me = namespaceLattice.insertVertex(scopes[i].name);
                    namespaceLattice.insertEdge(parent, me);
                }
                parent = me;
            }

            std::cout <<(nns>0?" ":"") <<"namespace " <<scopes[i].name <<" {";
            ++nns;
        }
    }
    return nns;
}

static void
generateDeclaration(const std::string &fileName, const Location &loc, const Scopes &scopes) {
    if (isAnonymous(scopes))
        return;
    std::string enumName = fullyQualifiedEnumName(scopes);
    std::cout <<"// DO NOT EDIT -- This implementation was automatically generated for the enum defined at\n"
              <<"// ";
    if (!relativeBase.empty() && boost::starts_with(fileName, relativeBase)) {
        std::cout <<fileName.substr(relativeBase.size());
    } else {
        std::cout <<fileName;
    }
    std::cout <<" line " <<(loc.first+1) <<"\n";
    size_t nns = generateNamespaces(outerNamespace, scopes, 1);
    std::cout <<"\n"
              <<"    /** Convert " <<enumName <<" enum constant to a string. */\n"
              <<"    const char* " <<scopes.back().name <<"(int64_t);\n"
              <<"\n"
              <<"    /** Convert " <<enumName <<" enum constant to a string. */\n"
              <<"    std::string " <<scopes.back().name <<"(int64_t, const std::string &strip);\n"
              <<"\n"
              <<"    /** Return all " <<enumName <<" member values as a vector. */\n"
              <<"    const std::vector<int64_t>& " <<scopes.back().name <<"();\n";
    std::cout <<std::string(nns, '}') <<"\n\n";
}

static void
generateImplementation(const std::string &fileName, const Location &loc,
                       const Scopes &scopes, const EnumMembers &members) {
    if (isAnonymous(scopes))
        return;
    std::string enumName = fullyQualifiedEnumName(scopes);

    typedef Sawyer::Container::Map<int64_t, std::string> ReverseMembers;
    ReverseMembers reverseMembers;
    BOOST_FOREACH (const EnumMembers::Node &node, members.nodes())
        reverseMembers.insert(node.value(), node.key());

    std::cout <<"// DO NOT EDIT -- This implementation was automatically generated for the enum defined at\n"
              <<"// ";
    if (!relativeBase.empty() && boost::starts_with(fileName, relativeBase)) {
        std::cout <<fileName.substr(relativeBase.size());
    } else {
        std::cout <<fileName;
    }
    std::cout <<" line " <<(loc.first+1) <<"\n";
    size_t nns = generateNamespaces(outerNamespace, scopes, 1);
    std::cout <<"\n"
              <<"    const char* " <<scopes.back().name <<"(int64_t i) {\n"
              <<"        switch (i) {\n";
    BOOST_FOREACH (const ReverseMembers::Node &node, reverseMembers.nodes())
        std::cout <<"            case " <<node.key() <<"L: return \"" <<node.value() <<"\";\n";
    std::cout <<"            default: return \"\";\n"
              <<"        }\n"
              <<"    }\n"
              <<"\n"
              <<"    std::string " <<scopes.back().name <<"(int64_t i, const std::string &strip) {\n"
              <<"        std::string s = " <<scopes.back().name <<"(i);\n"
              <<"        if (s.empty())\n"
              <<"            s = \"(" <<enumName <<")\" + boost::lexical_cast<std::string>(i);\n"
              <<"        if (boost::starts_with(s, strip))\n"
              <<"            s = s.substr(strip.size());\n"
              <<"        return s;\n"
              <<"    }\n"
              <<"\n";

    std::cout <<"    const std::vector<int64_t>& " <<scopes.back().name <<"() {\n";
    if (reverseMembers.isEmpty()) {
        std::cout <<"        static const std::vector<int64_t> retval;\n"
                  <<"        return retval;\n";
    } else {
        std::cout <<"        static const int64_t values[] = {";
        BOOST_FOREACH (const ReverseMembers::Node &node, reverseMembers.nodes()) {
            std::cout <<(node.key() == reverseMembers.nodes().begin()->key() ? "\n" : ",\n")
                      <<"            " <<node.key() <<"L";
        }
        std::cout <<"\n"
                  <<"        };\n"
                  <<"        static const std::vector<int64_t> retval(values, values + " <<reverseMembers.size() <<");\n"
                  <<"        return retval;\n";
    }
    std::cout <<"    }\n"
              <<"\n";

    std::cout <<std::string(nns, '}') <<"\n\n";
}

static std::string
compatibleEnumName(const Scopes &scopes) {
    std::string retval = "stringify";
    BOOST_FOREACH (const Scope &scope, scopes) {
        if (!boost::starts_with(scope.name, "<") && scope.name != "Rose") {
            if ((retval.size() > 0 && scope.name.size() > 0) &&
                ((isupper(retval[retval.size()-1]) && isupper(scope.name[0])) ||
                 ((islower(retval[retval.size()-1]) || isdigit(retval[retval.size()-1])) && islower(scope.name[0]))))
                retval += "_";
            retval += scope.name;
        }
    }
    return retval;
}

static void
generateCompatibleDeclaration(const Scopes &scopes) {
    if (isAnonymous(scopes))
        return;
    std::string enumName = (outerNamespace.empty()?"":outerNamespace+"::") + fullyQualifiedEnumName(scopes);
    std::cout <<"namespace Rose {\n"
              <<"    std::string " <<compatibleEnumName(scopes) <<"(int64_t n, const char *strip=NULL, bool canonic=false)";
    if (deprecateCompatible)
        std::cout <<"\n        SAWYER_DEPRECATED(\"use " <<enumName <<"\")";
    std::cout <<";\n"
              <<"    const std::vector<int64_t>& " <<compatibleEnumName(scopes) <<"()";
    if (deprecateCompatible)
        std::cout <<"\n        SAWYER_DEPRECATED(\"use " <<enumName <<"\")";
    std::cout <<";\n"
              <<"}\n\n";
}

static void
generateCompatibleImplementation(const Scopes &scopes) {
    if (isAnonymous(scopes))
        return;
    std::string enumName = fullyQualifiedEnumName(scopes);
    std::cout <<"namespace Rose {\n"
              <<"    std::string " <<compatibleEnumName(scopes) <<"(int64_t i, const char *strip, bool canonic) {\n"
              <<"        std::string retval = stringify::" <<enumName <<"(i);\n"
              <<"        if (retval.empty()) {\n"
              <<"            retval = \"(" <<enumName <<")\" + boost::lexical_cast<std::string>(i);\n"
              <<"        } else {\n"
              <<"            if (strip && !strncmp(strip, retval.c_str(), strlen(strip)))\n"
              <<"                retval = retval.substr(strlen(strip));\n"
              <<"            if (canonic)\n"
              <<"                retval = \"" <<enumName <<"::\" + retval;\n"
              <<"        }\n"
              <<"        return retval;\n"
              <<"    }\n"
              <<"\n"
              <<"    const std::vector<int64_t>& " <<compatibleEnumName(scopes) <<"() {\n"
              <<"        return stringify::" <<enumName <<"();\n"
              <<"    }\n"
              <<"}\n\n";
}

static void
parseEnum(TokenStream &tokens, Scopes &scopes) {
    ASSERT_require(tokens.matches(tokens[0], "enum"));
    Location enumLocation = tokens.location(tokens[0]);
    bool skipEnum = boost::contains(tokens.line(tokens[0]), "NO_STRINGIFY");
    tokens.consume();
    if (skipEnum)
        return;

    // Advance past the opening brace while picking up the enum name. If there is no enum
    // name then skip it. We don't support "typedef enum {...} name".
    std::string name;
    if (tokens.matches(tokens[0], "{")) {
        return;
    } else if (tokens[0].type() == TOK_WORD && tokens.matches(tokens[1], "{")) {
        name = tokens.lexeme(tokens[0]);
        scopes.push_back(Scope(name, tokens.location(tokens[0])));
        mlog[WHERE] <<where(tokens.fileName(), tokens.location(tokens[0])) <<": enum " <<scopes <<"\n";
        tokens.consume(2);
    } else {
        return;
    }

    // Did we already emit this enum?
    const std::string enumName = fullyQualifiedEnumName(scopes);
    NameAndLocation firstLocation;
    if (emittedEnums.getOptional(enumName).assignTo(firstLocation)) {
        std::cerr <<where(tokens.fileName(), enumLocation) <<": error: enum '" <<enumName <<"' is already defined\n";
        std::cerr <<where(firstLocation.fileName, firstLocation.location) <<": info: previous definition\n";
        return;
    }
    emittedEnums.insert(enumName, NameAndLocation(tokens.fileName(), enumLocation));
    
    // Parse the enum members
    currentEnumMembers.clear();
    EnumMembers members;
    int64_t memberValue = 0;
    while (tokens[0].type() != TOK_EOF && !tokens.matches(tokens[0], "}")) {
        // The enum member name
        if (tokens[0].type() != TOK_WORD) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: expected an enum constant\n";
            break;
        }
        std::string memberName = tokens.lexeme(tokens[0]);
        bool skipEnumMember = boost::contains(tokens.line(tokens[0]), "NO_STRINGIFY");
        tokens.consume();

        // The optional member value
        if (tokens.matches(tokens[0], "=")) {
            tokens.consume();
            memberValue = parseConstExpr(tokens);
        }

        // Comma separator
        if (tokens.matches(tokens[0], ",")) {
            tokens.consume();
        } else if (!tokens.matches(tokens[0], "}")) {
            std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: expected comma enum separator\n";
            break;
        }

        if (!skipEnumMember)
            members.insert(memberName, memberValue);
        currentEnumMembers.insert(memberName, memberValue);

        ++memberValue;
    }

    // Closing brace
    if (!tokens.matches(tokens[0], "}"))
        std::cerr <<where(tokens.fileName(), tokens.location(tokens[0])) <<": error: expected enum closing brace\n";

    if (emitImplementations) {
        generateImplementation(tokens.fileName(), enumLocation, scopes, members);
        if (emitCompatible)
            generateCompatibleImplementation(scopes);
    } else {
        generateDeclaration(tokens.fileName(), enumLocation, scopes);
        if (emitCompatible)
            generateCompatibleDeclaration(scopes);
    }
}

static void
generateOnce() {
    std::cout <<"// DO NOT EDIT -- This file is automatically generated.\n\n";
    if (emitCompatible) {
        std::cout <<"#ifndef ROSE_STRINGIFY_H\n"
                  <<"#define ROSE_STRINGIFY_H\n";
    } else if (outerNamespace.empty()) {
        std::cout <<"#ifndef stringify_H\n"
                  <<"#define stringify_H\n";
    } else {
        std::cout <<"#ifndef " <<outerNamespace <<"_stringify_H\n"
                  <<"#define " <<outerNamespace <<"_stringify_H\n";
    }
}

static void
generateStringifierClass() {
    std::cout <<"namespace Rose {\n"
              <<"/** Shortens names of int64_t stringifiers.\n"
              <<" *\n"
              <<" *  Enum stringifier functions are automatically and sometimes have very long names.  This class is intended to be a convenient\n"
              <<" *  way to make a locally short-named object that can be used in place of the long stringifier.\n"
              <<" *\n"
              <<" *  @code\n"
              <<" *   Stringifier stringifyType(stringifyBinaryAnalysisInstructionSemantics2DataFlowSemanticsDataFlowEdgeEdgeType);\n"
              <<" *   std::cout <<stringifyType(CLOBBER) <<\"\\n\";\n"
              <<" *  @endcode */\n"
              <<"class Stringifier {\n"
              <<"    std::string(*stringifier_)(int64_t, const char*, bool);\n"
              <<"public:\n"
              <<"    Stringifier(std::string(*stringifier)(int64_t, const char*, bool)): stringifier_(stringifier) {}\n"
              <<"    std::string operator()(int64_t n, const char *strip=NULL, bool canonic=false) {\n"
              <<"        return stringifier_(n, strip, canonic);\n"
              <<"    }\n"
              <<"};\n"
              <<"}\n\n";
}

static void
generateDocumentation() {
    BOOST_FOREACH (NamespaceLattice::VertexIterator root, namespaceLatticeRoots.values()) {
        size_t depth = 0;
        typedef Sawyer::Container::Algorithm::DepthFirstForwardGraphTraversal<NamespaceLattice> Traversal;
        for (Traversal t(namespaceLattice, root); t; ++t) {
            if (t.event() == Sawyer::Container::Algorithm::ENTER_VERTEX) {
                std::cout <<std::string(4*depth, ' ') <<"/** Stringification. */\n"
                          <<std::string(4*depth, ' ') <<"namespace " <<t.vertex()->value() <<" {\n";
                ++depth;
            } else if (t.event() == Sawyer::Container::Algorithm::LEAVE_VERTEX) {
                ASSERT_require(depth > 0);
                --depth;
                std::cout <<std::string(4*depth, ' ') <<"}\n";
            }
        }
    }
}

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    mlog.initialize("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);
    mlog[INFO].enable();
    globalSymbols.insert("false", 0);
    globalSymbols.insert("true", 1);

    std::vector<boost::filesystem::path> inputNames = parseCommandLine(argc, argv);

    // File prologue
    if (!emitImplementations) {
        generateOnce();
        if (emitCompatible && deprecateCompatible)
            std::cout <<"#include <Sawyer/Sawyer.h> // for SAWYER_DEPRECATED\n";
    }
    std::cout <<"#include <boost/algorithm/string/predicate.hpp>\n"
              <<"#include <boost/lexical_cast.hpp>\n"
              <<"#include <string>\n"
              <<"#include <vector>\n\n";
    if (!emitImplementations && emitCompatible)
        generateStringifierClass();

    // Declarations or implementations
    BOOST_FOREACH (const boost::filesystem::path &inputName, inputNames) {
        if (!boost::filesystem::exists(inputName)) {
            std::cerr <<inputName.string() <<":0.0: error: no such file\n";
            continue;
        }
        if (!boost::filesystem::is_regular_file(inputName)) {
            std::cerr <<inputName.string() <<":0.0: error: not a regular file\n";
            continue;
        }
        if (boost::filesystem::file_size(inputName) == 0)
            continue;

        Scopes scopes;
        TokenStream tokens(inputName.string());
        while (tokens[0].type() != TOK_EOF) {
#if 0 // DEBUGGING [Robb Matzke 2018-09-13]
            std::cerr <<where(inputName, tokens.location(tokens[0])) <<": here\n";
#endif
            switch (tokens[0].type()) {
                case TOK_LEFT:
                    if (tokens.matches(tokens[0], "{")) {
                        scopes.push_back(Scope(tokens.location(tokens[0])));
                        SAWYER_MESG(mlog[DEBUG]) <<where(inputName, tokens.location(tokens[0])) <<": enter " <<scopes <<"\n";
                    }
                    tokens.consume();
                    break;

                case TOK_RIGHT:
                    if (tokens.matches(tokens[0], "}")) {
                        SAWYER_MESG(mlog[DEBUG]) <<where(inputName, tokens.location(tokens[0])) <<": leave " <<scopes <<"\n";
                        if (scopes.empty()) {
                            std::cerr <<where(inputName, tokens.location(tokens[0])) <<": error: unbalanced closing brace\n";
                        } else {
                            scopes.pop_back();
                        }
                    }
                    tokens.consume();
                    break;

                case TOK_WORD:
                    if (tokens.matches(tokens[0], "namespace")) {
                        parseNamespace(tokens, scopes);
                    } else if (tokens.matches(tokens[0], "class") || tokens.matches(tokens[0], "struct")) {
                        parseClass(tokens, scopes);
                    } else if (tokens.matches(tokens[0], "extern")) {
                        parseExtern(tokens, scopes);
                    } else if (tokens.matches(tokens[0], "enum")) {
                        parseEnum(tokens, scopes);
                    } else {
                        tokens.consume();
                    }
                    break;

                default:
                    tokens.consume();
                    break;
            }
        }

        if (!scopes.empty()) {
            std::cerr <<where(inputName, tokens.location(tokens[0])) <<": error: unbalanced braces encountered\n";
            while (!scopes.empty()) {
                std::cerr <<where(inputName, scopes.back().location)
                          <<": error: no close brace for this open brace\n";
                scopes.pop_back();
            }
        }
    }

    // File epilogue
    if (!emitImplementations) {
        generateDocumentation();
        std::cout <<"\n#endif\n";
    }
}
