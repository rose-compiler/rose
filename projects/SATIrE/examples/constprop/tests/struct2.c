
#define SHOW_ERROR

typedef unsigned char uint8;

typedef struct {
  uint8 u;
  uint8 v;
} Num_t;

Num_t const num={
  4,
  2
};

typedef struct {
  int x;
  void* ptr;
}
  Base_t;

#ifdef SHOW_ERROR

Base_t const base={
  1,
  (void*)&num.v
};

#else

Base_t const base={
  1,
  (void*)&num.u // first field does not cause error
};

#endif


/* ------------------------------------- */
/* same error other general syntax */
/* ------------------------------------- */

/*


struct Num{
    int u;
    int v;
};

const struct Num n={
  4,
  2
};

struct Base {
    int n;
    void* ptr;
};

const struct Base b={
  1,
  (void*)&n.v
};


*/
