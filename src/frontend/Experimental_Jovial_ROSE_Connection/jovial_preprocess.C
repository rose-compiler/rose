#include <iostream>
#include <fstream>
#include <string>

//
// Replace the double quote symbol " with ? in the DefineString of a
// DefineDeclaration.  The " symbol wrecks havoc on the current lexer,
// primarily because it is also used to delineate comments in the Jovial language.
//
static void preprocess(std::istream & in_stream, std::ostream & out_stream)
{
   std::string line;

   while (std::getline(in_stream, line)) {
      if (line.find("DEFINE") != std::string::npos) {
         for (int i = 0; i < line.size(); i++) {
            if (line.compare(i, 1, "\"") == 0) {
               line.replace(i, 1, "?");
            }
         }
         out_stream << line << "\n";
      }
      else {
         out_stream << line << "\n";
      }
   }
}

int main(int argc, char* argv[])
{
   std::ifstream in_stream;
   std::ofstream out_stream;

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
      preprocess(std::cin, std::cout);
   }
   else if (in_stream.is_open() && out_stream.is_open()) {
      preprocess(in_stream, out_stream);
   }
   else {
      if (in_stream.is_open()) {
         preprocess(in_stream, std::cout);
      }
      if (out_stream.is_open()) {
         preprocess(std::cin, out_stream);
      }
   }

   if (in_stream.is_open()) {
      in_stream.close();
   }

   if (out_stream.is_open()) {
      out_stream.close();
   }

   return 0;
}
