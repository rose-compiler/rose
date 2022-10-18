#ifndef ROSE_BinaryAnalysis_Debugger_GdbResponse_H
#define ROSE_BinaryAnalysis_Debugger_GdbResponse_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB

#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/Yaml.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

class GdbResponse {
    enum TokenType {
        TOK_EOF,                                        // end of file
        TOK_LEFT,                                       // '{' or '['
        TOK_RIGHT,                                      // '}' or ']'
        TOK_DIGITS,                                     // sequence of digits
        TOK_SYMBOL,                                     // sequence of letters
        TOK_STRING,                                     // C-style string literal
        TOK_GDB,                                        // the string "(gdb)"
        TOK_OTHER                                       // anything else
    };

    class Token {
        friend class TokenStream;
        TokenType type_ = TOK_EOF;
        size_t begin_ = 0;
        size_t end_ = 0;

    public:
        Token() {}
        Token(TokenType type, size_t begin, size_t end)
            : type_(type), begin_(begin), end_(end) {
            ASSERT_require(end >= begin);
        }

        TokenType type() const {
            return type_;
        }

        size_t begin() const {
            return begin_;
        }

        size_t end() const {
            return end_;
        }

        size_t size() const {
            return end_ - begin_;
        }
    };

    class TokenStream {
        std::string content_;
        size_t at_ = 0;
        std::vector<Token> tokens_;

    public:
        explicit TokenStream(const std::string &sentence)
            : content_(sentence) {}

        const Token& operator[](size_t lookahead);
        void consume(size_t n = 1);
        std::string lexeme(size_t lookahead) const;
        const std::string& content() const;

    private:
        void makeNextToken();
    };

    enum class ResultClass { DONE, RUNNING, CONNECTED, ERROR, EXIT, UNSPECIFIED };

    enum class AsyncClass {
        STOPPED,
        RUNNING,
        THREAD_GROUP_ADDED,
        THREAD_GROUP_STARTED,
        THREAD_CREATED,
        OTHER,
        UNSPECIFIED
    };

    struct ResultRecord {
        ResultClass rclass = ResultClass::UNSPECIFIED;
        Yaml::Node results;
    };

    struct AsyncRecord {
        AsyncClass aclass = AsyncClass::UNSPECIFIED;
        Yaml::Node results;
    };

public:
    std::string raw;                                    // raw string from GDB, no line termination
    bool atEnd = false;                                 // saw a "(gdb)" line
    std::string token;                                  // optional GDB "token", a sequence of digits
    ResultRecord result;
    AsyncRecord exec;
    AsyncRecord status;
    AsyncRecord notify;
    std::string console;
    std::string target;
    std::string log;

public:
    static GdbResponse parse(const std::string &line);
    void print(std::ostream&) const;

private:
    static Exception syntaxError(std::string mesg, TokenStream&, size_t);
    static void parseResultRecord(TokenStream&, GdbResponse&);
    static std::pair<AsyncClass, Yaml::Node> parseAsyncOutput(TokenStream&);
    static void parseExecAsyncOutput(TokenStream&, GdbResponse&);
    static void parseStatusAsyncOutput(TokenStream&, GdbResponse&);
    static void parseNotifyAsyncOutput(TokenStream&, GdbResponse&);
    static void parseConsoleStreamOutput(TokenStream&, GdbResponse&);
    static void parseTargetStreamOutput(TokenStream&, GdbResponse&);
    static void parseLogStreamOutput(TokenStream&, GdbResponse&);
    static std::pair<std::string, Yaml::Node> parseResult(TokenStream&);
    static Yaml::Node parseValue(TokenStream&);
    static std::string nodeToString(const Yaml::Node&, const std::string&);
};

std::ostream& operator<<(std::ostream&, const GdbResponse&);

} // namespace
} // namespace
} // namespace

#endif
#endif
