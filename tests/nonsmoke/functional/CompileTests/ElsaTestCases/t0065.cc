// cc.in65
// some tricky const-eval expressions

const int five = 5;
int array[five];

typedef int PRInt32;
const   PRInt32 kDefaultStringSize = 64;
enum  eCharSize {eOneByte=0,eTwoByte=1};
char mBuffer[kDefaultStringSize<<eTwoByte];
