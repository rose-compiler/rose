
foreach tw_rand_* function call 
        generate tw_rand_reverse_unif(lp->rng); in the reverse code

TODO: put pointer to reverse function on the restore-stack.

in general:
for functions with side-effects (and existing reverse function) store the pointer to the reverse function on the reverse-stack.

ROSS: two memory modifying operations:
modify state
modify ross-ints for restoring destroyed information
