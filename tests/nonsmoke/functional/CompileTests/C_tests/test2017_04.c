#include <stdio.h>

void sendMessage()
   {
     printf ("Inside of sendMessage() \n");
   }

void sendMessage_forward() 
   {
     printf ("Inside of sendMessage_forward() \n");
     return sendMessage();
   }

int main()
   {
     sendMessage_forward();
     return 0;
   }
