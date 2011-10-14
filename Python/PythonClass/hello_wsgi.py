#!/usr/bin/env python
# The above line is required for Unix systems.  Make sure
# you use the command 'chmod +x hello.py' to make this program executable

import time

def hello_app(environ,start_response):
    headers = [
        ('Content-type','text/html')
        ]
    start_response("200 OK", headers)

    # This is a little weird at first, but the result should be a list
    # of byte strings that make up the output
    result = []
    result.append("""\
<html>
<body>
<h1>Hello World!</h1>
""")
    result.append("The time is " + time.ctime())
    result.append("""\
</body>
</html>
""")
    return result

# Run as a CGI script
import wsgiref.handlers
wsgiref.handlers.CGIHandler().run(hello_app)