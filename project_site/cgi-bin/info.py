#!/usr/bin/env python3

import os
import sys

method = os.environ.get('REQUEST_METHOD')

print("Content-type: text/html\r\n\r\n")
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>CGI Response</title><link rel='stylesheet' href='/css/style.css'></head>")
print("<body>")
print("<div class='container'>")
print("<header><h1>CGI Script Output</h1></header>")

print("<h2>Request Details</h2>")
print("<p><strong>Method:</strong> {}</p>".format(method))

if method == "GET":
    print("<h3>GET Request Data (Query String)</h3>")
    query_string = os.environ.get('QUERY_STRING', '')
    if query_string:
        print("<p>{}</p>".format(query_string))
    else:
        print("<p>No query string data found.</p>")
elif method == "POST":
    print("<h3>POST Request Data (Request Body)</h3>")
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        try:
            post_body = sys.stdin.read(content_length)
            print("<p>{}</p>".format(post_body))
        except Exception as e:
            print("<p class='warning'>Error reading POST body: {}</p>".format(e))
    else:
        print("<p>No POST body received.</p>")

print("</div>")
print("</body>")
print("</html>")