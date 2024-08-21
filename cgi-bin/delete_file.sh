#!/bin/bash

# Read the POST data
read -n $CONTENT_LENGTH POST_DATA

# Get directory
DIRECTORY="$UPLOAD_DIR"

# Extract the file name from the POST data
FILENAME=$(echo "$POST_DATA" | sed -n 's/^.*file=\([^&]*\).*$/\1/p')

# Decode URL-encoded string (replace + with space and decode %XX)
FILENAME=$(echo -e "${FILENAME//+/ }" | sed 's/%\([0-9A-Fa-f][0-9A-Fa-f]\)/\\x\1/g')

# Ensure the UPLOAD_DIR is set, and construct the correct file path
if [ -n "$UPLOAD_DIR" ]; then
    FILE_PATH="$UPLOAD_DIR/$FILENAME"
else
    # Fallback to the current directory if UPLOAD_DIR is not set (optional)
    FILE_PATH="./$FILENAME"
fi

# Check if the file exists and delete it
if [ -f "$FILE_PATH" ]; then
    rm "$FILE_PATH"
    echo "HTTP/1.1 200 OK"
    echo "Content-Type: text/html"
    echo ""
    echo "<html><body><h1>File Deleted</h1><p>$FILENAME has been deleted from $UPLOAD_DIR.</p>
    <p><a href=\"$DIRECTORY/files.sh\">Back</a></p></body></html>"
else
    echo "HTTP/1.1 404 Not Found"
    echo "Content-Type: text/html"
    echo ""
    echo "<html><body><h1>Error</h1><p>File $FILENAME not found in $UPLOAD_DIR.</p>
    <p><a href=\"$DIRECTORY/files.sh\">Back</a></p></body></html>"
fi
