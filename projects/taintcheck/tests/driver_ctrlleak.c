// This is an interesting example of how confidential data may accidentally affect
// public data.  This was actually my first attemp at writing a printer driver
// example.  The problem I had was that the "filename" determine if there is
// and ink error or not, thereby determining what kind of message is sent to
// the public ink request.
// This means in some sense the confidential file name has leaked to the public.


#include <stdio.h>
#include <stdlib.h>


//Get user input file from network
#pragma leek classified
char* getFileToPrint(char* file) {
  
  file = gets(file);
  
  return file;
}

int printFile(char* file) {

  if(file == "ERROR") {
    return -1; //Error
  }
  puts(file);
  return 0;
}

void makeInkRequest(char* color) {
  
#pragma leek public
  puts(color);
}

int main() {

  int error;
  char* file = malloc(256);

  file = getFileToPrint(file);
 
  error = printFile(file);

  if(error) {
    if(error == -1) 
      file = "red";
    else
      file = "black";
  
    makeInkRequest(file);
  }

  return 0;
}
