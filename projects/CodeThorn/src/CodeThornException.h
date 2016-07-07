#ifndef CODETHORN_EXCEPTION
#define CODETHORN_EXCEPTION

  class CodeThornException : public std::exception {
  public:
    CodeThornException(std::string text) {
      this->text=text;
    }
    const char* what() const throw() {
      return text.c_str();
    }
    ~CodeThornException() throw() {
    }
  private:
    std::string text;
  };

#endif
