typedef struct huft_t {} huft_t;

typedef struct state_t 
   {
     huft_t *inflate_codes_tl;
   } state_t;

// #define S() state->

// DQ (12/29/2013): Note that this macro is expanded twice...so it is a bug of sorts.
// #define inflate_codes_tl    (S()inflate_codes_tl   )
#define inflate_codes_tl    (state->inflate_codes_tl   )

static void huft_free(huft_t *p)
   {
   }

static void huft_free_all(state_t *state)
   {
  // Macro expanded to include string that includes the name of the macro 
  // (so it is then expanded again in the compilation of the backend generated code).
     huft_free(inflate_codes_tl);
   }

