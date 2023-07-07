procedure opergen9 is
  package Movies is
     type Rating is new Integer range -3..17; 
  end Movies;

  a : Movies.Rating := Movies."-"(Right => 5, Left => 2);
  b : Movies.Rating := Movies."abs"(Right => a);
begin
  null;
end opergen9;
