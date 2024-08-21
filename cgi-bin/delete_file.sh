#!/bin/bash
# Read the POST data
read -n $CONTENT_LENGTH POST_DATA
# Get directory
DIRECTORY="$UPLOAD_DIR"
# Extract the file name from the POST data
FILENAME=$(echo "$POST_DATA" | sed -n 's/^.*file=\([^&]*\).*$/\1/p')
# Decode URL-encoded string (replace + with space and decode %XX)
FILENAME=$(echo -e "$(echo "$FILENAME" | sed 's/+/ /g; s/%\([0-9A-Fa-f][0-9A-Fa-f]\)/\\x\1/g')")
# Trim any whitespace
FILENAME=$(echo "$FILENAME" | xargs)
# Debugging: Print the original POST data and decoded filename
echo "Original POST data: $POST_DATA" >&2
echo "Decoded filename: '$FILENAME'" >&2
# Ensure the UPLOAD_DIR is set, and construct the correct file path
if [ -n "$UPLOAD_DIR" ]; then
    FILE_PATH="$UPLOAD_DIR/$FILENAME"
else
    FILE_PATH="./$FILENAME"
fi
# Check if the file exists exactly as expected
if [ ! -f "$FILE_PATH" ]; then
    echo "File not found after decoding: '$FILE_PATH'" >&2
    echo "HTTP/1.1 404 Not Found"
    echo "Content-Type: text/html"
    echo ""
    echo "<html><body><h1>Error</h1><p>File $FILENAME not found in $UPLOAD_DIR.</p>
     <p><a href=\"files.sh\">Back</a></p></body></html>"
    exit 1
fi
# Delete the file
rm "$FILE_PATH"
echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo ""
echo "<html><body><h1>File Deleted</h1><p>$FILENAME has been deleted from $UPLOAD_DIR.</p>
     <p><a href=\"files.sh\">Back</a></p></body></html>"