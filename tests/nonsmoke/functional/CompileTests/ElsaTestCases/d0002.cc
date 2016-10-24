struct _IO_FILE {
  int unbuffered () {}
  void unbuffered (int)   {
    unbuffered ();
  }
};
