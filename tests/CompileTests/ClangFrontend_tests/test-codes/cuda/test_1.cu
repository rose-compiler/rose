
int cudaConfigureCall(int,int);

//__global__ void kernel() {
__attribute__((global)) void kernel(float * a) {

}

void caller() {
    float * a;
    kernel<<<1,1>>>(a);
}
