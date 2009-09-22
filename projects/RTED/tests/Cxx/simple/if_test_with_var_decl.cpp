
int main() {

    if( int* x = new int ) {
        // fail to free x, memory leak
    }

    return 0;
}
