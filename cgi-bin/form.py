#!/usr/bin/env python3

#get information from query string, set the variables and send the cookie (Set-Cookie) header

import os
import cgi
import cgitb
import http.cookies
import datetime

# Enable CGI debugging
cgitb.enable()

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
name = form.getvalue("fname")
intra_id = form.getvalue("iname")

# Set cookies with the form data
cookie = http.cookies.SimpleCookie()

# Set cookies
if name:
    cookie["name"] = name
    expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
    cookie["name"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")
if intra_id:
    cookie["intra_id"] = intra_id
    expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
    cookie["intra_id"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")

# Get time and date
x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

status = 200

# Build body
body = f"""<!DOCTYPE html>
    <html>
    <body>
    
    <h1>Thank you {name} for your precious data!!</h1>
    <p><a href="/index.html">Back</a></p>
    
    </body>
    </html>"""

# Build header
header = f"""HTTP/1.1 {status} OK\r
    Content-Length: {len(body)}\r
    Content-type: text/html\r
    Connection: close\r
    {cookie.output()}\r
    Date: {date}\r
    Last-Modified: {date}\r
    Server: {os.environ.get("SERVER")}\r\n\r"""

# Output header and body
print(header)
print(body)
print("\0")
