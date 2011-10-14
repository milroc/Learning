#!/usr/bin/env python

# A template string containing the desired output.  Modify
# as needed to change the output

from string import Template
import urllib, csv

temp = Template("""\
<b>$symbol</b> ($name)<br>
<font size=+2><b>$price</b></font> 
<font color="$color">$change ($percent)</font> 
$date $time <br>
<table>
<tr><td>Open:</td><td>$open</td><td>Volume:</td><td>$volume</td></tr>
<tr><td>High:</td><td>$high</td><td>Low:</td><td>$low</td></tr>
</table>
""")

def stockquote_app(environ,start_response):
    # Get form fields
    import cgi
    fields = cgi.FieldStorage(environ['wsgi.input'],
                              environ=environ)
    s = fields.getvalue('s')

    # Fetch data about the selected stock
    request = {
        's' : s,
        'f' : 'snl1d1t1c1p2ohgv',
        'e' : '.csv'
        }


    parms = urllib.urlencode(request)

    u = urllib.urlopen("http://download.finance.yahoo.com/d/quotes.csv?"+parms)
    reader = csv.DictReader(u,['symbol','name','price','date','time','change','percent',
                               'open','high','low','volume'])

    record = reader.next()

    if float(record['change']) > 0:
        record['color'] = 'green'
    else:
        record['color'] = 'red'

    # Generate the response
    headers = [
            ('Content-type','text/html')
            ]
    start_response("200 OK", headers)

    # Render the template and return it
    return [temp.substitute(record)]

# Run as a CGI script
import wsgiref.handlers
wsgiref.handlers.CGIHandler().run(stockquote_app)
