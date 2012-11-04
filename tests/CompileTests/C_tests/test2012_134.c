// This is the essential bug in ROSE from the module.c file of the zsh C application.
// This is the same as the issue with params.c from zsh also.

typedef struct hookdef *Hookdef;
typedef struct linknode *LinkNode;
typedef union linkroot *LinkList;

struct linknode
   {
     void *dat;
   };

struct linklist
   {
     LinkNode last;
   };

union linkroot 
   {
     struct linklist list;
   };

typedef int (*Hookfn) (Hookdef, void *);

struct hookdef
   {
     LinkList funcs;
   };


int
runhookdef(Hookdef h, void *d)
   {
     LinkNode p;
     int r;

  // The calls to the function might need the cast to Hookfn.
  // Both locations appear to have the same bug.

  // Unparses to: r = (p -> dat)(h,d);
     r = ((Hookfn)((p)->dat))(h, d);

  // This fails to compile...needs a cast: "(p -> dat)" should be "((Hookfn)(p -> dat))"
  // r = (((p)->dat))(h, d);
     r = ((Hookfn)(p -> dat))(h,d);

  // Unparses to: r = (h -> funcs -> list.last -> dat)(h,d);
     r = ((Hookfn)((((h->funcs)->list.last))->dat))(h, d);

   }


