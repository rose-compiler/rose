struct list_head 
   {
     struct list_head *next, *prev;
   };

#define LIST_HEAD_INIT(name) { &(name), &(name) }

struct notifier_block 
   {
//   int (*notifier_call)(struct notifier_block *, unsigned long, void *);
     struct list_head chain;
//   int priority;
   };

struct notifier_head 
   {
     struct notifier_block head;
   };

#define NOTIFIER_INIT(name) { .head.chain = LIST_HEAD_INIT(name.head.chain) }

#define NOTIFIER_HEAD(name) struct notifier_head name = NOTIFIER_INIT(name)

static NOTIFIER_HEAD(cpu_chain);

// struct notifier_head name = { &(name), &(name) };
