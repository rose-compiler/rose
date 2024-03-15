#ifndef ROSE_BinaryAnalysis_Debugger_GdbResponse_H
#define ROSE_BinaryAnalysis_Debugger_GdbResponse_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <Rose/BinaryAnalysis/Debugger/Exception.h>

#include <Rose/Yaml.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Reponse from the GNU debugger, GDB.
 *
 *  This class represents one parsed line of output from GDB's machine interface (MI) output. */
class GdbResponse {

    // Token types from the lexical analyzer
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

    // One token from the GDB MI output langauge
    class Token {
        friend class TokenStream;
        TokenType type_ = TOK_EOF;
        size_t begin_ = 0;
        size_t end_ = 0;

    public:
        Token();
        Token(TokenType, size_t begin, size_t end);

        TokenType type() const;
        size_t begin() const;
        size_t end() const;
        size_t size() const;
    };

    // Lexical analysis and the resulting sequence of tokens.
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

public:
    /** GDB result class for a result records. */
    enum class ResultClass { DONE, RUNNING, CONNECTED, ERROR, EXIT, UNSPECIFIED };

    /** GDB async class for asynchronous records. */
    enum class AsyncClass {
        STOPPED,
        RUNNING,
        THREAD_GROUP_ADDED,
        THREAD_GROUP_STARTED,
        THREAD_GROUP_EXITED,
        THREAD_CREATED,
        THREAD_EXITED,
        BREAKPOINT_MODIFIED,
        OTHER,
        UNSPECIFIED
    };

    /** GDB result record. */
    struct ResultRecord {
        ResultClass rclass = ResultClass::UNSPECIFIED;  /**< The result class. */
        Yaml::Node results;                             /**< YAML node representing the results. */

        /** True if this record is initialized. */
        explicit operator bool() const {
            return rclass != ResultClass::UNSPECIFIED;
        }
    };

    /** GDB asynchronous record. */
    struct AsyncRecord {
        AsyncClass aclass = AsyncClass::UNSPECIFIED;    /**< The asynchronous class. */
        Yaml::Node results;                             /**< YAML node representing the results. */

        /** True if this record is initialized. */
        explicit operator bool() const {
            return aclass != AsyncClass::UNSPECIFIED;
        }
    };

public:
    std::string raw;                                    /**< The raw string from GDB without line termination. */
    bool atEnd = false;                                 /**< Whether the line matches "(gdb)". */
    std::string token;                                  /**< The optional GDB "token", a sequence of digits. */
    ResultRecord result;                                /**< GDB '^' records. */
    AsyncRecord exec;                                   /**< GDB '*' records. */
    AsyncRecord status;                                 /**< GDB '+' records. */
    AsyncRecord notify;                                 /**< GDB '=' records. */
    std::string console;                                /**< GDB '~' records. */
    std::string target;                                 /**< GDB '@@' records. */
    std::string log;                                    /**< GDB '@&' records. */

public:
    /** Parse a response from a GDB MI output line. */
    static GdbResponse parse(const std::string &line);

    /** Print a response to a stream. */
    void print(std::ostream&) const;

private:
    // Construct an exception for a syntax error with information about the error and where it occurs.
    static Exception syntaxError(std::string mesg, TokenStream&, size_t);

    // Parsers
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

    // Returns indented YAML unparsed from the specified node tree.
    static std::string nodeToString(const Yaml::Node&, const std::string&);
};

/** Print a GDB response record to a stream. */
std::ostream& operator<<(std::ostream&, const GdbResponse&);

} // namespace
} // namespace
} // namespace

#endif
#endif
