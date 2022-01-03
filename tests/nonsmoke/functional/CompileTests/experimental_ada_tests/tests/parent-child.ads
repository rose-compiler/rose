package Parent.Child is
  type Domain_Member is private;

  function create return Domain_Member;
  
private
   type Domain_Member is new Integer;
end Parent.Child;
