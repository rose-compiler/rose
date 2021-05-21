class Root {
public:
   int  a_value;
   int  b_value;
   virtual int Get_Value ();
   Root();              // Default constructor
   Root(int v);         // 1st non-default constructor
   Root(int v, int w);  // 2nd non-default constructor
};
