
_Alignas(int) char buf[100];
int buf_alignment = _Alignof(int);

void foo()
   {
     _Alignas(int) char local_buf[100];
     int local_buf_alignment = _Alignof(int);
   }
