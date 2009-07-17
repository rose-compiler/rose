#include "rose.h"
#include <qrose.h>

// QROSE is under namespace qrs
using namespace qrs;  

int main(int argc, char *argv[]) {
   // initialize QROSE
   QROSE::init(argc, argv);
   
   // creates a window. Try QROSE::LeftRight and see how it
   // affects the layout
   QRWindow &w = *new QRWindow("main",  QROSE::TopDown);
   w.setTitle("test2: simple layout");

   w["w1"] << new QRSelect(QRSelect::Combo, "widget 1");

   // the layout of the panel will be LeftRight because the
   // main window orientation is TopDown. 
   QRPanel &p = w["panel"] << *new QRPanel(QROSE::UseSplitter);
      p["w2"] << new  QRSelect(QRSelect::Box, "widget 2");
      p["w3"] << new QRSelect(QRSelect::Check);
   p.setTileSize(60); // set first tile with 60% of the width

   w.setGeometry(0, 0, 500, 200);

   // starts main-event loop 
   return QROSE::exec(); 
}

