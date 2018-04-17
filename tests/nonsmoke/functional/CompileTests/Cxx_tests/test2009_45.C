/*
This compiles with g++, but not with the ROSE identity translator:

-Jeff
*/

#include <signal.h>
#include <unistd.h>

static struct sigaction sigtermact ;

void SigTermDieGracefully(int sig)
   {
   }

void foo()
   {
     sigtermact.sa_handler = SigTermDieGracefully;
   }



