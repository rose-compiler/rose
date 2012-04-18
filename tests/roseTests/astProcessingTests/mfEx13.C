int ss(int q) {
    q = q + 1;
    return q;
}


int main() {
    if (ss(2) == 3) {
        return 3;
    }
    else {
        return ss(2);
   }
}
