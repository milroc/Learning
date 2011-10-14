from xml.etree.ElementTree import parse

def bus_locations(filename):
    doc = parse(filename)
    for elem in doc.findall('bus'):
        print "%s, %s, %s, %s, %s" % (elem.findtext('id'),
                                      elem.findtext('route'),
                                      elem.findtext('direction'),
                                      elem.findtext('latitude'),
                                      elem.findtext('longitude'))

        
    
    
if __name__ == '__main__':
    bus_locations("Data/allroutes.xml")