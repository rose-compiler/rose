// ! scalar to array dependences
void foo()
{
  int i;
  int a[100];

  int temp; 
  for (i=0;i<99;i++)
  {
    temp = a [i];
    a[i+1]+=temp + 1;
  }  
}  
