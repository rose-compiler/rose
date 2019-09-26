#include <AMReX.H>
#include <AMReX_Gpu.H>
#include <memory>

using namespace amrex;

struct MyFabData
   {
     double* p = nullptr;
     long n = 0;
   };

struct MyFab : public MyFabData
   {
     MyFab () = default;

     void unsafe_print ()
        {
          amrex::launch_global<<<1,1>>>(
               [=] AMREX_GPU_HOST_DEVICE () {
               printf("unsafe_print: Number of Points is %ld\n", n);
             });
        }

void safe_print ()
   {
     const auto local_n = n;
     amrex::launch_global<<<1,1>>>(
          [=] AMREX_GPU_HOST_DEVICE () {
          printf("safe_print: Number of Points is %ld\n", local_n);
        });
   }

};

int main (int argc, char* argv[])
   {
     amrex::Initialize(argc, argv);
        {
          MyFab fab;
          fab.safe_print();
          fab.unsafe_print();
        }

     amrex::Finalize();
   }

