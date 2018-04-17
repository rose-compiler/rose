// t0305.cc
// (int)0 works as NULL pointer

typedef long int time_t;
extern time_t time (time_t *__timer)  ;

void f()
{
  time_t tt;
  tt = time((time_t)0 );
}
