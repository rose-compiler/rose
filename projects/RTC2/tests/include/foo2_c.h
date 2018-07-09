#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "limits.h"
#include "float.h"
#include "time.h"

#ifdef __cplusplus
using namespace std;
#endif

enum boolean {
  TRUE = 0,
  FALSE = 1
};

#define Check_Signed_Add(limit, OP1, OP2, Res) \
  do{ \
    Res = OP1 + OP2; \
    if((!((OP1 & limit) ^ (OP2 & limit))) && \
      ((OP1 & limit) ^ (Res & limit))) { \
        printf("Overflow Recorded\n"); \
    } \
  }while(0)


#define Check_Unsigned_Add(limit, OP1, OP2) \
  do{ \
    if(OP1 > (limit - OP2)) { \
      printf("Overflow Recorded\n"); \
    } \
  }while(0)


#define Check_Signed_Div(limit, OP1, OP2, Res) \
  do{ \
    if ((!OP2) || ((!(OP1 ^ limit)) && (!(~OP2)))) { \
      printf("Division undefined/overflow\n"); \
      Res = 0; \
    } \
    else { \
      Res = OP1/OP2; \
    } \
  }while(0)

#define Check_Unsigned_Div(OP1, OP2, Res) \
  do{ \
    if(!OP2) { \
      printf("Division undefined\n"); \
      Res = 0; \
    } \
    else { \
      Res = OP1/OP2; \
    } \
  }while(0)



//#ifdef PRINT_DEBUG
#if 1
#define Begin_Print(FnName, OPs) \
  do{ \
    printf("------------------------------------\n"); \
    printf("%s: Begin\n", FnName); \
    printf("%s", OPs); \
  }while(0)
#else
#define Begin_Print(FnName, OPs) \
  do{ \
  }while(0)
#endif


#ifdef PRINT_DEBUG
#define End_Print(FnName, ResC, OverflowBitMacro, OverflowBitNormal) \
  do{ \
    printf("%s", ResC); \
    printf("OverflowBit: Macro: %s, Normal: %s\n", \
        OverflowBitMacro ? "true" : "false", \
        OverflowBitNormal ? "true" : "false"); \
    printf("%s: End\n", FnName); \
    printf("------------------------------------\n"); \
  }while(0)
#else
#define End_Print(FnName, ResC, OverflowBitMacro, OverflowBitNormal) \
  do { \
  }while(0)
#endif



static inline
int Int_Add_Int_Int(int OP1, int OP2)
{
  int Res;

  // Macro version
  Check_Signed_Add(INT_MIN, OP1, OP2, Res);

  return Res;
}

static inline
long Long_Add_Long_Long(long OP1, long OP2) {

  long Res;

  // Macro version
  Check_Signed_Add(LONG_MIN, OP1, OP2, Res);

  return Res;
}

static inline
unsigned int UInt_Add_UInt_UInt(unsigned int OP1,unsigned int OP2)
{
  // Macro version
  Check_Unsigned_Add(UINT_MAX, OP1, OP2);

  return OP1 + OP2;
}

static inline
unsigned long ULong_Add_ULong_ULong(unsigned long OP1, unsigned long OP2)
{
  // Macro version
  Check_Unsigned_Add(ULONG_MAX, OP1, OP2);

  return OP1 + OP2;
}

static inline
int Int_Sub_Int_Int(int OP1, int OP2)
{
  // Res = OP1 - OP2
  // Equivalent to Res = OP1 + (-OP2)

  // FIXME: The negation of OP2, might cause an overflow

  return Int_Add_Int_Int(OP1, -OP2);
}

static inline
long Long_Sub_Long_Long(long OP1, long OP2)
{
  // Res = OP1 - OP2
  // Equivalent to Res = OP1 + (-OP2)

  // FIXME: The negation of OP2, might cause an overflow
  return Long_Add_Long_Long(OP1, -OP2);
}

static inline
unsigned int UInt_Sub_UInt_UInt(unsigned int OP1, unsigned int OP2)
{
//  assert(0 && "UInt_Sub_UInt_UInt cannot overflow!");
//  assert(0);
}

static inline
unsigned long ULong_Sub_ULong_ULong(unsigned long OP1, unsigned long OP2)
{
//  assert(0 && "ULong_Sub_ULong_ULong cannot overflow!");
//  assert(0);
}

// Check_Int.. Sign bit problems -- doesn't work
#if 0
bool Check_Int_Mult_Int_Int(int OP1, int OP2) {

  printf("Check_Int_Mult_Int_Int\n");

  bool status = false;

  long int OP1_Long = (long int)OP1;
  long int OP2_Long = (long int)OP2;

  long int Res = OP2_Long << (long int)(log2(OP1_Long - 1) + 1);

  unsigned int UInt_Mask1 = UINT_MAX;
  long unsigned int ULong_Mask1 = UInt_Mask1;

  long unsigned int ULong_Mask2 = ~ULong_Mask1;

  long int Res_HighOrderBits = Res & ULong_Mask2;

  if((Res_HighOrderBits ^ ULong_Mask2) && (Res_HighOrderBits ^ (long int)0)) {
    printf("Overflow recorded (High Order - Not all 0s/1s)\n");
    status = true;
  }


  int OP1_SignBit = OP1 & INT_MIN;
  int OP2_SignBit = OP2 & INT_MIN;
  int Res_SignBit = (int)Res & INT_MIN;

  #ifdef PRINT_DEBUG
  printf("OP1_SignBit: %d, OP2_SignBit: %d, Res_SignBit: %d\n", OP1_SignBit,
          OP2_SignBit, Res_SignBit);
  #endif

//  if(Res != (OP1_SignBit ^ OP2_SignBit)) {
  if(Res_SignBit ^ (OP1_SignBit ^ OP2_SignBit)) {
    printf("Overflow recorded (Sign)\n");
    status = true;
  }

  return status;
}
#endif


// Convert the multiplication overflow/underflow work into a set
// of macros which are called based on the **size** of the inputs
// This requires knowledge of the next higher size and then
// casting to that size in the macro.
#if 0
#define Check_Signed_Mult_4_4(OP1, OP2, res) \
  do{ \

  }while(0)
#endif


// From here till the end -- macro enables mult and divide
#if 1

#if 0
#ifdef PORTABLE_VERSION
int Int_Mult_Int_Int(int OP1, int OP2) {

  if(sizeof(long int) == 2 * sizeof(int)) {
    return handleSignedInt2LongMult(OP1, OP2)
  }
  else if(sizeof(long long int) == 2 * sizeof(int)) {
    long long int OP1_LL = OP1;
    long long int OP2_LL = OP2;
    long long int Res;
    handleSignedMult(OP1_LL, OP2_LL, Res, UINT_MAX, INT_MIN);
    return (int)Res;
  }
  else if(sizeof(int64_t) == 2 * sizeof(int)) {
    int64_t OP1_64 = OP1;
    int64_t OP2_64 = OP2;
    int64_t Res;
    handleSignedMult(OP1_64, OP2_64, Res, UINT_MAX, INT_MIN);
    return (int)Res;
  }
  else {
    printf("Can't find a larger data type!\n");
  }
}
#else

static inline
int handleSignedInt2LongMult(int OP1, int OP2) {

  // int to long int
  long int OP1_Long = (long int)(OP1);
  long int OP2_Long = (long int)(OP2);

  #ifdef PRINT_DEBUG
  printf("OP1: %d\n", OP1);
  printf("OP1_Long: %ld\n", OP1_Long);
  printf("OP2: %d\n", OP2);
  printf("OP2_Long: %ld\n", OP2_Long);
  #endif

  // Res = OP1 * OP2
  long int Res = OP1_Long * OP2_Long;

  unsigned int UInt_Mask1 = UINT_MAX;
  long unsigned int ULong_Mask1 = UInt_Mask1;

  #ifdef PRINT_DEBUG
  printf("UInt_Mask1: %u\n", UInt_Mask1);
  printf("ULong_Mask1: %lu\n", ULong_Mask1);
  #endif
  long unsigned int ULong_Mask2 = ~ULong_Mask1;

  #ifdef PRINT_DEBUG
  printf("After: ULong_Mask2: %lu\n", ULong_Mask2);
  #endif


  // Mask out all the lower half bits (incl sign bit) in Res
  // Can't do a simple (-1) -- even if we cast it -- because
  // it will be sign extended!
  #ifdef PRINT_DEBUG
  printf("Res: %ld\n", Res);
  #endif
  long int Res_HighOrderBits = Res & ULong_Mask2;

  #ifdef PRINT_DEBUG
  printf("Res_HighOrderBits: %ld\n", Res_HighOrderBits);
  #endif

  // Now, we have the higher order bits of the result.
  // We need to check that they are
  // 1. not all ones
  // 2. not all zeroes
  // Both these cases represent sign extension, and hence
  // no overflow.


  // For this, Res_HighOrderBits should not be same as ULong_Mask2 (i.e. all ones)
  // nor should it be same as 0
  if((Res_HighOrderBits ^ ULong_Mask2) && (Res_HighOrderBits ^ (long int)0)) {
    printf("Overflow recorded (High Order - Not all 0s/1s)\n");
  }

  #if 0
//  if(Res_HighOrderBits) {
  if(Res_HighOrderBits ^ (long int)(0)) {
    printf("Overflow recorded (High Order)\n");
  }
  #endif


  int OP1_SignBit = OP1 & INT_MIN;
  int OP2_SignBit = OP2 & INT_MIN;
  int Res_SignBit = (int)Res & INT_MIN;

  #ifdef PRINT_DEBUG
  printf("OP1_SignBit: %d, OP2_SignBit: %d, Res_SignBit: %d\n", OP1_SignBit,
          OP2_SignBit, Res_SignBit);
  #endif

//  if(Res != (OP1_SignBit ^ OP2_SignBit)) {
  if(Res_SignBit ^ (OP1_SignBit ^ OP2_SignBit)) {
    printf("Overflow recorded (Sign)\n");
  }

  return (int)Res;
}
#endif
#endif


int Int_Mult_Int_Int(int OP1, int OP2)
{
  // Check_Int.. Sign bit problems -- doesn't work
  #if 0
  if(!Check_Int_Mult_Int_Int(OP1, OP2)) {
//  if(!Check_Int_Mult_Int_Int) {
    printf("Check: No overflow\n");
    return OP1 * OP2;
  }
  #endif

  #if 0
  printf("Int_Mult_Int_Int\n");
  #endif

  // int -> 4 bytes
  // long int -> 8 bytes
  // int to long int
  long int OP1_Long = (long int)(OP1);
  long int OP2_Long = (long int)(OP2);

  #ifdef PRINT_DEBUG
  printf("OP1: %d\n", OP1);
  printf("OP1_Long: %ld\n", OP1_Long);
  printf("OP2: %d\n", OP2);
  printf("OP2_Long: %ld\n", OP2_Long);
  #endif

  // Res = OP1 * OP2
  long int Res = OP1_Long * OP2_Long;

  unsigned int UInt_Mask1 = UINT_MAX;
  long unsigned int ULong_Mask1 = UInt_Mask1;

  #ifdef PRINT_DEBUG
  printf("UInt_Mask1: %u\n", UInt_Mask1);
  printf("ULong_Mask1: %lu\n", ULong_Mask1);
  #endif
  long unsigned int ULong_Mask2 = ~ULong_Mask1;

  #ifdef PRINT_DEBUG
  printf("After: ULong_Mask2: %lu\n", ULong_Mask2);
  #endif


  // Mask out all the lower half bits (incl sign bit) in Res
  // Can't do a simple (-1) -- even if we cast it -- because
  // it will be sign extended!
  #ifdef PRINT_DEBUG
  printf("Res: %ld\n", Res);
  #endif
  long int Res_HighOrderBits = Res & ULong_Mask2;

  #ifdef PRINT_DEBUG
  printf("Res_HighOrderBits: %ld\n", Res_HighOrderBits);
  #endif

  // Now, we have the higher order bits of the result.
  // We need to check that they are
  // 1. not all ones
  // 2. not all zeroes
  // Both these cases represent sign extension, and hence
  // no overflow.


  // For this, Res_HighOrderBits should not be same as ULong_Mask2 (i.e. all ones)
  // nor should it be same as 0
  if((Res_HighOrderBits ^ ULong_Mask2) && (Res_HighOrderBits ^ (long int)0)) {
    printf("Overflow recorded (High Order - Not all 0s/1s)\n");
  }

  #if 0
//  if(Res_HighOrderBits) {
  if(Res_HighOrderBits ^ (long int)(0)) {
    printf("Overflow recorded (High Order)\n");
  }
  #endif


  int OP1_SignBit = OP1 & INT_MIN;
  int OP2_SignBit = OP2 & INT_MIN;
  int Res_SignBit = (int)Res & INT_MIN;

  #ifdef PRINT_DEBUG
  printf("OP1_SignBit: %d, OP2_SignBit: %d, Res_SignBit: %d\n", OP1_SignBit,
          OP2_SignBit, Res_SignBit);
  #endif

//  if(Res != (OP1_SignBit ^ OP2_SignBit)) {
  if(Res_SignBit ^ (OP1_SignBit ^ OP2_SignBit)) {
    printf("Overflow recorded (Sign)\n");
  }

  //printf("%d\n", (int)Res);
  return (int)Res;
}
#endif

static inline
unsigned int UInt_Mult_UInt_UInt(unsigned int OP1, unsigned int OP2)
{
  printf("UInt_Mult_UInt_UInt\n");

  // unsigned int -> 4 bytes
  // unsigned long int -> 8 bytes
  // unsigned int to unsigned long int

  unsigned long int OP1_ULong = (long unsigned int)(OP1);
  unsigned long int OP2_ULong = (long unsigned int)(OP2);

  #ifdef PRINT_DEBUG
  printf("OP1: %u\n", OP1);
  printf("OP1_ULong: %lu\n", OP1_ULong);
  printf("OP2: %u\n", OP2);
  printf("OP2_ULong: %lu\n", OP2_ULong);
  #endif

  // Res = OP1 * OP2
  long unsigned int Res = OP1_ULong * OP2_ULong;

  unsigned int UInt_Mask1 = UINT_MAX;
  long unsigned int ULong_Mask1 = UInt_Mask1;

  #ifdef PRINT_DEBUG
  printf("UInt_Mask1: %u\n", UInt_Mask1);
  printf("ULong_Mask1: %lu\n", ULong_Mask1);
  #endif

  ULong_Mask1 = ~ULong_Mask1;

  #ifdef PRINT_DEBUG
  printf("After: ULong_Mask1: %lu\n", ULong_Mask1);
  #endif

  #ifdef PRINT_DEBUG
  printf("Res: %lu\n", Res);
  #endif
  long unsigned int Res_HighOrderBits = Res & ULong_Mask1;
  #ifdef PRINT_DEBUG
  printf("Res_HighOrderBits: %lu\n", Res_HighOrderBits);
  #endif


//  if(Res_HighOrderBits) {
  if(Res_HighOrderBits ^ (long unsigned int)(0)) {
    printf("Overflow recorded (High Order)\n");
  }

  return (unsigned int)Res;
}

static inline
unsigned long int ULong_Mult_ULong_ULong(unsigned long int OP1, unsigned long int OP2) {

  printf("ULong_Mult_ULong_ULong\n");
  // Map
  // a - OP1
  // b - OP2
  // a1 - OP1_HighOrderBits
  // a0 - OP1_LowOrderBits
  // b1 - OP2_HighOrderBits
  // b0 - OP2_LowOrderBits

  #ifdef PRINT_DEBUG
  printf("OP1: %lu\n", OP1);
  printf("OP2: %lu\n", OP2);
  #endif

  unsigned int UInt_Mask1 = UINT_MAX;
  unsigned long int ULong_Mask1 = UInt_Mask1;

  unsigned long int OP1_LowOrderBits = OP1 & ULong_Mask1;
  unsigned long int OP2_LowOrderBits = OP2 & ULong_Mask1;

  #ifdef PRINT_DEBUG
  printf("OP1_LowOrderBits: %lu\n", OP1_LowOrderBits);
  printf("OP2_LowOrderBits: %lu\n", OP2_LowOrderBits);
  #endif

  unsigned long int ULong_Mask2 = ~ULong_Mask1;

  unsigned long int OP1_HighOrderBits = OP1 & ULong_Mask2;
  unsigned long int OP2_HighOrderBits = OP2 & ULong_Mask2;


  unsigned long int OP1_HighOrderBits_Shifted = OP1_HighOrderBits >> 32;
  unsigned long int OP2_HighOrderBits_Shifted = OP2_HighOrderBits >> 32;

  #ifdef PRINT_DEBUG
  printf("OP1_HighOrderBits: %lu\n", OP1_HighOrderBits);
  printf("OP2_HighOrderBits: %lu\n", OP2_HighOrderBits);

  printf("OP1_HighOrderBits_Shifted: %lu\n", OP1_HighOrderBits_Shifted);
  printf("OP2_HighOrderBits_Shifted: %lu\n", OP2_HighOrderBits_Shifted);
  #endif



  if((OP1_HighOrderBits_Shifted != 0) && (OP2_HighOrderBits_Shifted != 0)) {
    printf("Overflow Recorded (High Order)\n");
    return OP1 * OP2;
  }

  unsigned long int InterRes = (OP1_HighOrderBits_Shifted * OP2_LowOrderBits) +
    (OP1_LowOrderBits * OP2_HighOrderBits_Shifted);

  if(InterRes > ULong_Mask1) {
    printf("Overflow Recorded (Intermediate Multiply)\n");
    return OP1 * OP2;
  }

  printf("Unsigned Add-");
  Check_Unsigned_Add(ULONG_MAX,InterRes, OP1_LowOrderBits * OP2_LowOrderBits);

  return OP1 * OP2;
}



static inline
int Int_Div_Int_Int(int OP1, int OP2)
{
  int Res;

  // Macro version
  Check_Signed_Div(INT_MIN, OP1, OP2, Res);

  return Res;
}

static inline
long Long_Div_Long_Long(long OP1, long OP2)
{
  long Res;

  // Macro version
  Check_Signed_Div(LONG_MIN, OP1, OP2, Res);

  return Res;
}


static inline
unsigned int UInt_Div_UInt_UInt(unsigned int OP1, unsigned int OP2)
{
  unsigned int Res;

  // Macro version
  Check_Unsigned_Div(OP1, OP2, Res);

  return Res;
}

static inline
unsigned long ULong_Div_ULong_ULong(unsigned long OP1, unsigned long OP2)
{
  unsigned long Res;

  // Macro version
  Check_Unsigned_Div(OP1, OP2, Res);

  return Res;
}

#define CHECK_NAN_INF(Res) \
  do{ \
    if(!isfinite(Res)) { \
      printf("(Float)Overflow Recorded\n"); \
    } \
  }while(0)

static inline
float Float_Add_Float_Float(float OP1, float OP2)
{
  float Res = OP1 + OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
float Float_Sub_Float_Float(float OP1, float OP2)
{
  float Res = OP1 - OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
float Float_Mult_Float_Float(float OP1, float OP2)
{
  float Res = OP1 * OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
float Float_Div_Float_Float(float OP1, float OP2)
{
  float Res = OP1 / OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
double Double_Add_Double_Double(double OP1, double OP2)
{
  double Res = OP1 + OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
double Double_Sub_Double_Double(double OP1, double OP2)
{
  double Res = OP1 - OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
double Double_Mult_Double_Double(double OP1, double OP2)
{
  double Res = OP1 * OP2;
  CHECK_NAN_INF(Res);
  return Res;
}

static inline
double Double_Div_Double_Double(double OP1, double OP2)
{
  double Res = OP1 / OP2;
  CHECK_NAN_INF(Res);
  return Res;
}


static clock_t start;

static inline
void StartClock()
{
  start = clock();
}

static inline
void EndClock()
{
  clock_t end = clock();
  clock_t numticks = end - start;
  double elapsed = ((double)(end - start))/CLOCKS_PER_SEC;
  printf("%lu\n%lf\n", numticks, elapsed);
}
