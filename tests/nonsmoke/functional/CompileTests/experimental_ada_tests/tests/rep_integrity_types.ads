-- with Interfaces;

package Rep_INTEGRITY_types is
    -- type Address is new Interfaces.Unsigned_32;
    type Base is mod 2**31;
    type Address is new Base;
    DataBuffer          : constant  := 16#11#;
    Immediate           : constant  := 16#13#;
    LastBuffer          : constant  := 16#80#;
end Rep_INTEGRITY_types;
