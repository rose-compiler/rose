int HighPassFilter(int *input, int inLen, int *output, int threshold)
{
   int outLen = 0 ;
   int i ;
   for (i=0; i<inLen; ++i) {
      if (input[i] > threshold) {
        output[outLen++] = input[i] ;
      }
   }
   return outLen ;
}

