
SEPARATE (gentask)
TASK BODY SHARE IS
     VARIABLE : VALUE_TYPE;
BEGIN
     LOOP
          SELECT
               ACCEPT INIT (VALUE : IN VALUE_TYPE) DO
                    VARIABLE := VALUE;
               END INIT;
          OR
               TERMINATE;
          END SELECT;
     END LOOP;
END SHARE;
