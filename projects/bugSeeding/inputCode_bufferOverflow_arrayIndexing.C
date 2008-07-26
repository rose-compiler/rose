

void foobar(int x)
   {
  // This is a static array declaration (might be part of a potential buffer overflow vulnerability)
     float array[10];

  // This is a potential buffer overflow vulnerability
     array[4] = 0;

  // This is a potential buffer overflow vulnerability in a loop
     for (int i=0; i < 5; i++)
          array[i] = 0;
   }

