/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 ***************************************************************************/

#include <qrose.h>

using namespace qrs;

int main(int argc, char **argv) {

   SgProject *project = frontend(argc, argv);

   // initialize QROSE
   QROSE::init(argc, argv);

   // Let's create two windows
   QRWindow &win1 = *new QRWindow("win1");
   QRWindow &win2 = *new QRWindow("win2");

   // And place a component in each window
   win1 << new QRTreeBox(project);
   win2 << new QRSourceBox(project);

   // uncomment below to get a console report
   // on all elements of win1
   /*win1.setDebugMode(true);*/

   // set position and size for each dialog
   win1.setGeometry(0,0,595,411);
   win2.setGeometry(0,476,694,202);

   // start the main event loop
   return QROSE::exec();
}

