/*
   POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <poet_ASTeval.h>
#include <string>
#include <list>

extern bool debug_time;
std::vector<std::string> poet_files, extra_input;


int main(int argc, char** argv)
{
  int index = initialize(argc, argv);
  EvaluatePOET::startup();
  try {
  std::cerr << "POET files: " ;
  for ( ; index < argc; ++index) {
     const char* fname = argv[index];
     if (*fname == '-')  break; //* stop processing POET files
     std::cerr << argv[index] << " ";
     poet_files.push_back(fname);
  }
  std::cerr << "\n";
  if (index < argc) std::cerr << "extra parameters: ";
  for ( ; index < argc; ++index) {
      std::cerr << argv[index] << " ";
      extra_input.push_back(argv[index]);
  }
  std::cerr << "\n";
  for (int i = 0; i < poet_files.size(); i++){
     const char* fname = poet_files[i].c_str();
     EvaluatePOET::eval_program(process_file(fname));
  }
  }
  catch (Error err) { return 1;}
  return 0;
}
