struct nsAString {
  void Assign (const nsAString & aReadable) {}
  void Assign (char aChar) {}
  void f (const nsAString & aReadable) {
    Assign (aReadable);
  }
};
