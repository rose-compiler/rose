// This causes infinit recursion in ROSE...

// void foo_b (int __mode , struct X* __list[__restrict]);
// void foo_b (int __mode , struct X* __list[]);

void foo (struct X abc);
