int main() {
  int i=42;
  int x = i;
  switch (i)
    {
    case 1: {
      x = 2;
      break;
      int y = 4; // dead
    }
    case 2: {
      x = 2;
      int z = 5; // unused
    }
    case 3: {
      // empty
    }
    default:; // empty stmt-expr
    }
  
  7; // no-effect expr
}
