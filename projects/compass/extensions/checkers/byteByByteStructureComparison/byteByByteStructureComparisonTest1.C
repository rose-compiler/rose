
//Your test file code goes here.

typedef unsigned long size_t;
int memcmp(const void *s1, const void *s2, size_t n);

struct my_buf {
  size_t size;
  char buffer[50];
};

typedef struct my_buf new_buf;

unsigned int buf_compare(struct my_buf *s1, struct my_buf *s2) {
  if (!memcmp(s1, s2, sizeof(struct my_buf))) {
    return 1;
  }
  return 0;
}

int foo(new_buf* a, new_buf* b)
{
	if(!memcmp(a, b, sizeof(new_buf))) {
		return 1;
	}
	return 0;
}
	
int bar()
{
	new_buf a;
	new_buf b;

	if(!memcmp(&a, &b, sizeof(new_buf))) {
		return 1;
	}
	return 0;
}	

