// Bug reported by David Svoboda (at CERT)

#include <stdio.h>
#include <string.h>

const int MAX_USER = 8;
const int MAX_PASS = 8;

int do_auth(void) {
  char username[MAX_USER];
  char password[MAX_PASS];

  puts("Please enter your username: ");
  fgets(username, MAX_USER, stdin);
  puts("Please enter your password: ");
  fgets(password, MAX_PASS, stdin);

  if (!strcmp(username, "user") && !strcmp(password, "password")) {
    return 0;
  }
  return -1;
}

void log_error(char *msg) {
  char *log;
  char buffer[24];

  sprintf(buffer, "Error: %s", log);
  printf("%s\n", buffer);
}

int main(void) {
  if (do_auth() == -1) {
    log_error("Unable to login");
  }
  return 0;
}
