struct list_head
   {
     struct list_head *next, *prev;
   };



struct notifier_block
   {
     int (*notifier_call)(struct notifier_block *, unsigned long, void *);
     struct list_head chain;
     int priority;
   };

struct notifier_head
   {
     struct notifier_block head;
   };





static struct notifier_head cpu_chain = { .head.chain = { &(cpu_chain.head.chain), &(cpu_chain.head.chain) } };
