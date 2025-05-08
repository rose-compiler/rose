#!/bin/sh
# Reads text from standard input and prints it as a C string to standard output.


while IFS= read -r line || [ -n "$line" ]; do
    # Escape backslashes, double quotes, and tabs.
    escaped_line=$(printf "%s" "$line" | sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/\t/\\t/g')
    # Print the line in double-quotes.
    echo "\"${escaped_line}\""
done