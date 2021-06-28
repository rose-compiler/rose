* This directory contains rose-compiler plugins

  - acuity
    Plugin to analyze class hierarchies, class casts, and virtual functions

    sample invocation:
    $ROSE_ROOT/rose-compiler -I$ROSE_ROOT/include -I$ROSE_ROOT/include/rose -rose:plugin_lib libacuity.so -rose:plugin_lib libacuity.so -rose:plugin_action Acuity -rose:plugin_arg_Acuity --dot=simple.dot  -rose:plugin_arg_Acuity --layout=simple_class_layout.txt -rose:plugin_arg_Acuity --virtual_functions=simple_virtual_functions.txt -rose:skipfinalCompileStep -rose:skip_unparse SRCFILE.cc

    - arguments to the plugin must be prefixed by -rose:plugin_lib libacuity.so
    - arguments include:
      --dot=OUTPUT.dot
      --layout=LAYOUT_FILE.txt
      --virtual_functions=VIRTUAL_FUNCTION_FILE.txt
      --original_name=NUM  encodes class and function names. NUM is the number of characters the remain from the original name.


    
