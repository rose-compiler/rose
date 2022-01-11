package acess_definition is
Hello       : aliased String := "Hi, world.";
Hello_Msg : constant access String := Hello'Access; 
end acess_definition;
