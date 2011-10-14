from socket import *

''' EXERCISE 1_1 '''
#s = socket(AF_INET, SOCK_STREAM)
#s.connect(("www.python.org",80))
#s.send("GET /index.html HTTP/1.0\r\n\r\n")
#
#chunks = [] 
#while True:
#    chunk = s.recv(16384)
#    if not chunk:
#        break
#    chunks.append(chunk)
#
#s.close()
#response = ''.join(chunks)
#print response

''' EXERCISE 1_2 '''
''' 1_2_a'''
#s = socket(AF_INET, SOCK_STREAM)
#s.bind(("",15000))
#s.listen(5)
#c,a = s.accept()
#print c,a



#s = socket.socket(addr_family, type)
#
#socket.AFINET #IPV4
#socket.AF_INET6 #IPV6
#
#socket.SOCK_STREAM 