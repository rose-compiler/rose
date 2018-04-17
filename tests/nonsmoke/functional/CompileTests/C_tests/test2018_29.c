typedef void (*action_t) (void);




typedef struct
{
  volatile action_t action;
}
actions_entry_t;


static actions_entry_t static_actions[32];
static actions_entry_t * volatile actions = static_actions;
// static sig_atomic_t volatile actions_count = 0;
// static size_t actions_allocated = (sizeof(static_actions) / sizeof(static_actions[0]));




// static struct sigaction saved_sigactions[64];



static inline void
uninstall_handlers (void)
{
}



static void
fatal_signal_handler (int sig)
{
}



static inline void
install_handlers (void)
{
}



void
at_fatal_signal (action_t action)
   {
   }
