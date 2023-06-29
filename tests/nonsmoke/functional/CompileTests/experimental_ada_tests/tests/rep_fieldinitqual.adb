

procedure Rep_FieldInitQual is
  package Types is
    type Colors is (red, green, blue);
    type Intensity is range 0..255;
    
    type Image is record
           rating : Integer;
         end record;
    
    c_red   : constant Colors := red;
    c_green : constant Colors := green;
    c_blue  : constant Colors := blue;
  end Types;
  
  type ColorArray is array (Types.Colors) of Types.Intensity;
  
  blackPixel : ColorArray := (Types.c_red => 0, Types.c_green => 0, Types.c_blue => 0);
  img        : Types.Image := ( rating => 0 );
begin
  null;
end Rep_FieldInitQual;
