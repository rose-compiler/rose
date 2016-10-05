// Moved this code from conflict with milind's copy of test2013_249.C

int main(int argc, char** argv) {
    int j = 0;
    for(int i = 0 ; i < 100; i+=2)
        j++; // must be blockified
    for(; ;)
        j++; // must be blockified

    while(1)
        j++; // must be blockified

    do
        j--; // must be blockified
    while (1);
    
    try {
      1/j; // must be blockified
    }
    catch(...) {
       j++; // must be blockified
    }
    
    switch (j) {
        case 1:
            // must be blockified
            j++;
        case 2:
            // must be blockified
            j++;
            break;
        default:
            // must be blockified
            j++;
            break;
    }
    
    switch (j) // must be blockified
            default: // must be blockified
            break;

    switch (j)  // must be blockified
        case 1: {
            break;
        }
}

