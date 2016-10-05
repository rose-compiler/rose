// gb0004.c
// missing semicolon in structure member declaration

// rejected by icc, so Elsa will continue to reject

struct category {
  struct cat_item   {
  }
};
