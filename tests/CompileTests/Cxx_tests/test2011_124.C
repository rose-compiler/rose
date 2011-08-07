const int five = 5;
int array[five];

class A
   {
     public:
          enum values { zero, nonzero };
   };

typedef int PRInt32;
const   PRInt32 kDefaultStringSize = 64;
enum  eCharSize {eOneByte=0,eTwoByte=1};
char mBuffer_1[kDefaultStringSize << eTwoByte];

char mBuffer_2[kDefaultStringSize << A::zero];

// Note that "A::nonzero"  will be unparsed as unparsed as "eTwoByte"
// because the types are equivalent.  Not sure where this should be 
// fixed or if it might be an EDG issue.  I expect that our name
// mangling is using values for enums rather than names. This is
// not a critical issue but should be fixed at some point, the 
// resulting code is still correct, but transformations would be
// an issue.
char mBuffer_3[kDefaultStringSize << A::nonzero];
