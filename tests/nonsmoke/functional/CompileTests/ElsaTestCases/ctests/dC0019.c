void CreateArray() {
  int x = sizeof (struct {});

  // this is from k0035.cc  
  x = (x) + (sizeof(struct {}));
}
