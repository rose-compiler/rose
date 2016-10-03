// from the kernel; as Simon points out, this is the trick for finding
// the offset at static time of a struct member

typedef int size_t;

struct B {
  int q;
};

struct scsi_cmnd {
  struct B b;
};

int main() {
  switch(3) {
    // this was originally in a different context requiring const
    // evaluation: an array size.  I just wanted to simplfy the
    // expression
  case ((size_t) &((struct scsi_cmnd *)0)->b.q) :
    break;
  }
};
