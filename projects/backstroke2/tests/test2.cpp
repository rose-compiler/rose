class UserType {
public:
  UserType() {}
  ~UserType() {}
  int x;
};

void event() {
  // forward code: UserType* a=Backstroke::new_array<UserType>(10);
  UserType* a=new UserType[10];

  for(int i=0;i<10;i++) {
    a[i].x=i;
  }
  //forward code: Backstroke::delete_array<UserType>(a);
  delete[] a;
}

UserType* foo(UserType& b) {
  b.x=5;
  UserType* a;
  a=&b;
  return a;
}
