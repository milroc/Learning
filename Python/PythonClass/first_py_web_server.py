
# webserve.py
# Note: The program must be placed in the PythonNetwork/ folder

from BaseHTTPServer import HTTPServer
from CGIHTTPServer import CGIHTTPRequestHandler
import os
os.chdir("Exercises")
serv = HTTPServer(("",8080),CGIHTTPRequestHandler)
serv.serve_forever()