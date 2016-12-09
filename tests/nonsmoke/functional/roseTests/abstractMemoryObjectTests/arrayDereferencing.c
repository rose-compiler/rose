// dereference of arrays
// subscripts of pointers
#include <assert.h> 
int array[10];
int array2[10][3];

int main()
{
  int i, j;
  for (i=0; i<10; i++)
  {
    array[i] = i; 
    for (j=0; j<3; j++)
    {
      array2[i][j] = i+j;
    }
  }


  int deref_1 = *array;
  int y = array[0]; 

  int* deref_2; 
  deref_2 = * array2; 

  for (j=0; j<3; j++)
  {
    assert (deref_2[j] == j) ;
  }

  return 0; 
}
