#include "preprocess.h"
#include "Tokens.h"

namespace Jovial {

constexpr bool TRACE_CONSTRUCTION = false;

// Return true if character c is whitespace
bool whitespace_char(int c)
{
   return (c == ' ' || c == '\t' || c == '\n'|| c == '\v' || c == '\f' || c == '\r');
}

// Return true if character c may start a name
bool start_name_char(int c)
{
   if (c == '$') return true;
   else if (c >= 'A' && c <= 'Z') return true;
   else if (c >= 'a' && c <= 'z') return true;
   return false;
}

// Return true if character c may be part of a name
bool name_char(int c)
{
   if (start_name_char(c)) return true;
   else if (c == '\'') return true;
   else if (c >= '0' && c <= '9') return true;
   return false;
}

enum class State {
    start_token, E1, F, I, N, E2, WS, name_start, name, close_paren,
    define_quote1, define_quote2, end_comment_quote1, end_comment_quote2
};
static const char* enum_str[] {
   "start_token", "E1", "F", "I", "N", "E2", "WS", "name_start", "name", "close_paren",
   "define_quote1", "define_quote2", "end_comment_quote1", "end_comment_quote2"
};

int to_int(State s) { return static_cast<int>(s); }
const char* to_string(State s) { return enum_str[to_int(s)]; }


//
// Replace the double quote symbol " with ? in the DefineString of a
// DefineDeclaration.  The " symbol wrecks havoc on the current lexer,
// primarily because it is also used to delineate comments in the Jovial language.
//
int preprocess(std::istream &in_stream, std::ostream &out_stream, std::ostream &token_stream)
{
   int c;
   int cLineCount{0};      // comment line counter
   int line{1}, col{0};    // line and column counter
   int bLine{1}, eLine{1}; // token beginning and ending line
   int bCol{0}, eCol{0};   // token beginning and ending column
   State state{State::start_token}; // start looking for a token
   std::string lexeme{};

   int comment_type{static_cast<int>(Rose::builder::JovialEnum::comment)};
   int previous{'\n'}; // pretend the file starts with a newline

   while ((c = in_stream.get()) != EOF) {
     col += 1;

     if (TRACE_CONSTRUCTION) {
       std::cerr << "\n";
       std::cerr << "(" << line << "," << col << ")";
       std::cerr <<": " << std::to_string(c) << ": ";
       if (c != '\n') std::cerr.put(c);
       else std::cerr.put(' ');
       std::cerr << ": state is " << to_string(state) << ": ";
     }

     if (c == '\n') {
       line += 1;
       col = 0;
     }

     switch (state) {
        case State::start_token: // start of a token (currently only DEFINE or comment)
          bLine = line; bCol = col;
          if (c == 'd' || c == 'D') {
            // assume DEFINE preceded by whitespace (e.g., no "ITEM SOMETHING'DEFINE")
            if (whitespace_char(previous)) {
              state = State::E1;
            }
          }
          else if (c == '"') {
            cLineCount = 0;
            lexeme.push_back('"');
            state = State::end_comment_quote1; // beginning of a '"' comment
          }
          else if (c == '%') {
            cLineCount = 0;
            lexeme.push_back('%');
            state = State::end_comment_quote2; // beginning of a '%' comment
          }
          break;
        case State::E1:
           if (c == 'e' || c == 'E') state = State::F;
           else state = State::start_token;
           break;
        case State::F:
           if (c == 'f' || c == 'F') state = State::I;
           else state = State::start_token;
           break;
        case State::I:
           if (c == 'i' || c == 'I') state = State::N;
           else state = State::start_token;
           break;
        case State::N:
           if (c == 'n' || c == 'N') state = State::E2;
           else state = State::start_token;
           break;
        case State::E2:
           if (c == 'e' || c == 'E') state = State::WS;
           else state = State::start_token;
           break;
        case State::WS:
           if (whitespace_char(c)) {
              state = State::name_start;
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = State::start_token;
           }
           break;
        case State::name_start:
           if (whitespace_char(c)) {
              break; // keep looking for start of DefineName
           }
           else if (start_name_char(c)) {
              state = State::name;
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = State::start_token;
           }
           break;
        case State::name:
           if (name_char(c)) {
              break; // keep looking for characters in DefineName
           }
           else if (c == '(') {
              state = State::close_paren; // look for end of macro variable list
              break;
           }
           else if (whitespace_char(c)) {
              state = State::define_quote1; // look for starting quote
           }
           else if (c == '"') {
              state = State::define_quote2; // look for ending quote
              c = '?'; // change '"' to '?' to make lexer happy
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = State::start_token;
           }
           break;
        case State::close_paren:
           if (c == ')') {
              state = State::define_quote1;
           }
           break;
        case State::define_quote1:
           if (c == '"') {
              state = State::define_quote2;
              c = '?'; // change '"' to '?' to make lexer happy
           }
           break;
        case State::define_quote2:
           if (c == '"') {
              state = State::start_token;
              c = '?'; // change '"' to '?' to make lexer happy
           }
           break;
        case State::end_comment_quote1:
          eLine = line; eCol = col;

          // break up "large" comment into two comments
          if (c == '\n') cLineCount += 1;
          if (cLineCount > 96) {  // has been tested by breaking at 100 count
            out_stream.put('"'); out_stream.put('"');
            cLineCount = 0;
          }

          lexeme.push_back(c);
          if (c == '"') {
            token_stream << comment_type << "," << bLine << "," << bCol << "," << eLine << "," << eCol << ",";
            token_stream << lexeme << "\n";
            bLine = eLine; bCol = eCol;
            lexeme.clear();
            state = State::start_token; // found end of a '"' comment
          }
          break;
        case State::end_comment_quote2:
          eLine = line; eCol = col;

          // break up "large" comment into two comments
          if (c == '\n') cLineCount += 1;
          if (cLineCount > 96) {  // tested by breaking at 100 count
            out_stream.put('%'); out_stream.put('%');
            cLineCount = 0;
          }

          lexeme.push_back(c);
          if (c == '%') {
            token_stream << comment_type << "," << bLine << "," << bCol << "," << eLine << "," << eCol << ",";
            token_stream << lexeme << "\n";
            bLine = eLine; bCol = eCol;
            lexeme.clear();
            state = State::start_token; // found end of a '%' comment
          }
          break;
        default:
          out_stream.put(c); // program probably illegal
          return -1;
     }

     previous = c;
     out_stream.put(c);
   }
   return 0;
}

} // namespace Jovial
