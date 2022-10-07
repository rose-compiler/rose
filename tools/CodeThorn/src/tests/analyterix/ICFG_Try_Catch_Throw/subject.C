int bar37() {
  return -1;
}

void foo37() { 
  try {
    throw bar37();
  }
  catch(int val) {
    return;
  }
  throw;
  throw "hello";
}
