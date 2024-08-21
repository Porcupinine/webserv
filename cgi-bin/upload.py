#!/usr/bin/env python3

import datetime
import os
import cgi
import cgitb
import logging

logger = logging.getLogger(__name__)
if not os.path.isdir("logs"):
    os.mkdir("logs")
logging.basicConfig(filename='logs/upload.log', encoding='utf-8', level=logging.DEBUG)

cgitb.enable()
cgitb.enable(display=0, logdir="logdir")  # for debugging

def uploadFile() -> (int, str):
    # Set logdir for debugging purposes
    if not os.path.isdir("logdir"):
        os.mkdir("logdir")

    # Get data parsed from cgi module
    form = cgi.FieldStorage()
    fileData = form['filename']
    name, extension = os.path.splitext(fileData.filename)

    # Check if filename contains spaces
    if ' ' in fileData.filename:
        return 400, "Filenames with spaces are not allowed.\n"

    path = os.path.join(upDir, fileData.filename)
    x = 0

    # Check file extension
    valid_extensions = {'.jpg', '.jpeg', '.png', '.gif'}
    ext = extension.lower()

    if ext not in valid_extensions:
        return 400, "Invalid file type.\n"

    path = os.path.join(upDir, fileData.filename)

# Check if file already exists to set a new file name
    x = 0
    while os.path.isfile(path):
        x += 1
        path = os.path.join(upDir, f"{name}({x}){extension}")

    # Write data into newly created file
    try:
        with open(path, 'wb') as f:
            f.write(fileData.file.read())
        return 201, "Thanks for your file!\n"
    except Exception as e:
        logger.error(f"Failed to write file: {e}")
        return 500, "Oops! Something went wrong.\n"

try:
    message = ""
    status = 0
    upDir = os.environ["UPLOAD_DIR"]

    logger.info("Starting file upload process")

    # Check if folder exists, if not, create such folder
    if not os.path.isdir(upDir):
        logger.error("Upload directory does not exist")
        os.mkdir(upDir)

    if os.environ.get("REQUEST_METHOD") == "POST":
        status, message = uploadFile()
    else:
        status, message = 405, "Method Not Allowed\n"

    # Get time and date
    x = datetime.datetime.now()
    date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

    # Build body
    body = f"""<!DOCTYPE html>
<html>
<body>

<h1>Welcome to the ______ webserv!!</h1>

<p> {message}</p>
<p>We can take some .png .gif .jpg and .jpeg!</p>
<p><a href="/upload.html">Give us more images?</a></p>
<p><a href="/index.html">Back</a></p>

</body>
</html>"""

    # Build header
    header = f"""HTTP/1.1 {status}\r
Content-Length: {len(body)}\r
Content-type: text/html\r
Connection: closed\r
Date: {date}\r
Last-Modified: {date}\r
Server: {os.environ.get("SERVER")}\r\n\r"""

    logger.info(f'header: {header}')
    logger.info(f'body: {body}')
    print(header)
    print(body)
except:
    logger.error("Your script failed!!")
finally:
    print("\0")
