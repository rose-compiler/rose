#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <Rose/BinaryAnalysis/Debugger/GdbResponse.h>

#include <Rose/StringUtility/Convert.h>
#include <Rose/StringUtility/Escape.h>
#include <stringify.h>

#include <Sawyer/Message.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <cstring>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GdbResponse::Token
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GdbResponse::Token::Token() {}

GdbResponse::Token::Token(TokenType type, size_t begin, size_t end)
    : type_(type), begin_(begin), end_(end) {
    ASSERT_require(end >= begin);
}

GdbResponse::TokenType
GdbResponse::Token::type() const {
    return type_;
}

size_t
GdbResponse::Token::begin() const {
    return begin_;
}

size_t
GdbResponse::Token::end() const {
    return end_;
}

size_t
GdbResponse::Token::size() const {
    return end_ - begin_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GdbReponse::TokenStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const GdbResponse::Token&
GdbResponse::TokenStream::operator[](size_t lookahead) {
    while (lookahead >= tokens_.size()) {
        makeNextToken();
        ASSERT_require(!tokens_.empty());
        if (tokens_.back().type() == TOK_EOF)
            return tokens_.back();
    }
    return tokens_[lookahead];
}

void
GdbResponse::TokenStream::makeNextToken() {
    if (!tokens_.empty() && tokens_.back().type() == TOK_EOF)
        return;
    while (at_ < content_.size() && isspace(content_[at_]))
        ++at_;

    if (at_ >= content_.size()) {
        tokens_.push_back(Token(TOK_EOF, at_, at_));
    } else if (!strncmp(content_.c_str()+at_, "(gdb)", 5)) {
        tokens_.push_back(Token(TOK_GDB, at_, at_+5));
        at_ += 5;
    } else if ('[' == content_[at_] || '{' == content_[at_]) {
        tokens_.push_back(Token(TOK_LEFT, at_, at_+1));
        ++at_;
    } else if (']' == content_[at_] || '}' == content_[at_]) {
        tokens_.push_back(Token(TOK_RIGHT, at_, at_+1));
        ++at_;
    } else if (isdigit(content_[at_])) {
        size_t begin = ++at_;
        while (at_ < content_.size() && isdigit(content_[at_]))
            ++at_;
        tokens_.push_back(Token(TOK_DIGITS, begin, at_));
    } else if (isalpha(content_[at_])) {
        size_t begin = at_++;
        while (at_ < content_.size() && (isalnum(content_[at_]) || strchr("_-", content_[at_])))
            ++at_;
        tokens_.push_back(Token(TOK_SYMBOL, begin, at_));
    } else if ('"' == content_[at_]) {
        size_t begin = ++at_;
        while (at_ < content_.size() && '"' != content_[at_]) {
            if ('\\' == content_[at_])
                ++at_;
            ++at_;
        }
        ASSERT_require2(at_ < content_.size() && '"' == content_[at_],
                        "no closing quote in '" + content_.substr(begin) + "'");
        tokens_.push_back(Token(TOK_STRING, begin, at_));
        ++at_;
    } else {
        tokens_.push_back(Token(TOK_OTHER, at_, at_+1));
        ++at_;
    }
}

void
GdbResponse::TokenStream::consume(size_t n) {
    if (n >= tokens_.size()) {                          // likely
        tokens_.clear();
    } else {
        tokens_.erase(tokens_.begin(), tokens_.end() + n);
    }
}

std::string
GdbResponse::TokenStream::lexeme(size_t lookahead) const {
    const Token &t = tokens_[lookahead];
    ASSERT_require(t.begin() <= t.end());
    ASSERT_require(t.end() <= content_.size());
    return content_.substr(t.begin(), t.end() - t.begin());
}

const std::string&
GdbResponse::TokenStream::content() const {
    return content_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GdbResponse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GdbResponse
GdbResponse::parse(const std::string &line) {
    GdbResponse response;
    response.raw = line;

    // Some lines contain only the reponse "(gdb)" which signifies the end of a group of related lines.
    if (line == "(gdb)") {
        response.atEnd = true;
        return response;
    }

    // Otherwise split the line into a list of tokens and parse the sentence
    TokenStream tokens(line);

    // All lines start with an optional GDB reponse "token" which is "any sequence of digits"
    if (tokens[0].type() == TOK_DIGITS) {
        response.token = tokens.lexeme(0);
        tokens.consume();
    }

    // Other than the optional GDB "token", the first thing on a line is a character that says what kind of line it is.
    if (tokens[0].type() != TOK_OTHER || tokens[0].size() != 1 || !strchr("^*+=~@&", line[tokens[0].begin()]))
        throw syntaxError("expected '^', '*', '+', '=', '~', '@', or '&'", tokens, 0);
    const char lineType = line[tokens[0].begin()];
    tokens.consume();

    switch (lineType) {
        case '^':
            parseResultRecord(tokens, response);
            break;
        case '*':
            parseExecAsyncOutput(tokens, response);
            break;
        case '+':
            parseStatusAsyncOutput(tokens, response);
            break;
        case '=':
            parseNotifyAsyncOutput(tokens, response);
            break;
        case '~':
            parseConsoleStreamOutput(tokens, response);
            break;
        case '@':
            parseTargetStreamOutput(tokens, response);
            break;
        case '&':
            parseLogStreamOutput(tokens, response);
            break;
        default:
            ASSERT_not_reachable("error handled above");
    }

    // We should be at the end of the line now
    if (tokens[0].type() != TOK_EOF)
        throw syntaxError("unexpected additional text", tokens, 0);

    return response;
}

std::string
GdbResponse::nodeToString(const Yaml::Node &node, const std::string &prefix = "") {
    std::stringstream ss;
    Yaml::serialize(node, ss);
    return StringUtility::prefixLines(ss.str(), prefix);
}

void
GdbResponse::print(std::ostream &out) const {
    namespace Stringify = stringify::Rose::BinaryAnalysis::Debugger::GdbResponse;

    out <<"GDB response:\n"
        <<"  raw: " <<StringUtility::yamlEscape(raw) <<"\n";

    if (atEnd)
        out <<"  end-record: yes\n";

    if (!token.empty())
        out <<"  token: " <<token <<"\n";

    if (result) {
        out <<"  result-class: " <<Stringify::ResultClass((int64_t)result.rclass, "") <<"\n"
            <<nodeToString(result.results, "  ");
    }

    if (exec) {
        out <<"  exec-async:\n"
            <<"    async-class: " <<Stringify::AsyncClass((int64_t)exec.aclass, "") <<"\n"
            <<nodeToString(exec.results, "    ");
    }

    if (status) {
        out <<"  status-async:\n"
            <<"    async-class: " <<Stringify::AsyncClass((int64_t)status.aclass, "") <<"\n"
            <<nodeToString(status.results, "    ");
    }

    if (notify) {
        out <<"  notify-async:\n"
            <<"    async-class: " <<Stringify::AsyncClass((int64_t)notify.aclass, "") <<"\n"
            <<nodeToString(notify.results, "    ");
    }

    if (!console.empty())
        out <<"  console-output: " <<StringUtility::yamlEscape(console) <<"\n";
    if (!target.empty())
        out <<"  console-output: " <<StringUtility::yamlEscape(target) <<"\n";
    if (!log.empty())
        out <<"  console-output: " <<StringUtility::yamlEscape(log) <<"\n";
}

std::ostream& operator<<(std::ostream &out, const GdbResponse &r) {
    r.print(out);
    return out;
}

Exception
GdbResponse::syntaxError(std::string mesg, TokenStream &tokens, size_t i) {
    mesg += std::string("\n") +
            "in GDB response: " + tokens.content() + "\n"
            "           here--" + (std::string(tokens[i].begin(), '-') +
                                   std::string(std::max(tokens[i].size(), size_t{1}), '^')) + "\n";
    throw Exception(mesg);
}

void
GdbResponse::parseResultRecord(TokenStream &tokens, GdbResponse &response) {
    // result-class := 'done' | 'running' | 'connected' | 'error' | 'exit'
    if (tokens[0].type() != TOK_SYMBOL)
        throw syntaxError("expected result-class", tokens, 0);
    const std::string resultClass = tokens.lexeme(0);
    if ("done" == resultClass) {
        response.result.rclass = ResultClass::DONE;
    } else if ("running" == resultClass) {
        response.result.rclass = ResultClass::RUNNING;
    } else if ("connected" == resultClass) {
        response.result.rclass = ResultClass::CONNECTED;
    } else if ("error" == resultClass) {
        response.result.rclass = ResultClass::ERROR;
    } else if ("exit" == resultClass) {
        response.result.rclass = ResultClass::EXIT;
    } else {
        throw syntaxError("expected result-class", tokens, 0);
    }
    tokens.consume();

    // zero or more 'result' phrases each introduced by a comma.
    while (tokens[0].type() == TOK_OTHER && "," == tokens.lexeme(0)) {
        tokens.consume();
        auto pair = parseResult(tokens);
        response.result.results[pair.first] = pair.second;
    }
}

std::pair<GdbResponse::AsyncClass, Yaml::Node>
GdbResponse::parseAsyncOutput(TokenStream &tokens) {
    // async-class := 'stopped' | to-be-decided
    AsyncClass aclass = AsyncClass::UNSPECIFIED;
    if (tokens[0].type() != TOK_SYMBOL)
        throw syntaxError("expected async-class", tokens, 0);
    const std::string asyncClass = tokens.lexeme(0);

    // The AsyncClass enum constants are named identically to the string we get from GDB except GDB uses hypens instead of
    // underscores. You may need to run scripts/restringify if you've recently changed the
    // Rose::BinaryAnalysis::Debugger::GdbResponse::AsyncClass enum type.
    for (int64_t enumVal: stringify::Rose::BinaryAnalysis::Debugger::GdbResponse::AsyncClass()) {
        using namespace boost;
        const std::string enumString =
            to_lower_copy(replace_all_copy(stringify::Rose::BinaryAnalysis::Debugger::GdbResponse::AsyncClass(enumVal, ""),
                                           "_", "-"));
        if (asyncClass == enumString) {
            aclass = (AsyncClass)enumVal;
            break;
        }
    }
    if (AsyncClass::UNSPECIFIED == aclass)
        throw syntaxError("expected async-class", tokens, 0);
    tokens.consume();

    // zero or more 'result' phrases each introduced by a comma.
    Yaml::Node root;
    while (tokens[0].type() == TOK_OTHER && "," == tokens.lexeme(0)) {
        tokens.consume();
        auto pair = parseResult(tokens);
        root[pair.first] = pair.second;
    }

    return {aclass, root};
}

void
GdbResponse::parseExecAsyncOutput(TokenStream &tokens, GdbResponse &response) {
    auto pair = parseAsyncOutput(tokens);
    response.exec.aclass = pair.first;
    response.exec.results = pair.second;
}

void
GdbResponse::parseStatusAsyncOutput(TokenStream &tokens, GdbResponse &response) {
    auto pair = parseAsyncOutput(tokens);
    response.status.aclass = pair.first;
    response.status.results = pair.second;
}

void
GdbResponse::parseNotifyAsyncOutput(TokenStream &tokens, GdbResponse &response) {
    auto pair = parseAsyncOutput(tokens);
    response.notify.aclass = pair.first;
    response.notify.results = pair.second;
}

void
GdbResponse::parseConsoleStreamOutput(TokenStream &tokens, GdbResponse &response) {
    if (tokens[0].type() != TOK_STRING)
        throw syntaxError("expected console output string", tokens, 0);
    response.console = StringUtility::cUnescape(tokens.lexeme(0));
    tokens.consume();
}

void
GdbResponse::parseTargetStreamOutput(TokenStream &tokens, GdbResponse &response) {
    if (tokens[0].type() != TOK_STRING)
        throw syntaxError("expected console output string", tokens, 0);
    response.target = StringUtility::cUnescape(tokens.lexeme(0));
    tokens.consume();
}

void
GdbResponse::parseLogStreamOutput(TokenStream &tokens, GdbResponse &response) {
    if (tokens[0].type() != TOK_STRING)
        throw syntaxError("expected console output string", tokens, 0);
    response.log = StringUtility::cUnescape(tokens.lexeme(0));
    tokens.consume();
}

std::pair<std::string, Yaml::Node>
GdbResponse::parseResult(TokenStream &tokens) {
    // result := variable '=' value
    if (tokens[0].type() != TOK_SYMBOL)
        throw syntaxError("expected variable name", tokens, 0);
    const std::string variableName = tokens.lexeme(0);
    tokens.consume();

    if (tokens[0].type() != TOK_OTHER || "=" != tokens.lexeme(0))
        throw syntaxError("expected \"=\" after variable name", tokens, 0);
    tokens.consume();

    Yaml::Node value = parseValue(tokens);
    return {variableName, value};
}

Yaml::Node
GdbResponse::parseValue(TokenStream &tokens) {
    // value := const | tuple | list
    // const := STRING
    // tuple := '{}' | '{' result (',' result)* '}'
    // list  := '[]' | '[' value (',' value)* ']' | '[' result (',' result)* ']'

    if (tokens[0].type() == TOK_STRING) {
        Yaml::Node root(tokens.lexeme(0));
        tokens.consume();
        return root;

    } else if (tokens[0].type() == TOK_LEFT && "{" == tokens.lexeme(0)) {
        tokens.consume();
        std::map<std::string, Yaml::Node> results;
        while (true) {
            if (tokens[0].type() == TOK_RIGHT && "}" == tokens.lexeme(0)) {
                tokens.consume();
                break;
            }
            if (!results.empty()) {
                if (tokens[0].type() == TOK_OTHER && "," == tokens.lexeme(0)) {
                    tokens.consume();
                } else {
                    throw syntaxError("expected comma", tokens, 0);
                }
            }
            results.insert(parseResult(tokens));
        }

        Yaml::Node root;
        for (const auto &result: results)
            root[result.first] = result.second;
        return root;

    } else if (tokens[0].type() == TOK_LEFT && "[" == tokens.lexeme(0)) {
        tokens.consume();
        std::list<Yaml::Node> results;
        while (true) {
            if (tokens[0].type() == TOK_RIGHT && "]" == tokens.lexeme(0)) {
                tokens.consume();
                break;
            }
            if (!results.empty()) {
                if (tokens[0].type() == TOK_OTHER && "," == tokens.lexeme(0)) {
                    tokens.consume();
                } else {
                    throw syntaxError("expected comma", tokens, 0);
                }
            }

            results.push_back(parseValue(tokens));
        }

        Yaml::Node root;
        for (const auto &result: results)
            root.pushBack() = result;
        return root;

    } else {
        throw syntaxError("expected string, tuple, or list", tokens, 0);
    }
}

} // namespace
} // namespace
} // namespace

#endif
