#include <signal.h>
void ignoreBrokenPipe(){
   struct sigaction act;
  act.sa_handler=SIG_IGN;
}
