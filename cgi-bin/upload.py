#!/usr/bin/env python3

import datetime
import os
import cgi
import cgitb

cgitb.enable()
cgitb.enable(display=0, logdir="logdir")  # for debugging


def uploadFile() -> (int, str):
    if not os.path.isdir("logdir"):
        os.mkdir("logdir")

    form = cgi.FieldStorage()
    fileData = form['filename']
    upDir = os.environ["UPLOAD_DIR"]
    path = upDir + "/" + fileData.filename
    name, extension = os.path.splitext(fileData.filename)
    x = 0
    if not os.path.isdir(upDir):
        os.mkdir(upDir)
    while os.path.isfile(path):
        x += 1
        path = upDir + "/" + name + "(" + str(x) + ")" + extension
    try:
        with open(os.path.join(path), 'wb') as f:
            f.write(fileData.file.read())
        return 201, f"Well done!!!\n"
    except:
        return 500, "Ooopsie!!!\n"


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
    Content-type: text/html; charset=utf-8\r
    Connection: close\r
    Date: {date}\r
    Last-Modified: {date}\r
    Server: {os.environ.get("SERVER")}\r\n\r"""

print(header)
print(body)
print("\0")
