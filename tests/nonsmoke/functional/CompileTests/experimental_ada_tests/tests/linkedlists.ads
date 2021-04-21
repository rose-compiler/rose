package LinkedLists is

type IntElem;
type IntElemPtr is access IntElem;
type IntElem is record
       val : Integer;
       nxt : IntElemPtr;
     end record;

type Obj is tagged;
type ObjPtr is access Obj;
type Obj is tagged record
       nxt : ObjPtr;
     end record;

end LinkedLists;
