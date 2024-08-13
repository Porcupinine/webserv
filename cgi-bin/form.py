#!/usr/bin/env python3

#get information from query string, set the variables and send the cookie (Set-Cookie) header

import os
import cgi
import http.cookies
import datetime

# Enable CGI debugging
import cgitb
cgitb.enable()

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
name = form.getvalue("fname")
intra_id = form.getvalue("iname")

# Set cookies with the form data
cookie = http.cookies.SimpleCookie()

# Set a cookies
if name:
    cookie["name"] = name
    expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
    cookie["name"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")
if intra_id:
    cookie["intra_id"] = intra_id
    expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
    cookie["intra_id"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")

# Output the HTTP headers
print("Content-Type: text/html")
if name or intra_id:
    print(cookie.output())
print()  # Blank line to end the headers

# Output the HTML response
print("<html>")
print("<body>")
print("<h1>Thank you for submitting your data!</h1>")

if name:
    print(f"<p>Name: {name}</p>")
if intra_id:
    print(f"<p>Intra ID: {intra_id}</p>")

print("<p><a href='/index.html'>Back to the homepage</a></p>")
print("</body>")
print("</html>")
