
// Similar to test2018_18.C, but with using #if 1
void foobar()
   {
     int x;
     switch (x)
        {
       // These cases that hold a nested case are normalized to each hold a SgNullStatement.
          case 42:
          case 43:

          case 0: if (x)
          case 1:      x = 1;
                    else
       // case 2:      x = 2;
       // case 2: case 3: x = 2;
#if 1
       // This case will be handled correctly.
          case 2:      x = 2;
#else
       // This case will cause a block to be generated, but correctly 
       // handles the cases (and is semantically equivalent).
          case 2: case 3: x = 2;
#endif
          break;
        }
   }
