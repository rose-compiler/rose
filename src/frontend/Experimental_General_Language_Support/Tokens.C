//===-- src/frontend/Experimental_General_Language_Support/Tokens.C ----*- C++ -*-===//
//
// Supports reading tokens from files (for now)
//
//===-----------------------------------------------------------------------------===//

#include "Tokens.h"
#include <iostream>
#include <sstream>

namespace Rose {
  namespace builder {

TokenStream::TokenStream(const std::string &filename) {
  std::string line, word;
  std::vector<std::string> row(6);
  std::ifstream ifs{filename};

  while (ifs) {
    row.clear();
    std::getline(ifs, line);
    if (line.length() == 0) break;
    for (std::stringstream s{line}; getline(s, word, ','); ) {
      // There may be commas in the lexeme string (last element), join them
      if (row.size() == 6) {
        word = row.back() + "," + word;
        row.pop_back();
      }
      row.emplace_back(word);
    }
    if (row.size() == 6) {
      tokens_.emplace_back(Token{row});
    }
    else {
      std::cerr << "TokenStream:: WARNING, incorrect number of token elements, skipping row\n";
    }
  }
  ifs.close();

  next_ = 0;
}

std::ostream& operator<< (std::ostream &os, const Token &tk) {
  os << static_cast<int>(tk.type_) << ','
     << tk.bLine_ << ',' << tk.bCol_ << ','
     << tk.eLine_ << ',' << tk.eCol_ << ',' << tk.lexeme_;
  return os;
}

  } // namespace builder
} // namespace Rose
