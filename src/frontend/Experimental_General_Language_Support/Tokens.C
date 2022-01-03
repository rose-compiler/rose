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

TokenStream::TokenStream(std::istringstream &is) {
  std::vector<std::string> row(6);
  int error = 0;

  while (is.peek() != std::istream::traits_type::eof()) {
    // Read type and line,column information
    for (int i = 0; i < 5; i++) {
      row[i].clear(); // clear old string
      if ((error = getTokenElement(is,row[i])) < 0) break;
    }

    // Read lexeme
    JovialEnum type = static_cast<JE>(std::stoi(row[0]));
    if (!error && type == JovialEnum::comment) {
      row[5].clear(); // clear old string
      error = getTokenComment(is,row[5]);
    }

    if (!error) {
      // Append the token
      tokens_.emplace_back(Token{row});
    }
    else {
      std::cerr << "TokenStream:: WARNING, error occurred, skipping row\n";
      break;
    }
  }
  next_ = 0;
}

int TokenStream::getTokenElement(std::istream &is, std::string &word) {
  char c;

  while (is.get(c)) {
    if (c != ',') word.append(1,c);
    else return 0; // success
  }
  std::cerr << "TokenStream::getTokenElement: WARNING, error finding token element\n";
  return 1; // failure
}

int TokenStream::getTokenComment(std::istream &is, std::string &comment) {
  char c, terminal;
  int error = 1;

  // Get comment terminal ('%' or '"' for Jovial)
  if (is.get(terminal)) {
    if (terminal == '%' || terminal == '"') {
      comment.append(1,terminal);
    }
    error = 0;
  }

  if (!error) {
    while (is.get(c)) {
      comment.append(1,c);
      if (c == terminal && is.get(c)) {
        if (c == '\n') return 0; // success
      }
    }
  }

  // Report an error
  std::cerr << "TokenStream::getTokenComment: WARNING, error finding comment, lexeme will be empty\n";
  comment.clear();
  return 1; // failure
}

std::ostream& operator<< (std::ostream &os, const Token &tk) {
  os << static_cast<int>(tk.type_) << ','
     << tk.bLine_ << ',' << tk.bCol_ << ','
     << tk.eLine_ << ',' << tk.eCol_ << ',' << tk.lexeme_;
  return os;
}

  } // namespace builder
} // namespace Rose
