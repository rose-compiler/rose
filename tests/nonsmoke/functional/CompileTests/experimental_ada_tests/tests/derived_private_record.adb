procedure derived_private_record is

  type R1 is null record;

  package X is
    type R2 is private;
  private
    type R2 is new R1;
  end X;

begin
  null;
end derived_private_record;
