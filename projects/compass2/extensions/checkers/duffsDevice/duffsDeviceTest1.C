// Duff's Device in its almost original form.
void send(int *to, int *from, int count)
{
  int n = (count+7) / 8;
  switch(count%8) {
  case 0: do { *to++ = *from++;
  case 7:      *to++ = *from++;
  case 6:      *to++ = *from++;
  case 5:      *to++ = *from++;
  case 4:      *to++ = *from++;
  case 3:      *to++ = *from++;
  case 2:      *to++ = *from++;
  case 1:      *to++ = *from++;
             } while (--n>0);
  }
}

// An equivalent function without optimization.
void send2(int *to, int *from, int count)
{
    for (int i = 0; i < count; i++)
        *to++ = *from++;
}
