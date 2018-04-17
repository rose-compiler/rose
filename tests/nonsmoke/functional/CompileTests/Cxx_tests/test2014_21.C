class ostream 
   {
   };

template <typename T>
class Base;

template <typename T>
ostream& operator<< (ostream &, const Base<T>&);

template <typename T>
class Base
{
  public:
 // template<> friend ostream& operator<< <T>(ostream &, const Base<T> &);
    template<typename S> friend ostream& operator<< (ostream &, const Base<S> &);
};

template <typename T>
ostream& operator<< ( ostream &out, const Base<T>& e )
{
   // return out << e->data;
   return out;
}

