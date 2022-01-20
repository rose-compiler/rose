#include "preprocess.h"
#include "Tokens.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


int main(int argc, char* argv[])
{
  std::ifstream in_stream;
  std::ofstream out_stream;
  std::ofstream token_stream;
  std::ostringstream string_stream;

  int result{0};
  bool tokens_only{false};

  for (int i = 1; i < argc; i++) {
    std::string arg;
    arg += argv[i];
    if (arg.compare("-i") == 0) {
      in_stream.open(argv[i+1]);
    }
    else if (arg.compare("-o") == 0) {
      out_stream.open(argv[i+1]);
    }
    else if (arg.compare("-t") == 0) {
      token_stream.open(argv[i+1]);
    }
    else if (arg.compare("--tokens-only") == 0) {
      tokens_only = true;
    }
  }

  // There must always be an input file specified
  if (!in_stream.is_open()) {
    std::cerr << "Usage: jovial_preprocess -i input_file "
              << "[-o preprocessed_file_out -t token_file_out --tokens-only]\n";
    if (out_stream.is_open()) out_stream.close();
    if (token_stream.is_open()) token_stream.close();
    exit(1);
  }

  // If only tokens are to be processed, redirect preprocessed file to /dev/null
  if (tokens_only) {
    if (out_stream.is_open()) out_stream.close();
    out_stream.open("/dev/null");
    if (token_stream.is_open()) {
      result = Jovial::preprocess(in_stream, out_stream, token_stream);
    }
    else {
      result = Jovial::preprocess(in_stream, out_stream, string_stream);
    }
  }
  // Normal usage: preprocessing to std::cout and no tokens
  else if (!out_stream.is_open()) {
    if (!token_stream.is_open()) token_stream.open("/dev/null");
    result = Jovial::preprocess(in_stream, std::cout, token_stream);
  }
  else {
    if (!token_stream.is_open()) token_stream.open("/dev/null");
    result = Jovial::preprocess(in_stream, out_stream, token_stream);
  }

  if (in_stream.is_open()) in_stream.close();
  if (out_stream.is_open()) out_stream.close();
  if (token_stream.is_open()) token_stream.close();

  return result;
}
