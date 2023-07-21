procedure Dispatching is
    package Base is
        type Object is tagged null record;
        
        procedure Init(self : in out Object);
        procedure Register(self : in Object'Class);
    end Base;
    
    package Derived is
        type Coor is new Base.Object with record
          x,y: Integer := 0;
        end record;
        
        -- procedure Init(self : in out Coor);
    end Derived;
    
    package body Base is
        procedure Init(self : in out Object) is
        begin
          null;
            -- Put_Line("Init Object");
        end Init;
        
        procedure Register(self : in Object'Class) is
        begin
          null;
            -- Put_Line("Register Object"); 
        end Register;
    end Base;
    
    -- package body Derived is
    --     procedure Init(self : in out Coor) is
    --     begin
    --         Put_Line("Init Coor");
    --     end Init;
    -- end Derived;

    type PObject is access Base.Object'Class;
    type PCoor is access Derived.Coor'Class;

    obj : PCoor := new Derived.Coor;
    
begin
  Derived.Init(obj.all);
  obj.all.Init;
  
  Base.Register(obj.all);
  obj.all.Register;
end Dispatching;
