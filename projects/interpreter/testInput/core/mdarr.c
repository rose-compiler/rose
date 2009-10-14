int arr[2][3][2] = {
        { {1, 2}, {3,  4}, { 5,  6} },
        { {7, 8}, {9, 10}, {11, 12} }
};

int test(int x)
   {
     int sum = 0;
     for (int i = 0; i < 2; ++i)
        {
          for (int j = 0; j < 3; ++j)
             {
               for (int k = 0; k < 2; ++k)
                  {
                    sum += arr[i][j][k];
                  }
             }
        }
     return sum;
   }
