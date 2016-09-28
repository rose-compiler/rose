class __pthread_cleanup_class
   {
     void (*__cancel_routine) (void *);
     void *__cancel_arg;
     int __do_it;
     int __cancel_type;

     public:
          __pthread_cleanup_class (void (*__fct) (void *), void *__arg) : __cancel_routine (__fct), __cancel_arg (__arg), __do_it (1) { }

         void foo () { __cancel_routine (__cancel_arg); }

       // ~__pthread_cleanup_class () { if (__do_it) __cancel_routine (__cancel_arg); }

       // void __setdoit (int __newval) { __do_it = __newval; }
       // void __defer () { pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, &__cancel_type); }
       // void __restore () const { pthread_setcanceltype (__cancel_type, 0); }
   };

