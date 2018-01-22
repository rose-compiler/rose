/*
Test handling of ++ operator
Contributed by Jeff Keasler
By Liao, 10/28/2009
*/

int HighPassFilter(int *input,int inLen,int *output,int threshold)
{
  int outLen = 0;
  int i;
  for (i = 0; i <= inLen - 1; i += 1) {
    if (input[i] > threshold) {
      output[outLen++] = input[i];
    }
  }
  return outLen;
}
