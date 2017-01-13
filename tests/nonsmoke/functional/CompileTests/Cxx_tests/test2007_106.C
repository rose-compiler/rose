class nsAString 
  {
    public:
      __attribute__ ((visibility ("default"))) void __attribute__ ((regparm (3), stdcall)) Assign( const nsAString& );
    };
class nsDependentSubstring : public nsAString
  {
  };

class nsString : public nsAString
  {
       public:
      explicit
      nsString( const nsAString& readable )
        {
          Assign(readable);
        }
  };

  void
MakeMutable()
{
  nsDependentSubstring mString;
  nsString temp(mString); 
  mString.Assign(temp);   
}

