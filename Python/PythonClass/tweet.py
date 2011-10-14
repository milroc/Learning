# tweet.py

import urllib
import json
import pprint

def tweets(subject,nresults):
    fields = {
        'q' : subject,
        'rpp' : nresults
        }
    parms = urllib.urlencode(fields)
    u = urllib.urlopen("http://search.twitter.com/search.json?"+parms)
    data = u.read()
    return json.loads(data)

d = tweets("raymondh", 5)

#pprint.pprint(d)

##for result in d['results']:
##    print result['from_user']
##    print
