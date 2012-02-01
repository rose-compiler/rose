
void f() {
    int i, j;
    int res = 0;

  start1:
    i = 0;

  loop1:
    if (i >= 10) goto end1;

    {
      start2:
        j = 0;

      loop2:
        if (j >= 10) goto end2;

        {
            res++;
        }

        j++;
        goto loop2;

      end2:
        ;
    }

    i++;
    goto loop1;

  end1:
    ;
}

