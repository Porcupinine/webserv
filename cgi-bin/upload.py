#!/usr/bin/env python3

# POST /upload HTTP/1.1
# Host: example.com
# Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryE19zNvXGzXaLvS5C
# Content-Length: 123456
#
# ------WebKitFormBoundaryE19zNvXGzXaLvS5C
# Content-Disposition: form-data; name="file"; filename="example.jpg"
# Content-Type: image/jpeg
#
# (binary data of the image file goes here)
# ------WebKitFormBoundaryE19zNvXGzXaLvS5C--

#will get the request, from body parse header and body, find filename

# import cgitb
# cgi.