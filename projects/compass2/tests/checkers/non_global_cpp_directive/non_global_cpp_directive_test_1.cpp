// 3 matches

#define Global_1 1

int main()
    #define Global_2 2
  {
    #define NonGlobal_1 3
    {
      #define NonGlobal_2 4
    }
    #define NonGlobal_3 5
  }
    #define Global_3 6

void foo();
#define Global_4 7
