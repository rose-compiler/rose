char* strcpy(char*, char*);
char* getenv(char*);

int main(int argc, char** argv) {
  /* ... */
  char buff[256];
  strcpy(buff, getenv("EDITOR"));
  /* ... */
  return 0;
}

