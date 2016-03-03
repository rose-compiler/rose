// struct cuda_traversal {};

// This is what we want users to write. And we key off of the use of the __device__ used to annotate the loop body.
// This code could use the __attribute((__device__)) if the simpler __device__ keyword does not compile.
// Note that the attribute is associated with the type.
template <typename LOOP_BODY>
// void forall(cuda_traversal, int length, LOOP_BODY __device__ loop_body)
// void forall(cuda_traversal, int length, LOOP_BODY __attribute((device)) loop_body)
void forall(LOOP_BODY __attribute((device)) loop_body)
   {
   }

int main(int argc, char *argv[])
{
   int* value = 0L;

// This is what we want users to write.
// forall(cuda_traversal(), 10, [=](int i) {
   forall([=](int i) {
       value[i] = i;
   } ) ;

   return 0 ;
}

