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

def uploadFile():
    if not os.path.isdir("logdir"):
        os.mkdir("logdir")

    form = cgi.FieldStorage()
    fileData = form['filename']
    upDir = os.environ["UPLOAD_DIR"]
    path = upDir + "/" + fileData.filename
    name, extention = os.path.splitext(fileData.filename)
    x = 0
    if not os.path.isdir(upDir):
        os.mkdir(upDir)
    while os.path.isfile(path):
        x += 1
        path = upDir + "/" + name + "(" + str(x) + ")" + extention
    try:
        with open(os.path.join(path), 'wb') as f:
            f.write(fileData.file.read())
        return (201, f"uploaded successfully!!!")
    except:
        return (500, "Upload failed!!!")

class TestCGIScript(unittest.TestCase):
    @patch('sys.stdin', new_callable=BytesIOWithBuffer)
    @patch.dict(os.environ, {
        "REQUEST_METHOD": "POST",
        "QUERY_STRING": "",
        "CONTENT_TYPE": "multipart/form-data; boundary=---------------------------340347531311166895761615598862",
        "CONTENT_LENGTH": "513",
        "UPLOAD_DIR": "/sam/Codam/webserv/cgi-bin/uploads",
        "ACCEPT": "text/html;application/xhtml+xml;application/xml;q=0.9;image/avif;image/webp;*/*;q=0.8",
        "ACCEPT_ENCODING": "gzip; deflate; br; zstd",
        "ACCEPT_LANGUAGE": "en-US;en;q=0.5",
        "CONNECTION": "keep-alive",
        "COOKIE": "name=localhost; id=339071893504; trigger=cookie",
        "HOST": "localhost:9090",
        "ORIGIN": "http://localhost:9090",
        "PRIORITY": "u=1",
        "REFERER": "http://localhost:9090/upload.html",
        "SEC_FETCH_DEST": "document",
        "SEC_FETCH_MODE": "navigate",
        "SEC_FETCH_SITE": "same-origin",
        "SEC_FETCH_USER": "?1",
        "UPGRADE_INSECURE_REQUESTS": "1",
        "USER_AGENT": "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:127.0) Gecko/20100101 Firefox/127.0"
    })
    def test_do_my_stuff(self, mock_stdin):
        # Simulate input data
        input_data = b"""-----------------------------340347531311166895761615598862\r\nContent-Disposition: form-data; name="filename"; filename="1x1#FFFFFF.jpg"\r\nContent-Type: image/jpeg\r\n\r\n\xff\xd8\xff\xe0\x00\x10JFIF\x00\x01\x01\x01\x00H\x00H\x00\x00\xff\xe1\x00\x18Exif\x00\x00MM\x00*\x00\x00\x00\x08\x00\x03\x01\x12\x00\x03\x00\x00\x00\x01\x00\x01\x00\x00\x02\x01\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\r\n-----------------------------340347531311166895761615598862--\r\n"""
        mock_stdin.write(input_data)
        mock_stdin.seek(0)

        uploadFile()


if __name__ == '__main__':
    unittest.main()
message = ""
status = 0
files = os.listdir(os.environ.get("UPLOAD_DIR"))

if os.environ.get("REQUEST_METHOD") == "POST":
    status, message = uploadFile()
else:
    message = "Sorry, can't do!\n"

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body = f"""<!DOCTYPE html>
    <html>
    <body>
    
    <h1>Welcome to the ______ webserv!!</h1>
    
    <p>Now give us your best picture!</p>
    <p> {message}</p>
       <p><a href="/upload.html">Upload another picture</a></p>
    <p><a href="/index.html">Back</a></p>
    
    </body>
    </html>"""

header = f"""HTTP/1.1 {status}\r
    Content-Length: {len(body)}\r
    Content-type: text/html\r
    Connection: close\r
    Date: {date}\r
    Last-Modified: {date}\r
    Server: {os.environ.get("SERVER")}\r\n\r"""

print(header)
print(body)
print("\0")
