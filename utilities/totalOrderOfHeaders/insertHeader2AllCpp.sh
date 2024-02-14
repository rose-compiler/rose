#!/bin/bash
# a bash script to find all *.cpp from current and its nested paths
# then prepend #include "total_header.hh" to each of *.cpp files

# It also has a undo mode to remove the prepended #include header_file

# Usage function to display help text
usage() {
    echo "This script prepends a specified header file to all .cpp files found in current path (including nested folders)"
    echo "It also has a undo mode to remove the inserted #include directive."
    echo "Usage: $0 [--undo] total_header_file_name"
    echo "Options:"
    echo "  --undo           Undo the previous inclusion of the header in .cpp files."
    exit 1
}


# Check for the presence of the --undo option
UNDO_MODE=0
if [ "$1" == "--undo" ]; then
    UNDO_MODE=1
    shift # Shift the arguments to the left, removing the first argument
fi


# Check if a header filename was provided
if [ $# -eq 0 ]
then
  usage
fi

HEADER_FILE=$1

# Find all .cpp files and process them
find . -type f -name "*.cpp" | while read -r file; do
    # Display a message indicating which file is being modified

    if [ $UNDO_MODE -eq 1 ]; then
      # check first line only	    
      if  head -n 1 "$file" | grep -q "^#include \"$HEADER_FILE\"$" "$file"; then
         echo "Undoing the insertion of header file for $file..."
         # Target the first line and remove it if it matches the include directive
          sed -i "1{/^\#include \"$HEADER_FILE\"$/d;}" "$file"
      else
          echo "the include directive does not exist. skipping undo insertion for $file..."
      fi 	      
    else
       echo "Inserting the header file into $file..."
        # Check if file is empty
        if [ ! -s "$file" ]; then
            # File is empty, just add the include line
            echo "#include \"$HEADER_FILE\"" > "$file"
        else
            # File is not empty, insert include line at the top
            #sed -i '1s/^/#include \"$HEADER_FILE\"\n/' "$file"
	     if ! grep -q "^#include \"$HEADER_FILE\"$" "$file"; then
                sed -i "1s|^|#include \"$HEADER_FILE\"\n|" "$file"
             else
                echo "the include directive already exist. skipping insertion for $file..."
             fi			
        fi
    fi
done

if [ $UNDO_MODE -eq 1 ]; then
    echo "Undo operation complete."
else
    echo "Insertion complete."
fi

