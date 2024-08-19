#!/bin/bash

#check if exists, delete, call files again?

# Build body
BODY="<!DOCTYPE html>
<html>
<body>

<h1>File was successfully deleted!!</h1>

<p>Thank you for your file!</p>
<p> {message}</p>
<p><a href="/upload.html">Upload another picture</a></p>
<p><a href="/index.html">Back</a></p>

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
