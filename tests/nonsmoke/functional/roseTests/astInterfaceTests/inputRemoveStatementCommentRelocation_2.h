// Introduce a variable that would be an error if the associated function is not removed properly.
#ifndef USE_ROSE
int removeThisFunctionToTestAttachedInfoAfterStatement = variable_hidden_in_header_file;
#endif

// Define this outside of the "#ifndef USE_ROSE".
int variable_hidden_in_header_file_2;

