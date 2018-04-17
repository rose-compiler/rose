enum AVLockOp
   {
     zero, 
     one,
     two
   };

static int (*ff_lockmgr_cb)(void **mutex, enum AVLockOp op);
