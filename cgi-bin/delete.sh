#!/bin/bash

#check if exists, delete, call files again? parse body to get

# Check if UPLOAD_DIR is set
if [ -z "$UPLOAD_DIR" ]; then
    HEADER=""
    echo "<html><body><h1>Error</h1><p>UPLOAD_DIR environment variable is not set.</p></body></html>"
    exit 1
fi

# Get directory
DIRECTORY="$UPLOAD_DIR"

# Build body
BODY="<!DOCTYPE html>
<html>
<body>

<h1>File was successfully deleted!!</h1>

<p>We are sad to see your data go!</p>
<p><a href=\"$DIRECTORY/files.sh\">Back</a></p>


</body>
</html>"

# Build header
HEADER="HTTP/1.1 200\r\n
Connection: close\r\n
Content-length: $BODYLEN\r\n
Content-type: text/html\r\n\r\n
"

# Output -e so the backslash is interpreted
echo -e $HEADER
echo -e $BODY
