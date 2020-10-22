package Exception_Rename is
  MyException : exception;
  MyOtherException : exception renames MyException;
end Exception_Rename;
