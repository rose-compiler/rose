#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <sstream>

using namespace std;

class UserType {
public:
  UserType() { cout<<"UserType::constructor"<<endl;}
  ~UserType() { cout<<"UserType::deconstructor {x="<<x<<"}"<<endl;}
  int x;
};

/* Notes:
   size only available if user-defined destructor exists
 */

namespace backstroke {
  template <typename ArrayElementType>
  ArrayElementType* new_array(size_t arraysize) {
    // allocate one additional size_t for size
    size_t* rawMemory=static_cast<size_t*>(::operator new (static_cast<size_t>(arraysize*sizeof(UserType))+1));
    // store size
    cout<<"INFO: rawMemory: "<<rawMemory<<endl;
    *rawMemory=arraysize;
    cout<<"INFO: stored size: "<<*rawMemory<<endl;
    // return array-pointer (excluding size field)
    ArrayElementType* arrayPointer=reinterpret_cast<ArrayElementType*>(rawMemory+1);
    cout<<"INFO: array pointer: "<<arrayPointer<<endl;
    return arrayPointer;
  }

  template <typename ArrayElementType>
  void delete_array(ArrayElementType* arrayPointer) {
    // determine array size (platform specific)
    std::size_t* rawMemory=reinterpret_cast<std::size_t*>(arrayPointer)-1;
    std::size_t arraySize=*rawMemory;
    cout<<"INFO: determined array size: "<<arraySize<<endl;
    if(arrayPointer != 0) {    
      ArrayElementType *p = arrayPointer + arraySize;
      while (p != arrayPointer)        
        (--p)->~UserType();
      ::operator delete [](rawMemory);
    }
  }
}

void printUsage(char* command) {
  cout<<"Usage: "<<command<<" MODE"<<endl;
  cout<<"  where MODE is one of:"<<endl;
  cout<<"    0: C++ new/delete operators"<<endl;
  cout<<"    1: C++ operator[] new/operator[] delete operators"<<endl;
  cout<<"    2: C++ backstroke::array_new/backstroke::array_delete operators"<<endl;
  cout<<"    3: C++ new/backstroke::array_delete operators"<<endl;
}

int main(int argc,char* argv[]) {
  if(argc<2) {
    printUsage(argv[0]);
    return 1;
  }

  istringstream ss(argv[1]);
  int mode=-1;
  if (!(ss >> mode)) {
    cerr << "Invalid number " << argv[1] << '\n';
    return 1;
  }

  const int arraysize=10;
  int* iarray=new int[arraysize];
  ::operator delete[](iarray);
  cout<<"INFO: int array deleted."<<endl;
  cout<<"CHECK 1: PASS"<<endl;

  UserType* uarray=(UserType*)::operator new ((size_t)(arraysize*sizeof(UserType)));
  ::operator delete[](uarray);
  cout<<"INFO: user-type array deleted."<<endl;
  cout<<"CHECK 2: PASS"<<endl;

  // new/allocation
  cout<<"INFO: mode: "<<mode<<endl;
  UserType* a=0;
  switch(mode) {
  case 0: a = new UserType[arraysize];break;
  case 1: a = (UserType*)::operator new ((size_t)(arraysize*sizeof(UserType)));break;
  case 2: a = backstroke::new_array<UserType>(arraysize);break;
  case 3: a = new UserType[arraysize];break;
  }

  cout<<"Array a: "<<a<<endl;
  for(int i=0;i<arraysize;i++)
    a[i].x=i+100;

  // delete/deallocation
  switch(mode) {
  case 0:
    delete[] a;
    cout<<"STATUS: deleted all objects."<<endl;
    break;
  case 1: {
    void* ptr=(void*) a;
    cout<<"Array a: "<<a<<" (void*)"<<endl;
    ::operator delete[](ptr);
    cout<<"STATUS: bs::deallocated all objects (without destructor calls)."<<endl;
    break;
  }
  case 2:
  case 3:
    backstroke::delete_array<UserType>(a);
    cout<<"STATUS: bs::deleted all objects."<<endl;
    break;
  }
  cout<<"CHECK 3: PASS."<<endl;
  cout<<"CHECK: PASS"<<endl;
}
