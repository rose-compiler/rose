/*
This program causes a crash in either identityTranslator or pdfGenerator.
It should probably be added as a test case.

                                                        Jeremiah Willcock
*/

void foo()
   {
     int x = 1;
     +x;
   }

