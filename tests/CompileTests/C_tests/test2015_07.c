struct list_head
   {
  // struct list_head *next, *prev;
     struct list_head *next;
   };

struct list_head global_list_head;

struct notifier_block
   {
     struct list_head chain;
   };

struct notifier_head
   {
     struct notifier_block head;
   };

// Bug in designated initialized (when nested)
// Unparsed as: static struct notifier_head cpu_chain = {.head = .chain = {(&cpu_chain . head . chain), (&cpu_chain . head . chain)}};
// static struct notifier_head cpu_chain = { .head.chain = { &(cpu_chain.head.chain), &(cpu_chain.head.chain) } };
// static struct notifier_head cpu_chain = { .head.chain = { &(cpu_chain.head.chain) } };
static struct notifier_head cpu_chain = { .head.chain = { &global_list_head } };
