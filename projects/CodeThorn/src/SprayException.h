#ifndef SPRAY_EXCEPTION
#define SPRAY_EXCEPTION

  class SprayException : public std::exception {
  public:
    SprayException(std::string text) {
      this->text=text;
    }
    const char* what() const throw() {
      return text.c_str();
    }
    ~SprayException() throw() {
    }
  private:
    std::string text;
  };

#endif
