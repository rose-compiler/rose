extern void at_fatal_signal (void (*function) (void));

typedef void (*action_t) (void);

void at_fatal_signal (action_t action)
   {
   }
