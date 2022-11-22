PROCEDURE gen_overloaded_funcs IS
     GENERIC
          TYPE T1 IS (<>);
          TYPE T2 IS (<>);
     PACKAGE G_PACK IS
          PROCEDURE HF (P1 : T1);
          PROCEDURE HF (P2 : T2);
     END G_PACK;

     PACKAGE BODY G_PACK IS
          PROCEDURE HF (P1 : T1) IS
          BEGIN
               null;
          END HF;

          PROCEDURE HF (P2 : T2) IS
          BEGIN
               null;
          END HF;
     END G_PACK;
BEGIN
  null;
END gen_overloaded_funcs;
