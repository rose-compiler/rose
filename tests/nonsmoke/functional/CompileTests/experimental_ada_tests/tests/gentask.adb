
PACKAGE BODY gentask IS
     TASK SHARE IS
          ENTRY init (VALUE : IN VALUE_TYPE);
     END SHARE;

     TASK BODY SHARE IS SEPARATE;

     PROCEDURE init (VALUE : IN VALUE_TYPE) IS
     BEGIN
          SHARE.init (VALUE);
     END init;
END gentask;
