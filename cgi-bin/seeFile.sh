#!/bin/bash

# Function to determine MIME type
get_mime_type() {
    case "$1" in
        *.html) echo "text/html" ;;
        *.css) echo "text/css" ;;
        *.js) echo "application/javascript" ;;
        *.json) echo "application/json" ;;
        *.txt) echo "text/plain" ;;
        *.jpg|*.jpeg) echo "image/jpeg" ;;
        *.png) echo "image/png" ;;
        *.gif) echo "image/gif" ;;
        *.pdf) echo "application/pdf" ;;
        *.mp3) echo "audio/mpeg" ;;
        *.mp4) echo "video/mp4" ;;
        *.avi) echo "video/x-msvideo" ;;
        *.zip) echo "application/zip" ;;
        *.tar) echo "application/x-tar" ;;
        *.gz) echo "application/gzip" ;;
        *.doc) echo "application/msword" ;;
        *.docx) echo "application/vnd.openxmlformats-officedocument.wordprocessingml.document" ;;
        *) echo "application/octet-stream" ;;  # Default for binary files
    esac
}

# Extract the filename from the query strin
FILE=$(echo "$QUERY_STRING" | sed 's/^.*file=//')
FILE_PATH="$UPLOAD_DIR/$FILE"

# Check if the file exists
if [ ! -f "$FILE_PATH" ]; then
    echo "Status: 404 Not Found"
    echo "Content-type: text/plain"
    echo ""
    echo "Error: File not found."
    exit 1
fi

# Get the MIME type based on the file extension
MIME_TYPE=$(get_mime_type "$FILE_PATH")

# Output the content-type header
echo "Content-type: $MIME_TYPE"
echo ""

# Output the binary content of the file
cat "$FILE_PATH"
