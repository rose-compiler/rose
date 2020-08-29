#include <iostream>
#include <fstream>
#include <string>

// Return true if character c is whitespace
static bool whitespace_char(int c)
{
   return (c == ' ' || c == '\t' || c == '\n'|| c == '\v' || c == '\f' || c == '\r');
}

// Return true if character c may start a name
static bool start_name_char(int c)
{
   if (c == '$') return true;
   else if (c >= 'A' && c <= 'Z') return true;
   else if (c >= 'a' && c <= 'z') return true;
   return false;
}

// Return true if character c may be part of a name
static bool name_char(int c)
{
   if (start_name_char(c)) return true;
   else if (c == '\'') return true;
   else if (c >= '0' && c <= '9') return true;
   return false;
}

//
// Replace the double quote symbol " with ? in the DefineString of a
// DefineDeclaration.  The " symbol wrecks havoc on the current lexer,
// primarily because it is also used to delineate comments in the Jovial language.
//
static int preprocess(std::istream & in_stream, std::ostream & out_stream)
{
   enum State {
      D, E1, F, I, N, E2, WS, name_start, name, close_paren,
      define_quote1, define_quote2, end_comment_quote
   };
   State start = D;
   int c, state = start;

// This is implemented as a finite state machine

   while ((c = in_stream.get()) != EOF) {
      switch (state) {
        case D:
           if (c == 'd' || c == 'D') state = E1;
           else if (c == '"') state = end_comment_quote; // beginning of a comment
           break;
        case E1:
           if (c == 'e' || c == 'E') state = F;
           else state = start;
           break;
        case F:
           if (c == 'f' || c == 'F') state = I;
           else state = start;
           break;
        case I:
           if (c == 'i' || c == 'I') state = N;
           else state = start;
           break;
        case N:
           if (c == 'n' || c == 'N') state = E2;
           else state = start;
           break;
        case E2:
           if (c == 'e' || c == 'E') state = WS;
           else state = start;
           break;
        case WS:
           if (whitespace_char(c)) {
              state = name_start;
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = start;
           }
           break;
        case name_start:
           if (whitespace_char(c)) {
              break; // keep looking for start of DefineName
           }
           else if (start_name_char(c)) {
              state = name;
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = start;
           }
           break;
        case name:
           if (name_char(c)) {
              break; // keep looking for characters in DefineName
           }
           else if (c == '(') {
              state = close_paren; // look for the end of a macro variable list
           }
           else if (whitespace_char(c)) {
              state = define_quote1; // look for starting quote
           }
           else if (c == '"') {
              state = define_quote2; // look for ending quote
              c = '?'; // change '"' to '?' to make lexer happy
           }
           else {
              // make sure that "UNDEFINED" isn't recognized as part of a DEFINE statement
              state = start;
           }
           break;
        case close_paren:
           if (c == ')') {
              state = define_quote1;
           }
           break;
        case define_quote1:
           if (c == '"') {
              state = define_quote2; // start of the DefinitionPart
              c = '?'; // change '"' to '?' to make lexer happy
           }
           break;
        case define_quote2:
           if (c == '"') {
              state = start; // end of the DefinitionPart
              c = '?'; // change '"' to '?' to make lexer happy
           }
           break;
        case end_comment_quote:
           if (c == '"') {
              state = start; // found end of the comment
           }
           break;
        default:
           out_stream.put(c); // program probably illegal
           return -1;
      }
      out_stream.put(c);
   }

   return 0;
}

int main(int argc, char* argv[])
{
   std::ifstream in_stream;
   std::ofstream out_stream;
   int result = 0;

   for (int i = 1; i < argc; i++) {
      std::string arg;
      arg += argv[i];
      if (arg.compare("-i") == 0) {
         in_stream.open(argv[i+1]);
      }
      if (arg.compare("-o") == 0) {
         out_stream.open(argv[i+1]);
      }
   }

   // This should somehow be simpler.
   // Try to figure out how to reduce if/else if/else logic
   //
   if (!in_stream.is_open() && !out_stream.is_open()) {
      result = preprocess(std::cin, std::cout);
   }
   else if (in_stream.is_open() && out_stream.is_open()) {
      result = preprocess(in_stream, out_stream);
   }
   else {
      if (in_stream.is_open()) {
         result = preprocess(in_stream, std::cout);
      }
      if (out_stream.is_open()) {
         result = preprocess(std::cin, out_stream);
      }
   }

   if (in_stream.is_open()) {
      in_stream.close();
   }

   if (out_stream.is_open()) {
      out_stream.close();
   }

   return result;
}
