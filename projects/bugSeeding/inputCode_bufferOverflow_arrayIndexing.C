

void foobar(int x)
   {
  // Static array declaration
     float array[10];

  // potential vulnerability: buffer overflow of an array NOT in a loop
     array[4] = 0;

  // potential vulnerability: buffer overflow of an array IN a loop
     for (int i=0; i < 5; i++)
          array[i] = 0;
   }

