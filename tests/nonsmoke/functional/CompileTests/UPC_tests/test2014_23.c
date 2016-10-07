shared void *shmalloc(unsigned nbytes);
void shfree(shared void *ptr);
void free(void *ptr);

void foo(void){
	shared int *sptr;
	sptr = shmalloc(57);
	free(sptr);   /* Should generate an error. */
}
