#!/usr/bin/env python
# The above line is required for Unix systems.  Make sure
# you use the command 'chmod +x hello.py' to make this program executable

print "Content-type: text/html"
print
print """
<html>
<body>
<h1>Hello World!</h1>
"""

import time
print "The time is ", time.ctime()

print """
</body>
</html>
"""