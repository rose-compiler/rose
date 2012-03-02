  void fa (int a[], int b[]) { 
    int i=0, j=0; 

    while (i < 100) { 
      if (a[i] < 10) 
        j++;
      else
        a[i]=10;
      i++; 
      if (b[i] < 10)
        j++;
    }
  } 
