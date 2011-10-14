from socket import *

def geturl(url, port=80):
    proto, host, resource = parse(url)
    s = socket(AF_INET,SOCK_STREAM)
    s.connect((host, port))
    s.send("GET %s HTTP/1.0\r\nHost: %s\r\n\r\n" % (resource, host))
    chunks = []
    while True:
            chunk = s.recv(16384)
            if not chunk: break
            chunks.append(chunk)
    s.close()
    response = "".join(chunks)
    return response

def parse(s):
    ''' Split into a protocol, host, and resource

        >>> parse('http://www.python.org/index.html')
        ('http', 'www.python.org', '/index.html')

    '''
    proto, sep, url = s.partition('://')
    host, slash, resource = url.partition('/')
    return proto, host, '/' + resource


print geturl('http://www.python.org/index.html')
