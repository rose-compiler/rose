class ostream 
   {
   };

template <typename T>
class Base;

template <typename T>
ostream& operator<< (ostream &, const int&);

template <typename T>
class Base
{
  public:
 // template<> friend ostream& operator<< <T>(ostream &, const int &);
    friend ostream& operator<< <>(ostream &, const int &);
};

template <typename T>
ostream& operator<< ( ostream &out, const int& e )
{
   // return out << e->data;
   return out;
}

