// I think this example supports why the designator must be a list (to hold the array
// or references that represents multi-deminsional array indexing such as "[0][0]").
int multidimensionalArray[11][22][33] = { [3][4][5] = 76 };

#if 1
static short grid[3] [4] = { [0][0]=8, [0][1]=6,
                             [0][2]=4, [0][3]=1,
                             [2][0]=9, [2][1]=3,
                             [2][2]=1, [2][3]=1 };
#endif


