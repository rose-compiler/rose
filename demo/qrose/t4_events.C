#include "rose.h"
#include <qrose.h>
#include <icons.h>

// QROSE is under namespace qrs
using namespace qrs;  

void exitHandler(int) {
   QROSE::exit(0);
}

void cellDblClick(int col, int row) {
   QROSE::msgBox("You pressed cell [col=%d, row=%d]", col, row);
}

void selIcon(int id) {
   QRWindow &w = *QROSE::getWindow("main");

   QRTable *table = w["table"];
   switch (id) {
      case 0: table->clearPicture(QRTable::All, QRTable::All); break;
      case 1: table->setPicture(iconClock, QRTable::All, QRTable::All); break;
      case 2: table->setPicture(iconHouse, QRTable::All, QRTable::All); break;
      case 3: table->setPicture(iconGear, QRTable::All, QRTable::All); break;
   }
}

void selBackground(int id) {
   QRWindow &w = *QROSE::getWindow("main");

   QRTable *table = w["table"];
   switch (id) {
      case 0: table->setBgColor(QColor("white"), 0, QRTable::All); break;
      case 1: table->setBgColor(QColor(255,200,200), 0, QRTable::All); break;
      case 2: table->setBgColor(QColor(200,255,200), 0, QRTable::All); break;
      case 3: table->setBgColor(QColor(200,200,255), 0, QRTable::All); break;
   }
}   

int main(int argc, char *argv[]) {
   // initialize QROSE
   QROSE::init(argc, argv);
   
   // creates a window. Try QROSE::LeftRight and see how it
   // affects the layout
   QRWindow &w = *new QRWindow("main",  QROSE::TopDown);
   w.setTitle("test4: table");

   // creates a table
   QRTable &t = w["table"] << *new QRTable;
   QROSE::link(&t, SIGNAL(dblClicked(int,int)), &cellDblClick);
   // add 10 rows   add 2 columns
   t.addRows(10);   t.addCols(2);

   // set "H" to all header columns 
   t.setText("H", QRTable::All, QRTable::Header);
   // set "V" to all header rows
   t.setText("V", QRTable::Header, QRTable::All);

   // add content to cells
   for (int i = 0; i < t.columnCount(); i++) {
      for (int j = 0; j < t.rowCount(); j++) {
         std::string cellText = QROSE::format("(%d, %d)", i, j);
         t.setText(cellText, i, j);
         if ((j % 2) == 0)
            t.setHAlignment(false, false, i, j); // center
         else
            t.setHAlignment(true, false, i, j); // left-aligned
         
      }
   }

   
   // radio-buttons - note that QRButtons is a specialized panel
   QRButtons &backsel = w["background selection"] << *new QRButtons("background selection");

   backsel.addButtons(4, QRButtons::Radio);
   backsel.setCaption(0, "white");
   backsel.setCaption(1, "red");
   backsel.setCaption(2, "green");
   backsel.setCaption(3, "blue");
   QROSE::link(&backsel, SIGNAL(clicked(int)), &selBackground);

   // a combo-box
   QRSelect &iconSel = w << *new QRSelect(QRSelect::Combo, "icon selection");
   iconSel.addItem("<none>");
   iconSel.addItem("clock");
   iconSel.addItem("house");
   iconSel.addItem("gear");
   QROSE::link(&iconSel, SIGNAL(selected(int)), &selIcon);
    
   // horizontal line 
   w << new QRSeparator;

   // add a button to exit
   QRButtons &bexit = w["exit"] << *new QRButtons;
   // if you comment this line what happens?
   bexit << new QRSpaceWidget; // add space to the left
   bexit.addButtons(1, QRButtons::Normal); // add one button to the center
   // and what if you comment this one?
   bexit << new QRSpaceWidget; // add space to the right
   bexit.setCaption(0, "Exit");
   QROSE::link(&bexit, SIGNAL(clicked(int)), &exitHandler);

   w.setGeometry(0, 0, 260, 600);

   // starts main-event loop
   return QROSE::exec(); 
}

