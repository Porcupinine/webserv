#!/usr/bin/env python3
import datetime
import io
import os
import sys
import cgi
import cgitb
import unittest
from unittest.mock import patch

cgitb.enable()
cgitb.enable(display=0, logdir="logdir")  # for debugging

class BytesIOWithBuffer(io.BytesIO):
    @property
    def buffer(self):
        return self

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
        return 500, "Oops! Something went wrong.\n"

class TestCGIScript(unittest.TestCase):
    @patch('sys.stdin', new_callable=BytesIOWithBuffer)
    @patch.dict(os.environ, {
        "ACCEPT_CHARSETS": "POST",
        "ACCEPT_LANGUAGE": "",
        "ALLOW": "",
        "AUTH_SCHEME": "",
        "AUTHORIZATION": "",
        "CONNECTION": "Keep-Alive",
        "CONTENT_LANGUAGE": "",
        "CONTENT_LENGTH": "712",
        "CONTENT_LOCATION": "",
        "CONTENT_TYPE": "multipart/form-data; boundary=------------------------mgxwWXZJLDLtXEBepqWhgk",
        "COOKIE": "",
        "DATE": "",
        "HOST": "localhost:8989",
        "LAST_MODIFIED": "",
        "LOCATION": "",
        "REFERER": "",
        "RETRY_AFTER": "",
        "SERVER": "",
        "TRANSFER_ENCODING": "",
        "USER_AGENT": "curl/8.5.0",
        "WWW_AUTHENTICATE": "",
        "REQUEST_METHOD": "POST",
        "QUERY_STRING": "",
        "UPLOAD_DIR": "/sam/Codam/web2/www/uploads",
        "SERVER": "localhost",
        "CGIPATH": "/sam/Codam/web2/cgi-bin"
    })
    def test_do_my_stuff(self, mock_stdin):
        # Simulate input data
        input_data = b"""--------------------------mgxwWXZJLDLtXEBepqWhgk\r\nContent-Disposition: form-data; name=\"userid\"\r\n\r\n1\r\n--------------------------mgxwWXZJLDLtXEBepqWhgk\r\nContent-Disposition: form-data; name=\"filecomment\"\r\n\r\nThis is an image file\r\n--------------------------mgxwWXZJLDLtXEBepqWhgk\r\nContent-Disposition: form-data; name=\"image\"; filename=\"pixel.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n\377\330\377\340\000\020JFIF\000\001\001\001\000H\000H\000\000\377\333\000C\000\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\377\333\000C\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\377\300\000\021\b\000\001\000\001\003\001\021\000\002\021\001\003\021\001\377\304\000\024\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\n\377\304\000\024\020\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\304\000\024\001\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\304\000\024\021\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\332\000\f\003\001\000\002\021\003\021\000?\000\177\000\377\331\r\n--------------------------mgxwWXZJLDLtXEBepqWhgk--\r\n"""
        mock_stdin.write(input_data)
        mock_stdin.seek(0)

        uploadFile()


if __name__ == '__main__':
    unittest.main()
# message = ""
# status = 0
# upDir = os.environ["UPLOAD_DIR"]
#
# # Check if folder exists, if not, create such folder
# if not os.path.isdir(upDir):
#     os.mkdir(upDir)
#
# if os.environ.get("REQUEST_METHOD") == "POST":
#     status, message = uploadFile()
# else:
#     status, message = 405, "Method Not Allowed\n"
#
# # Get time and date
# x = datetime.datetime.now()
# date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")
#
# # Build body
# body = f"""<!DOCTYPE html>
# <html>
# <body>
#
# <h1>Welcome to the ______ webserv!!</h1>
#
# <p> {message}</p>
# <p>We can take some .png .gif .jpg and .jpeg!</p>
# <p><a href="/upload.html">Give us more images?</a></p>
# <p><a href="/index.html">Back</a></p>
#
# </body>
# </html>"""
#
# # Build header
# header = f"""HTTP/1.1 {status}\r
# Content-Length: {len(body)}\r
# Content-type: text/html\r
# Connection: closed\r
# Date: {date}\r
# Last-Modified: {date}\r
# Server: {os.environ.get("SERVER")}\r\n\r"""
#
# print(header)
# print(body)
# print("\0")