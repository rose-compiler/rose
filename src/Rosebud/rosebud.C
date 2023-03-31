static const char* gPurpose = "generate ROSE AST node types";
static const char* gDescription =
    "Not written yet.";

#include <Rosebud/Ast.h>
#include <Rosebud/Utility.h>
#include <Rosebud/RosettaGenerator.h>
#include <Rosebud/YamlGenerator.h>

#include <Sawyer/CommandLine.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptors.hpp>

#include <cctype>
#include <iostream>

using namespace Rosebud;
using namespace Sawyer::Message::Common;
using namespace Sawyer::Language::Clexer;

static Sawyer::Message::Facility mlog;

static const std::vector<std::string> validAttrNames {
    "Rosebud::accessors",                               // accessor name(s) or empty
    "Rosebud::ctor_arg",                                // attribute is constructor argument
    "Rosebud::data",                                    // data member name
    "Rosebud::large",                                   // property type is large
    "Rosebud::no_serialize",                            // don't serialize the attribute
    "Rosebud::property",                                // data member is a property even if no other attributes are specified
    "Rosebud::mutators",                                // mutator names or empty
    "Rosebud::rosetta",                                 // make attribute known to ROSETTA
    "Rosebud::traverse"                                 // make attribute part of the traversed AST
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Sawyer::CommandLine::Parser
makeCommandLineParser() {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser.errorStream(mlog[FATAL]);
    parser.purpose(gPurpose);
    parser.doc("Description", gDescription);

    parser.with(Switch("help", 'h')
                .action(showHelpAndExit(0))
                .doc("Show this documentation."));

    parser.with(Switch("log")
                .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                .argument("config")
                .whichValue(SAVE_ALL)
                .doc("Configures diagnostics. See @s{log}=help."));

    parser.with(Switch("color")
                .argument("when", enumParser<When>(settings.usingColor)
                          ->with("always", When::ALWAYS)
                          ->with("never", When::NEVER)
                          ->with("auto", When::AUTO))
                .doc("When to use color in the diagnostic output."));

    parser.with(Switch("verbose", 'v')
                .intrinsicValue(false, settings.debugging)
                .doc("Show debugging output."));

    parser.with(Switch("backend")
                .argument("generator", enumParser<Backend>(settings.backend)
                          ->with("rosetta", Backend::ROSETTA_BINARY)
                          ->with("yaml", Backend::YAML)
                          ->with("none", Backend::NONE))
                .doc("How to generate code. The choices are:"
                     "@named{rosetta}{Generate code according to Robb's single-file ROSETTA kludge.}"
                     "@named{yaml}{Generate YAML output that can be parsed by standalone backends.}"
                     "@named{none}{Do not generate code, but only check the input.}"));

    parser.with(Switch("location")
                .intrinsicValue(true, settings.showingCppLineDirectives)
                .doc("Emit CPP #line directives refering to the Rosebud input files."));

    parser.with(Switch("warnings")
                .intrinsicValue(true, settings.showingWarnings)
                .hidden(true));
    parser.with(Switch("no-warnings")
                .intrinsicValue(false, settings.showingWarnings)
                .key("warnings")
                .doc("Do not show warnings about questionable things in the input."));

    return parser;
}

static std::vector<std::string>
parseCommandLine(Sawyer::CommandLine::Parser &parser, int argc, char *argv[]) {
    std::vector<std::string> positionalArgs = parser.parse(argc, argv).apply().unreachedArgs();
    if (positionalArgs.empty())
        message(ERROR, "no input files specified; see --help");
    return positionalArgs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DSL parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Process (but do not consume) left and right parens, braces, and brackets.
//
// Left tokens push a token onto the stack and return true.
//
// Right tokens that match with the top of the stack pop a token from the stack and return true.
//
// All other situations return false.
static bool
adjustParens(const Ast::File::Ptr &file, size_t at, std::vector<Token> &nestingStack) {
    if (file->token(at).type() == TOK_LEFT) {
        nestingStack.push_back(file->token(at));
        return true;
    } else if (file->token(at).type() == TOK_RIGHT && !nestingStack.empty()) {
        const std::string right = file->lexeme(at);
        const std::string expectedLeft = matching(right);
        if (file->matches(nestingStack.back(), expectedLeft)) {
            nestingStack.pop_back();
            return true;
        }
    }
    return false;
}

// Is this token a doxygen comment?
static bool
isDoxygenComment(const Ast::File::Ptr &file, const Token &token) {
    ASSERT_not_null(file);
    return (token.type() == TOK_COMMENT &&
            file->startsWith(token, "/**") &&
            std::isspace(file->tokenStream().getChar(token.begin() + 3)));
}

// Reparse part of a file and return just the CPP directives and comments
static std::vector<Token>
parseCppAndComments(const Ast::File::Ptr &file, size_t begin, size_t end) {
    ASSERT_not_null(file);
    if (begin >= end)
        return {};

    std::vector<Token> retval;
    TokenStream tokens(file->tokenStream(), Sawyer::Language::Clexer::Indices::hull(begin, end - 1));
    tokens.skipPreprocessorTokens(false);
    tokens.skipCommentTokens(false);
    while (tokens[0]) {
        if (tokens[0].type() == TOK_CPP || tokens[0].type() == TOK_COMMENT)
            retval.push_back(tokens[0]);
        tokens.consume();
    }
    return retval;
}

// Look at the C preprocessor and comment tokens in the specified input area and add relevant things to the AST.
static void
parsePriorRegion(const Ast::File::Ptr &file, const Ast::Definition::Ptr &defn, Ast::CppStack::Stack &runningCppStack,
                 size_t begin, size_t end) {
    ASSERT_not_null(file);
    ASSERT_not_null(defn);
    if (begin >= end)
        return;

    // Open a token stream to (re)parse part of the input
    TokenStream tokens(file->tokenStream(), Sawyer::Language::Clexer::Indices::hull(begin, end - 1));
    tokens.skipPreprocessorTokens(false);
    tokens.skipCommentTokens(false);

    // Process all tokens
    Token doxyToken;
    while (tokens[0]) {
        switch (tokens[0].type()) {
            case TOK_CPP:
                Ast::CppStack::process(file, tokens[0], runningCppStack);
                tokens.consume();
                break;

            case TOK_COMMENT:
                if (isDoxygenComment(file, tokens[0]))
                    doxyToken = tokens[0];
                tokens.consume();
                break;

            case TOK_CHAR:
            case TOK_STRING:
            case TOK_LEFT:
            case TOK_RIGHT:
                doxyToken = Token();                    // the Doxygen comment was for something other than `defn`
                tokens.consume();
                break;

            case TOK_WORD: {
                const std::string lexeme = tokens.lexeme(tokens[0]);
                if ("private" == lexeme || "protected" == lexeme || "public" == lexeme) {
                    // These do not cancel the Doxygen comment
                } else {
                    doxyToken = Token();                // the Doxygen comment was for something other than `defn`
                }
                tokens.consume();
                break;
            }

            default:
                tokens.consume();
                break;
        }
    }

    // Save information such as the CPP conditional compilation stack and documentation
    defn->cppStack->stack = runningCppStack;
    if (doxyToken) {
        defn->docToken = doxyToken;
        const size_t n = tokens.location(doxyToken).second;
        defn->doc = std::string(n, ' ') + tokens.lexeme(doxyToken);
    }
}

static bool
isAtClassDefinition(const Ast::File::Ptr &file) {
    ASSERT_not_null(file);
    return file->matches(0, "class") &&
        file->token(1).type() == TOK_WORD &&
        (file->matches(2, ":") || file->matches(2, "{"));
}

// Parse the file to the beginning of the next class definition or unmatched right token.  Does not consume an unmatched right or
// the class definition.
static void
parseToClassDefinition(const Ast::File::Ptr &file) {
    std::vector<Token> parenStack;
    while (file->token()) {
        if (adjustParens(file, 0, parenStack)) {
            file->consume();
        } else if (file->token().type() == TOK_RIGHT) {
            break;
        } else if (isAtClassDefinition(file)) {
            break;
        } else {
            file->consume();
        }
    }
}

static std::string
parseVisibility(const Ast::File::Ptr &file, const std::string &dflt) {
    ASSERT_not_null(file);
    if (file->matches(0, "private") ||
        file->matches(0, "protected") ||
        file->matches(0, "public")) {
        const std::string retval = file->lexeme();
        file->consume();
        return retval;
    } else {
        return dflt;
    }
}

// Parse a qualified name but do not consume tokens. A qualified name is NAME ('::' NAME)*
static std::vector<Token>
parseQualifiedName(const Ast::File::Ptr &file, size_t begin) {
    ASSERT_not_null(file);
    size_t at = begin;
    std::vector<Token> retval;

    if (file->token(at).type() == TOK_WORD)
        retval.push_back(file->token(at++));

    while (file->matches(at, "::") && file->token(at+1).type() == TOK_WORD) {
        retval.push_back(file->token(at++));
        retval.push_back(file->token(at++));
    }

    return retval;
}

// Parse a qualified name and consume tokens. Return an empty string if the name does not exist. A qualified name is NAME ('::'
// NAME)*
static std::string
parseQualifiedName(const Ast::File::Ptr &file) {
    ASSERT_not_null(file);
    const std::vector<Token> tokens = parseQualifiedName(file, 0);
    file->consume(tokens.size());
    return file->content(tokens, Expand::NONE);
}

static Ast::Class::Inheritance
parseClassInheritance(const Ast::File::Ptr &file) {
    ASSERT_not_null(file);
    Ast::Class::Inheritance retval;
    while (true) {
        const std::string visibility = parseVisibility(file, "private");
        const std::string name = parseQualifiedName(file);
        if (name.empty()) {
            message(ERROR, file, file->token(), "class name expected for inheritance");
        } else {
            retval.push_back(std::make_pair(visibility, name));
        }

        if (file->matches(0, ",")) {
            file->consume();
        } else {
            break;
        }
    }

    return retval;
}

// Parse a list of zero or more tokens up to EOF or a token matching stopAt. Parens, curly braces, and square braces are
// honored, so the stopAt must appear outside those grouping tokens.
static Ast::TokenList::Ptr
parseBalancedTokens(const Ast::File::Ptr &file, size_t at, const std::vector<std::string> &stopAt) {
    ASSERT_not_null(file);
    std::vector<Token> nestingStack;
    auto retval = Ast::TokenList::instance();

    while (Token token = file->token(at)) {
        if (nestingStack.empty() && file->matchesAny(at, stopAt)) {
            return retval;
        } else if (adjustParens(file, at, nestingStack)) {
            retval->tokens.push_back(token);
            ++at;
        } else if (token.type() == TOK_RIGHT) {
            return retval;
        } else {
            retval->tokens.push_back(token);
            ++at;
        }
    }
    return retval;
}

static Ast::TokenList::Ptr
parseBalancedTokens(const Ast::File::Ptr &file, size_t at, const std::string &stopAt) {
    return parseBalancedTokens(file, at, std::vector<std::string>{stopAt});
}

// Parse an attribute, which is a name followed by an optional argument list. Do not consume tokens.
static std::pair<size_t, Ast::Attribute::Ptr>
parseAttribute(const Ast::File::Ptr &file, size_t at, const std::string &nameSpace) {
    ASSERT_not_null(file);
    const size_t begin = at;
    const auto attrNameTokens = parseQualifiedName(file, at);
    if (attrNameTokens.empty()) {
        message(ERROR, file, file->token(at), "expected attribute name");
        return {0, {}};
    }
    const std::string baseName = file->content(attrNameTokens, Expand::NONE);
    const std::string fqName = nameSpace.empty() ? baseName : nameSpace + "::" + baseName;
    at += attrNameTokens.size();
    auto attribute = Ast::Attribute::instance(fqName, attrNameTokens);

    // Attribute argument list
    if (file->matches(at, "(")) {
        ++at;
        attribute->arguments = Ast::ArgumentList::instance();
        while (file->token(at) && !file->matches(at, ")")) {
            if (const auto argument = parseBalancedTokens(file, at, ",")) {
                attribute->arguments->push_back(argument);
                at += argument->size();
            }

            if (file->matches(at, ",")) {
                ++at;
            } else {
                break;
            }
        }
        if (file->matches(at, ")")) {
            ++at;
        } else {
            message(ERROR, file, file->token(at), "expected \")\" after attribute argument list");
        }
    }

    ASSERT_require(at > begin);
    ASSERT_not_null(attribute);
    return std::make_pair(at - begin, attribute);
}

// Parse attributes. If we find an attribute list that contains at least one attribute in the "Rosebud" namespace, then add
// all the attributes to the `attributes` list argument and return the number of tokens parsed. Otherwise only return zero.
// Do not consume any tokens.
static size_t
parseOptionalAttributes(const Ast::File::Ptr &file, const Ast::AttributeList::Ptr &attributes) {
    ASSERT_not_null(file);
    ASSERT_not_null(attributes);
    size_t at = 0;

    while (file->matches(at, "[") && file->matches(at+1, "[")) {
        at += 2;
        std::string nameSpace;

        while (file->token(at)) {
            // parse "using NAMESPACE:"
            if (file->matches(at, "using") && file->token(at+1).type() == TOK_WORD && file->matches(at+2, ":")) {
                if (!nameSpace.empty())
                    message(ERROR, file, file->token(at), "multiple \"using\" in attribute list");
                if (!attributes->empty())
                    message(ERROR, file, file->token(at), "\"using\" must appear before attributes");
                nameSpace = file->lexeme(at+1);
                at += 3;
            }

            // Parse an attribute
            const std::pair<size_t, Ast::Attribute::Ptr> attrPair = parseAttribute(file, at, nameSpace);
            if (attrPair.second) {
                attributes->push_back(attrPair.second);
                at += attrPair.first;
            } else {
                message(ERROR, file, file->token(at), "attribute expected");
                break;
            }

            if (file->matches(at, ",")) {
                ++at;
            } else {
                break;
            }
        }

        // Closing "]]"
        if (file->matches(at, "]") && file->matches(at+1, "]")) {
            at += 2;
        } else {
            message(ERROR, file, file->token(at), "expected \"]]\" at end of attribute list");
        }
    }

    // Parsing was a success only if we found at least one Rosebud attribute
    for (const auto &attribute: *attributes) {
        if (boost::starts_with(attribute->fqName, "Rosebud::"))
            return at;
    }

    return 0;
}

// False if name starts with "Rosebud::" but is not one of the valid Rosebud attribute names.
static bool
checkRecognizedAttribute(const std::string &name) {
    if (!boost::starts_with(name, "Rosebud::")) {
        return true;
    } else {
        for (const std::string &validName: validAttrNames) {
            if (name == validName)
                return true;
        }
        return false;
    }
}

// Complain and return false if the attribute's number of arguments is incorrect.
static bool
checkNumberOfArguments(const Ast::File::Ptr &file, const Ast::Attribute::Ptr &attr, size_t minArgs, size_t maxArgs = 0) {
    ASSERT_not_null(file);
    ASSERT_not_null(attr);
    if (0 == maxArgs)
        maxArgs = minArgs;
    ASSERT_require(minArgs <= maxArgs);

    const size_t nArgs = attr->arguments ? attr->arguments->size() : 0;
    if (minArgs == maxArgs && nArgs != minArgs) {
        message(ERROR, file, attr->nameTokens, "attribute \"" + attr->fqName + "\" has " +
                boost::lexical_cast<std::string>(nArgs) + (1 == nArgs ? " argument" : " arguments") + " but needs " +
                boost::lexical_cast<std::string>(minArgs) + (1 == minArgs ? " argument" : " arguments"));
        return false;
    } else if (minArgs < maxArgs && (nArgs < minArgs || nArgs > maxArgs)) {
        message(ERROR, file, attr->nameTokens, "attribute \"" + attr->fqName + "\" has " +
                boost::lexical_cast<std::string>(nArgs) + (1 == nArgs ? " argument" : " arguments") + " but needs between " +
                boost::lexical_cast<std::string>(minArgs) + " and " + boost::lexical_cast<std::string>(maxArgs) +
                (1 == maxArgs ? " argument" : " arguments"));
        return false;
    }
    return true;
}

// Apply certain attributes to the property definition. Some of this checking could be delayed until the backend.
static void
checkAndApplyAttributes(const Ast::File::Ptr &file, const Ast::Property::Ptr &property) {
    ASSERT_not_null(file);
    ASSERT_not_null(property);
    std::map<std::string, Ast::Attribute::Ptr> seen;

    for (const auto &attr: *property->attributes()) {
        if (!checkRecognizedAttribute(attr->fqName)) {
            // Unknown name
            message(ERROR, file, attr->nameTokens,
                    "\"" + attr->fqName + "\" is not a recognized Rosebud attribute; did you mean \"" +
                    bestMatch(validAttrNames, attr->fqName) +"\"?");

        } else if (!seen.insert(std::make_pair(attr->fqName, attr())).second) {
            // Used multiple times
            message(ERROR, file, attr->nameTokens, "attribute \"" + attr->fqName + "\" is specified multiple times");
            message(INFO, file, seen[attr->fqName]->nameTokens, "previously specified here");

        } else if ("Rosebud::ctor_arg" == attr->fqName ||
                   "Rosebud::large" == attr->fqName ||
                   "Rosebud::no_serialize" == attr->fqName ||
                   "Rosebud::property" == attr->fqName ||
                   "Rosebud::traverse" == attr->fqName) {
            // Wrong number of arguments
            checkNumberOfArguments(file, attr(), 0);

        } else if ("Rosebud::rosetta" == attr->fqName) {
            checkNumberOfArguments(file, attr(), 0);
            if (auto other = property->findAttribute("Rosebud::data")) {
                message(ERROR, file, attr->nameTokens,
                        "attributes \"" + attr->fqName + "\" and \"Rosebud::data\" are mutually exclusive");
            }

        } else if ("Rosebud::data" == attr->fqName) {
            // One argument, which must be the symbol to use as the data member name for the property.
            if (!checkNumberOfArguments(file, attr(), 1)) {
                // error already printed
            } else {
                for (const auto &arg: *attr->arguments()) { // there's just one
                    ASSERT_forbid(arg->empty());
                    if (arg->size() != 1) {
                        message(ERROR, file, arg->tokens,
                                "attribute \"" + attr->fqName + "\" argument must be the symbol to use as the data member name "
                                "for the property");
                    } else {
                        property->dataMemberName = arg->string(file);
                    }
                }
            }

        } else if ("Rosebud::accessors" == attr->fqName) {
            if (attr->arguments) {
                std::vector<std::string> names;
                for (const auto &arg: *attr->arguments()) {
                    ASSERT_forbid(arg->empty());
                    if (arg->size() != 1) {
                        message(ERROR, file, arg->tokens, "attribute \"" + attr->fqName + "\" argument must be the symbol to use "
                                "as the accessor member function name for the property");
                    } else {
                        names.push_back(arg->string(file));
                    }
                }
                property->accessorNames = names;
            } else {
                // Zero or more arguments which must the be symbols to use as data members for this property.
                message(ERROR, file, attr->nameTokens,
                        "explicit argument list required for attribute \"" + attr->fqName + "\", even if no arguments present");
            }

        } else if ("Rosebud::mutators" == attr->fqName) {
            // Zero or more arguments which must the be symbols to use as data members for this property.
            if (attr->arguments) {
                std::vector<std::string> names;
                for (const auto &arg: *attr->arguments()) {
                    ASSERT_forbid(arg->empty());
                    if (arg->size() != 1) {
                        message(ERROR, file, arg->tokens, "attribute \"" + attr->fqName + "\" argument must be the symbol to use "
                                "as the mutator member function name for the property");
                    } else {
                        names.push_back(arg->string(file));
                    }
                }
                property->mutatorNames = names;
            } else {
                message(ERROR, file, attr->nameTokens,
                        "explicit argument list required for attribute \"" + attr->fqName + "\", even if no arguments present");
            }

        } else {
            ASSERT_not_implemented("attribute = " + attr->fqName);
        }
    }
}

// Parse an optional property and return it if parsed and consumed from the input.
static Ast::Property::Ptr
parseOptionalProperty(const Ast::File::Ptr &file, Ast::CppStack::Stack &runningCppStack) {
    ASSERT_not_null(file);
    auto property = Ast::Property::instance();
    property->startToken = file->token();

    // Look for attributes. At least one of them must be in the 'Rosebud' namespace in order for this to be a property.
    size_t at = parseOptionalAttributes(file, property->attributes());
    if (0 == at)
        return {};

    //-----------------------------------------------------------------------------------------
    // Everything beyond here is an error since we know by now that we have Rosebud attributes.
    // However, don't consume any tokens if there's an error because they might be things we
    // need to emit as non-Rosebud C++.
    //-----------------------------------------------------------------------------------------

    // Parse the type and name of the data member, up to an "=" or ";". The type is the first N-1 tokens and the name is the last
    // token.
    property->cType = parseBalancedTokens(file, at, std::vector<std::string>{"=", ";"});
    if (property->cType->size() < 2) {
        message(ERROR, file, file->token(at), "property type and name expected");
        return {};
    }
    at += property->cType->size();
    property->nameToken = property->cType->tokens.back();
    property->name = file->lexeme(property->nameToken);
    property->cType->tokens.pop_back();

    // If the property name is followed by an "=" then parse the initialization expression up to the next ";" that occurs at this
    // same paren nesting level.
    if (file->matches(at, "=")) {
        ++at;
        property->cInit = parseBalancedTokens(file, at, ";");
        if (property->cInit->empty()) {
            message(ERROR, file, file->token(at), "property initialization expression expected");
            return {};
        }
        at += property->cInit->size();
    }

    // Semicolon at the end of the property definition
    if (file->matches(at, ";")) {
        ++at;
    } else {
        message(ERROR, file, file->token(at), "\";\" expected at end of property declaration");
        return {};
    }

    // Some of the Rosebud attributes cause changes to the AST
    checkAndApplyAttributes(file, property);

    file->consume(at);
    return property;
}

static void
checkClassCppDirectives(const Ast::File::Ptr &file,
                        const Ast::CppStack::Stack &cstack, const Ast::CppStack::Stack &pstack,
                        const Token &classDefnStart, const Token &endToken) {
    ASSERT_not_null(file);
    if (pstack.size() < cstack.size() ||
        (pstack.size() == cstack.size() && !pstack.empty() && pstack.back().size() > cstack.back().size())) {
        // FIXME[Robb Matzke 2023-03-30]: This error could have more specificity. Sometimes the closing directive occurs well before
        // the endToken. In the extreme case, the closing directive could be just after the start of the definition for a large
        // class definition with no properties, and it's not discovered until we get to the closing brace for the
        // definition. Unfortunately this would require some redesign because when we're parsing CPP directives the #endif causes
        // information about the nesting structure to be discarded. One way would be to move this function and its arguments inside
        // the loop that processes the CPP directives.
        message(ERROR, file, endToken,
                "C preprocessor conditional compilation directives that were opened before the start of the class "
                "definition cannot be closed until after the end of the class definition. Such an \"#endif\", \"#else\", "
                "or \"#elif\" appears somewhere before this line");
        message(INFO, file, classDefnStart, "start of class definition");
    }
}

static void
parseClassDefinitionBody(const Ast::File::Ptr &file, const Ast::Class::Ptr &c, Ast::CppStack::Stack &runningCppStack) {
    ASSERT_not_null(file);
    ASSERT_not_null(c);
    std::vector<Token> nestingStack;
    size_t filePos = file->token().prior();

    while (file->token() && (!nestingStack.empty() || !file->matches("}"))) {
        const size_t startOfProperty = file->token().begin();
        if (auto property = parseOptionalProperty(file, runningCppStack)) {
            // Parse part of class definition before the property
            parsePriorRegion(file, property, runningCppStack, filePos, startOfProperty);
            checkClassCppDirectives(file, c->cppStack->stack, property->cppStack->stack, c->startToken, property->startToken);
            property->priorText = file->trimmedContent(filePos, startOfProperty, property->docToken);
            c->properties->push_back(property);
            filePos = file->token().prior();          // end of property

            // Check for property problems
            if (settings.showingWarnings) {
                ASSERT_forbid(property->name.empty());
                if (property->name.find('_') != std::string::npos || std::isupper(property->name[0]))
                    message(WARN, file, property->nameToken, "property name should be camelCase");

                if (property->doc.empty() &&
                    ((property->accessorNames && !property->accessorNames->empty()) ||
                     (property->mutatorNames && !property->mutatorNames->empty()))) {
                    message(WARN, file, property->nameToken, "no Doxygen documentation for property \"" + property->name + "\"");
                }

                if (!property->cInit) {
                    std::regex integralTypeRe("bool|char|short|int|long|long long"
                                              "|unsigned"
                                              "|unsigned (char|short|int|long|long long)"
                                              "|signed"
                                              "|signed (char|short|int|long|long long)"
                                              "|u?int[0-9]+_t");
                    std::regex floatingTypeRe("float|double");
                    const std::string cType = property->cType->string(file);
                    if (boost::ends_with(cType, "*")) {
                        message(WARN, file, property->cType->tokens,
                                "no initializer for raw pointer (did you intend to initialize with \"= nullptr\"?)");
                    } else if (std::regex_match(cType, integralTypeRe)) {
                        message(WARN, file, property->cType->tokens,
                                "no initializer for integral type (did you intend to initialize with \"= 0\"?)");
                    } else if (std::regex_match(cType, floatingTypeRe)) {
                        message(WARN, file, property->cType->tokens,
                                "no initializer for floating-point type (did you intend to initialize with \"= NAN\"?)");
                    }
                }
            }

        } else {
            if (!adjustParens(file, 0, nestingStack) && file->token().type() == TOK_RIGHT)
                message(ERROR, file, file->token(), "unmatched \"" + file->lexeme() + "\"");
            file->consume();
        }
    }

    // Check that the code after the last property but before the end of the class definition follows the rules for C preprocessor
    // conditional compilation directives. I.e., conditional compilation directives that are opened before the beginning of the
    // class definition are not closed until after the end of the class definition.
    Ast::CppStack::Stack tempStack = runningCppStack;
    std::vector<Token> tokens = parseCppAndComments(file, filePos, file->token().begin());
    for (const Token &token: tokens)
        Ast::CppStack::process(file, token, tempStack);
    checkClassCppDirectives(file, c->cppStack->stack, tempStack, c->startToken, file->token());

    // Accumulate the input that appears inside the class definition after the last property but before the definition's final
    // closing brace.
    c->endText = file->trimmedContent(filePos, file->token().begin());
}

// Parse an entire class definition if the token stream for the file is positioned at the beginning of a class definition. If it
// isn't, then return a null pointer.
static Ast::Class::Ptr
parseClassDefinition(const Ast::File::Ptr &file, Ast::CppStack::Stack &runningCppStack) {
    ASSERT_not_null(file);
    auto c = Ast::Class::instance();

    // The "class" keyword.
    if (file->matches(0, "class")) {
        c->startToken = file->consume();
    } else {
        message(ERROR, file, file->token(), "\"class\" expected at start of class definition");
        return {};
    }

    // Class name
    if (file->token().type() == TOK_WORD) {
        message(DEBUG, file, file->token(), "defining class \"" + file->lexeme() + "\"");
        c->name = file->lexeme();
        c->nameToken = file->consume();
    } else {
        message(ERROR, file, file->token(), "class name expected at start of class definition");
        return {};
    }

    // Class inheritance
    if (file->matches(0, ":")) {
        file->consume();
        c->inheritance = parseClassInheritance(file);
    }

    // Class body from the "{" to the matching "}"
    if (file->matches(0, "{")) {
        file->consume();
    } else {
        message(ERROR, file, file->token(), "expected \"{\" at start of class definition");
        return {};
    }
    parseClassDefinitionBody(file, c, runningCppStack);
    if (file->matches(0, "}")) {
        file->consume();
    } else {
        message(ERROR, file, file->token(), "expected \"}\" at end of class definition");
        return {};
    }

    // Semicolon after the definition
    if (file->matches(0, ";")) {
        file->consume();
    } else {
        message(ERROR, file, file->token(), "expected semicolon after class definition");
        return {};
    }

    return c;
}

static void
parseFile(const Ast::File::Ptr &file) {
    ASSERT_not_null(file);
    size_t filePos = 0;
    Ast::CppStack::Stack runningCppStack;

    while (file->token()) {
        parseToClassDefinition(file);
        if (isAtClassDefinition(file)) {
            size_t startOfClass = file->token().begin();
            if (auto c = parseClassDefinition(file, runningCppStack)) {
                // Parse area before the class
                parsePriorRegion(file, c, runningCppStack, filePos, startOfClass);
                c->priorText = file->trimmedContent(filePos, startOfClass, c->docToken);
                file->classes->push_back(c);
                filePos = file->token().prior();

                // Show warnings for questionable things about a class
                if (settings.showingWarnings) {
                    if (c->doc.empty())
                        message(WARN, file, c->nameToken, "no Doxygen documentation for class \"" + c->name + "\"");
                    ASSERT_forbid(c->name.empty());
                    if (!std::isupper(c->name[0]) || c->name.find('_') != std::string::npos)
                        message(WARN, file, c->nameToken, "class name \"" + c->name + "\" should be PascalCase");
                }
            }
        } else {
            file->consume();
        }
    }

    // Accumulate the part of the file that appears after the last class definition
    file->endText = file->trimmedContent(filePos, file->token().end());

    // Warn about file problems
    if (file->classes->empty())
        message(WARN, file, "file contains to class definitions");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main program
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    // Initialize things
    Sawyer::initializeLibrary();
    mlog.initialize("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);
    mlog[INFO].enable();

    // Parse the command-line
    Sawyer::CommandLine::Parser cmdlineParser = makeCommandLineParser();
    RosettaGenerator rosettaGenerator;
    rosettaGenerator.adjustParser(cmdlineParser);
    YamlGenerator yamlGenerator;
    yamlGenerator.adjustParser(cmdlineParser);
    const std::vector<std::string> args = parseCommandLine(cmdlineParser, argc, argv);

    // Parse the input files to produce the AST
    auto project = Ast::Project::instance();
    for (const std::string &arg: args)
        parseFile(project->files->push_back(Ast::File::instance(arg))());

    switch (settings.backend) {
        case Backend::YAML:
            yamlGenerator.generate(project);
            break;
        case Backend::NONE:
            break;
        case Backend::ROSETTA_BINARY:
            rosettaGenerator.generate(project);
            break;
    }
}
