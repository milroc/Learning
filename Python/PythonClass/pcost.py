# pcost.py

import glob

def portfolio_cost(globList):
    total_cost = 0.0
    ret = ''
    fileList = glob.glob(globList)
    for file in fileList:
        f = open(file,"r")
        for line in f:
            fields  = line.split()
            try:
                nshares = int(fields[1])
                price   = float(fields[2])
                total_cost = total_cost + nshares*price
            except ValueError:
                print "Bad Line", line,
            except IndexError:
                print "Bad Index", line,   
        f.close()
        ret = ret + (file + ": " + str(total_cost) + "\n")
    return ret
print portfolio_cost("Data/portfolio*.dat")


