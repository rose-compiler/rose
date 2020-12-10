template<typename To, typename From> To convert(From f);
 
void g(double d) 
{
    int i = convert<int>(d); // calls convert<int,double>(double)
    char c = convert<char>(d); // calls convert<char,double>(double)
    int(*ptr)(float) = convert; // instantiates convert<int, float>(float)
}


