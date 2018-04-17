/* Example minimalOstream.C from Andreas */

/*
   There is two problems displayed here. One when constructing and 
   initializing the ofstream object, and the other while using the 
   operator << from class String.
*/
#include <iostream.h>
#include <fstream.h>
#include <cstdlib>
static char* bt_nil_str = "";

class String
{
  struct StringRep
    {
      char* s;         // pointer to data
      int   refcount;  // reference counter
      // int sz; whould be convenient for avoiding errors, check in ok

      StringRep ()
        { refcount = 1; s = NULL; }
    } ;

  StringRep*  rep;
  static char* output_format_of_empty_string;

public:

  String ()                             // String x;
{
  rep = new StringRep;
  rep->s = bt_nil_str;
}

 ~String (){};
  bool empty () const;
  const char* c_str () const;
        char* c_str ();

  friend  ostream& operator <<  (ostream& out, const String& s);
};

const char* String:: c_str () const
{
  if (rep->s == NULL)
     exit(1);
  return rep->s;
}

/*inline*/ bool String:: empty () const
{
  if (rep->s == NULL) {
    return true;  // allow continued execution
  } else
    return false;
}
ostream& operator << (ostream& out, const String& s)
{
  if (s.empty())
    out << " ";  // alternative: "''" (can be read in again)
  else
    out << s.c_str();

  return out;
}

int main(){
 char *filename = "test.rose";
 ofstream file (filename, ios::app);
 String text;
 file << text << ((("\n\n")));
 return 0;
}
