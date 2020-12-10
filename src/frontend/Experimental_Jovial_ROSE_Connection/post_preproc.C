#include "post_preproc.h"

PostPreProc::PostPreProc()
{

}

void PostPreProc::process(std::istream & in_stream, std::ostream & out_stream)
{
   std::string line;
   int pos1, pos2;

   while (std::getline(in_stream, line)) {
      pos1 = line.find("DEFINE");
      pos2 = line.find(";");
      if (pos1 != std::string::npos && pos2 != std::string::npos) {
         for (int i = pos1; i < pos2 + 1; i++) {
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
   PostPreProc postPreProc;
   std::ifstream in_stream;
   std::ofstream out_stream;

   for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
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
      postPreProc.process(std::cin, std::cout);
   }
   else if (in_stream.is_open() && out_stream.is_open()) {
      postPreProc.process(in_stream, out_stream);
   }
   else {
      if (in_stream.is_open()) {
         postPreProc.process(in_stream, std::cout);
      }
      if (out_stream.is_open()) {
         postPreProc.process(std::cin, out_stream);
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
