WITH TEXT_IO; USE TEXT_IO;

PROCEDURE UseGenericInstance IS

     PACKAGE INT IS NEW INTEGER_IO(INTEGER);
     USE INT;
     FILE : FILE_TYPE;
     ITEM : INTEGER := 0;

BEGIN

          GET (ITEM => ITEM, WIDTH => 0);
          PUT (FILE => FILE, ITEM => ITEM, WIDTH => 4, BASE => 4);

END UseGenericInstance;
