These are notes to support the testing of the AST File I/O


Static data members in the AST have to be handled carefully
so that they can be merged. These include:
   1) static SgFunctionTypeTable* SgNode::p_globalFunctionTypeTable
   2) static std::map<int, std::string> Sg_File_Info::p_fileidtoname_map
   3) static std::map<std::string, int> Sg_File_Info::p_nametofileid_map

Upon construction all IR nodes have a p_freepointer set to AST_FILE_IO::IS_VALID_POINTER()
(which is a pointer value equal to string::npos; which is set to 0xffffff...ffff).

The delete operator will however set the p_freepointer to a valid pointer not equal to 
AST_FILE_IO::IS_VALID_POINTER() and so when we fail the test for 
   ROSE_ASSERT(p_freepointer == AST_FILE_IO::IS_VALID_POINTER());
it might be because the delete operator has been called.

