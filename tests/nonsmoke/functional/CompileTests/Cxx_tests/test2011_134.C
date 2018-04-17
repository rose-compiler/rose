// DQ (9/25/2011): This is currently a bug in ROSE (data stucture member initialization).

typedef struct { float a; float b; float c; float d; } __m128;

void foo(void)
{
// return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
   (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
}
