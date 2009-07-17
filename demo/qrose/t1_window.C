#include "rose.h"
#include <qrose.h>

// QROSE is under namespace qrs
using namespace qrs;  

int main(int argc, char *argv[]) {
   // initialize QROSE
   QROSE::init(argc, argv);
   
   // creates a window
   QRWindow *win = new QRWindow("main");
   win->setTitle("test1: Hello World!");
   win->setGeometry(0, 0, 500, 200);

   // starts main-event loop 
   return QROSE::exec(); 
}

