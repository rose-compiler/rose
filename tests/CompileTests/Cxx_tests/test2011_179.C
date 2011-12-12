typedef long streamoff;

template<typename _StateT>
class fpos
   {
     private:
         streamoff _M_off;
         _StateT   _M_state;

     public:
         fpos() : _M_off(0), _M_state() { }
   };

