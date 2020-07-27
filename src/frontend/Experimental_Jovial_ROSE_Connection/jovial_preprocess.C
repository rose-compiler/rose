#include <iostream>
#include <fstream>
#include <string>

//
// Replace the double quote symbol " with ? in the DefineString of a
// DefineDeclaration.  The " symbol wrecks havoc on the current lexer,
// primarily because it is also used to delineate comments in the Jovial language.
//
static int preprocess(std::istream & in_stream, std::ostream & out_stream)
{
   enum State {start, D, E1, F, I, N, E2, quote1};
   int c, state = start;

   while ((c = in_stream.get()) != EOF) {
      switch (state) {
        case start:
           if (c == 'd' || c == 'D') state = D;
           break;
        case D:
           if (c == 'e' || c == 'E') state = E1;
           else state = start;
           break;
        case E1:
           if (c == 'f' || c == 'F') state = F;
           else state = start;
           break;
        case F:
           if (c == 'i' || c == 'I') state = I;
           else state = start;
           break;
        case I:
           if (c == 'n' || c == 'N') state = N;
           else state = start;
           break;
        case N:
           if (c == 'e' || c == 'E') state = E2;
           else state = start;
           break;
        case E2:
           if (c == '"') {
              state = quote1;
              c = '?'; // change '"' to '?' to make lexer happy
           }
           break;
        case quote1:
           if (c == '"') {
              state = start;
              c = '?'; // change '"' to '?' to make lexer happy
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
