class UserType {
public:
  UserType() {}
  ~UserType() {}
  int x;
};

void event() {
  // ==> UserType* a=backstroke::new_array<UserType>(10);
  UserType* a=new UserType[10];
  for(int i=0;i<10;i++) {
    a[i].x=i;
  }
  // ==> UserType* a=backstroke::delete_array<UserType>();
  delete[] a;
}

