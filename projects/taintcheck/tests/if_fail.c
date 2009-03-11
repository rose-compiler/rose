// This test shows how our SDG based taint tracking will fail
// due to impossible control flow situations.  Even though
// str can never equal the tainted value "bad", the taint tracker
// thinks str may take on either value since they are two halves
// of an if.  Therefore str is considered tainted even though
// the else branch can never be taken.

#include <stdio.h>

main() {

  char *str = NULL;

  if(1) {
    str = "good";
  } else {
#pragma leek tainted
    str = "bad";
  }
 
#pragma leek trusted
  puts(str);

}
