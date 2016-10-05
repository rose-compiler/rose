/*
In case you didn't know about this already, the following code:
void f() {while (int x = 5) ++x;}
is parsed correctly, but produces incorrect code (a semicolon after the variable declaration) upon unparsing.
-- Jeremiah Willcock 
*/

void f() {while (int x = 5) ++x;}
