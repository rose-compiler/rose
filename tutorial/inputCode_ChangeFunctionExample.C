typedef int ssize_t;
typedef int size_t;

ssize_t send(int sock, const void *msg, size_t len, int flags);

int main() {
   const char *msg = "hello";
   ssize_t i = send(10, msg, 30, 12);
   return i;
}
