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
   int posDefine, posSkipWs, posSemicolon, posFirstQuote, posSecondQuote;
   bool multiLineDefine = false;

   while (std::getline(in_stream, line)) {
      if (!multiLineDefine) {                                 // no multi-line define currently being processed
         posSkipWs = line.find_first_not_of(" ");
         posDefine = line.find("DEFINE");
         posSemicolon = line.find(";");
         if (posDefine != std::string::npos && (posDefine == 0 || posDefine == posSkipWs)) {
            // found a DEFINE at the start of a line
            if (posSemicolon != std::string::npos) {          // found a ; in same line, can process single line
               posFirstQuote = line.find("\"");               // find quotes
               posSecondQuote = line.find("\"", posFirstQuote + 1);

               line.replace(posFirstQuote, 1, "?");           // replace quotes
               line.replace(posSecondQuote, 1, "?");

               out_stream << line << "\n";                    // output processed line
            }
            else {                                            // is a multiLineDefine
               multiLineDefine = true;
               posFirstQuote = line.find("\"");               // find first quote
               if (posFirstQuote != std::string::npos) {
                  line.replace(posFirstQuote, 1, "?");        // replace first quote
                  out_stream << line << "\n";                 // output processed line
               }
            }
         }
         else {                                               // line doesn't start with a DEFINE
            out_stream << line << "\n";                       // output unprocessed line
         }
      }
      else {                                                  // process following lines of multi-line define
         posSecondQuote = line.find("\"");
         if (posSecondQuote != std::string::npos) {           // if found second quote
            line.replace(posSecondQuote, 1, "?");             // replace second quote
            out_stream << line << "\n";                       // output processed line
            multiLineDefine = false;                          // reset bool as multi-line define is finished
         }
         else {                                               // second quote not found, multi-line define is not complete
            out_stream << line << "\n";                       // output unprocessed line
         }
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
