// g0014.cc
// compound lit with unsized array

int f()
{
  return (
           (int[]) {1, 2}
         )[0];
}
