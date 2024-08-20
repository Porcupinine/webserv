#!/usr/bin/env python3
import datetime
import io
import sys
import cgitb
import unittest
from unittest.mock import patch
import os
import cgi
import http.cookies

cgitb.enable()

class BytesIOWithBuffer(io.BytesIO):
    @property
    def buffer(self):
        return self

def fillForm():
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
        # expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
        # cookie["name"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")
    if intra_id:
        cookie["intra_id"] = intra_id
        # expires = datetime.datetime.utcnow() + datetime.timedelta(days=30)
        # cookie["intra_id"]["expires"] = expires.strftime("%a, %d-%b-%Y %H:%M:%S GMT")

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
            Connection: closed\r
            {cookie.output()}\r
            Date: {date}\r
            Last-Modified: {date}\r
            Server: {os.environ.get("SERVER")}\r\n\r"""

    # Output header and body
    print(header)
    print(body)
    print("\0")

class TestCGIScript(unittest.TestCase):
    @patch('sys.stdin', new_callable=BytesIOWithBuffer)
    @patch('sys.stdout', new_callable=BytesIOWithBuffer)
    @patch.dict(os.environ, {
        "ACCEPT_CHARSETS": "",
        "ACCEPT_LANGUAGE": "en-US,en;q=0.5",
        "ALLOW": "",
        "AUTH_SCHEME": "",
        "AUTHORIZATION": "",
        "CONNECTION": "keep-alive",
        "CONTENT_LANGUAGE": "",
        "CONTENT_LENGTH": "",
        "CONTENT_LOCATION": "",
        "CONTENT_TYPE": "",
        "COOKIE": "",
        "DATE": "",
        "HOST": "localhost:8080",
        "LAST_MODIFIED": "",
        "LOCATION": "",
        "REFERER": "http://localhost:8080/",
        "RETRY_AFTER": "",
        "SERVER": "localhost",
        "TRANSFER_ENCODING": "",
        "USER_AGENT": "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:129.0) Gecko/20100101 Firefox/129.0",
        "WWW_AUTHENTICATE": "",
        "REQUEST_METHOD": "GET",
        "QUERY_STRING": "fname=ser&iname=wer",
        "UPLOAD_DIR": "/sam/Codam/webserv/www/uploads",
    })
    def test_fillForm(self, mock_stdin, mock_stdout):
        # Simulate input data
        input_data = b""
        mock_stdin.write(input_data)
        mock_stdin.seek(0)

        # Call the function under test
        fillForm()

        # # Check the output
        # output = mock_stdout.getvalue().decode()
        #
        # # Assertions to ensure the correct response is generated
        # self.assertIn("HTTP/1.1 200 OK", output)
        # self.assertIn("Thank you ser for your precious data!!", output)
        # self.assertIn('Set-Cookie: name=ser', output)
        # self.assertIn('Set-Cookie: intra_id=wer', output)

if __name__ == '__main__':
    unittest.main()
