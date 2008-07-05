

void foobar(int x)
   {
  // Static array declaration
     float array[10];
     array[4] = 0;
     array[7] = 0;
  // array[9] = 0;

     for (int i=0; i < 5; i++)
          array[i] = 0;
   }

#if 0
void foobar()
   {
     float array2D[10][10];
     float array3D[10][10][10];
     float array4D[10][10][10][10];
     float array5D[10][10][10][10][10];
     float array6D[10][10][10][10][10][10];
     float array7D[10][10][10][10][10][10][10];

     array2D[4][7] = 0;

     for (int i=0; i < 5; i++)
        {
          for (int j=0; j < 5; i++)
             {
               array2D[i][j] = 0;
               array3D[i][j][0] = 0;
               array4D[i][j][0][0] = 0;
               array5D[i][j][0][0][0] = 0;
               array6D[i][j][0][0][0][0] = 0;
               array7D[i][j][0][0][0][0][0] = 0;
             }
        }
   }
#endif
