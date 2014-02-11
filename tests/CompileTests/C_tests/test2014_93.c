// This is from Busy-box.
// the call at the end to build the unstripped version requires this attribute to be represented properly.
union node {};
   
// static void evaltree(union node *, int);
static void evaltree(union node *n, int flags);

static void
evaltree(union node *n, int flags)
   {
   }

// Rose unparses this as: extern void evaltreenr(union node *,int ) __attribute__((noreturn)) __attribute__((alias(evaltree)));
void evaltreenr(union node *, int) __attribute__ ((alias("evaltree"),__noreturn__));

int main()
   {
     union node *node_ptr;
     evaltree(node_ptr,2);

  // Note that this can be called and this program linked without and defining declaration for "void evaltreenr(union node *, int)".
     evaltreenr(node_ptr,2);

     return 0;
   }
