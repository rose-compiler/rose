
for random-number functions: generate call of reverse functino

 tw_rand_exponential(lp->rng, mean)

foreach rand_* function call generate a     tw_rand_reverse_unif(lp->rng); call in the reverse code.

put pointer to reverse function on the reverse-stack.

in general:
for functions with side-effects (and existing reverse function) store the pointer to the reverse function on the reverse-stack.



two features:
modify state
modify ross-ints for restoring destroyed information
