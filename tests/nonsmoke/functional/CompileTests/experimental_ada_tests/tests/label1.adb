procedure Label1 is
  TYPE A IS (RED);
BEGIN
  DECLARE
    D : A;
  BEGIN
    <<RED>>     -- label <<RED>> overloads enum value RED
    D := Label1.RED;
  END;
end Label1;
