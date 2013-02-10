void foo()
   {
     int ngx_process;
     int signo;

     switch (ngx_process)
        {
//        case 1:
#if 1
          case 0:
               switch (signo) 
                  {
                    case 17:
                         break;
                  }
               break;
#endif
#if 1
//        case 3:
          case 4:
               switch (signo)
                  {
                  }
               break;
#endif
        }
   }
