class ostream;
extern ostream cout;

class IString {
public:
  IString           ( int  aShort );

friend ostream
 &operator <<       ( ostream       &aStream,
                      const IString &aString );
IString
  operator +       ( const IString &aString ) const;

IString
 &rightJustify      ( unsigned length,
                      char     padCharacter = ' ' ) ;
};

void foo() {
    int a = 1;
    cout << IString(IString(a) + IString(a)).rightJustify(1);
}
