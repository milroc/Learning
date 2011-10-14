import urllib
import json

u = urllib.urlopen("http://search.twitter.com/search.json?q=python&rpp=10")
 
rawdata = u.read()
#print rawdata
data = json.loads(rawdata)
print data
for tw in data['results']:
    print tw['text'].encode('ascii','replace')
    print
    
