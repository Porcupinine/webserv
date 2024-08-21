#!/usr/bin/env python3
import io
import sys
import os
import unittest
from unittest.mock import patch
from form import fillForm
class BytesIOWithBuffer(io.BytesIO):
    @property
    def buffer(self):
        return self

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

        # Check the output
        output = mock_stdout.getvalue().decode('utf-8')

        # Assertions to ensure the correct response is generated
        self.assertIn("HTTP/1.1 200 OK", output)
        self.assertIn("Thank you ser for your precious data!!", output)
        self.assertIn('Set-Cookie: name=ser', output)
        self.assertIn('Set-Cookie: intra_id=wer', output)

if __name__ == '__main__':
    unittest.main()
