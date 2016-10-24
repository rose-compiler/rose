int iam(int cool) {
    int verycool = cool;
    if (verycool > 0) {
       verycool = verycool + 1;
    }
    else {
        verycool = verycool - 1;
    }
    return verycool;
}

int main() {
    int x = 0;
    int i = iam(0);
   // if (i == 1) {
   //     return 2;
   // }
   // else {
        return 3;
   // }
}

