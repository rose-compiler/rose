#include <qrose.h>

// QROSE is under namespace qrs
using namespace qrs;  

class Events {
public:
   static void clicked(int id) {
      QRWindow &win = *QROSE::getWindow("main");
      switch (id) {
         case 0:
         {
            QRSelect *s1 = win["s1"];
            s1->clear(); // remove all elements
            for (int i = 0; i < 10; i++) {
               s1->addItem(QROSE::format("A-%d", i));
            }
            break;
         }
         
         case 1:
         {
            QRSelect *s2 = win["s2"];
            s2->clear(); // remove all elements
            for (int i = 0; i < 10; i++) {
               s2->addItem(QROSE::format("B-%d", i));
            }
            break;
         }

         case 2: {
            QROSE::exit(0); // ends main-loop event
         }
      }
   }
};


int main(int argc, char *argv[]) {
   // initialize QROSE
   QROSE::init(argc, argv);
   
   // creates a window. Try QROSE::LeftRight and see how it
   // affects the layout
   QRWindow &w = *new QRWindow("main",  QROSE::TopDown);
   w.setTitle("test3: callbacks");

   QRButtons &buttons = w << *new QRButtons;
   // you can also use QRButtons to add checkboxes, radiobuttons...
   buttons.addButtons(3, QRButtons::Normal);
   buttons.setCaption(0, "populate 1");
   buttons.setCaption(1, "populate 2");
   buttons.setCaption(2, "exit");

   // check QRButtons.h and other headers for a list of signals
   QROSE::link(&buttons, SIGNAL(clicked(int)), &Events::clicked);
   
   // the layout of the panel will be LeftRight because the
   // main window orientation is TopDown.
   QRPanel &p = w["panel"] << *new QRPanel(QROSE::UseSplitter, "Selection Boxes");
      p["s1"] << new  QRSelect(QRSelect::Combo, "widget 1");
      p["s2"] << new QRSelect(QRSelect::Check);
   p.setTileSize(60); // set first tile with 60% of the width

   w.setGeometry(0, 0, 500, 200);

   // starts main-event loop
   return QROSE::exec(); 
}

