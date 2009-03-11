// This is an example of a printer (fake) driver that successfully seperated
// the user's confiential input (their file) from the public request
// for new ink sent over the internet.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Make sure there is ink
int checkInk() {

  char ink[256];
  puts("ink? Y/N");
  gets(ink);
  
  if(strcmp(ink, "N") == 0) {
    return -1; //Error
  }
 
  return 0;
  
}

//Get user input file from network
char* getFileToPrint(char* file) {
  
#pragma leek classified
  file = gets(file);
  
  return file;
}

void printFile(char* file) {

  puts(file);
 
}

void makeInkRequest(char* color) {
  
#pragma leek public
  puts(color);
}

int main() {

  int error;
  char* file = malloc(256);

  file = getFileToPrint(file);

  
  error = checkInk(); 
 

  if(error) {
    if(error == -1) 
      file = "red";
    else if(error == -2) 
      file = "blue";
    else if(error == -3) 
      file = "yellow";
    else 
      file = "black";
  
    makeInkRequest(file);
  } else {
    printFile(file);
  }

  return 0;
}
