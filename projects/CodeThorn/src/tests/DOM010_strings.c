
int main() {
  char* data;
  char dataBuffer[10];
  data=dataBuffer;
  data[0]= '\0';
  char* stringPtr = "0123456789";
  char source[11] = "0123456789";
  //char source[11] = {'0','1',0};
  int i;
  int sourceLen=10;
  for (i = 0; i < sourceLen + 1; i++)
    {
      data[i] = source[i];
    }
  return 0;
}
