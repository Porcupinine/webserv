#!/bin/bash

 # get the folder to serch from the env, check if folder exists, if not, error
 #if folder exists, use ls to get all files inside the folder
 #fill html page with the path/name for each item and a delete button that calls the delete script

#TODO error headers

# Check if UPLOAD_DIR is set
if [ -z "$UPLOAD_DIR" ]; then
    HEADER=""
    echo "<html><body><h1>Error</h1><p>UPLOAD_DIR environment variable is not set.</p></body></html>"
    exit 1
fi

# Get directory
DIRECTORY="$UPLOAD_DIR"
# Echo the header
echo ""

# Start HTML output
BODY="<html>"
BODY+="<head><title>File List</title></head>"
BODY+="<body>"
BODY+="<h1>Check out our precious files</h1>"

# Check if the directory exists
if [ -d "$DIRECTORY" ]; then
    BODY+="<ul>"

    # List files in the specified directory
    for FILE in "$DIRECTORY"/*; do
        if [ -f "$FILE" ]; then
            FILENAME=$(basename "$FILE")
            BODY+="<li>"
            BODY+="<a href=\"$DIRECTORY/$FILENAME\" target=\"_blank\">$FILENAME               </a>"
            BODY+="<form style=\"display:inline;\" method=\"delete\" action=\"/cgi-bin/delete_file.cgi\">"
            BODY+="<input type=\"hidden\" name=\"file\" value=\"$FILENAME\" />"
            BODY+="<input type=\"submit\" value=\"Delete\" />"
            BODY+="</form>"
            BODY+="</li>"
        fi
    done

    BODY+="</ul>"
else
    #what error code?
    BODY="<p>The directory $DIRECTORY does not exist.</p>"
fi

# End HTML output
BODY+="<p><a href="/index.html">Back</a></p>"
BODY+="</body>"
BODY+="</html>"

# Calculate the size of the HTML body
BODYLEN=$(echo -n "$BODY" | wc -c)

# Build header
HEADER="HTTP/1.1 200\r\n
Connection: closed\r\n
Content-length: $BODYLEN\r\n
Content-type: text/html\r\n\r\n
"

# Output -e so the backslash is interpreted
echo -e $HEADER
echo -e $BODY

