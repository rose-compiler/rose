// This test code is demonstrating a problem with copy and delete for ASTs.
// The astCopy mechanism shares the Sg_File_Info objects when it builds a
// copy and the astDelete will delete the Sg_File_Info object that is being
// shared leaving a dangling pointer in the original AST.  Since the Sg_File_Info
// destructor is called, it's values are set to default values and so the 
// new physical_file_id is set to that of the NULL_FILE value and all of the
// boolean flags are set to false; this triggers an error in new code added
// to the ASTConsistancy tests that asserts that the physical file id is
// zero or positive when the flags are all false.


// #pragma rose_foo
int x;

