// This is a test code that is infinitely recursive and which can be found in the CentOS version of exim C application.
// A slightly different version of this has been identified and is in test2012_04.c

// extern int recvmmsg (int __fd, struct mmsghdr *__vmessages, unsigned int __vlen, int __flags, __const struct timespec *__tmo);
// int recvmmsg (int __fd, struct mmsghdr *__vmessages );
// void recvmmsg (struct mmsghdr *x );

void foo (struct X *abc);

