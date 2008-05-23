// Definitions in here vary across platforms
// #include <stdio.h>

void printf(const char*);

void testCFG()
{
  int i = 5;
  int j = 100;
  while (i < j) {
    printf("*** i < j ***");
    i++;
  }

  do {
    printf("***i < 2j***");
    i++;
    if (i > 300)
        break;
    else if (i < 100)
         continue;

    if (i == 100)
      goto exit;
  } while (i < 2 * j);   
  
  for ( i = 500 ; i > j; --i) {
  switch (i) {
  case 200: 
       printf("init");
       break;
  case 99:
       printf(" tmp");
  case 2 : printf(" error");
               return;
  case 120:
          continue;
  case 100 : printf(" ending"); 
              i = i+1;
              break;
  case 98 : goto exit;
  case 101:
            i = i-1;
            break;
  }
  if (i == 100)
    j--;
  else 
    j++;
  if (i == 101)
    break;
 }
exit: return;
}
