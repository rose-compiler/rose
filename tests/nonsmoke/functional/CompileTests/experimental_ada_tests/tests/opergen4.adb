procedure Opergen4 is

    package P1 is
        type T is tagged
            record
                C1 : Integer;
            end record;

        function "=" (L, R : T) return Boolean;
    end P1;

    package body P1 is
      function "=" (L, R : T) return Boolean is
      begin
          return abs L.C1 = abs R.C1;
      end "=";
    end P1;

    type Cwat is access P1.T'Class;
    type Cwat_Array is array (Positive range <>) of Cwat;

    A : constant Cwat_Array :=
       ( 1 => new P1.T'(C1 => 3),
         2 => new P1.T'(C1 => 4)
       );

begin
  --~ if P1."=" (A (1).all, A (2).all) /= (not P1."/=" (A (1).all, A (2).all)) then
      --~ null;
  --~ end if;

  if P1."/=" (A (1).all, A (2).all) then
      null;
  end if;

  --if "/=" (A (1), A (2)) then
  --    null;
  -- end if;

end Opergen4;
