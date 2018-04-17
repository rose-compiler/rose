template<typename _Res> 
void foobar (_Res);

template<typename _Res, typename _Class> 
void foobar(void (_Class::*)(_Res))
   {
     int x;
   }

