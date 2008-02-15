
int main()
   {
     int x[4];
     int y[4][4];

     for (int i=0; i < 4; i++)
        {
          x[i] = 7;
        }

     for (int i=0; i < 4; i++)
        {
          for (int j=0; j < 4; j++)
             {
               y[i][j] = 42;
             }
        }

     return 0;
   }
