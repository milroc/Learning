import urllib

#HOW DO YOU SILENCE????
def parse(file, types, names=None, sep=None):
    ret = []
    for i,line in enumerate(file,1):
        if not line: 
            continue
        fields = line.split(sep)
        fields = [f.strip('"') for f in fields]
        #THIS IS A HACK TO MAKE SURE THIS RUNS WITH URLLIBOPEN
        if len(fields) == 1:
            continue        
        try:
            cfields = [converter(value) for converter,value in zip(types, fields)]
        except ValueError as e:
            #print "Lineno: %s Couldn't parse: %s Reason: %s" % (i, line, e)
            continue
        if names is not None:
            record = dict(zip(names,cfields))
        else:
            record = tuple(cfields)
        ret.append(record)
    return ret


#print parse(open("Data/portfolio3.dat"), [str,int,float],['name','shares','price'])
