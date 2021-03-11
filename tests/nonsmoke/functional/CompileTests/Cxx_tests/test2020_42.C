// RC-61:  Tristan

    struct _direntry {};

     

    void foo() {

      struct { struct _direntry * next; } anchor;

      for (; (struct _direntry *)( anchor.next ); ) {}

    }

