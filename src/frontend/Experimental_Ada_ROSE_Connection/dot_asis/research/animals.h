class Carnivore {
public:
  virtual int Number_Of_Teeth () = 0;
};

class Domestic {
public:
  virtual void Set_Owner (char* Name) = 0;
};

class Animal {
public:
  int Age_Count;
  virtual void Set_Age (int New_Age);
};

class Dog : Animal, Carnivore, Domestic {
public:
  int Tooth_Count;
  char *Owner;

  virtual int  Number_Of_Teeth ();
  virtual void Set_Owner (char* Name);

  Dog();
};
