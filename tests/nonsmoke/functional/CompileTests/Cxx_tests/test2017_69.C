#include <emmintrin.h>
#include <stdio.h>

__m128 foo (__v4si a, __v4si b)
{
  return (__m128)__builtin_ia32_packssdw128(a, b);
}

int main (void)
{
  __v4si a = { 0, 0, 0, 0};
  __v4si b = { 0, 0, 0, 0};

  __v16qi c = (__v16qi)foo(a, b);

  if (__builtin_ia32_vec_ext_v4si((__v4si)c, 0) == 0)
    printf("packssdw-1 passed\n");
  else
    printf("packssdw-1 failed\n");

  return 0;
}
